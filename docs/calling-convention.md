# Anvil Calling Convention

Anvil follows the **Microsoft x64 calling convention** ("Win64 ABI") for
every public routine, exactly as documented by Microsoft and as expected
by Win32 imports. This document is the canonical reference for Anvil
contributors; if you remember nothing else, remember this page.

## Argument passing

| Position | Register      | Float / SIMD |
|----------|---------------|--------------|
| 1st      | `RCX`         | `XMM0`       |
| 2nd      | `RDX`         | `XMM1`       |
| 3rd      | `R8`          | `XMM2`       |
| 4th      | `R9`          | `XMM3`       |
| 5th+     | stack         | stack        |

Arguments are passed in their *positional* register regardless of type:
the float vs. integer choice is per-argument, not per-call. Mixed-type
calls (e.g. `int, float, int`) use `RCX, XMM1, R8` — the slots are
indexed by argument position.

## Return values

- Integer / pointer in `RAX`.
- Float / SIMD in `XMM0`.
- Larger-than-register structs: the caller passes a hidden first argument
  in `RCX` pointing to the return slot, and all other arguments shift right
  by one. Anvil avoids this pattern in public APIs.

## The shadow space and stack alignment

Every `CALL` instruction must be issued with `RSP` 16-byte-aligned **after**
the call's `PUSH` of the return address — equivalently, `RSP` must be
8-aligned-but-not-16-aligned at the moment of `CALL`.

The caller must also reserve **32 bytes of shadow space** below `RSP` for
the callee to spill its first four register arguments. The callee may
freely overwrite that 32 bytes. Anvil routines therefore always allocate
at least:

```nasm
    sub     rsp, 32 + <local space>     ; shadow + locals
    ...
    add     rsp, 32 + <local space>
    ret
```

If a routine calls into another routine with more than four arguments,
arguments 5+ live in 8-byte slots starting at `[rsp + 32]`. The shadow
space comes first.

## Volatile vs. non-volatile registers

| Volatile (caller-saved)      | Non-volatile (callee-saved)        |
|------------------------------|------------------------------------|
| `RAX RCX RDX R8 R9 R10 R11`  | `RBX RBP RDI RSI R12 R13 R14 R15`  |
| `XMM0`–`XMM5`                | `XMM6`–`XMM15`                     |

Anvil **must** preserve every non-volatile register it touches by saving
it on entry and restoring it on exit. The reverse is also true: never
assume a volatile register survives a call.

`RSP` is special — it is non-volatile but it is *also* the stack pointer.
Routines that adjust `RSP` must restore it exactly before returning.

## Direction flag

The `DF` flag must be cleared (`CLD`) on entry to and exit from every
public routine. String instructions assume `DF=0`.

## Frame pointer (`RBP`)

Anvil routines may use `RBP` as a general non-volatile register or as a
frame pointer; either is fine, but the choice should be consistent within
a file. Frame pointers help debuggers but are not required by the ABI.

## Anvil-specific conventions

On top of the Microsoft ABI, Anvil adds the following.

1. **Error returns.** Functions that can fail return their primary result
   in `RAX` and store an error code in `[rel g_anvil_last_error]` on
   failure. Sentinel values are documented per-routine but follow the
   pattern:
   - Pointer-returning functions return `0` (null) on failure.
   - Handle-returning functions return `-1` (`0xFFFFFFFFFFFFFFFF`) on
     failure.
   - Status-returning functions return `0` on success and a negative
     error code on failure.
2. **Boolean returns.** When a routine returns a boolean, it returns the
   full 64-bit value `0` (false) or `1` (true) — never the C convention
   of any-non-zero.
3. **Out parameters.** Pointer-typed out parameters appear *after* all
   in parameters in the argument list.
4. **No hidden globals.** A public routine reads no global state that the
   caller has not been told about in its docs. Static lookup tables that
   are part of the routine's implementation are fine.

## Worked example

```nasm
; rect_inflate_inplace(rect *r, i32 dx, i32 dy) -> void
;   In:  RCX = pointer to RECT
;        EDX = dx     (signed)
;        R8D = dy     (signed)
;   Out: -
;   Clobbers: RAX
global rect_inflate_inplace
rect_inflate_inplace:
    sub     rsp, 32                 ; shadow space (none called, but
                                    ;   keeps RSP aligned for tooling)
    mov     eax, [rcx + 0]          ; left
    sub     eax, edx
    mov     [rcx + 0], eax
    mov     eax, [rcx + 4]          ; top
    sub     eax, r8d
    mov     [rcx + 4], eax
    mov     eax, [rcx + 8]          ; right
    add     eax, edx
    mov     [rcx + 8], eax
    mov     eax, [rcx + 12]         ; bottom
    add     eax, r8d
    mov     [rcx + 12], eax
    add     rsp, 32
    ret
```
