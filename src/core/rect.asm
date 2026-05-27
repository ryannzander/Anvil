; =============================================================================
; src/core/rect.asm -- ANVIL_RECT operations
;
; A RECT is { i32 left; i32 top; i32 right; i32 bottom; } at offsets
; 0/4/8/12, matching Win32 RECT. Rectangles are half-open: a pixel (x, y)
; is inside iff  left <= x < right && top <= y < bottom.
;
; All routines treat the storage as plain memory; they do not allocate.
; =============================================================================

%define ANVIL_IMPL_RECT
%include "platform/win64.inc"
%include "core/types.inc"

section .text

global anvil_rect_make
global anvil_rect_width
global anvil_rect_height
global anvil_rect_contains_point
global anvil_rect_intersects
global anvil_rect_inflate
global anvil_rect_offset
global anvil_rect_is_empty
global anvil_rect_equal

; -----------------------------------------------------------------------------
; anvil_rect_make(RECT* out, i32 l, i32 t, i32 r, i32 b) -> void
;
;   Initializes the rectangle at 'out' with the four given coordinates.
;   Does not validate that l <= r or t <= b -- "empty" rectangles
;   (l == r or t == b) are legal and represent the zero area.
;
;   In:  RCX = out, EDX = left, R8D = top, R9D = right, [rsp+40] = bottom
;   Out: -
; -----------------------------------------------------------------------------
anvil_rect_make:
        mov     [rcx + ANVIL_RECT_left],   edx
        mov     [rcx + ANVIL_RECT_top],    r8d
        mov     [rcx + ANVIL_RECT_right],  r9d
        mov     eax, [rsp + 40]                 ; 5th arg above shadow space
        mov     [rcx + ANVIL_RECT_bottom], eax
        ret

; -----------------------------------------------------------------------------
; anvil_rect_width(RECT* r) -> i32
;
;   Returns r->right - r->left. May be negative if the rect is inverted.
;
;   In:  RCX = r
;   Out: EAX = width
; -----------------------------------------------------------------------------
anvil_rect_width:
        mov     eax, [rcx + ANVIL_RECT_right]
        sub     eax, [rcx + ANVIL_RECT_left]
        ret

; -----------------------------------------------------------------------------
; anvil_rect_height(RECT* r) -> i32
;
;   Returns r->bottom - r->top.
; -----------------------------------------------------------------------------
anvil_rect_height:
        mov     eax, [rcx + ANVIL_RECT_bottom]
        sub     eax, [rcx + ANVIL_RECT_top]
        ret

; -----------------------------------------------------------------------------
; anvil_rect_is_empty(RECT* r) -> bool
;
;   True iff width <= 0 or height <= 0. Empty rectangles cover no pixels.
;
;   In:  RCX = r
;   Out: RAX = 0 or 1
; -----------------------------------------------------------------------------
anvil_rect_is_empty:
        mov     eax, [rcx + ANVIL_RECT_right]
        cmp     eax, [rcx + ANVIL_RECT_left]
        jle     .yes
        mov     eax, [rcx + ANVIL_RECT_bottom]
        cmp     eax, [rcx + ANVIL_RECT_top]
        jle     .yes
        xor     eax, eax
        ret
.yes:
        mov     eax, 1
        ret

; -----------------------------------------------------------------------------
; anvil_rect_equal(RECT* a, RECT* b) -> bool
;
;   Field-wise equality.
; -----------------------------------------------------------------------------
anvil_rect_equal:
        mov     eax, [rcx + 0]
        cmp     eax, [rdx + 0]
        jne     .neq
        mov     eax, [rcx + 4]
        cmp     eax, [rdx + 4]
        jne     .neq
        mov     eax, [rcx + 8]
        cmp     eax, [rdx + 8]
        jne     .neq
        mov     eax, [rcx + 12]
        cmp     eax, [rdx + 12]
        jne     .neq
        mov     eax, 1
        ret
.neq:
        xor     eax, eax
        ret

; -----------------------------------------------------------------------------
; anvil_rect_contains_point(RECT* r, i32 x, i32 y) -> bool
;
;   Half-open containment: left <= x < right && top <= y < bottom.
;
;   In:  RCX = r, EDX = x, R8D = y
;   Out: RAX = 0 or 1
; -----------------------------------------------------------------------------
anvil_rect_contains_point:
        cmp     edx, [rcx + ANVIL_RECT_left]
        jl      .no
        cmp     edx, [rcx + ANVIL_RECT_right]
        jge     .no
        cmp     r8d, [rcx + ANVIL_RECT_top]
        jl      .no
        cmp     r8d, [rcx + ANVIL_RECT_bottom]
        jge     .no
        mov     eax, 1
        ret
.no:
        xor     eax, eax
        ret

; -----------------------------------------------------------------------------
; anvil_rect_intersects(RECT* a, RECT* b) -> bool
;
;   True iff the two rectangles share at least one pixel. Two adjacent
;   but non-overlapping rectangles (e.g. a->right == b->left) do NOT
;   intersect under half-open semantics.
;
;   The algorithm: they intersect iff none of the four separating-axis
;   conditions hold.
;
;   In:  RCX = a, RDX = b
;   Out: RAX = 0 or 1
; -----------------------------------------------------------------------------
anvil_rect_intersects:
        mov     eax, [rcx + ANVIL_RECT_right]
        cmp     eax, [rdx + ANVIL_RECT_left]
        jle     .no                             ; a.right <= b.left
        mov     eax, [rcx + ANVIL_RECT_left]
        cmp     eax, [rdx + ANVIL_RECT_right]
        jge     .no                             ; a.left  >= b.right
        mov     eax, [rcx + ANVIL_RECT_bottom]
        cmp     eax, [rdx + ANVIL_RECT_top]
        jle     .no
        mov     eax, [rcx + ANVIL_RECT_top]
        cmp     eax, [rdx + ANVIL_RECT_bottom]
        jge     .no
        mov     eax, 1
        ret
.no:
        xor     eax, eax
        ret

; -----------------------------------------------------------------------------
; anvil_rect_inflate(RECT* r, i32 dx, i32 dy) -> void
;
;   Expands the rectangle outwards by dx pixels on the left and right
;   edges, and dy pixels on the top and bottom edges. Negative deltas
;   shrink. Result width = old width + 2*dx; height + 2*dy.
;
;   In:  RCX = r, EDX = dx, R8D = dy
; -----------------------------------------------------------------------------
anvil_rect_inflate:
        mov     eax, [rcx + ANVIL_RECT_left]
        sub     eax, edx
        mov     [rcx + ANVIL_RECT_left], eax

        mov     eax, [rcx + ANVIL_RECT_top]
        sub     eax, r8d
        mov     [rcx + ANVIL_RECT_top], eax

        mov     eax, [rcx + ANVIL_RECT_right]
        add     eax, edx
        mov     [rcx + ANVIL_RECT_right], eax

        mov     eax, [rcx + ANVIL_RECT_bottom]
        add     eax, r8d
        mov     [rcx + ANVIL_RECT_bottom], eax
        ret

; -----------------------------------------------------------------------------
; anvil_rect_offset(RECT* r, i32 dx, i32 dy) -> void
;
;   Translates the rectangle by (dx, dy). Width and height unchanged.
; -----------------------------------------------------------------------------
anvil_rect_offset:
        mov     eax, [rcx + ANVIL_RECT_left]
        add     eax, edx
        mov     [rcx + ANVIL_RECT_left], eax

        mov     eax, [rcx + ANVIL_RECT_right]
        add     eax, edx
        mov     [rcx + ANVIL_RECT_right], eax

        mov     eax, [rcx + ANVIL_RECT_top]
        add     eax, r8d
        mov     [rcx + ANVIL_RECT_top], eax

        mov     eax, [rcx + ANVIL_RECT_bottom]
        add     eax, r8d
        mov     [rcx + ANVIL_RECT_bottom], eax
        ret
