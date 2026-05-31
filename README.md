# Anvil

> A GUI library written from scratch in x86-64 assembly.

Anvil is a small, structured GUI toolkit implemented entirely in NASM
assembly for Windows x64. It speaks directly to the Win32 API and exposes a
layered, well-documented set of primitive points, rectangles, colors,
windows, an event loop, drawing primitives, and a widget framework.

## Status

Early. The repository is built up commit-by-commit, layer-by-layer. See
[`CHANGELOG.md`](CHANGELOG.md) for what has landed.

## Architecture

Anvil is built as a stack of layers. Each layer only depends on layers
below it.

```
  ┌─────────────────────────────────────┐
  │  Layer 5  Layout / containers       │
  ├─────────────────────────────────────┤
  │  Layer 4  Widgets (Button, Label …) │
  ├─────────────────────────────────────┤
  │  Layer 3  Drawing primitives        │
  ├─────────────────────────────────────┤
  │  Layer 2  Window + event loop       │
  ├─────────────────────────────────────┤
  │  Layer 1  Core types (Point, Rect…) │
  ├─────────────────────────────────────┤
  │  Layer 0  Win32 syscall bindings    │
  └─────────────────────────────────────┘
```

See [`docs/architecture.md`](docs/architecture.md) for the long form.

## Building

You need:

- [NASM](https://nasm.us) 2.15+
- A linker that understands COFF / PE — the project is tested with
  [MinGW-w64](https://www.mingw-w64.org/)'s `gcc` driving `ld`.
- GNU Make (ships with MinGW / MSYS2)

Then:

```sh
make            # build the library and examples
make test       # run the test suite
make examples   # build the example programs
make clean      # remove build artifacts
```

On Windows without `make`, use the PowerShell driver:

```powershell
.\build.ps1
.\build.ps1 -Target test
```

## A taste

The "hello window" example, in roughly fifteen instructions of real code,
opens a titled window and pumps messages until the user closes it. See
[`examples/01_hello_window.asm`](examples/01_hello_window.asm).

## Contributing

Read [`CONTRIBUTING.md`](CONTRIBUTING.md) and the
[coding style guide](docs/coding-style.md) before sending patches. The
calling-convention rules in [`docs/calling-convention.md`](docs/calling-convention.md)
are non-negotiable — every public routine must follow them.

## License

MIT. See [`LICENSE`](LICENSE).
