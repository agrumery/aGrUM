# aGrUM/pyAgrum

**aGrUM** is a fast and powerful C++ library for developing applications based on graphical models such as Bayesian networks, influence diagrams, decision trees, GAI networks, and Markov decision processes. It provides fundamental building blocks for tasks such as:

- Graphical model learning/elicitation
- Probabilistic inference with graphical models
- Planning and decision-making

**pyAgrum** is a Python wrapper for the C++ **aGrUM** library (built using SWIG). It offers a high-level interface to simplify the creation, modeling, learning, inference, and embedding of Bayesian Networks and other graphical models. Additionally, it includes extensions for:

- **Scikit-learn-compatible probabilistic classifiers** based on Bayesian networks
- **Causal analysis tools**, including causal networks and do-calculus
- **Dynamic Bayesian networks**
- **Explainability tools** for Bayesian networks

## Quick Start

### Installation

Install **pyAgrum** via `pip` or `conda`:

```bash
pip install pyAgrum
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

**aGrUM** is designed to minimize external dependencies in its C++ codebase. All essential dependencies are included in the source code. The external libraries currently used by **aGrUM** are:

- [nanodbc](https://github.com/lexicalunit/nanodbc): A lightweight C++ wrapper for ODBC
- [lrs](http://cgm.cs.mcgill.ca/~avis/C/lrs.html): A vertex enumeration program
- [tinyxml](http://www.grinninglizard.com/tinyxml/): A simple and lightweight XML parser
- [CxxTest](http://cxxtest.com/): A unit testing framework for C++

### pyAgrum Dependencies

The Python wrapper **pyAgrum** introduces additional dependencies. These are managed separately and specified in the following files:

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

**aGrUM** was initially developed to support the research of the Graphical Models and Decision team at [LIP6](http://www.lip6.fr). Over time, it evolved into a comprehensive open-source library to aid both research and practical applications in decision support and data science.

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

**Note:** Contributors must sign a [contribution policy](https://gitlab.com/agrumery/aGrUM/blob/master/CONTRIBUTING.md) before their changes can be merged.

## Continuous Integration

Every commit is tested (with several compilers) on:

- Ubuntu (`22.04`, `x84-64`)
- macOS (`15.x`, `x86-64` & `ARM`)
- Windows (`11`, `x84-64`)

The CI pipeline builds and tests both **aGrUM** and **pyAgrum** to ensure cross-platform stability.

## Testing

Run tests with the `act test` command:

- **aGrUM**: Test individual modules using `-m MODULES` or specific suites using `-t TESTS`.
- **pyAgrum**: Choose between quick tests (`-t quick`) or comprehensive tests including Jupyter notebooks (`-t all`).

## Bibliography

For academic references, see the [aGrUM bibliography](https://agrum.gitlab.io/pages/reference.html).
