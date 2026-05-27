; =============================================================================
; core/types.inc -- Anvil core data types
;
; These structures cross module boundaries; their layout is part of the
; public ABI. Changing a field offset is a breaking change.
; =============================================================================

%ifndef ANVIL_CORE_TYPES_INC
%define ANVIL_CORE_TYPES_INC

; -----------------------------------------------------------------------------
; POINT -- two signed 32-bit coordinates. Matches Win32 POINT exactly so
; that Anvil POINTs can be passed directly to GDI.
; -----------------------------------------------------------------------------
%define ANVIL_POINT_size            8
%define ANVIL_POINT_x               0
%define ANVIL_POINT_y               4

; -----------------------------------------------------------------------------
; RECT -- left/top/right/bottom, signed 32-bit. Matches Win32 RECT.
; The rectangle is half-open: a pixel at (x, y) is inside iff
;   left <= x < right  &&  top <= y < bottom
; -----------------------------------------------------------------------------
%define ANVIL_RECT_size             16
%define ANVIL_RECT_left             0
%define ANVIL_RECT_top              4
%define ANVIL_RECT_right            8
%define ANVIL_RECT_bottom           12

; -----------------------------------------------------------------------------
; COLOR -- a 32-bit RGBA value laid out as 0xAABBGGRR.
;
; The byte order matches Win32 COLORREF (0x00BBGGRR) extended with an
; alpha byte in the high position. Anvil routines that talk to GDI mask
; the alpha away before passing to RGB()-style APIs; Anvil's own drawing
; primitives honour alpha where they can.
; -----------------------------------------------------------------------------
%define ANVIL_COLOR_size            4

%ifndef ANVIL_IMPL_POINT
extern  anvil_point_make            ; (i32 x, i32 y) -> packed i64 {x,y}
extern  anvil_point_eq              ; (POINT* a, POINT* b) -> bool
%endif

%ifndef ANVIL_IMPL_RECT
extern  anvil_rect_make             ; (RECT* out, i32 l, i32 t, i32 r, i32 b) -> void
extern  anvil_rect_width            ; (RECT* r) -> i32
extern  anvil_rect_height           ; (RECT* r) -> i32
extern  anvil_rect_contains_point   ; (RECT* r, i32 x, i32 y) -> bool
extern  anvil_rect_intersects       ; (RECT* a, RECT* b) -> bool
extern  anvil_rect_inflate          ; (RECT* r, i32 dx, i32 dy) -> void
extern  anvil_rect_offset           ; (RECT* r, i32 dx, i32 dy) -> void
extern  anvil_rect_is_empty         ; (RECT* r) -> bool
extern  anvil_rect_equal            ; (RECT* a, RECT* b) -> bool
%endif

%ifndef ANVIL_IMPL_COLOR
extern  anvil_color_rgb             ; (u8 r, u8 g, u8 b) -> u32 packed
extern  anvil_color_rgba            ; (u8 r, u8 g, u8 b, u8 a) -> u32 packed
extern  anvil_color_to_colorref     ; (u32 color) -> COLORREF
extern  anvil_color_red             ; (u32 color) -> u8
extern  anvil_color_green           ; (u32 color) -> u8
extern  anvil_color_blue            ; (u32 color) -> u8
extern  anvil_color_alpha           ; (u32 color) -> u8

extern  g_anvil_color_black
extern  g_anvil_color_white
extern  g_anvil_color_red
extern  g_anvil_color_green
extern  g_anvil_color_blue
extern  g_anvil_color_gray
%endif

%endif  ; ANVIL_CORE_TYPES_INC
