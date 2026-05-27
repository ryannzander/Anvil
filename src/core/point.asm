; =============================================================================
; src/core/point.asm -- ANVIL_POINT operations
;
; A POINT is two signed 32-bit ints laid out as { i32 x; i32 y; } at
; offsets 0 and 4. The layout exactly matches Win32 POINT so Anvil POINTs
; can be passed directly to GDI without translation.
; =============================================================================

%define ANVIL_IMPL_POINT
%include "platform/win64.inc"
%include "core/types.inc"

section .text

global anvil_point_make
global anvil_point_eq

; -----------------------------------------------------------------------------
; anvil_point_make(i32 x, i32 y) -> i64
;
;   Packs (x, y) into a single 64-bit value with x in the low dword and y
;   in the high dword. This is a convenient way to construct a POINT in a
;   register for callers that want to immediately store it.
;
;   In:  ECX = x (signed), EDX = y (signed)
;   Out: RAX = (y << 32) | (x & 0xFFFFFFFF)
; -----------------------------------------------------------------------------
anvil_point_make:
        mov     eax, ecx                ; eax = x (zero-extended into rax)
        shl     rdx, 32
        or      rax, rdx
        ret

; -----------------------------------------------------------------------------
; anvil_point_eq(POINT* a, POINT* b) -> bool
;
;   Returns 1 if a->x == b->x && a->y == b->y, else 0.
;
;   In:  RCX = a, RDX = b
;   Out: RAX = 0 or 1
; -----------------------------------------------------------------------------
anvil_point_eq:
        mov     eax, [rcx + ANVIL_POINT_x]
        cmp     eax, [rdx + ANVIL_POINT_x]
        jne     .neq
        mov     eax, [rcx + ANVIL_POINT_y]
        cmp     eax, [rdx + ANVIL_POINT_y]
        jne     .neq
        mov     eax, 1
        ret
.neq:
        xor     eax, eax
        ret
