# Anvil Error Codes

Anvil routines that can fail return a sentinel value (see
[calling-convention.md](../calling-convention.md)) and set the per-thread
last-error slot in `g_anvil_last_error`. The caller may then read it with
`anvil_last_error`.

## Values

| Code   | Symbol               | Meaning                                     |
|--------|----------------------|---------------------------------------------|
| 0      | `ANVIL_OK`           | No error.                                   |
| -1     | `ANVIL_E_GENERIC`    | Unspecified error.                          |
| -2     | `ANVIL_E_NOMEM`      | Out of memory.                              |
| -3     | `ANVIL_E_INVALID`    | An argument was invalid.                    |
| -4     | `ANVIL_E_OVERFLOW`   | An arithmetic or buffer overflow occurred.  |
| -5     | `ANVIL_E_PLATFORM`   | A Win32 call failed; consult `GetLastError`.|
| -6     | `ANVIL_E_NOTFOUND`   | A handle / resource was not found.          |
| -7     | `ANVIL_E_STATE`      | The object is in the wrong state.           |

`ANVIL_E_PLATFORM` is special: the Win32 last-error value is preserved in
`g_anvil_platform_error` and can be queried with `anvil_platform_error`.

## Layering rules

- Layer 0 routines may report `ANVIL_E_PLATFORM`.
- Layer 1 routines may report `ANVIL_E_INVALID`, `ANVIL_E_OVERFLOW`,
  `ANVIL_E_NOMEM`.
- Higher layers may pass through any code from below; they should not
  invent new codes without adding them here.
