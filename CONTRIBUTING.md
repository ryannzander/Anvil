# Contributing to Anvil

Thank you for considering a contribution. This document is short on purpose
— most of the project's conventions live in [`docs/`](docs/).

## Quick start

1. Fork the repository and create a topic branch off `main`.
2. Make your change. Keep the patch focused; one logical change per commit.
3. Add or update tests in [`tests/`](tests/).
4. Run `make test` until it passes.
5. Open a pull request describing **what** changed and **why**.

## Required reading

Before sending a patch that touches `.asm` files, please read:

- [`docs/coding-style.md`](docs/coding-style.md) — how to write Anvil
  assembly so that other people can read it.
- [`docs/calling-convention.md`](docs/calling-convention.md) — the Win64
  ABI as Anvil uses it, including register clobber rules.
- [`docs/architecture.md`](docs/architecture.md) — the layer model and
  what is allowed to depend on what.

## Commit messages

We follow a Linux-kernel-flavored style.

```
subsystem: short imperative summary (<= 60 chars)

A longer paragraph that explains the motivation for the change, the
approach taken, and anything subtle a future reader would want to know.
Wrap at 72 columns.

Signed-off-by: Your Name <you@example.com>
```

Acceptable `subsystem:` prefixes include `core`, `window`, `drawing`,
`widgets`, `platform`, `tests`, `docs`, `build`. New subsystems should be
introduced in a separate patch.

## Code of conduct

Be civil. Disagree on technical merit, never on personal grounds. The full
text is in [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md).

## License

By contributing you agree your work will be released under the MIT license
in [`LICENSE`](LICENSE).
