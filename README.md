# Path of Crafting

[![CI](https://github.com/3uclid3/path-of-crafting/actions/workflows/ci.yml/badge.svg)](https://github.com/3uclid3/path-of-crafting/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/3uclid3/path-of-crafting/graph/badge.svg?token=QNYkK1oRhd)](https://codecov.io/gh/3uclid3/path-of-crafting)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/bb1b8f9ec7eb4655ac785e33a0dab7e7)](https://app.codacy.com/gh/3uclid3/path-of-crafting/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

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
