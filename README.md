# aGrUM/pyAgrUM

[![Build Status](https://img.shields.io/badge/build-pending-lightgrey)] [![PyPI](https://img.shields.io/badge/pypi-unknown-lightgrey)] [![Docs](https://img.shields.io/badge/docs-latest-lightgrey)] [![License](https://img.shields.io/badge/license-MIT-lightgrey)] [![Release](https://img.shields.io/badge/release-vX.Y-lightgrey)]

**aGrUM** is a fast and powerful C++ library for developing applications based on graphical models such as Bayesian
networks, influence diagrams, decision trees, GAI networks, and Markov decision processes. It provides fundamental
building blocks for tasks such as:

- Graphical model learning/elicitation
- Probabilistic inference with graphical models
- Planning and decision-making

**pyAgrum** is a Python wrapper for the C++ **aGrUM** library (built using SWIG). It offers a high-level
interface to simplify the creation, modeling, learning, inference, and embedding of Bayesian Networks and
other graphical models. Additionally, it includes extensions for:

- **Scikit-learn-compatible probabilistic classifiers** based on Bayesian networks
- **Causal analysis tools**, including causal networks and do-calculus
- **Dynamic Bayesian networks**
- **Explainability tools** for Bayesian networks

## Table of Contents

- [Quick entrypoints](#quick-entrypoints)
- [Quick Start](#quick-start)
- [Dependencies](#dependencies)
- [Project Structure](#project-structure)
- [Philosophy & Design](#philosophy--design)
- [Wrappers](#wrappers)
- [Building aGrUM](#building-agrum)
- [Contributions](#contributions)
- [Guidelines](#guidelines)
- [Continuous Integration](#continuous-integration)
- [Testing](#testing)
- [Bibliography](#bibliography)

## Quick entrypoints

Choose the path that best matches your goal:

- I want to use `pyAgrum` from Python → see [Quick Start](#quick-start) and the `wrappers/pyAgrum` documentation.
- I want to build, extend or contribute to the C++ core → see [Building aGrUM](#building-agrum) and `/src`.

## Quick Start

### Installation

Install **pyAgrum** via `pip` or `conda`:

```bash
pip install pyagrum
```

```bash
conda install -c conda-forge pyagrum
```

### Learning Resources

- 📚 [Documentation](https://pyagrum.readthedocs.io)
- 🧑‍🏫 [Tutorials & Examples](https://webia.lip6.fr/~phw/aGrUM/docs/last/notebooks/)
- 🎮 [Interactive Examples on Binder](https://mybinder.org/v2/gl/agrumery%2FaGrUM/1.1.0)

---

## Dependencies

**aGrUM** is designed to minimize external dependencies in its C++ codebase. All essential dependencies are included in
the source code. The external libraries currently used by **aGrUM** are:

- [nanodbc](https://github.com/lexicalunit/nanodbc): A lightweight C++ wrapper for ODBC
- [lrs](http://cgm.cs.mcgill.ca/~avis/C/lrs.html): A vertex enumeration program
- [tinyxml](http://www.grinninglizard.com/tinyxml/): A simple and lightweight XML parser
- [doctest](https://github.com/doctest/doctest): A fast single-header C++ testing framework (since version 2.4.0, replaces CxxTest)

### pyAgrum Dependencies

The Python wrapper **pyAgrum** introduces additional dependencies. These are managed separately and specified
in the following files:

- **`requirements.txt`**: Mandatory dependencies
- **`optional_requirements.txt`**: Optional dependencies for advanced features

For most users, installation via `pip` or `conda` will automatically handle these dependencies.

---

## Project Structure

The project is organized as follows:

```bash
/acttools       # aGrUM Compiler Tool (ACT) implementation
/apps           # Application examples using aGrUM or pyAgrum
/src            # aGrUM's C++ source code
/wrappers       # Wrappers for aGrUM (Python, Java, etc.)
```

### Key Directories

- `/src/agrum`: Core C++ library
- `/wrappers/pyAgrum`: pyAgrum files, including tests and Sphinx documentation
- `/src/docs`: aGrUM documentation
- `/src/testunits`: Unit tests

## Philosophy & Design

**aGrUM** was initially developed to support the research of the Graphical Models and Decision team
at [LIP6](http://www.lip6.fr). Over time, it evolved into a comprehensive open-source library to aid both research and
practical applications in decision support and data science.

Key design principles include:

- **Modern C++20** development for cross-platform compatibility (GCC ≥ 8.0, Clang, MSVC)
- Emphasis on **performance** (multi-threaded algorithms for faster learning/inference)
- Support for **fine-grained customization** in learning, inference, and modeling

For more details, visit the [aGrUM feature list](https://agrum.gitlab.io/pages/agrum.html).

## Wrappers

To make **aGrUM** more accessible, various wrappers have been developed, including:

- **pyAgrum**: The primary Python wrapper
- **jAgrum**: An experimental Java wrapper

Wrappers are built using [SWIG](http://www.swig.org/).

## Building aGrUM

The recommended build tool is **ACT** (aGrUM Compilation Tool). It requires Python (≥ 3.9) and platform-specific tools:

- **Linux**: Python, g++, CMake
- **macOS**: Python, clang or g++, CMake
- **Windows**: Python, MSVC 2022, CMake

### ACT Commands

```bash
act [target] [version] [action]
```

- **Targets**: `aGrUM`, `pyAgrum`, `jAgrum`
- **Versions**: `release`, `debug`
- **Actions**: `install`, `uninstall`, `test`, `lib`, `doc`, `wheel`, etc.

Example:

```bash
act test release pyAgrum
```

For more details, run `act --help`.

## Contributions

We welcome contributions! Please fork the repository, make your changes, and submit a merge request.

**Note:** Contributors must sign a [contribution policy](https://gitlab.com/agrumery/aGrUM/blob/master/CONTRIBUTING.md)
before their changes can be merged.

## Guidelines

This section provides practical guidance for contributing to the aGrUM/pyAgrum project.

- Style and standards
    - Primary language: C++ (C++20). Respect the target compatibility (GCC ≥ 8.0, Clang, MSVC).
    - Use the repository formatting and analysis tools (e.g. clang-format / clang-tidy for C++, ruff for Python).
    - Keep the copyright header at the top of source files.

- Commits and branches
    - Use imperative, short and descriptive commit messages. Reference the issue/MR when relevant.
    - One feature or bugfix per merge request.

- Pull Requests / Merge Requests
    - Clearly describe the change, its purpose and its impact (breaking changes, public API).
    - Add or update automated tests covering main use cases and edge cases.
    - Make sure the MR passes CI on all supported platforms before requesting a review.

- Tests
    - Place C++ tests in `/src/testunits` and Python/pyAgrum tests in `wrappers/pyAgrum` following the repository
      structure.
    - Run local tests with the ACT tool:

  ```bash
  act test
  act test release pyAgrum
  ```

- Documentation
    - Update documentation (Sphinx for `pyAgrum`, C++ docs in `/src/docs`) and example notebooks when needed.
    - Add reproducible snippets or notebooks for new public features.

- Wrappers and SWIG
    - For any change to the public C++ API, update the SWIG files in `wrappers/` and verify the Python bindings.
    - Run Python tests after regenerating the wrappers.

- API compatibility and versioning
    - Avoid undocumented API breaks. If a breaking change is necessary, document it and bump the version according to
      the project policy.

- Code review
    - Address review comments, add tests when requested, and fix important warnings.

- Support and communication
    - Open an issue to discuss large refactorings or new features before implementation.
    - Use the repository issue tracker for bugs and feature requests.

## Continuous Integration

Every commit is tested (with several compilers) on:

- Ubuntu (`22.04`, `x84-64`)
- macOS (`15.x`, `x86-64` & `ARM`)
- Windows (`11`, `x84-64`)

The CI pipeline builds and tests both **aGrUM** and **pyAgrum** to ensure cross-platform stability.

## Testing

### C++ Tests (aGrUM)

Run C++ tests with the `act test` command:

```bash
# Run all C++ tests
act test release aGrUM

# Run specific modules (BASE, BN, MRF, CN, ID, CM, FMDP)
act test release aGrUM -m BN
act test release aGrUM -m BASE+BN

# Run specific test suites
act test release aGrUM -t BayesNetTestSuite
act test release aGrUM -t BayesNetTestSuite+BNLearnerTestSuite

# Show available modules/tests
act test release aGrUM -m show
act test release aGrUM -t show
```

#### Running Tests Directly (doctest)

After building with `act test release aGrUM`, you can run the test executable directly for faster iteration:

```bash
./gumTest                              # Run all tests
./gumTest --list-test-cases            # List all test cases
./gumTest --test-case="*BayesNet*"     # Run tests matching pattern
./gumTest --test-case="*[BN]*"         # Run all BN module tests
./gumTest --test-case="*[BASE]*"       # Run all BASE module tests
```

### Python Tests (pyAgrum)

```bash
act test release pyAgrum -t quick      # Fast tests only
act test release pyAgrum -t all        # Includes notebook tests
```

## Bibliography

For academic references, see the [aGrUM bibliography](https://agrum.gitlab.io/pages/reference.html).
