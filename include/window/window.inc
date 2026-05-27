; =============================================================================
; window/window.inc -- Anvil window subsystem
;
; A window in Anvil is a thin shell around a Win32 HWND. The library owns
; a single WndProc that runs for every Anvil window; per-window state is
; stored in the GWLP_USERDATA slot as a pointer to an ANVIL_WINDOW struct.
; =============================================================================

%ifndef ANVIL_WINDOW_INC
%define ANVIL_WINDOW_INC

; -----------------------------------------------------------------------------
; ANVIL_WINDOW layout (size 64)
;
;   offset  size  field
;   0       8     hwnd       Win32 HWND
;   8       8     user_data  arbitrary pointer the caller can stash here
;   16      8     wndproc    optional caller-provided WndProc override
;   24      8     root       root widget (may be NULL)
;   32      16    bounds     last known client RECT
;   48      4     flags      bit 0: visible, bit 1: destroyed
;   52      4     pad
;   56      8     reserved
; -----------------------------------------------------------------------------
%define ANVIL_WINDOW_size           64
%define ANVIL_WINDOW_hwnd           0
%define ANVIL_WINDOW_user_data      8
%define ANVIL_WINDOW_wndproc        16
%define ANVIL_WINDOW_root           24
%define ANVIL_WINDOW_bounds         32
%define ANVIL_WINDOW_flags          48
%define ANVIL_WINDOW_reserved       56

%define ANVIL_WINDOW_FLAG_VISIBLE   0x01
%define ANVIL_WINDOW_FLAG_DESTROYED 0x02

; -----------------------------------------------------------------------------
; ANVIL_EVENT -- translated Win32 message handed to widgets.
;
; The layout is intentionally small. Fields not relevant to a given event
; are zero.
;
;   offset  size  field
;   0       4     type       ANVIL_EV_*
;   4       4     key        keycode (for key events)
;   8       4     x          mouse X / size width
;   12      4     y          mouse Y / size height
;   16      4     buttons    bitmask
;   20      4     mods       modifier bitmask
; -----------------------------------------------------------------------------
%define ANVIL_EVENT_size            24
%define ANVIL_EVENT_type            0
%define ANVIL_EVENT_key             4
%define ANVIL_EVENT_x               8
%define ANVIL_EVENT_y               12
%define ANVIL_EVENT_buttons         16
%define ANVIL_EVENT_mods            20

%define ANVIL_EV_NONE               0
%define ANVIL_EV_CLOSE              1
%define ANVIL_EV_PAINT              2
%define ANVIL_EV_RESIZE             3
%define ANVIL_EV_KEY_DOWN           4
%define ANVIL_EV_KEY_UP             5
%define ANVIL_EV_CHAR               6
%define ANVIL_EV_MOUSE_MOVE         7
%define ANVIL_EV_MOUSE_DOWN         8
%define ANVIL_EV_MOUSE_UP           9

%define ANVIL_BTN_LEFT              0x01
%define ANVIL_BTN_RIGHT             0x02
%define ANVIL_BTN_MIDDLE            0x04

; -----------------------------------------------------------------------------
; Public routines
; -----------------------------------------------------------------------------
%ifndef ANVIL_IMPL_WINDOW
extern  anvil_init                  ; () -> i64 status
extern  anvil_shutdown              ; () -> void

extern  anvil_window_create         ; (LPCSTR title, i32 w, i32 h) -> ANVIL_WINDOW*
extern  anvil_window_destroy        ; (ANVIL_WINDOW*) -> void
extern  anvil_window_show           ; (ANVIL_WINDOW*) -> void
extern  anvil_window_hide           ; (ANVIL_WINDOW*) -> void
extern  anvil_window_set_title      ; (ANVIL_WINDOW*, LPCSTR) -> void
extern  anvil_window_invalidate     ; (ANVIL_WINDOW*) -> void
extern  anvil_window_hwnd           ; (ANVIL_WINDOW*) -> HWND
extern  anvil_window_set_root       ; (ANVIL_WINDOW*, ANVIL_WIDGET*) -> void

extern  anvil_run                   ; () -> i32  exit code
%endif

%endif  ; ANVIL_WINDOW_INC
