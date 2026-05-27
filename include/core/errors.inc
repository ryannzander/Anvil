; =============================================================================
; core/errors.inc -- Anvil error codes and last-error access
;
; See docs/api/errors.md for the long-form description. Every routine
; that can fail follows the discipline documented there.
; =============================================================================

%ifndef ANVIL_CORE_ERRORS_INC
%define ANVIL_CORE_ERRORS_INC

%define ANVIL_OK                    0
%define ANVIL_E_GENERIC             -1
%define ANVIL_E_NOMEM               -2
%define ANVIL_E_INVALID             -3
%define ANVIL_E_OVERFLOW            -4
%define ANVIL_E_PLATFORM            -5
%define ANVIL_E_NOTFOUND            -6
%define ANVIL_E_STATE               -7

; Public routines exported by src/core/errors.asm
extern  anvil_last_error            ; () -> i64
extern  anvil_set_last_error        ; (i64 code) -> void
extern  anvil_platform_error        ; () -> u32
extern  anvil_clear_errors          ; () -> void

; Globals (private to the errors translation unit; declared here for the
; small number of inlining macros that touch them).
extern  g_anvil_last_error          ; i64
extern  g_anvil_platform_error      ; u32

%endif  ; ANVIL_CORE_ERRORS_INC
