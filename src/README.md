# aGrUM — C++ Developer Guide

This document provides concise instructions for developers who want to build, test, and contribute to the core
C++ parts of aGrUM.

### Quick links

- Top-level README: `../README.md` (project entrypoint)
- Python bindings: `wrappers/pyAgrum`

### Requisites

- CMake (>= 3.15 recommended)
- A C++20-capable compiler (GCC, Clang or MSVC)
- Python 3.10+ if you plan to build the Python bindings
- Standard build tools (make, ninja)

### Quick build (out-of-source)

```bash
git clone https://gitlab.com/agrumery/aGrUM.git 
cd aGrUM
act {test|lib|install} release aGrUM
```

### Running tests

- C++ unit tests are located in `src/testunits`.
- Use the `act` helper tool for convenience (recommended):

```bash
# from repository root
act test  {releaseΛdebug} aGrUM
```

### Code style and contributions

- Follow the project's style rules documented in the top-level `README.md` (formatting tools).
- Use `clang-format` / `clang-tidy` for formatting and static checks before submitting a merge request.

- Naming conventions (C++)
    - Template parameters: use UPPERCASE identifiers (e.g., `T`, `NODE_TYPE`).
    - Concepts: prefix with `GUM_` (e.g., `GUM_Numeric`).
    - Classes: use PascalCase and start with an uppercase letter (e.g., `BayesianNetwork`).
    - Protected attributes: ends with a single underscore prefix (e.g., `size_`).
    - Private attributes: start and end with an underscore (e.g., `_count_`).

SWIG & bindings notes

- If you change public C++ APIs, update SWIG interface files in `wrappers/` and regenerate the wrappers.
- Run the Python test suite in `wrappers/pyAgrum` after updating wrappers.

Troubleshooting

- Inspect `CMakeError.log` and `CMakeOutput.log` in your build directory for configuration failures.
- For linker issues, verify that the correct compiler runtime (libstdc++ / libc++) is used consistently.
- If encountering SWIG namespace or symbol resolution issues, check the SWIG interface files and wrapper generation
  logs.

Contact & support

- Open an issue on the repository for bugs or questions. Include platform, compiler version, and relevant build logs.

License

- See the top-level `LICENSE.md` for license details.
