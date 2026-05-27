; =============================================================================
; widgets/widgets.inc -- Anvil widget framework
;
; A widget is a struct whose first field is a pointer to its vtable.
; Vtables are flat function-pointer tables; dispatch is a simple indirect
; call. There is no inheritance machinery beyond "the first N entries of
; a derived vtable match the base vtable" -- exactly the C convention.
; =============================================================================

%ifndef ANVIL_WIDGETS_INC
%define ANVIL_WIDGETS_INC

; -----------------------------------------------------------------------------
; Vtable layout (offset 0 of every widget points here)
;
;   offset  field
;   0       destroy(self)              -> void
;   8       on_event(self, EVENT*)     -> bool (true = handled)
;   16      on_paint(self, HDC)        -> void
;   24      measure(self, w, h, out)   -> void   (Layer 5)
;   32      arrange(self, RECT*)       -> void   (Layer 5)
;   40      name                       -> LPCSTR (for debugging)
;   size    48
; -----------------------------------------------------------------------------
%define ANVIL_VTBL_size             48
%define ANVIL_VTBL_destroy          0
%define ANVIL_VTBL_on_event         8
%define ANVIL_VTBL_on_paint         16
%define ANVIL_VTBL_measure          24
%define ANVIL_VTBL_arrange          32
%define ANVIL_VTBL_name             40

; -----------------------------------------------------------------------------
; ANVIL_WIDGET -- base widget layout. Concrete widgets embed this at the
; head of their own struct.
;
;   offset  size  field
;   0       8     vtbl
;   8       16    bounds       (RECT)
;   24      8     parent       (ANVIL_WIDGET* or NULL)
;   32      8     next         (intrusive sibling list)
;   40      8     first_child
;   48      4     flags        bit 0: visible, bit 1: focused
;   52      4     pad
;   56      8     user_data
;   size    64
; -----------------------------------------------------------------------------
%define ANVIL_WIDGET_size           64
%define ANVIL_WIDGET_vtbl           0
%define ANVIL_WIDGET_bounds         8
%define ANVIL_WIDGET_parent         24
%define ANVIL_WIDGET_next           32
%define ANVIL_WIDGET_first_child    40
%define ANVIL_WIDGET_flags          48
%define ANVIL_WIDGET_user_data      56

%define ANVIL_WFLAG_VISIBLE         0x01
%define ANVIL_WFLAG_FOCUSED         0x02
%define ANVIL_WFLAG_DISABLED        0x04

; -----------------------------------------------------------------------------
; ANVIL_BUTTON -- derives from ANVIL_WIDGET (must match layout for the
; first ANVIL_WIDGET_size bytes).
;
;   offset  size  field
;   0       64    widget
;   64      8     label_ptr     (LPCSTR, NUL terminated)
;   72      8     on_click      (void (*)(BUTTON*))
;   80      4     bg_color      Anvil COLOR
;   84      4     fg_color      Anvil COLOR
;   88      4     state         0=normal 1=hover 2=pressed
;   92      4     pad
;   size    96
; -----------------------------------------------------------------------------
%define ANVIL_BUTTON_size           96
%define ANVIL_BUTTON_label_ptr      64
%define ANVIL_BUTTON_on_click       72
%define ANVIL_BUTTON_bg_color       80
%define ANVIL_BUTTON_fg_color       84
%define ANVIL_BUTTON_state          88

%define ANVIL_BUTTON_STATE_NORMAL   0
%define ANVIL_BUTTON_STATE_HOVER    1
%define ANVIL_BUTTON_STATE_PRESSED  2

; -----------------------------------------------------------------------------
; ANVIL_LABEL -- text-only static widget.
;
;   offset  size  field
;   0       64    widget
;   64      8     text_ptr      (LPCSTR)
;   72      4     color
;   76      4     align         0=left 1=center 2=right
;   size    80
; -----------------------------------------------------------------------------
%define ANVIL_LABEL_size            80
%define ANVIL_LABEL_text_ptr        64
%define ANVIL_LABEL_color           72
%define ANVIL_LABEL_align           76

%define ANVIL_ALIGN_LEFT            0
%define ANVIL_ALIGN_CENTER          1
%define ANVIL_ALIGN_RIGHT           2

; Public routines
extern  anvil_widget_init           ; (WIDGET*, VTBL*) -> void
extern  anvil_widget_set_bounds     ; (WIDGET*, RECT*) -> void
extern  anvil_widget_add_child      ; (WIDGET* parent, WIDGET* child) -> void
extern  anvil_widget_dispatch_event ; (WIDGET*, EVENT*) -> bool
extern  anvil_widget_paint          ; (WIDGET*, HDC) -> void

extern  anvil_button_create         ; (LPCSTR label, void(*on_click)(BUTTON*)) -> BUTTON*
extern  anvil_button_destroy        ; (BUTTON*) -> void
extern  anvil_button_set_label      ; (BUTTON*, LPCSTR) -> void
extern  anvil_button_set_colors     ; (BUTTON*, u32 bg, u32 fg) -> void

extern  anvil_label_create          ; (LPCSTR text, u32 color) -> LABEL*
extern  anvil_label_destroy         ; (LABEL*) -> void
extern  anvil_label_set_text        ; (LABEL*, LPCSTR) -> void
extern  anvil_label_set_align       ; (LABEL*, u32 align) -> void

%endif  ; ANVIL_WIDGETS_INC
