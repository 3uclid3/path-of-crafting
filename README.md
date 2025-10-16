# Path of Crafting

[![codecov](https://codecov.io/gh/3uclid3/path-of-crafting/graph/badge.svg?token=QNYkK1oRhd)](https://codecov.io/gh/3uclid3/path-of-crafting)
[![CI](https://github.com/3uclid3/path-of-crafting/actions/workflows/ci.yml/badge.svg)](https://github.com/3uclid3/path-of-crafting/actions/workflows/ci.yml)

A lightweight **Path of Exile item analyzer** for your second monitor. Watches the clipboard, parses copied item text, and displays structured stats, rarity, and affixes in real time. Built for **speed, precision, and zero bloat**.

> ⚠️ **Work in Progress:** core systems are under active development. Expect missing features and frequent refactors.

## Requirements

- [LLVM](https://llvm.org)
- [xmake](https://xmake.io)
- [pre-commit](https://pre-commit.com)

## Build & Run

```bash
xmake f -m release -y
xmake build
xmake run poc
```

## License

[GPL-3.0-or-later](./LICENSE)

## Credits

Developed by a sleep-deprived human who was bored and wanted to code.
