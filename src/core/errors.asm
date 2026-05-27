; =============================================================================
; src/core/errors.asm -- Anvil last-error storage
;
; The error model is documented in docs/api/errors.md. This file holds
; two process-global slots and four tiny accessors.
;
; NOTE on thread safety. The slots are currently process-global because
; Anvil is single-threaded by design (see docs/architecture.md). When
; Anvil gains threading support, these will move to TLS via the
; FlsAlloc/FlsGetValue family. The accessor signatures will not change.
; =============================================================================

%define ANVIL_IMPL_ERRORS
%include "platform/win64.inc"
%include "core/errors.inc"

section .bss
        align 8
g_anvil_last_error:             resq 1
g_anvil_platform_error:         resd 1
                                resd 1      ; pad to 8

section .text

global g_anvil_last_error
global g_anvil_platform_error
global anvil_last_error
global anvil_set_last_error
global anvil_platform_error
global anvil_clear_errors
global anvil_set_platform_error

; -----------------------------------------------------------------------------
; anvil_last_error() -> i64
;
;   Returns the most recent Anvil error code (0 if no error).
;
;   In:  -
;   Out: RAX = code
; -----------------------------------------------------------------------------
anvil_last_error:
        mov     rax, [rel g_anvil_last_error]
        ret

; -----------------------------------------------------------------------------
; anvil_set_last_error(i64 code) -> void
;
;   Used internally by Anvil routines. Storing 0 clears the slot.
;
;   In:  RCX = code
;   Out: -
; -----------------------------------------------------------------------------
anvil_set_last_error:
        mov     [rel g_anvil_last_error], rcx
        ret

; -----------------------------------------------------------------------------
; anvil_platform_error() -> u32
;
;   Returns the saved Win32 GetLastError() value associated with the
;   most recent ANVIL_E_PLATFORM result.
; -----------------------------------------------------------------------------
anvil_platform_error:
        xor     eax, eax
        mov     eax, [rel g_anvil_platform_error]
        ret

; -----------------------------------------------------------------------------
; anvil_clear_errors() -> void
;
;   Resets both error slots. Tests call this between cases.
; -----------------------------------------------------------------------------
anvil_clear_errors:
        mov     qword [rel g_anvil_last_error], 0
        mov     dword [rel g_anvil_platform_error], 0
        ret

; -----------------------------------------------------------------------------
; anvil_set_platform_error() -> i64 (= ANVIL_E_PLATFORM)
;
;   Internal helper. Calls GetLastError(), stores it in the platform
;   slot, and sets the Anvil last-error code to ANVIL_E_PLATFORM.
;   Returns ANVIL_E_PLATFORM in RAX for tail-style error returns.
; -----------------------------------------------------------------------------
anvil_set_platform_error:
        PROLOGUE 0
        call    GetLastError
        mov     [rel g_anvil_platform_error], eax
        mov     qword [rel g_anvil_last_error], ANVIL_E_PLATFORM
        mov     rax, ANVIL_E_PLATFORM
        EPILOGUE 0
        ret
