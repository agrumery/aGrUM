aGrUM is a C++ library designed for easily building applications using graphical 
models such as Bayesian networks, influence diagrams, decision trees or Markov 
decision processes.

It is written to provide the basic building blocks to perform the following 
tasks :

* graphical models learning/elicitation,
* probabilistic inference with graphical models,
* planification.

For a less technical introduction, checkout 
[the project's homepage](http://agrum.gitlab.io).

# Philosophy & Design

aGrUM initial purpose was to support, sustain and share new frameworks and algorithms from the Graphical Models and Decision team at [LIP6](http://www.lip6.fr): [Christophe Gonzales](https://www.lip6.fr/actualite/personnes-fiche.php?ident=P37&LANG=en), [Pierre-Henri Wuillemin](https://www.lip6.fr/actualite/personnes-fiche.php?ident=P67) and [students and former students](http://agrum.gitlab.io/pages/hall-of-fame.html). aGrUM then took the form of a generic framework designed to ease the emergence and experimentation of new ideas. More and more, as a consequence, it began to fulfill all the requirements for providing a complete library to build applications using Graphical Models, with a strong emphasis on Bayesian Networks. Once the prohect reached this state, the team decided to provide it as an open source contribution for the decision support and data science communities. The last move in that direction has been to port its main code repository at gitlab.

aGrUM main goals include code quality, code reuse and performance: aGrUM is written in modern C++11/17 (cross-platform : gcc>=4.8, clang and visual C++); tests and continuous integration are at the core of aGrUM's development; many classical algorithms have been reimplemented and multi-threaded to speed up learning and inference.

Moreover, aGrUM allows its users to very finely tune learning (choosing priors, score, constraints, algorithms and more), inference (choosing their algorithms but also their components like message passing algorithms, triangulation algorithms, etc.) and modeling (choosing the type of discrete variables, making qualitative and quantitative introspection in the model, etc.). The different frameworks (Bayesian networks, credal networks, FMDP, etc.) are treated in the same way (see [here](http://agrum.gitlab.io/pages/agrum.html) for a more exhaustive list of features).

## Wrappers

The main drawback of such a complex toolbox is of course the long and arduous learning curve associated to its inherent complexities. It is the reason why it has been decided to ship wrappers in easier language with easier APIs together with aGrUM. pyAgrum (for python) is the main wrapper of aGrUM. jAgrum and other experimental wrappers could be shipped soon (if needed or asked).

aGrUM uses [swig](http://www.swig.org/) to build these wrappers.

## Documentation

Documentation is a never-ending story for a library with a complex API. The team tries its best to keep
- [doxygen aGrUM documentation](http://docs.agrum.org/aGrUM/latest/)
- [sphinx pyAgrum documentation](https://pyagrum.readthedocs.io/en/latest/)
- [jupyter notebooks as tutorials](http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/) (pyAgrum)

up to date but maintaining them is a difficult task and we are actively looking for people to help us writing
those documentations and tutorials.

## Dependencies

aGrUM's team tries its best to not introduce external dependencies with aGrUM (understand no external dependencies for the C++ code of aGrUM). All external dependencies are included in aGrUM's source. At the moment the external programs shipped with aGrUM are:
- [nanodbc](https://github.com/lexicalunit/nanodbc)
- [lrs](http://cgm.cs.mcgill.ca/~avis/C/lrs.html)
- [tinyxml](http://www.grinninglizard.com/tinyxml/)
- [CxxTest](http://cxxtest.com/)

# Project Structure

The project's root is composed of the following folders:
- [acttools](/acttools): implementation of ACT, the aGrUM Compiler Tool
- [apps](/apps): Applications using aGrUM or pyAgrum
- [src](/src): aGrUM's C++ source code
- [wrappers](/wrappers): aGrUM's wrappers

In [src](/src) you can find the following folders:
- [agrum](/src/agrum): aGrUM's C++ source code
- [cmake](/src/cmake): CMake files used to compile aGrUM
- [docs](/src/docs): aGrUM's documentation
- [run](/src/run): minmial code to run aGrUM
- [testunits](/src/testunits): aGrUM's tests

In [wrappers](/wrappers) you can find the following folders:
- [swig](/wrappers/swig): generic files declaring the part of aGrUM's API that will be wrapped.
- [pyAgrum](/wrapper/pyAgrum): pyAgrum files (including proper tests and sphinx documentation)
- [jAgrum](/wrappers/jAgrum): experimental java wrapper

# Building

It is strongly recommended to use ACT to build aGrUM. Act requires Python (2.7 or >=3.5 )
and you will need, depending of your platform, the following tools:
- Linux:
    - Python
    - g++
    - CMake
- MacOS:
    - Python
    - xCode
    - CMake
- Microsoft Windows:
    - Python
    - Microsoft Visual 2015 Comunity Edition
    - CMake

If you chose not to use ACT, you can directly use CMake to build aGrUM, but you will need
some tweaking to run tests or use specific compilation options.

## act: aGrUM Compilation Tool

To get the full list of `act` commands, you can run `act --help`. `act` uses three 
agruments: a `target`, a `version` and an `action`. Values for each argument do 
not intersect, so order is not important.

The `target` argument tells ACT what to build: `aGrUM`, `pyAgrum` or `jAgrum`.

The `version` argument tells ACT to build either in `release` or `debug`.

The `action` argument tells ACT what to do one the build is finished. Some actions 
are only available for one target. For example the `wheel` action only works with
the `pyAgrum` target and builds the pyAgrum Python wheel.

Common actions to all targets are:
- `install`: installs the target, use option `-d` to set installation path
- `uninstall`: uninstalls the target
- `test`: execute tests for the given target
- `lib`: compiles the target 
- `doc`: builds the target's documentation

Actions `install`, `uninstall` and `test` are self explanatory (we will cover
options later). Action `lib` will just compile the target.

Some actions are not related to any target:
- `clean`: cleans the build folder
- `autoindent`: applies clang formatting to all C++ source code
- `show`: shows current values for each options of act

`act` caches the last command, so you can simply execute `act` to rerun the previous
command.

One action only applies to the target `pyAgrum`:
- wheel: builds a PIP wheel, use option `-d` to define the wheels localisation

## Building aGrUM
To build aGrUM, the following command can be used: `act test release aGrUM`.

Testing your build IS recommended, but it running all tests is long so use action
`lib` to not run any tests or use option `-m` or `-t` to reduce the number of test.

## Building pyAgrum
To build pyAgrum, the following command can be used: `act test release pyAgrum`. 

As for aGrUM, testing your build IS recommended. You can use action `lib` to not
run any tests or use option `-t quick` to not execute notebooks tests (which are 
quite long).

### Building Wheels
To build a wheel of pyAgrum use the following command: `act wheel release pyAgrum -d $(pwd)`
to copy the wheel in your current folder.

Building wheels do not use CMake to compile aGrUM and pyAgrum, so the compile can be
quite longer on some systems.

### Building Conda Packages
Conda package are automatically build [here](https://github.com/conda-forge/pyagrum-feedstock).

## Building with MVSC
To build with MVSC you will need to use the same commands as stated above with 
the option `--mvsc` for 64 bits builds or `--mvsc32` for 32 bits builds.

# Contributing
To contribute to aGrUM, you should fork the project and proceed with a merge request
one your contribution is done.

Please note that there is a contribution policy to sign and email us before we
can accept any contribution. You can find the details for upload your contribution 
agreement [here](https://gitlab.com/agrumery/aGrUM/blob/master/CONTRIBUTING.md).

# Branches
We use the following convention for branch names:
- `documentation/*` for documentation branches
- `internal/*` for any utility development (CI, act, etc..)
- `bug/*` for debugging
- `feature/*` for implementing new features
- `backport/*` for maintaining specific tags

# Continuous Integration
Each commits are tested on Ubuntu 17.04, Mac OS El Capitan and Windows 10. The build
process as follow:
1. Builds aGrUM on all platforms
2. Builds pyAgrum on all platforms
3. Tests aGrUM on all platforms
4. Tests pyAgrum on all platforms

We are continuously improving on automatic builds, so expect this to change on a
regularly basis.

# Testing

## Testing aGrUM
Testing aGrUM is done using action `test` on target `aGrUM`. Important options are:
- `-m MODULES`: executes tests for modules specified by `MODULES`
- `-t TETS`: executes tests for tests suites specified by `TESTS`

The `-t` option supersedes `-m` options, but act will raise an error if you ask
for test suites not in the modules list set by `-m`.

Test files are located in [src/testunits](/src/testunits). Test suites are placed
in there respective module folder:
- [module_BASE](/src/testunits/module_BASE) is used to test modules core, graph,
multidim, variables
- module_XXXX is used to test module agrum/XXX

## Testing pyAgrum
Testing pyAgrum is done using action `test` on target `pyAgrum`. It is not possible
to test a single test suite for pyAgrum, you can however run tests including
notebooks with option `-t all` or only tests with `-t quick`.

Notebooks are not written with the intent to serve as tests, but we find it 
useful to make our tests fails when a given cell in a notebooks fails to keep
them up-to-date.

# Bibliography
See [aGrUM's reference](agrum.gitlab.io/pages/reference.html) and
