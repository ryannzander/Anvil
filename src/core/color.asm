; =============================================================================
; src/core/color.asm -- ANVIL_COLOR helpers
;
; Anvil packs colors as 32-bit u32 values in 0xAABBGGRR order. The low
; 24 bits exactly match Win32 COLORREF (0x00BBGGRR), so converting to
; COLORREF is a mask.
;
; Predefined COLOR values live in .rodata.
; =============================================================================

%define ANVIL_IMPL_COLOR
%include "platform/win64.inc"
%include "core/types.inc"

section .rodata
        align 4
global g_anvil_color_black
global g_anvil_color_white
global g_anvil_color_red
global g_anvil_color_green
global g_anvil_color_blue
global g_anvil_color_gray

g_anvil_color_black:    dd 0xFF000000
g_anvil_color_white:    dd 0xFFFFFFFF
g_anvil_color_red:      dd 0xFF0000FF       ; A=FF, B=00, G=00, R=FF
g_anvil_color_green:    dd 0xFF00FF00       ; A=FF, B=00, G=FF, R=00
g_anvil_color_blue:     dd 0xFFFF0000       ; A=FF, B=FF, G=00, R=00
g_anvil_color_gray:     dd 0xFF808080

section .text

global anvil_color_rgb
global anvil_color_rgba
global anvil_color_to_colorref
global anvil_color_red
global anvil_color_green
global anvil_color_blue
global anvil_color_alpha

; -----------------------------------------------------------------------------
; anvil_color_rgb(u8 r, u8 g, u8 b) -> u32
;
;   Builds 0xFF_BB_GG_RR. Implicit alpha = 0xFF (opaque).
;
;   In:  CL = r, DL = g, R8B = b
;   Out: EAX = packed color
; -----------------------------------------------------------------------------
anvil_color_rgb:
        movzx   eax, cl                 ; R
        movzx   edx, dl                 ; G
        shl     edx, 8
        or      eax, edx
        movzx   edx, r8b                ; B
        shl     edx, 16
        or      eax, edx
        or      eax, 0xFF000000         ; A = FF
        ret

; -----------------------------------------------------------------------------
; anvil_color_rgba(u8 r, u8 g, u8 b, u8 a) -> u32
;
;   Builds 0xAA_BB_GG_RR.
;
;   In:  CL = r, DL = g, R8B = b, R9B = a
;   Out: EAX = packed color
; -----------------------------------------------------------------------------
anvil_color_rgba:
        movzx   eax, cl
        movzx   edx, dl
        shl     edx, 8
        or      eax, edx
        movzx   edx, r8b
        shl     edx, 16
        or      eax, edx
        movzx   edx, r9b
        shl     edx, 24
        or      eax, edx
        ret

; -----------------------------------------------------------------------------
; anvil_color_to_colorref(u32 color) -> COLORREF
;
;   Strips alpha to produce a Win32 COLORREF (0x00BBGGRR).
;
;   In:  ECX = color
;   Out: EAX = COLORREF
; -----------------------------------------------------------------------------
anvil_color_to_colorref:
        mov     eax, ecx
        and     eax, 0x00FFFFFF
        ret

; -----------------------------------------------------------------------------
; Channel extractors. Each reads one byte at a fixed shift.
; -----------------------------------------------------------------------------
anvil_color_red:
        movzx   eax, cl
        ret

anvil_color_green:
        mov     eax, ecx
        shr     eax, 8
        and     eax, 0xFF
        ret

anvil_color_blue:
        mov     eax, ecx
        shr     eax, 16
        and     eax, 0xFF
        ret

anvil_color_alpha:
        mov     eax, ecx
        shr     eax, 24
        and     eax, 0xFF
        ret
