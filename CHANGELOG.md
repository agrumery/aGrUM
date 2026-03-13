# aGrUM Changelog

## Changelog for 2.3.2

- pyAgrum
    - Fix sklearn `check_X_y` compatibility with mixed-type DataFrames.

## Changelog for 2.3.1

- aGrUM
    - Fixed a typo in BIFXML export (thanks to Pierre-François Gimenez).
    - Better use of PCH in CMakefiles (thanks to Julien Schueller).
    - o4DGContext :disambiguated log2 namespace.
    - Make -fno-assume-unique-vtables public (thanks to Julien Schueller).
    - better ci for linux.
    - Improved logic for `gum::BNLearner::learnParameters()`.

- pyAgrum
    - `scikit-learn` as a required-dist.
    - New action for `act` : `pipinstall`.
    - Improved documentation and notebooks.
    - Improved logic for `pyagrum.BNLearner.learnParameters()` and `pyagrum.BNLearner.fitParameters()`.

## Changelog for 2.3.0

- aGrUM
    - Use of std::format` (C++20) to correctly print doubles (thanks to Christopher Eveland).
    - Updated docker images, runners and CI for C++20 and python 3.14.
    - Moved `minimalCondSet` from `gum::IBayesNet` to `gum::DAG{model}`.

- pyAgrum
    - Added python 3.14 suppor and fixed some `Py_DecRef`.
    - Refreshed binder files (python 3.10)
    - Introduced the new `pyagrum.explain` submodule for explainability in Bayesian Networks.
    - Improved SHAP value computation performance in in new package `pyagrum.explain` (thanks to Rayane Nasri).
    - Added support for SHAP values on partial subsets of features in `pyagrum.explain` (thanks to Rayane Nasri).
    - Introduced the concept of SHALL values for explaining log-likelihood in Bayesian Networks in `pyagrum.explain` (
      thanks to Ekaterina Bogush and Amélie Chu).
    - Added tool for exporting documentation to the new site.

## Changelog for 2.2.1

- aGrUM
    - Fixed bugs in `gum::PDAG`(thanks to Louis Derumaux).
    - Added  `gum::Set::popFirst()`

- pyAgrum
    - Updated some requirements
    - Fixed bugs in `pyagrum.PDAG`(thanks to Louis Derumaux).

## Changelog for 2.2.0

- aGrUM
    - (internal) Improved cmake files and act
    - Enhanced `gum::BNLearner::copyState` to more correctly handle constraints.
    - Enhanced `gum::BNLearner`'s EM algorithm to enable user-based initialization.
    - Enhanced `gum::BNLearner`'s EM algorithm to provide more flexible stopping criteria.
    - Added first version of `CITATION.cff` file.

- pyAgrum
    - Improved documentations coverage
    - Update wheels
    - Improved API for `pyagrum.lib.DiscreteTypeProcessor`
    - Used quantile_method='averaged_inverted_cdf' when quantile method is used for discretization. (sklearn >=1.7
      needed).
    - New (experimental) option for building pyAgrum :--no-gil.
    - New target for sphinx : markdown for website (new target `site` for sphinx Makefile).
    - Enhanced `pyagrum.BNLearner`'s EM algorithm to enable user-based initialization.
    - Enhanced `pyagrum.BNLearner`'s EM algorithm to provide more flexible stopping criteria.
    - Improved documentaiton. Especially BNLearner and EM's documentation and tutorial.

## Changelog for 2.1.1

- pyAgrum
    - working on documentation (improved coverage and BN input/ouput mainly)
    - now using `ruff` for formatting (and linting) python files (also in `act guideline`)

## Changelog for 2.1.0

- aGrUM
    - Added a new constructor for `gum::Tensor` with a list of variables to automatically add.
    - Enhanced BIFXML reader/writer to support fast syntax (both for Bayesian Network and for Influence Diagram).
    - Added deterministic and uniform tensor creation methods in `Tensor` class.
    - Added `BayesNet.contextualize` method for structurally handling observations and interventions.
    - Improved code for `BayesNet::operator==`.
    - `gum::influenceDiagramGenerator` now creates `gum::RangeVariable` instead of `gum::LabelizedVariable`.
    - Improved readability of range parsing in `allDiscreteVariables_tpl.h`.
    - Fixed a bug in `gum::Instantiation::contains(std::string)`.
    - Added `gum::trim_copy` method.

- pyAgrum
    - De-modularized pyagrum to resolve weird bugs.
    - Enhanced BIFXML reader/writer to support fast syntax (both for Bayesian Network and for Influence Diagram).
    - Added a new constructor for `pyAgrum.Tensor` with a list of variables to automatically add.
    - Added deterministic and uniform tensor creation methods in `Tensor` class.
    - Added `BayesNet.contextualize` method for structurally handling observations and interventions.
    - `gum::influenceDiagramGenerator` now creates `gum::RangeVariable` instead of `gum::LabelizedVariable`.
    - Added `pyagrum.lib.notebook.inspectBN` function to visualize a (small) Bayesian Network and its CPTs in a
      notebook.
    - Simplified some pyagrum's error messages.
    - Removed useless methods `newFactory`.
    - Fixed typos in RTD's documentation and added `pyAgrum.mutilateBN` in the documentation.

## Changelog for 2.0.1

- pyAgrum
    - Fixed python version for `readthedocs`.

## Changelog for 2.0.0

This major update brings several important changes that align with our goals of improving usability, compliance, and
consistency. Below are the three main motivations behind this significant upgrade:

#### 1. License Change : MIT+LGPLV3

The first major change in version 2.0.0 is the update to our software license. This change ensures better alignment with
modern open-source standards and provides clearer guidelines for usage, distribution, and
modification ([see here for details](https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)).

#### 2. Package Name Update to Follow PEP8

To adhere to Python's PEP8 naming conventions, we have renamed the package from `pyAgrum` to `pyagrum`. We are aware
that this change may cause some inconvenience, but it is necessary to ensure consistency with Python's style guidelines
and improve the overall user experience. The transition to version 2.0.0 is the right time to implement this change, as
it allows us to make a clean break and set a solid foundation for future development.

#### 3. Class Renaming: Potential to Tensor

In an effort to make the library more intuitive and reflective of its functionality, we have renamed the `Potential`
class to `Tensor`. This change better represents the mathematical nature of the class and aligns with common terminology
in the field ([see here for details](https://agrum.gitlab.io/articles/tensors-in-pyagrum.html)).

#### 4. Modular Code Structure

We have reworked the internal structure of the code to make it more modular. This redesign allows for easier
extensibility and maintenance in the long term. By breaking down the code into more independent and reusable components,
we aim to facilitate future enhancements and possible contributions from the community.

-----

#### Detailed Changelog

- aGrUM
    - Split library into `BASE`/`BN`/`CN`/`FMDP`/`ID`/`MRF` sub-libraries .
    - Enhanced swig files (thanks to Julien Schueller!).
    - Improved compilation time for the whole library.
    - Removed obsolete deprecated, experimental code.
    - Moved `gum::Potential` to `gum::Tensor`.
    - used functions of `std::filesystem`.
    - Removed uses of `mkstemp`/`opendir`/`dup`/etc. and inclusions of `dirent.h` and `unistd.h`.
    - Minor typos in aGrUM's testsuite.

- pyAgrum
    - Changed name of pyAgrum's package: from `pyAgrum` to `pyagrum`.
    - Split module into `pyagrum.base`/`bn`/`cn`/`id`/`mrf` sub-modules.
    - Alignd wheel naming with binary distribution format.
    - Renamed `pyagrum.lib.Discretizer` to `pyagrum.lib.DiscreteTypeProcessor`.
    - Fixed a bug for `pyagrum.DiscreteTypeProcessor` with a boolean variable in the database (thanks to Yann Le
      Biannic).
    - Fixed a bug then calling `pyagrum.BNLearner.learnEssentialGraph` with a score-based algorithm (thanks to Yann Le
      Biannic).
    - *pyAgrum-izing* types in `swig`-generated `cpp` files.
    - Removed obsolete deprecated, experimental codes.
    - Moved `pyagrum.Potential` to `pyagrum.Tensor`.
    - Added glossary in readthedoc site.
    - Improved notebooks, docstrings and sphinx documentations.
    - Stable abi for wheel generation. Added possibility to turn off stable abi use.
    - Update CIs for apple silicon.
    - Removed old deprecation.
    - Added new experimental pyAgrum's model : Mixture of Bayesian Networks (thanks to Rodrique Tavernier).
    - Licences are now included in the wheels.

# Older changelogs

- [Changelogs for `1.*.*`](changelogs/changelogs.1.md)
- [Changelogs for `0.*.*`](changelogs/changelogs.0.md)
