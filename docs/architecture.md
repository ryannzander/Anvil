# Anvil Architecture

This document is the long-form view of how Anvil is organized. It is meant
to be read once, on entry to the project, and then referred to as needed.

## Goals

1. **Small, layered, comprehensible.** A reader should be able to start at
   Layer 0 and walk upward without ever needing to peek at a higher layer.
2. **No magic.** Every public routine has an explicit calling convention,
   an explicit failure mode, and a test.
3. **Self-contained.** The library does not link against the C runtime. It
   talks directly to Win32 imports (`kernel32.dll`, `user32.dll`,
   `gdi32.dll`).

## The layer model

```
  +-------------------------------------------------+
  |  Layer 5  Layout / containers                   |
  |           - Stack, Grid containers              |
  |           - measure/arrange protocol            |
  +-------------------------------------------------+
  |  Layer 4  Widgets                               |
  |           - Widget vtable                       |
  |           - Button, Label                       |
  +-------------------------------------------------+
  |  Layer 3  Drawing                               |
  |           - Pen, Brush                          |
  |           - Lines, rectangles, text             |
  +-------------------------------------------------+
  |  Layer 2  Window + event loop                   |
  |           - WindowClass registration            |
  |           - WndProc dispatch                    |
  |           - GetMessage / DispatchMessage pump   |
  +-------------------------------------------------+
  |  Layer 1  Core types                            |
  |           - POINT, RECT, COLOR, fixed strings   |
  |           - Tiny arena allocator                |
  +-------------------------------------------------+
  |  Layer 0  Platform / Win32 bindings             |
  |           - extern declarations                 |
  |           - thin wrappers (shadow-space safe)   |
  +-------------------------------------------------+
```

**Dependency rule:** Layer N may only `%include` headers from Layers `0..N`.
Patches that violate this rule should be rejected in review.

## Why these layers?

- **Layer 0** is the *only* place that knows the names of Win32 imports.
  Everything else calls into Anvil routines. This means a port (to Linux
  + Xlib, say) replaces Layer 0 and Layer 2 and nothing else.
- **Layer 1** holds plain-old-data structures with stable, documented
  offsets. These structures cross module boundaries; their layout is part
  of the public ABI.
- **Layer 2** owns the message loop and the central WndProc. Widgets do
  not see Win32 messages directly; they see events translated by Layer 2.
- **Layer 3** is pure rendering. It takes a device context (HDC) handle
  and shapes; it does not know what a widget is.
- **Layer 4** introduces the widget vtable — the first place in the
  library where dispatch is dynamic.
- **Layer 5** composes widgets. It is the only layer allowed to touch a
  widget's geometry fields.

## Memory model

Anvil has no heap allocator of its own beyond a small bump-style arena in
[`src/core/memory.asm`](../src/core/memory.asm). For non-arena allocations
the library calls `HeapAlloc` on the process heap, returned by
`GetProcessHeap`. Every allocation function in Anvil has a matching
deallocation function; ownership is documented in the per-routine header.

## Threading

Anvil is **not** thread-safe. The event loop runs on the thread that called
`anvil_run`. Widgets and windows must be created and torn down on that
thread. A future revision may relax this for the drawing layer.

## Error handling

There are no exceptions and no errno-style globals. Every routine that can
fail returns its primary result in `RAX` with a sentinel value (usually 0
or -1) on failure, and sets the per-thread last-error slot in
`g_anvil_last_error`. See [`docs/api/errors.md`](api/errors.md).
