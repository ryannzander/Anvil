; =============================================================================
; src/core/memory.asm -- heap helpers, arena allocator, mem* helpers
;
; All non-arena allocations go through HeapAlloc on the process heap.
; The handle returned by GetProcessHeap is cached on first use, which
; saves one indirect call per allocation.
;
; The arena allocator is a simple bump pointer. It does not own its
; backing memory; the caller provides a buffer (often a static .bss
; region or a fresh HeapAlloc result).
; =============================================================================

%define ANVIL_IMPL_MEMORY
%include "platform/win64.inc"
%include "core/errors.inc"
%include "core/memory.inc"

section .bss
        align 8
g_process_heap:         resq 1      ; cached GetProcessHeap result

section .text

global anvil_alloc
global anvil_calloc
global anvil_free
global anvil_memzero
global anvil_memcopy
global anvil_memcompare
global anvil_arena_init
global anvil_arena_alloc
global anvil_arena_reset
global anvil_arena_used
global anvil_arena_remaining

; -----------------------------------------------------------------------------
; static get_heap() -> HANDLE
;
;   Returns the cached process heap, fetching it on first use.
;   Internal helper -- not exported.
; -----------------------------------------------------------------------------
get_heap:
        mov     rax, [rel g_process_heap]
        test    rax, rax
        jnz     .done
        PROLOGUE 0
        call    GetProcessHeap
        mov     [rel g_process_heap], rax
        EPILOGUE 0
.done:
        ret

; -----------------------------------------------------------------------------
; anvil_alloc(u64 bytes) -> void*
;
;   Allocates 'bytes' bytes on the process heap. Returns NULL on failure
;   and sets ANVIL_E_NOMEM.
;
;   In:  RCX = bytes
;   Out: RAX = pointer or NULL
; -----------------------------------------------------------------------------
anvil_alloc:
        PROLOGUE 16
        mov     [rsp + 32], rcx                 ; save size
        call    get_heap
        mov     rcx, rax                        ; arg1: heap
        xor     edx, edx                        ; arg2: flags = 0
        mov     r8,  [rsp + 32]                 ; arg3: size
        call    HeapAlloc
        test    rax, rax
        jnz     .ok
        mov     qword [rel g_anvil_last_error], ANVIL_E_NOMEM
.ok:
        EPILOGUE 16
        ret

; -----------------------------------------------------------------------------
; anvil_calloc(u64 bytes) -> void*
;
;   Zero-initialized allocation. Same failure mode as anvil_alloc.
; -----------------------------------------------------------------------------
anvil_calloc:
        PROLOGUE 16
        mov     [rsp + 32], rcx
        call    get_heap
        mov     rcx, rax
        mov     edx, HEAP_ZERO_MEMORY
        mov     r8,  [rsp + 32]
        call    HeapAlloc
        test    rax, rax
        jnz     .ok
        mov     qword [rel g_anvil_last_error], ANVIL_E_NOMEM
.ok:
        EPILOGUE 16
        ret

; -----------------------------------------------------------------------------
; anvil_free(void*) -> void
;
;   Frees a block previously returned by anvil_alloc / anvil_calloc.
;   Passing NULL is a no-op.
;
;   In:  RCX = pointer (may be NULL)
; -----------------------------------------------------------------------------
anvil_free:
        test    rcx, rcx
        jz      .done
        PROLOGUE 16
        mov     [rsp + 32], rcx
        call    get_heap
        mov     rcx, rax
        xor     edx, edx
        mov     r8,  [rsp + 32]
        call    HeapFree
        EPILOGUE 16
.done:
        ret

; -----------------------------------------------------------------------------
; anvil_memzero(void* dst, u64 n) -> void
;
;   Sets n bytes at dst to 0. Handles unaligned destinations.
;
;   In:  RCX = dst, RDX = n
;   Clobbers: RAX, RDI (we restore RDI), flags
; -----------------------------------------------------------------------------
anvil_memzero:
        push    rdi
        mov     rdi, rcx
        mov     rcx, rdx
        xor     eax, eax
        cld
        rep     stosb
        pop     rdi
        ret

; -----------------------------------------------------------------------------
; anvil_memcopy(void* dst, void* src, u64 n) -> void
;
;   Non-overlapping copy. Uses REP MOVSB; modern CPUs (ERMS / FSRM) make
;   this near-optimal without an unrolled loop. Behaviour with
;   overlapping regions is undefined.
;
;   In:  RCX = dst, RDX = src, R8 = n
;   Clobbers: RDI, RSI (restored), RAX
; -----------------------------------------------------------------------------
anvil_memcopy:
        push    rdi
        push    rsi
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rcx, r8
        cld
        rep     movsb
        pop     rsi
        pop     rdi
        ret

; -----------------------------------------------------------------------------
; anvil_memcompare(void* a, void* b, u64 n) -> i64
;
;   Returns 0 if the n bytes are equal, < 0 if a < b at the first
;   differing byte, > 0 if a > b. (memcmp-equivalent.)
;
;   In:  RCX = a, RDX = b, R8 = n
;   Out: RAX
; -----------------------------------------------------------------------------
anvil_memcompare:
        push    rdi
        push    rsi
        xor     eax, eax
        test    r8, r8
        jz      .done
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rcx, r8
        cld
        repe    cmpsb
        jz      .done                           ; all equal
        ; rdi and rsi point just past the differing byte
        movzx   eax, byte [rsi - 1]
        movzx   r10d, byte [rdi - 1]
        sub     rax, r10
.done:
        pop     rsi
        pop     rdi
        ret

; =============================================================================
; Arena allocator
;
; struct ANVIL_ARENA {
;     void *base;   // 0
;     u64   cap;    // 8
;     u64   used;   // 16
;     u64   marker; // 24  -- magic for assertion
; };
; =============================================================================

; -----------------------------------------------------------------------------
; anvil_arena_init(ARENA* a, void* buf, u64 cap) -> i64
;
;   Initializes an arena to use buf as its backing store.
;
;   In:  RCX = a, RDX = buf, R8 = cap
;   Out: RAX = ANVIL_OK on success, ANVIL_E_INVALID on bad args
; -----------------------------------------------------------------------------
anvil_arena_init:
        test    rcx, rcx
        jz      .bad
        test    rdx, rdx
        jz      .bad
        test    r8,  r8
        jz      .bad

        mov     [rcx + ANVIL_ARENA_base], rdx
        mov     [rcx + ANVIL_ARENA_cap],  r8
        mov     qword [rcx + ANVIL_ARENA_used], 0
        mov     rax, ANVIL_ARENA_MAGIC
        mov     [rcx + ANVIL_ARENA_marker], rax
        xor     eax, eax                        ; ANVIL_OK
        ret
.bad:
        mov     qword [rel g_anvil_last_error], ANVIL_E_INVALID
        mov     rax, ANVIL_E_INVALID
        ret

; -----------------------------------------------------------------------------
; anvil_arena_alloc(ARENA* a, u64 bytes) -> void*
;
;   Bump-allocates 'bytes' bytes from the arena, 8-byte aligning the
;   returned pointer. Returns NULL on out-of-space.
;
;   In:  RCX = a, RDX = bytes
;   Out: RAX = pointer or NULL
; -----------------------------------------------------------------------------
anvil_arena_alloc:
        ; Validate the magic to catch passing the wrong pointer.
        mov     rax, [rcx + ANVIL_ARENA_marker]
        mov     r10, ANVIL_ARENA_MAGIC
        cmp     rax, r10
        jne     .badarena

        ; round bytes up to next multiple of 8
        add     rdx, 7
        and     rdx, -8

        mov     r9, [rcx + ANVIL_ARENA_used]
        mov     r10, [rcx + ANVIL_ARENA_cap]
        mov     rax, r9
        add     rax, rdx
        jc      .full                           ; overflow
        cmp     rax, r10
        ja      .full

        ; new used <- rax; result <- base + old used
        mov     [rcx + ANVIL_ARENA_used], rax
        mov     rax, [rcx + ANVIL_ARENA_base]
        add     rax, r9
        ret

.badarena:
        mov     qword [rel g_anvil_last_error], ANVIL_E_INVALID
        xor     eax, eax
        ret
.full:
        mov     qword [rel g_anvil_last_error], ANVIL_E_NOMEM
        xor     eax, eax
        ret

; -----------------------------------------------------------------------------
; anvil_arena_reset(ARENA*) -> void
;
;   Resets the bump pointer back to zero. The backing buffer is *not*
;   freed; the caller owns it.
; -----------------------------------------------------------------------------
anvil_arena_reset:
        mov     qword [rcx + ANVIL_ARENA_used], 0
        ret

; -----------------------------------------------------------------------------
; anvil_arena_used(ARENA*) -> u64
; -----------------------------------------------------------------------------
anvil_arena_used:
        mov     rax, [rcx + ANVIL_ARENA_used]
        ret

; -----------------------------------------------------------------------------
; anvil_arena_remaining(ARENA*) -> u64
; -----------------------------------------------------------------------------
anvil_arena_remaining:
        mov     rax, [rcx + ANVIL_ARENA_cap]
        sub     rax, [rcx + ANVIL_ARENA_used]
        ret
