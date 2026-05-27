; =============================================================================
; anvil.inc -- master include for the Anvil GUI library
;
; A user of Anvil includes this single file. It pulls in the per-subsystem
; headers in the right order so that downstream files do not need to know
; the layering.
;
; The library's calling convention, layering, and naming rules are
; documented under docs/. Read docs/calling-convention.md before writing
; any assembly that calls into here.
; =============================================================================

%ifndef ANVIL_INC
%define ANVIL_INC

; ---- Version ----------------------------------------------------------------
%define ANVIL_VERSION_MAJOR     0
%define ANVIL_VERSION_MINOR     1
%define ANVIL_VERSION_PATCH     0

; ---- Layered headers --------------------------------------------------------
%include "platform/win64.inc"   ; Layer 0  - Win32 import names + ABI helpers
%include "core/errors.inc"      ; Layer 1  - error codes
%include "core/types.inc"       ; Layer 1  - POINT / RECT / COLOR
%include "core/memory.inc"      ; Layer 1  - tiny arena + heap helpers
%include "window/window.inc"    ; Layer 2  - windows + event loop
%include "drawing/drawing.inc"  ; Layer 3  - pen / brush / shapes / text
%include "widgets/widgets.inc"  ; Layer 4  - widget vtable + Button / Label

%endif  ; ANVIL_INC
