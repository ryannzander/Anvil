; =============================================================================
; drawing/drawing.inc -- Anvil drawing primitives
;
; All drawing happens through a device-context handle (HDC) that the
; caller has obtained from Win32 (typically inside a WM_PAINT handler via
; BeginPaint). Anvil owns pen/brush lifetime; the caller passes an
; HDC and high-level shape parameters.
; =============================================================================

%ifndef ANVIL_DRAWING_INC
%define ANVIL_DRAWING_INC

; -----------------------------------------------------------------------------
; ANVIL_PEN  -- thin wrapper around a Win32 HPEN
;   offset  size  field
;   0       8     hpen
;   8       4     style    PS_SOLID etc.
;   12      4     width
;   16      4     color    Anvil COLOR (0xAABBGGRR)
;   20      4     pad
;   size    24
; -----------------------------------------------------------------------------
%define ANVIL_PEN_size              24
%define ANVIL_PEN_hpen              0
%define ANVIL_PEN_style             8
%define ANVIL_PEN_width             12
%define ANVIL_PEN_color             16

; -----------------------------------------------------------------------------
; ANVIL_BRUSH -- thin wrapper around a Win32 HBRUSH (solid only for now)
;   offset  size  field
;   0       8     hbrush
;   8       4     color
;   12      4     pad
;   size    16
; -----------------------------------------------------------------------------
%define ANVIL_BRUSH_size            16
%define ANVIL_BRUSH_hbrush          0
%define ANVIL_BRUSH_color           8

; Public routines
extern  anvil_pen_create            ; (i32 style, i32 width, u32 color, PEN* out) -> i64
extern  anvil_pen_destroy           ; (PEN*) -> void

extern  anvil_brush_create_solid    ; (u32 color, BRUSH* out) -> i64
extern  anvil_brush_destroy         ; (BRUSH*) -> void

extern  anvil_draw_line             ; (HDC, PEN*, i32 x1, i32 y1, i32 x2, i32 y2) -> void
extern  anvil_draw_rect             ; (HDC, PEN*, BRUSH*, RECT*) -> void
extern  anvil_draw_filled_rect      ; (HDC, BRUSH*, RECT*) -> void
extern  anvil_draw_ellipse          ; (HDC, PEN*, BRUSH*, RECT*) -> void
extern  anvil_draw_text             ; (HDC, LPCSTR, i32 len, i32 x, i32 y, u32 color) -> void

%endif  ; ANVIL_DRAWING_INC
