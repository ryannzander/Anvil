; =============================================================================
; platform/win64.inc -- Win32 import declarations and ABI helpers
;
; This file is the *only* place in Anvil that names Win32 imports. Higher
; layers must go through one of the anvil_* wrappers in src/platform/.
;
; Conventions:
;   - All Win32 functions are declared with the 'A' (ANSI) suffix. Anvil
;     does not use UTF-16 internally; UTF-8 -> ANSI conversion happens at
;     the boundary if it happens at all.
;   - Every extern is annotated with its arity for sanity.
; =============================================================================

%ifndef ANVIL_PLATFORM_WIN64_INC
%define ANVIL_PLATFORM_WIN64_INC

; ---- kernel32 ---------------------------------------------------------------
extern  GetModuleHandleA        ; (LPCSTR lpModuleName) -> HMODULE
extern  GetProcessHeap          ; () -> HANDLE
extern  HeapAlloc               ; (HANDLE, DWORD, SIZE_T) -> LPVOID
extern  HeapFree                ; (HANDLE, DWORD, LPVOID) -> BOOL
extern  ExitProcess             ; (UINT uExitCode) -> noreturn
extern  GetLastError            ; () -> DWORD
extern  Sleep                   ; (DWORD dwMilliseconds) -> void

; ---- user32 -----------------------------------------------------------------
extern  RegisterClassExA        ; (const WNDCLASSEXA*) -> ATOM
extern  CreateWindowExA         ; (DWORD, LPCSTR, LPCSTR, DWORD,
                                ;  int, int, int, int,
                                ;  HWND, HMENU, HINSTANCE, LPVOID) -> HWND
extern  DefWindowProcA          ; (HWND, UINT, WPARAM, LPARAM) -> LRESULT
extern  ShowWindow              ; (HWND, int nCmdShow) -> BOOL
extern  UpdateWindow            ; (HWND) -> BOOL
extern  DestroyWindow           ; (HWND) -> BOOL
extern  GetMessageA             ; (LPMSG, HWND, UINT, UINT) -> BOOL
extern  PeekMessageA            ; (LPMSG, HWND, UINT, UINT, UINT) -> BOOL
extern  TranslateMessage        ; (const MSG*) -> BOOL
extern  DispatchMessageA        ; (const MSG*) -> LRESULT
extern  PostQuitMessage         ; (int) -> void
extern  PostMessageA            ; (HWND, UINT, WPARAM, LPARAM) -> BOOL
extern  LoadCursorA             ; (HINSTANCE, LPCSTR) -> HCURSOR
extern  LoadIconA               ; (HINSTANCE, LPCSTR) -> HICON
extern  BeginPaint              ; (HWND, LPPAINTSTRUCT) -> HDC
extern  EndPaint                ; (HWND, const PAINTSTRUCT*) -> BOOL
extern  InvalidateRect          ; (HWND, const RECT*, BOOL) -> BOOL
extern  GetClientRect           ; (HWND, LPRECT) -> BOOL
extern  MessageBoxA             ; (HWND, LPCSTR, LPCSTR, UINT) -> int
extern  SetWindowTextA          ; (HWND, LPCSTR) -> BOOL
extern  GetWindowTextA          ; (HWND, LPSTR, int) -> int
extern  SetWindowLongPtrA       ; (HWND, int, LONG_PTR) -> LONG_PTR
extern  GetWindowLongPtrA       ; (HWND, int) -> LONG_PTR
extern  FillRect                ; (HDC, const RECT*, HBRUSH) -> int

; ---- gdi32 ------------------------------------------------------------------
extern  CreatePen               ; (int, int, COLORREF) -> HPEN
extern  CreateSolidBrush        ; (COLORREF) -> HBRUSH
extern  DeleteObject            ; (HGDIOBJ) -> BOOL
extern  SelectObject            ; (HDC, HGDIOBJ) -> HGDIOBJ
extern  MoveToEx                ; (HDC, int, int, LPPOINT) -> BOOL
extern  LineTo                  ; (HDC, int, int) -> BOOL
extern  Rectangle               ; (HDC, int, int, int, int) -> BOOL
extern  Ellipse                 ; (HDC, int, int, int, int) -> BOOL
extern  TextOutA                ; (HDC, int, int, LPCSTR, int) -> BOOL
extern  SetTextColor            ; (HDC, COLORREF) -> COLORREF
extern  SetBkMode               ; (HDC, int) -> int
extern  GetStockObject          ; (int) -> HGDIOBJ

; -----------------------------------------------------------------------------
; Win32 constant grab-bag
;
; Symbols are named to match the Microsoft SDK so existing documentation
; carries over. Only constants Anvil actually uses appear here; do not
; speculate.
; -----------------------------------------------------------------------------

; HeapAlloc flags
%define HEAP_ZERO_MEMORY                    0x00000008

; Cursors / icons -- IDC_ARROW is intptr 32512 cast to LPCSTR
%define IDC_ARROW                           32512
%define IDI_APPLICATION                     32512

; Window class style bits
%define CS_HREDRAW                          0x0002
%define CS_VREDRAW                          0x0001
%define CS_OWNDC                            0x0020

; Window styles
%define WS_OVERLAPPED                       0x00000000
%define WS_CAPTION                          0x00C00000
%define WS_SYSMENU                          0x00080000
%define WS_THICKFRAME                       0x00040000
%define WS_MINIMIZEBOX                      0x00020000
%define WS_MAXIMIZEBOX                      0x00010000
%define WS_VISIBLE                          0x10000000
%define WS_CHILD                            0x40000000
%define WS_OVERLAPPEDWINDOW                 \
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | \
         WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

; ShowWindow nCmdShow
%define SW_HIDE                             0
%define SW_SHOW                             5
%define SW_SHOWDEFAULT                      10

; CreateWindow default-position sentinel
%define CW_USEDEFAULT                       0x80000000

; Messages
%define WM_NULL                             0x0000
%define WM_CREATE                           0x0001
%define WM_DESTROY                          0x0002
%define WM_SIZE                             0x0005
%define WM_PAINT                            0x000F
%define WM_CLOSE                            0x0010
%define WM_QUIT                             0x0012
%define WM_KEYDOWN                          0x0100
%define WM_KEYUP                            0x0101
%define WM_CHAR                             0x0102
%define WM_MOUSEMOVE                        0x0200
%define WM_LBUTTONDOWN                      0x0201
%define WM_LBUTTONUP                        0x0202
%define WM_RBUTTONDOWN                      0x0204
%define WM_RBUTTONUP                        0x0205
%define WM_COMMAND                          0x0111
%define WM_APP                              0x8000

; Window-class GWL indices used by Anvil
%define GWLP_WNDPROC                        -4
%define GWLP_USERDATA                       -21

; SetBkMode modes
%define TRANSPARENT                         1
%define OPAQUE                              2

; Stock objects
%define WHITE_BRUSH                         0
%define LTGRAY_BRUSH                        1
%define GRAY_BRUSH                          2
%define DKGRAY_BRUSH                        3
%define BLACK_BRUSH                         4
%define NULL_BRUSH                          5
%define DEFAULT_GUI_FONT                    17

; MessageBox flags
%define MB_OK                               0x00000000
%define MB_ICONERROR                        0x00000010
%define MB_ICONINFORMATION                  0x00000040

; Pen styles
%define PS_SOLID                            0
%define PS_DASH                             1
%define PS_DOT                              2
%define PS_NULL                             5

; -----------------------------------------------------------------------------
; Useful sizes and field offsets for Win32 structures
;
; These mirror the SDK definitions and must match exactly. If you change
; one, also update the matching test in tests/test_platform_layout.asm.
; -----------------------------------------------------------------------------

; WNDCLASSEXA -- size = 80 on x64
%define WNDCLASSEXA_size                    80
%define WNDCLASSEXA_cbSize                  0
%define WNDCLASSEXA_style                   4
%define WNDCLASSEXA_lpfnWndProc             8
%define WNDCLASSEXA_cbClsExtra              16
%define WNDCLASSEXA_cbWndExtra              20
%define WNDCLASSEXA_hInstance               24
%define WNDCLASSEXA_hIcon                   32
%define WNDCLASSEXA_hCursor                 40
%define WNDCLASSEXA_hbrBackground           48
%define WNDCLASSEXA_lpszMenuName            56
%define WNDCLASSEXA_lpszClassName           64
%define WNDCLASSEXA_hIconSm                 72

; MSG -- size = 48 on x64 (HWND, message, WPARAM, LPARAM, time, pt.x, pt.y, lPrivate)
%define MSG_size                            48
%define MSG_hwnd                            0
%define MSG_message                         8
%define MSG_wParam                          16
%define MSG_lParam                          24
%define MSG_time                            32
%define MSG_pt_x                            36
%define MSG_pt_y                            40

; PAINTSTRUCT -- size = 72 on x64 (HDC, fErase, rcPaint, fRestore,
;                                  fIncUpdate, rgbReserved[32])
%define PAINTSTRUCT_size                    72
%define PAINTSTRUCT_hdc                     0
%define PAINTSTRUCT_fErase                  8
%define PAINTSTRUCT_rcPaint                 12
%define PAINTSTRUCT_fRestore                28
%define PAINTSTRUCT_fIncUpdate              32

; RECT -- size = 16, four 32-bit signed ints
%define RECT_size                           16
%define RECT_left                           0
%define RECT_top                            4
%define RECT_right                          8
%define RECT_bottom                         12

; POINT -- size = 8
%define POINT_size                          8
%define POINT_x                             0
%define POINT_y                             4

; -----------------------------------------------------------------------------
; ABI helper macros
;
; The Microsoft x64 calling convention requires 32 bytes of shadow space
; below RSP before any CALL, and 16-byte alignment of RSP at the point
; of the CALL instruction. These macros centralise the boilerplate so
; that calling-convention bugs cannot quietly creep into individual
; routines.
;
;   PROLOGUE locals_size
;       Reserves locals_size bytes of locals plus 32 bytes of shadow
;       space, keeping RSP 16-byte aligned. locals_size must be a
;       multiple of 8. Pass 0 if no locals are needed.
;
;   EPILOGUE locals_size
;       Counterpart to PROLOGUE; must be called with the same argument.
; -----------------------------------------------------------------------------

%macro PROLOGUE 1
        sub     rsp, 32 + %1 + ((%1 + 8) & 8)
%endmacro

%macro EPILOGUE 1
        add     rsp, 32 + %1 + ((%1 + 8) & 8)
%endmacro

%endif  ; ANVIL_PLATFORM_WIN64_INC
