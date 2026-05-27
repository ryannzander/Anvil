# Anvil Coding Style

A patch that compiles, passes tests, and follows these rules is in good
shape. A patch that ignores them will be sent back.

## Syntax

- **NASM syntax only.** No MASM-isms (no `PROC`, no `INVOKE`, no implicit
  type promotion).
- **Intel operand order.** Destination first, source second.
- **Lowercase mnemonics and registers.** `mov rax, rcx`, not
  `MOV RAX, RCX`. Directives stay uppercase or lowercase depending on
  NASM's own habit (`%include`, `%macro`, `global`, `extern`, `section`).

## Layout

- Lines are at most 100 columns. 80 is preferred for new code.
- Indent operands with **four spaces** after the mnemonic. Operand columns
  do not need to line up across the file, but within a routine they
  should.
- One instruction per line.
- A blank line separates logical paragraphs inside a routine, *and*
  separates routines.

## Routine header

Every public (`global`) routine must be preceded by a header comment of
the form:

```nasm
; -----------------------------------------------------------------------------
; routine_name(arg1_t arg1, arg2_t arg2) -> return_t
;
;   Brief one-sentence description of what the routine does.
;
;   In:  RCX = arg1 — description
;        RDX = arg2 — description
;   Out: RAX = return — description
;   Clobbers: <volatile registers actually clobbered, beyond the ABI default>
;   Errors: <conditions under which it fails, and the sentinel returned>
; -----------------------------------------------------------------------------
```

The "Clobbers" line lists registers that the caller would not normally
expect to lose (e.g. `XMM0` if your int-only-looking routine uses it).
You do not need to list `RAX`, `RCX`, `RDX`, `R8`, `R9`, `R10`, `R11` —
they are volatile and listed by the ABI.

## Naming

- **Files** are `snake_case.asm`.
- **Public symbols** are `anvil_<subsystem>_<verb>` — for example
  `anvil_rect_inflate`, `anvil_window_create`.
- **Private (file-local) labels** start with `.` when they are inside a
  routine, or are `static_<name>` and not `global`'d otherwise.
- **Structure offsets** are `<STRUCT>_<FIELD>` upper-case constants:
  `RECT_LEFT`, `WIDGET_VTBL`.

## Comments

- Comments use `;`. `# ` is reserved for the assembler (preprocessor lines
  in some flavors); avoid it.
- Comments explain **why**, not what. The instruction `mov rax, rcx` does
  not need the comment `; move rcx into rax`.
- A short prose comment above a non-obvious block is better than a comment
  on every line.

## Macros

- Use macros sparingly. Prefer a real routine if the body is more than
  a handful of instructions, because real routines get tests.
- Macros that are local to a file go at the top of that file. Macros that
  are shared across files live in an include under [`include/`](../include/).
- All macros must be `%macro` / `%endmacro`; never abuse `%define` for
  multi-line code.

## Sections

Anvil files use the standard PE sections:

- `.text` — code.
- `.rodata` — read-only data (string literals, lookup tables).
- `.data` — initialized read-write data.
- `.bss` — zero-initialized read-write data.

A file should put each kind of data in the appropriate section. Mixing
`db`s into `.text` is forbidden.

## Tests

Every file that defines a `global` routine must either have a test file in
[`tests/`](../tests/), or be exempted by a comment in its header. Tests
go through the harness described in
[`tests/README.md`](../tests/README.md).
