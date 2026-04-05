#!/usr/bin/env python3
"""
compose.py — XCT template composer
Usage: python compose.py --type [app|headeronly|static] --name <ProjectName>
"""

import argparse
import shutil
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Config
# ---------------------------------------------------------------------------

PLACEHOLDER_LOWER = "xct"
PLACEHOLDER_UPPER = "XCT"

VALID_TYPES = ["app", "headeronly", "static"]

TEMPLATE_DIR = Path(__file__).parent / "template"
CORE_DIR     = TEMPLATE_DIR / "core"
PARTS_DIR    = TEMPLATE_DIR / "parts"
# Files/dirs to never touch during text substitution (binary, generated, etc.)
BINARY_SUFFIXES = {
    ".png", ".jpg", ".jpeg", ".gif", ".ico", ".svg",
    ".ttf", ".woff", ".woff2",
    ".zip", ".tar", ".gz", ".7z",
    ".exe", ".dll", ".so", ".a", ".lib",
    ".pdf",
}

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def is_text_file(path: Path) -> bool:
    return path.is_file() and path.suffix.lower() not in BINARY_SUFFIXES


def substitute(text: str, name: str) -> str:
    """Replace xct/XCT placeholders with the project name."""
    text = text.replace(PLACEHOLDER_UPPER, name.upper())
    text = text.replace(PLACEHOLDER_LOWER, name.lower())
    return text


def rename_path(path: Path, name: str) -> Path:
    """Return the path with xct/XCT replaced in the filename."""
    new_name = path.name
    new_name = new_name.replace(PLACEHOLDER_UPPER, name.upper())
    new_name = new_name.replace(PLACEHOLDER_LOWER, name.lower())
    return path.parent / new_name


def copy_tree(src: Path, dst: Path) -> None:
    """Copy src directory contents into dst, merging if dst exists."""
    dst.mkdir(parents=True, exist_ok=True)
    for item in src.rglob("*"):
        rel = item.relative_to(src)
        target = dst / rel
        if item.is_dir():
            target.mkdir(parents=True, exist_ok=True)
        else:
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(item, target)


def substitute_all(root: Path, name: str) -> None:
    """Substitute placeholders in all text file contents under root."""
    for path in root.rglob("*"):
        if is_text_file(path):
            try:
                original = path.read_text(encoding="utf-8")
                updated = substitute(original, name)
                if updated != original:
                    path.write_text(updated, encoding="utf-8")
            except UnicodeDecodeError:
                pass  # skip files that aren't actually utf-8


def rename_all(root: Path, name: str) -> None:
    """Rename files and directories that contain xct/XCT in their names.
    Process deepest paths first so parent renames don't break children."""
    paths = sorted(root.rglob("*"), key=lambda p: len(p.parts), reverse=True)
    for path in paths:
        if not path.exists():
            continue
        new_path = rename_path(path, name)
        if new_path != path:
            path.rename(new_path)


def cleanup(script_path: Path) -> None:
    """Remove template/ dir and this script."""
    template_dir = script_path.parent / "template"
    if template_dir.exists():
        shutil.rmtree(template_dir)
        print(f"  removed  template/")

    print(f"  removed  {script_path.name}")
    script_path.unlink()  # must be last


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compose an XCT project from template parts."
    )
    parser.add_argument(
        "--type", "-t",
        required=True,
        choices=VALID_TYPES,
        metavar="TYPE",
        help=f"Project type: {', '.join(VALID_TYPES)}",
    )
    parser.add_argument(
        "--name", "-n",
        required=True,
        metavar="NAME",
        help="Project name (replaces 'xct'/'XCT' everywhere)",
    )
    parser.add_argument(
        "--dry-run", "-d",
        action="store_true",
        help="Compose into build/<name>/ without touching the repo or self-destructing",
    )
    return parser.parse_args()


def validate(args: argparse.Namespace) -> None:
    errors = []

    if not CORE_DIR.exists():
        errors.append(f"Missing core directory: {CORE_DIR}")

    parts_type_dir = PARTS_DIR / args.type
    if not parts_type_dir.exists():
        errors.append(f"Missing parts directory for type '{args.type}': {parts_type_dir}")

    name = args.name
    if not name.isidentifier():
        errors.append(
            f"Project name '{name}' is not a valid identifier "
            f"(letters, digits, underscores only, cannot start with digit)."
        )

    if errors:
        for e in errors:
            print(f"error: {e}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    args = parse_args()
    validate(args)

    name      = args.name
    proj_type = args.type
    script    = Path(__file__)
    repo_root = script.parent

    parts_type_dir = PARTS_DIR / proj_type

    dry_run = args.dry_run
    out_dir = repo_root / "build" / name if dry_run else repo_root

    if dry_run:
        if out_dir.exists():
            shutil.rmtree(out_dir)
        out_dir.mkdir(parents=True)
        print(f"\n[dry-run] Composing '{name}' ({proj_type}) → build/{name}/\n")
    else:
        print(f"\nComposing '{name}' ({proj_type})\n")

    # 1. Copy core
    dest_label = f"build/{name}/" if dry_run else "./"
    print(f"  copying  template/core  →  {dest_label}")
    copy_tree(CORE_DIR, out_dir)

    # 2. Copy part-specific files (may override core files)
    print(f"  copying  template/parts/{proj_type}  →  {dest_label}")
    copy_tree(parts_type_dir, out_dir)

    # 3. Substitute xct/XCT in all text file contents
    print(f"\n  substituting '{PLACEHOLDER_LOWER}' → '{name.lower()}'")
    print(f"  substituting '{PLACEHOLDER_UPPER}' → '{name.upper()}'")
    substitute_all(out_dir, name)

    # 4. Rename files/dirs containing xct/XCT
    print(f"  renaming paths...")
    rename_all(out_dir, name)

    if dry_run:
        print(f"\n[dry-run] Output in build/{name}/ — repo untouched, script kept.\n")
    else:
        # 5. Self-destruct
        print(f"\n  cleaning up...")
        cleanup(script)
        print(f"\nDone! Project '{name}' is ready.\n")
if __name__ == "__main__":
    main()