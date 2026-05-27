; =============================================================================
; core/memory.inc -- heap helpers and arena allocator
;
; Anvil does not link against the C runtime. Allocations are either:
;   - made through anvil_alloc / anvil_free, which wrap HeapAlloc /
;     HeapFree on the process heap, or
;   - made through an arena, for short-lived bulk allocations.
; =============================================================================

%ifndef ANVIL_CORE_MEMORY_INC
%define ANVIL_CORE_MEMORY_INC

; -----------------------------------------------------------------------------
; Arena layout
;
;   offset  size  field
;   0       8     base    -- pointer to the underlying buffer
;   8       8     cap     -- total bytes in the buffer
;   16      8     used    -- bytes consumed so far
;   24      8     marker  -- magic value 'ANVILARN' (0x4E52414C49564E41)
; -----------------------------------------------------------------------------
%define ANVIL_ARENA_size            32
%define ANVIL_ARENA_base            0
%define ANVIL_ARENA_cap             8
%define ANVIL_ARENA_used            16
%define ANVIL_ARENA_marker          24

%define ANVIL_ARENA_MAGIC           0x414E5649_4C41524E   ; 'ANVI' 'LARN'

; Heap helpers
extern  anvil_alloc                 ; (u64 bytes) -> void*  (NULL on fail)
extern  anvil_calloc                ; (u64 bytes) -> void*  (zeroed)
extern  anvil_free                  ; (void*) -> void

; Arena allocator
extern  anvil_arena_init            ; (ARENA*, void* buf, u64 cap) -> i64 status
extern  anvil_arena_alloc           ; (ARENA*, u64 bytes) -> void*
extern  anvil_arena_reset           ; (ARENA*) -> void
extern  anvil_arena_used            ; (ARENA*) -> u64
extern  anvil_arena_remaining       ; (ARENA*) -> u64

; Misc
extern  anvil_memzero               ; (void* dst, u64 n) -> void
extern  anvil_memcopy               ; (void* dst, void* src, u64 n) -> void
extern  anvil_memcompare            ; (void* a, void* b, u64 n) -> i64

%endif  ; ANVIL_CORE_MEMORY_INC
