# AGENTS Guidelines

This repository follows these guidelines for contributions by AI agents or humans:

## Code Style

### Naming Conventions

- snake_case for code files and directories
- UPPER_SNAKE_CASE for macros

### Includes ordering

#### Header files

- Standard library headers
- Third-party library headers
- Project headers

#### Source files

- Corresponding header file
- Standard library headers
- Third-party library headers
- Project headers

#### Test files

- doctest.h
- Tested file's header
- Standard library headers
- Third-party library headers
- Project headers

### Others

- Trailing return types

## Commits

### Message structure

```text
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Definitions

1. **fix:** a commit of the type `fix` patches a bug (this correlates with **PATCH** in Semantic Versioning).
2. **feat:** a commit of the type `feat` introduces a new feature (this correlates with **MINOR** in Semantic Versioning).
3. **BREAKING CHANGE:** a commit that has a footer `BREAKING CHANGE: <description>`, **or** appends a `!` after the type/scope, introduces a breaking API change (correlating with **MAJOR** in Semantic Versioning). A BREAKING CHANGE can be part of commits of any type.
4. **build** a commit of the type `build` affects the build system or external dependencies (example scopes: gulp, broccoli, npm).
5. **chore:** a commit of the type `chore` is for changes that do not modify src or test files (example scopes: build, ci, docs, style).
6. **ci:** a commit of the type `ci` is for changes to our CI configuration files and scripts (example scopes: Travis, Circle, BrowserStack, SauceLabs).
7. **docs:** a commit of the type `docs` is for documentation only changes.
8. **style:** a commit of the type `style` is for changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc).
9. **refactor:** a commit of the type `refactor` is for code changes that neither fix a bug nor add a feature.
10. **perf:** a commit of the type `perf` is for performance improvements.
11. **test:** a commit of the type `test` is for adding missing tests or correcting existing tests.

```text
feat(parser): add ability to parse arrays
```

### References

- [Conventional Commits](https://www.conventionalcommits.org/)
- [Semantic Versioning](https://semver.org/)
