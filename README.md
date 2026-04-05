# xct - xmake C++ template

## Usage

```sh
python compose.py --type <type> --name <ProjectName>
```

| `--type`     | Description             |
|--------------|-------------------------|
| `app`        | Executable + library    |
| `headeronly` | Header-only library     |
| `static`     | Static library          |

Dry run — preview output in `build/<ProjectName>/` without modifying the repo:

```sh
python compose.py --dry-run --type app --name MyProject
```

`compose.py` and `template/` are removed automatically on a real run.

## Structure

```
template/
├── core/          # Shared across all types
└── parts/
    ├── app/
    ├── headeronly/
    └── static/
compose.py
```
