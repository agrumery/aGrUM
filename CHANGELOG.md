# aGrUM Changelog

## Changelog for 2.3.0

- aGrUM
    - Use of std::format` (C++20) to correctly print doubles (thanks to Christopher Eveland).
    - Updated docker images, runners and CI for C++20 and python 3.14.

- pyAgrum
    - Added python 3.14 support
    - Refreshed binder files (python 3.10)
    - Introduced the new `pyagrum.explain` submodule for explainability in Bayesian Networks.
    - Improved SHAP value computation performance in `pyagrum.explain` (thanks to Rayane Nasri).
    - Added support for SHAP values on partial subsets of features in `pyagrum.explain` (thanks to Rayane Nasri).
    - Introduced the concept of SHALL values for explaining log-likelihood in Bayesian Networks in `pyagrum.explain` (thanks to Ekaterina Bogush and Amélie Chu).


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

## Changelog for 1.17.2

- pyAgrum
    - Fixed some bugs in `pyAgrum.lib.shapley` (thanks Stefano Mariani !).
    - Changed last MN->MRF (`pyAgrum.{loadMRF|saveMRF|availableMRFExts}`).
    - Improved pickling of `pyAgrum.skbn.BNClassifier`.

## Changelog for 1.17.1

- aGrUM
    - Fixed extra semicolon.

- pyAgrum
    - Fixed some pyAgrum types in generated files.
    - Added check for pandas in tests.
    - Renamed files in module `pyAgrum.clg`.
    - Fixed METADATA for pypi.

## Changelog for 1.17.0

- pyAgrum
    - Added support for `python 3.13`.
    - Added generator `pyAgrum.Instantiation.loopIn()` to iterate on an `pyAgrum.Instantiation`.
    - Significantly accelerated `pyAgrum.Potential.fillFromDistribution()`.
    - Major acceleration of `pyAgrum.clg.ForwardSampling.make_sample()`.

## Changelog for 1.16.0

- aGrUM
    - Added 3 behaviors for generating sampling from `gum::DiscretizedVariable` in `gum::BNDatabaseGenerator` :
      `INTERVAL`/`MEDIAN`/`RANDOM`. The default was `INTERVAL` and is now `RANDOM`.
    - Added timeout for `gum::BNDatabaseGenerator` to prevent slowish (maybe infinite) rejection sampling.
    - Added two new constraints for learning :`gum::learning::NoParentNode`,`gum::learning::NoChildNode`.
    - Added these new constraints in `gum::BNLearner` : `addNoParentNode(nodeid)` and `addNoChildrenNode(nodeid)`.
    - (internal) Improved `act`'s scripts.

- pyAgrum
    - Dropped support of python 3.9 (following [NEP29](https://numpy.ingg/nings/nep-0029-deprecation_policy.html)).
    - Added 3 behaviors for generating sampling from `pyAgrum.DiscretizedVariable` in `pyAgrum.BNDatabaseGenerator` :
      `INTERVAL`/`MEDIAN`/`RANDOM`. The default was `INTERVAL` and is now `RANDOM`.
    - Added timeout for `pyAgrum.BNDatabaseGenerator` to prevent slowish (maybe infinite) rejection sampling.
    - Added two new constraints `pyAgrum.BNLearner.addNoParentNode()` and `pyAgrum.BNLearner.addNoChildrenNode()`.
    - (internal) Standardized licence in python files
    - Added new `pyAgrum.causal.CausalEffectsEstimation` (thanks to Thierry Rioual).
    - `pyAgrum.skbn.BNClassfier`can now be pickled (thanks to Valentin Vassilev).
    - Added module `pyAgrum.lib.shapley` (included in `pyAgrum.lib.explain`) to allow the use of `ShapValues` not only
      from notebooks (thanks to Stefano Mariani).
    - Added a parameter `filename` to `pyAgrum.lib.shapley.ShapValues`'s methods for saving the figure instead of
      showing it (thanks to Stefano Mariani).

## Changelog for 1.15.1

- aGrUM
    - Added `gum::Potential<GUM_SCALAR>::memoryFootprint` and `gum::IBayesNet::memoryFootprint` (thanks to Benjamin
      Datko).
    - Fixed a bug when subscripting `gum::DiscretizedVariable` with the (last) interval (thanks to Gaspard Ducamp).
    - Removed gcc warnings (psabi, false positive uninitialized).

- pyAgrum
    - Added `pyAgrum.Potential.memoryFootprint` and `pyAgrum.BayesNet.memoryFootprint` (thanks to Benjamin Datko).
    - Fixed a bug when subscripting `gum::DiscretizedVariable` with the (last) interval (thanks to Gaspard Ducamp).
    - Fixed a bug in `pyAgrum.skbn.BNDiscretizer` for `NoDiscretization` method with given `paramDiscretization`.
    - Moved function `pyAgrum.lib.export.toFast` to method `pyAgrum.{BayesNet|InfluenceDiagram|MRF}.toFast()`.
    - Moved `pyAgrum.skbn.BNDiscretizer` to `pyAgrum.lib.discretizer`.
    - Worked on documentation for CTBN
    - Fixed `fontcolor` configuration for causal model.

## Changelog for 1.15.0

We continue to add (full python) experimental models. In 1.15, we introduce the CTBN model (Continuous Time Bayesian
Network) featuring, as usual, modelisation and representation, inference (exact and sampling) and a learning algorithm.

- aGrUM
    - Added `gum::NodeId gum::EssentialGraph::idFromName(const std::string& name)` and  `const std::string&  
    gum::EssentialGraph::nameFromId(gum::NodeId node)`.

- pyAgrum
    - Added `pyAgrum.EssentialGraph.idFromName(str)->int` and `pyAgrum.EssentialGraph.nameFromId(int)->str`
    - Improved documentation of `pyAgrum.lib.explain`
    - Better `pyAgrum.clg.CLG.toDot()` and `pyAgrum.clg.CLG._repr_html()`.
    - New model Continuous Time Bayesian Network `pyAgrum.ctbn`.
    - Formatted and adjustments in `pyAgrum.ctbn`.
    - Updated documentations for python experimental models notebooks.
    - Updated thumbnails for python experimental models notebooks.
    - Added serialization (pickle) for CLG and CTBN (consistent with other models in pyAgrum).
    - Improved `pyAgrum.lib.utils.{apply_}dot_layout`
    - Added `pyAgrum.lib.utils.async_html2image` for exported HTML as png or pdf (notably for
      `pyAgrum.lib.notebook.getSideBySide` and `pyAgrum.lib.notebook.getPotential`).

## Changelog for 1.14.1

- aGrUM
    - Fixed compilation errors for `clang18`.

- pyAgrum
    - Fixed python version for `readthedocs`.

## Changelog for 1.14.0

- aGrUM
    - `gum::Potential::marg{Xyz}{In|Out}` are renamed `gum::Potental::{xyz}{In|Out}` to cut down on some verbosity.
      (for instance : `p.margSumOut` becomes `p.sumOut`)
    - Better error messages for `gum::BNLearner<GUM_DATA>`

- pyAgrum
    - Dropped support of python 3.8 (following [NEP29](https://numpy.org/neps/nep-0029-deprecation_policy.html)).
    - Fixed some memory leaks (in special cases) found by Christophe Gonzales.
    - new `pyAgrum.lib.explain.generalizedMarkovBlanket{Names}` and documentation.
    - `pyAgrum.Potential.marg{Xyz}{In|Out}` are renamed `pyAgrum.Potental.{xyz}{In|Out}` to cut down on some verbosity.
      (for instance : `p.margSumOut` becomes `p.sumOut`)
    - Better error messages for `pyAgrum.BNLearner`
    - Added new python's experimental model : Conditional Linear Gaussian (`pyAgrum.clg.*`) : model, learning and
      inference.

## Changelog for 1.13.2

- aGrUM
    - Better (compact) format for numerical labels.
    - Better uniformly distributed random discrete distribution.

- pyAgrum
    - Better (compact) format for numerical labels.
    - Better uniformly distributed random discrete distribution.
    - Continue to implement some automatic `pyAgrum.Potential` filled tools with `pyAgrum.Potential.fillFromExpression`.
    - Worked on better plots for probability+utility
    - Speeded up `gum.BayesNet.addArcs()` with *jit* memory allocation
    - Slight adjustments to histogram display.
    - Fixed documentation of `gum.BNLearner.mutualInformation`.
    - Fixed (deprecated) arguments in `pyAgrum.skbn.BNClassifier`'s methods.

## Changelog for 1.13.1

This release is mainly about documentation.

- pyAgrum
    - Fixed last references to LIP6's notebooks instead of rtd's notebooks (thanks to Benjamin Datko).
    - Added 'Book of Why's notebooks to readthedoc's documentation.
    - Updated Notebooks and thumbnails.
    - Improved documentation coverage for pyAgrum class methods (from 86.19% to 87.05%)
    - `gum.BNLearner.learnEssentialGraph` for all learning algorithm.
    - Better wrapped of `pyAgrum.BNLearner.setPossibleEdges()`.

## Changelog for 1.13.0

- Internal
    - New servers for (linux and windows) CI !
- aGrUM
    - Fast syntax for empirical `gum::DiscretizedVariable`: name suffixed by '+'.
    - Reorganized a bit `gum::DiscreteVariable`'s hierarchy.
    - Worked on quasi-continuous (numerical discrete variable with large domain size) :
        - Logical operators  (`&`,`|`, `~`) to combine evidence (monodimensionnal `gum::Potential` not forced to sum to
          1).
        - New `gum::Potential` and `gum::BayesNet` methods to generate numerical evidence : `evEq`, `evIn`, `evLt`,
          `evGt`.
        - Fast syntax now accepts any type as default (and not only the size of the domain).

- pyAgrum
    - Fixed a bug in `pyAgrum.lib.bn2scores.
    - Fast syntax for empirical `pyAgrum.DiscretizedVariable`: name suffixed by '+'.
    - Reorganized `pyAgrum.lib._colors.py` and `pyAgrum.lib.utils.py`.
    - Changed a default for `pyAgrum.Instantiation.todict`.
    - Improved (a lot) histograms for `pyAgrum.DiscretizedVariable`.
    - Worked on quasi-continuous (numerical discrete variable with large domain size) :
        - Logical operators (`&`,`|`, `~`) to combine evidence (monodimensionnal `pyAgrum.Potential` not forced to sum
          to 1).
        - New `pyAgrum.Potential` and `pyAgrum.BayesNet` methods to generate numerical evidence : `evEq`, `evIn`,
          `evLt`, `evGt`.
        - Fast syntax now accepts any type as default (and not only the size of the domain).
        - `pyAgrum.Potential` accepted as evidence for all inference.
        - Long-awaited `pyAgrum.Potential.fillFrom{Function|Distribution}`.
    - Added `pyAgrum.config.{push|pop}`.
    - Removed obsolete code for `python2.7`.
    - Removed some oldest depreactionWarning.
    - Added '__iter__' for graphs, graphical models, variable and potentials.

## Changelog for 1.12.1

- aGrUM
    - Added a new job dured CI for sphinx documentation.
    - Fixed a typo in `gum::MeeRules::propagate(s)`.
- pyAgrum
    - Fixed a typo in `pyAgrum.MeekRules.propagate(s)`.
    - Added new rules for 'python typification' of generated files.
    - Fixed and updated documentations and notebooks.

## Changelog for 1.12.0

- aGrUM
    - Added a new class `gum::MeekRules` to complete a learning PDAG to CPADG or BN.
    - Prevented 'infinities' in the definition of numerical discrete variable.
    - Added a new constructor for `gum::PDAG` from`gum::MixedGraph`.
    - Better (stricter) `operator==` between `gum::DiscreteVariable`.
    - Added a new static constructor `gum::{Undi|Di}Graph::completeGraph(int n)`.
    - Added `closestIndex()` and `closestLabel()` methods for all numerical `gum::DiscreteVariable`.
    - Fixed an exception raised in inferences due to the removal of uninformative potentials contained only barren
      variables
    - new syntax for `fastVariable` : `[a:b:n]` = `gum.Discretiezd` with n intervalls from a to b.

- pyAgrum
    - Pickle for graphs and graphicals models !
    - Prevented 'infinities' in the definition of numerical discrete variable.
    - Added a new class `pyAgrum.MeekRules` to complete a learning PDAG to CPADG or BN.
    - Better (stricter)  `__eq__` and `__ne_`_ between `pyAgrum.DiscreteVariable`.
    - Added a new constructor for `pyAgrum.PDAG` from`pyAgrum.MixedGraph`.
    - Added a new `expert` discretization method to `pyAgrum.skbn.BNDiscretizer` (thanks to Ahmed Mabrouk).
    - `pyAgrum.skbn.BNDiscretizer` can now take csv filename for input.
    - Added `pyAgrum.fastGraph` to create `pyAgrum.{Di|Undi|Mixed}Graph`.
    - Dataframes can now come from `pandas` or `polars` (thanks to Mads Linksou).
    - Added a `compact` mode for histogram for discretized variable controlled by `histogram_mode`
      `histogram_epsilon` in `gum.config` (thanks to Gaspard Ducamp).
    - Added a new static constructor `pyAgrum.{Undi|Di}Graph::completeGraph(int n)`.
    - Added subscripted to graphical models : `g[name]` gives now a direct access to the `pyAgrum.DiscreteVariable` by
      its
      name.
    - New syntax for `pyAgrum.fast{Var|BN|ID|MRF}` : `[a:b:n]` = `pyAgrum.DiscretizedVariable` with n intervalls from a
      to b.
    - Added `closestIndex()` and `closestLabel()` methods for all numerical `pyAgrum.DiscreteVariable`.
    - Added `g.adjacencyMatrix()` for all graphs and graphical models (thanks to Kenneth Lee).
    - Added new `bgcolor` parameter (None by default) for `showROC/PR` in `pyAgrum.lib.bn2roc` and `pyAgrum.skbn` (
      thanks to Clara Charon).
    - Added optional `dialect` parameter to `pyAgrum.lib.bn2roc.computeScores`.
    - new syntax for `fastVariable` : `[a:b:n]` = `pyAgrum.Discretiezd` with n intervalls from a to b.

## Changelog for 1.11.0

- Internal
    - Removed some warnings in CMakeLists.txt
    - Refactored CI, cleaned wheelhouse

- aGrUM
    - Added `gum::LazyPropagation::mpe{Log2Posterior}` to compute the Most Probable Explanation with some evidence.
    - Improved MIIC algorithm and application of Meek Rules in ordre to to better solve (partially) oriented cycle
      problems.
    - Fixed a bug in `gum::BNLearner` : EM parameter estimation bug due to a cache used in record counters.
    - Fixed a bug in `gum::GibbsSampling` : some evidence may not be taken into account (thanks to Micromix@discord).
    - `gum::PDAG` must accept partially directed cycle (thanks to Kenneth Lee).
    - 'infinities' is now forbidden in the definition of numerical `gum::DiscreteVariable`.

- pyAgrum
    - Updated and added 3.12 in metadatas.
    - Added `pyAgrum.LazyPropagation.mpe{Log2Posterior}` to compute the Most Probable Explanation with some evidence.
    - Removed reference to `sys.log` and to `setuptools`.
    - Removed legacy setuptools related files.
    - Fixed a bug in `pyAgrum.BNLearner` : EM parameter estimation bug due to a cache used in record counters.
    - Fixed a bug in `pyAgrum.GibbsSampling` : some evidence may not be taken into account (thanks to Micromix@discord).
    - Fixed a bug in frontdoor detection in a causal model with multiple connected components.
    - Fixed some regexp expressions.
    - `pyAgrum.PDAG` must accept partially directed cycle (thanks to Kenneth Lee).
    - 'infinities' is now forbidden in the definition of numerical `pyAgrum.DiscreteVariable`.

## Changelog for 1.10.0

- aGrUM
    - added contraints to `MIIC` BN learning algorithm.
    - `MIIC` is now the default for `gum::BNLearner`.
    - Fixed some minor bugs in `gum::BNLearner`.
    - Removed old `3off2` BN learning algorithm (use MIIC instead).
    - added `gum::MIIC::learnPDAG` and give access to this method via `gum::BNLearner`.

- pyAgrum
    - package for python 3.12 !
    - New configuration for specification of graphviz engine and rankdir .
    - Constraint for `MIIC` algorithm in `pyAgrum.BNLearner`.
    - `MIIC` is now the default for `pyAgrum.BNLearner`.
    - Fixed minor bugs in `pyAgrum.BNLearner`.
    - new method `pyAgrum.BNLearner.learnPDAG`.
    - Back to classical representation of do() operator in `pyAgrum.causal`.
    - Examples from the Book Of Why (Pearl) are now in the sphinx documentation.
    - The name of a loaded BN should not include the complete path but only the name of the file (thanks to Jules
      Cassan).

## Changelog for 1.9.0

- aGrUM
    - learning algorithm `gum::learning::MIIC` can use the weighted databases.
    - Internal improvements for `act` tool, `cmake` and compilers (`clang`).

- pyAgrum
    - New visualisation for `gum::DiscretizedVariable` + new config to select this visualisation.
    - `pyAgrum.BNLearner` can use now the weighted databases for all learning algorithms.
    - Documentation improvements.
    - `pyAgrum.lib.bn2roc`
        - added new functions `get{ROC|PR}points()`.
        - accepted `pandas.DataFrame` as data source (`datasrc`).
        - added Fbeta (beta!=1) scores to bn2roc.
        - added F-Beta threshold on ROC and PR curves.
        - `bn2roc` functions now force many parameters to be keyword-arguments in order to prevent the risk of mixed
          arguments.
        - added new functions `anim{ROC|PR}`.
    - `pyAgrum.skbn.Discretizer` can propose a set of labels (that includes the labels from the database) when
      `"NoDiscretization"` is selected. (see tutorial `52-Classifier_Discretizer`).

## Changelog for 1.8.3

- aGrUM
    - Removed GCC warnings about overloaded virtual methods.
    - Fixed a bug in jointree inference in rare cases (when an irrelevant soft evidence is added).

- pyAgrum
    - Fixed a bug in jointree inference in rare cases (when an irrelevant soft evidence is added).
    - Removed deprecated methods from pyAgrum<1.1.0.
    - Added new syntax for set (conditional) marginal with dict : `p[{"A":0,"C":1}]={"no":0.3,"yes":0.7}`.
    - small updates for pyAgrum's documentation.

## Changelog for 1.8.2

- aGrUM
    - Fixed a bug (still) in `gum::PDAG::cSeparation` (thanks to Keneth Lee).
    - Fixed a bug in `gum::BIFXMLIdWriter::save`.
    - typos in `gum::PDAG::toDot()`.

- pyAgrum
    - Fixed a bug in `pyAgrum/{load|save}ID`.
    - Wrapped `gum::PDAG::moralizedAncestralGraph()`.
    - Added a warned when importing `pyAgrum.lib.notebook` if graphviz is not installed.

## Changelog for 1.8.1

- aGrUM
    - Fixed bugs in `gum::PDAG::cSeparation` (thanks to Keneth Lee).
    - Used 'table' instead of 'default' for marginal cpt in `gum::BIFWriter`.
    - Added `std::cout<<(gum::Timer& t)`

- pyAgrum
    - Updated build image for rtd.
    - Improved documentations.
    - Documentation for pyAgrum.InformationTheory.
    - Uniformized `cmap` to `cmapNode` in `pyAgrum.lib.notebook`.
    - Added beeswarmplot for shapley values figures in `pyAgrum.lib.explain`.

## Changelog for 1.8.0

- aGrUM
    - Added new class `gum::InfomationTheory` that regroups every information theory concepts (entropies, conditional
      entropies and mutual information, ...) with only one inference, which allows also conditioned information theory
      concepts.
    - Added `gum::Potential::expectedValue()`
    - Added the alternative *fast* syntax for NumericalDiscreteVariable : `{1.5:3.5:3}` meaned from 1.5 to 3.5 in 3
      steps.
    - Updated `GraphicalModel::exists(const std::string&)` : return false instead of throwed an exception.
    - Added `gum::graphicalModels::variables()` and a new alias for `gum::VariableSet`.
    - Fixed a bug in `gum::ShaferShenoyInference` for joint posterior when some nodes in the target received hard
      evidence.
    - Improved error message w.r.t `gum::*::jointPosterior`.
    - Added mingw as a target for CI.
    - Fixed dangled pointers in testsuite, only detected by MSVC.
    - Optimized `gum::PDAG::cSeparation`
    - Renamed `gum::Set::isProper{Sub|Super}Set` to `gum::Set::isStrict{Sub|Super}Set`.

- pyAgrum
    - swig>4.1 is now needed.
    - Enabled installation with poetry (thanks to Lorenzo Conti)
    - Added new class `pyAgrum.InfomationTheory` that regroups every information theory concepts (entropies, conditional
      entropies and mutual information, ...) with only one inference, which allows also conditioned information theory
      concepts.
    - Added `pyAgrum.Potential.expectedValue()`.
    - Restructured a bit the sphinx documentation.
    - Down-exported the read-only API for every types of discrete Variable to `pyAgrum.DiscreteVariable`. (see
      `VariablesTestSuite.testExportDerivedReadOnlyAPIforDiscreteVariable`)
    - Updated notebooks to the new down-exported read-only API of `pyAgrum.DiscreteVariable`.
    - Added the alternative *fast* syntax for NumericalDiscreteVariable : `{1.5:3.5:3}` meaned from 1.5 to 3.5 in 3
      steps.
    - Updated docs for new *fast* syntax.
    - Updated `GraphicalModel::exists(const std::string&)` : return false instead of throw an exception.
    - Initiated a new notebook about the interaction with ipywidget
    - Optimized and correctly wrapped `gum::PDAG::cSeparation` (thanks to Kenneth Lee).
    - New functions `pyAgrum.lib.explain.nestedMarkovBlankets` and `pyAgrum.lib.explain.nestedMarkovBlanketsNames`.

## Changelog for 1.7.1

- aGrUM
    - Added `gum::{AllDiscreteVariable}.toFast()` to write the variable in *fast* syntax.
    - Fixed a bug in BN's random generators : `domain_size` incremented by 1 (thanks to Kenneth Lee).
    - Fixed a bug in *fast* syntax : `'A[n]' with n<2`.

- pyAgrum
    - Added `pyAgrum.{AllDiscreteVariable}.toFast()` to write the variable in *fast* syntax.
    - Fixed a bug in BN's random generators (e.g. `pyAgrum.randomBN`) : `domain_size` incremented by 1 (thanks to
      Kenneth Lee).
    - Fixed a bug in *fast* syntax : `'A[n]' with n<2`.
    - Modifyed (lightly) the structure for documentation.

## Changelog for 1.7.0

- aGrUM
    - Add `gum::Potential::sign` and `gum::Potential::new_sign`
    - Bug fix in causalImpact found by Musfiqur Rahman
    - Generalization and rationalization of access of aGrUM's generator of pseudo-random values
    - Added `gum::Timer::toString()`.
    - Improved BN random generator.
    - get rid of `sprintf` as much as possible.
    - Fixed random seed.
    - Fixed `gum::Potential::random()` : not a distribution.
    - Updated *Fast* syntax : enumerated consecutive integers (e.g `{1|2|3|4}`) creates a `gum::RangeVariable[1,4]`
      rather than an `gum::IntegerVariable{1|2|3|4}`.
- pyAgrum
    - Add `pyAgrum.Potential.sign` and `pyAgrum.Potential.new_sign`
    - Bug fix in causalImpact found by Musfiqur Rahman
    - Added `pyAgrum.Timer.__str()__`.
    - Improved BN random generator.
    - Fixed random seed.
    - Fixed `pyAgrum.Potential.random()` : not a distribution.
    - Added access to `gum.fastVariable()`.
    - Updated *Fast* syntax : enumerated consecutive integers (e.g `{1|2|3|4}`) creates a `pyAgrum.RangeVariable[1,4]`
      rather than an `pyAgrum.IntegerVariable{1|2|3|4}`.
    - add a new module to export in different formats (for now just GML).

## Changelog for 1.6.1

Quick fix for the configuration of pyAgrum documentation generation by readthedoc.

## Changelog for 1.6.0

The main points of this tag is the renamed of *Markov network* (`MarkovNet`, `MN`) for the better known *Markov Random
Field* (`MarkovRandomField`, `MRF`) and a new reader/writer of XDSL format (Genie/Smile) for Bayesian networks. Other
improvements and corrections have naturally also been made.

- aGrUM
    - Renamed `gum::MarkovNet` to `gum::MarkovRandomField`. Renamed `gum::*MN*` to `gum::*MRF*` when necessary.
    - Fixed glitches and bugs induced or revealed by `gum::MarkovNet`->`gum::MarkovRandomField`.
    - new `XDSL` Reader/writer for Bayesian network.
    - Renamed `gum::Learning::BNLearner::learnMixedStructure` to `gum::Learning::BNLearner::learnPDAG`
    - worked on documentation : better rendered for doxygen pages.
    - Renamed `gum::dSeparation` to `gum::dSeparationAlgorithm`.

- pyAgrum
    - Renamed `pyAgrum.MarkovNet` to `pyAgrum.MarkovRandomField`. Renamed `pyAgrum.*MN*` to `pyAgrum.*MRF*` when
      necessary.
    - new `XDSL` Reader/writer for Bayesian network.
    - Renamed `pyAgrum.BNLearner.learnMixedStructure()` to `pyAgrum.BNLearner.learnPDAG()`.
    - For figure contained nodes drawn by matplotlib (e.g. inference), use the same font for all nodes (default from
      matplotlib) (thanks to Jonathon Blackford).
    - Worked on documentation : better rendered for readthedocs pages, improved structuration, new thumbnail image for
      some notebooks/tutorials.
    - Significant improvement of the documentation coverage.
    - Improved `gum.DiscreteVariable.to[typeOfVariable]`, renamed as `gum.DiscreteVariable.as[typeOfVariable]` and added
      documentations.

## Changelog for 1.5.2

Mainly documentation and new analytics for the different sites.

- pyAgrum
    - Added a new and very easy to use `pyAgrum.randomBN` that creates a BN from a list of names.
    - Removed obsolete `pyAgrum.generateCSV` replaced by `pyAgrum.generateSample`. Fixed tests.
    - Fixed and improved pyAgrum documentation.

## Changelog for 1.5.1

- aGrUM
    - Improved `gum::BNLearner::state()`.
    - (internals) new macro `GUM_TEST` and `GUM_INACTIVE_TEST` for CI

- pyAgrum
    - Improved `gum.BNLearner.state()`.
    - Improved style for readthedoc (pygments and no StickySideBar)
    - Improved documentation coverage (for methods) from 84.9% to 90.7% : more than 100 newly covered methods.

## Changelog for 1.5.0

- aGrUM

    - (internals) Updated `act --stats`.
    - (internals) Reorganized build folder : `build/{aGrUM|pyAgrum}/{debug|release}`.
    - (internals) Improved cmake & CIs.
    - Threadsafe graphs and Graphical Models.
    - Added new graph `gum::PDAG` (Partially Directed Acyclic Graph).
    - Renamed `gum::MixedGraph::adjacents` with correct graph notion : `gum::MixedGraph::boundary`.
    - Initialized the majority of the end/rend iterators at compile time.
    - Added AVL binary search trees: `gum::AVLTree`.
    - Added priority queues that can be iterated in order: `gum::SortedPriorityQueue`.
    - Better messages for `gum::BayesNet::check()`.

- pyAgrum

    - (internals) New docker images with linux gcc 11 for wheels.
    - (internals) CMake: Use FindPython module.
    - (internals) Removed some (false positive) warned notifications from swig.
    - Fixed NaN bugs for new versions of Graphviz.
    - Improved gum.lib.notebooks.flow and light/dark theme compatibility.
    - Renamed `pyAgrum.MixedGraph.adjacents` with correct graph notion : `pyAgrum.MixedGraph.boundary`.
    - New graph class `gum.PDAG` (Partially Directed Acyclic Graph).
    - Fixed a small typo when displayed function as Potential.
    - better messages for `pyAgrum.BayesNet.check()`.
    - More robust `gum.explain.showInformation()` w.r.t. NaN.
    - Fixed typos in documentation.
    - Improved the organization of ReadTheDoc documentation.

## Changelog for 1.4.1

- pyAgrum

    - bugfixes in `pyAgrum.lib.[causal.]notebook`.

## Changelog for 1.4.0

- aGrUM

    - better `toString` for BN : show the memory used by the parameters.

- pyAgrum

    - __added conda and pip packages for python 3.11__
    - better `__str__` for BN : show the memory used by the parameters.
    - bug fix in obsolete pyAgrum.`BNLearner.useNoAPriori()`.
    - bug fix when displayed a `pyAgrum.causal.CausalFormula` generated by do-Calculus : retrieved the original `doing`
      and `knowing` sets.

## Changelog for 1.3.2

- aGrUM

    - `gum::BNDataGenerator` : Add a way to generate sampls from Bayesian Network with evidence
    - More informative error message for `gum::DatabaseTable::insertRow()`.
    - Code structure for checked syntax before saved a Bayesian Network. Instantiated for BFI and DSL format.
    - Improved code for `gum::BinSearchTree`.
    - Better error messages for `gum::DSLReader`.

- pyAgrum
    - `pyAgrum.BNDataGenerator.drawSamples()` : Add a way to generate sampls from Bayesian Network with evidence.
    - More informative error message for `gum::DatabaseTable::insertRow()`.
    - `pyAgrum.lib.dBN` : better dot representation of unrolled BNs.
    - `pyAgrum.lib.notebook.show()` : bugs fixed for some types of parameters.
    - `pyAgrum.config` : new types accessors : `config.as{Int|Float|Bool}`.
    - Updated documentations for `pyAgrum.lib.notebook.show()`, `pyAgrum.BNDatabaseGenerator.drawSamples()`,
      `pyAgrum.skbn`.
    - New argument when drawed graphs : label on arcs (thanks to Mahdi Hadl Ali).

## Changelog for 1.3.1

- aGrUM

    - Updated readme

- pyAgrum

    - Fixed warnings in `gum.Potential.topandas()`.
    - Allowed label or index when addressed variables in dictionary for `gum.Instantiation` (such as in
      `gum.Potential.extract`).
    - Allowed dictionnary for mapped variables in `pyAgrum.Potential.fillWith`.
    - Fixed typos in documentation of `pyAgrum.Potential.fillWith`.
    - Fixed css for sphinx notebooks gallery.

## Changelog for 1.3.0

- aGrUM

    - Fixed errors in doxygen configuration.
    - Added a new class of discrete variable `gum::NumericalDiscreteVariable` whose support is a (finite) set of
      `double`.
    - Fixed new clang warnings.
    - Fixed a bug in `gum::Potential::max()` when all values are negative (thanks to Zakarie Aloui).
    - Added a new method `gum::learning::BNLearner::score()` which computes the currently selected score for a node and
      its parents.
    - Added a test for a variance beed not negative in `gum::ShaferSheoyLIMIDInference` (thanks to Benjamin Datko).
    - Added new methods `gum::learning::BNLeaner::mutualInformation()` and
      `gum::learning::BNLearner::correctedMutualInformation()`.

- pyAgrum

    - Added a new class of discrete variable `gum.NumericalDiscreteVariable` whose support is a (finite) set of
      `double`.
    - Updated notebooks w.r.t. this new variable (`16-Examples_quasiContinuous` and `18-Examples-BayesianBetaCoin`).
    - Added a new method `gum.BNLearner.score()` which computes the currently selected score for a node and its parents.
    - Added new methods `gum.BNLeaner.mutualInformation()` and `gum.BNLearner.correctedMutualInformation()`.

## Changelog for 1.2.0

- aGrUM

    - Moved named convention from `apriori` to `prior`.
    - `gum::CliqueGraph::addNode{WithId}` with the same syntax as in other graphs.
    - [act] Improved the compliance for various microsoft compilers.
    - [act] option `-windows` is now `-compiler` and accepts `gcc` or `clang` when platform is linux or mac.
    - [act] options `--clangpath` add `--gccpath` to specify the path of the compilers (if those compilers are not in
      PATH).
    - Changed internal representation of `gum::Exception` (now derived from `std::exception`).
    - Added a new prior : `gum::learning::DirichelPriorFromBN`.
    - Renamed `genericBNLearner` to `IBNLearner`.
    - Added `gum::learning::BNLearner::useDirichletPriorFromBN`.

- pyAgrum

    - Moved named convention from `apriori` to `prior`.
    - Added `pyAgrum.BNLearner.useDirichletPriorFromBN`.
    - Added a new function `gum::mutilateBN`.
    - `pyAgrum.CliqueGraph.addNode{WithId}` with the same syntax as in other graphs.

## Changelog for 1.1.1

- aGrUM

    - Fix a regression that was slowed down the inference in credal networks.
    - Better error messages when IOError caused by writed in 'bad' files.
    - Typos in several error messages.
    - Fix warnings from gcc 12.1 and integrate linter proposals.

- pyAgrum

    - PEP 3149 implemented.
    - Small typos in pyAgrum.causal.
    - Added acces to the database built for learning in `pyAgrum.skbn.BNClassifier.preparedData()`.
    - Small bug when represented Potential as fractions.
    - Small bug in type deduction from database for variable in `pyAgrum.skbn`.
    - notebooks now added in the sphinx documentation (and then in readthedocs site).

## Changelog for 1.1.0

- aGrUM

    - Parallelized inference for Markov Net : `gum::ShaferShenoyMNInference`.
    - Added a new `gum::fastVar(std::string)` factory function to easily build variable such as in
      `gum::*::fastPrototype(std::string)`.
      See [Documentation](https://pyagrum.readthedocs.io/en/1.1.0/functions.html#quick-specification-of-randomly-parameterized-graphical-models).
      This syntax is now used in every graphical model with `gum::{BayesNet|MarkovNet|etc}::add(std::string)` to easily
      add a new variable in the model.
    - Added a new graphical representation of cliqueGraph : `gum::CliqueGraph::mapToDot()`.

- pyAgrum

    - `gum.ShaferShenoyMNInference` is now parallelized too.
    - Wrapped the new fast syntax for created variable in functions `gum.{BayesNet|etc}.add(str)`.
      See [Documentation](https://pyagrum.readthedocs.io/en/1.1.0/functions.html#quick-specification-of-randomly-parameterized-graphical-models).
    - Introduced a better type induction for discrete variables when created by `gum.skbn.BNDiscretizer`.
      `gum.IntegerVariable` and `gum.RangeVariable` are now induced from the data.
    - Added `gum.JunctionTree.map()` (in notebook) to show a condensed map of a the junction tree.
    - Still worked on documentation (*One must imagine Sisyphus happy*).

## Changelog for 1.0.0

The aGrUM's team is very proud to announce the release of aGrUM/pyAgrum 1.0.0!

This long journey sometimes strewn with pitfalls, but which often brought great satisfactions, began with this first
commit (subversion):

> add8dbff5 | 13 years ago | phw | test?

aGrUM/pyAgrum is now definitely no longer a test :-) Parallelization of inference in Bayesian networks is the important
feature that allows us to symbolically assert a level of quality of aGrUM. The 1.0.0 release shows that we also believe
the API will remain stable in a medium term.

pyAgrum continues its path as a wrapper of aGrUM but with more and more specific features. Some of them are intended to
be implemented in C++ and eventually integrated in aGrUM (causality, etc.), others will certainly remain specificities
of pyAgrum.

In the near future, for aGrUM and pyAgrum, we expect many more new features and releases ! Stay tuned!

- aGrUM

    - VariableElimination, ShaferShenoy and LazyPropagation are now parallelized.
    - Better use of d-separation in ShaferShenoy and LazyPropagation.
    - Better initialization/registrations used Meyers singleton.
    - Better 0-dimensional Potential.
    - new `gum::IBayesNet::check()` to test if the BN is completely and well-defined.

- pyAgrum

    - Better documentation and argument order for `gum.BNLearner.__init__`.
    - Better numpystyle for docstrings.
    - Better tests and notebooks.
    - Better signature for `gum.Potential.arg{max|min}`.
    - New `gum.IBayesNet.check()` to test if the BN is completely and well-defined.
    - More consistent API : `with_labels` default is True everywhere.

## Changelog for 0.22.9

This tag is a pre-relase for 1.0.0 (!).

- aGrUM

    - Added a new Multithreaded facility which supports easily both `openMP` and `STL`.
    - Enabled exceptions raised by threads to be catched.
    - Made `gum::CredalNetworks` and `gum::learning::BNLearner` use the new multithreaded facility.
    - Made a fully new architecture for scheduled inferences.
    - Added a sequential and a parallel schedulers for inferences.
    - Enabled `gum::LazyPropagation` and `gum::ShaferShenoy` to use schedulers for their inferences.
    - `gum::DiscretizedVariable` can now be declared as "empirical". Meaned that the lower and upper ticks are not
      always hard limits.
    - improve a bit API for `gum::IntegerVariable`.

- pyAgrum

    - add a way to export BN samples as a `pandas.DataFrame` instead of csv files in `pyAgrum.BNDatabaseGenerator` and
      in the function `pyAgrum.generateSamples(....)`.
    - `gum.BNLearner` can now take a `pandas.DataFrame` as data source in its constructor.
    - Add support for default number of thread in `gum.config`.
    - Added methods to get/set the number of threads used by `pyAgrum.BNLearner`, `pyAgrum.LazyPropagation` and
      `pyAgrum.ShaferShenoy`.
    - small change in `pyAgrum.skbn.Discretizer.audit` : show the domain size for discrete variable.
    - better graphical diff between BN, even if a node is missed used `pyAgrum.bn_vs_bn` functionalities.
    - empirical gum.DiscretizedVariable used in `pyAgrum.skbn.Discretizer`
    - new configuration for (LaTeX) fractions in `gum.lib.notebook.showCPT` (see tutorial)
    - update a bit `pyAgrum.IntegerVariable` wrapper and documentation.

## Changelog for 0.22.8

- aGrUM

    - Remove clang 13 warnings (thanks to Julien Schueller).
    - Add the ability to chain many methods in `gum::learning::BNLearner`.

- pyAgrum

    - Reorganization of notebooks.
    - Add `pyAgrum.lib.image.export[Inference]` with no filename in order to export a `numpy.array` ready for
      `matplotlib.pyplot.imshow()` (thanks to Mads Lindskou).
    - Improve `pyAgrum.bn_vs_bn` graphical diff between BNs with many style configurations in `pyAgrum.config`.
    - Cosmetic improvements for `pyAgrum.lib.bn2scores`.
    - Add an access to `pyAgrum.lib.explain.getInformationGraph` in order to export the image (thanks to Dennis Mac R).
    - `pyAgrum.BNLearner`

        - Add the ability to chain many methods in `pyAgrum.BNLearner`.
        - Add an easy-to-use `pyAgrum.BNLearner.fitParameters` to learn in-place the parameters of the argument.
        - Add `pyAgrum.BNLearner.learnEssentialGraph`.

    - `pyAgrum.lib.notebook`

        - Add `_repr_html_` to `pydot.Dot`.
        - Add an easier-to-use `pyAgrum.lib.notebook.flow.add(obj,caption)`.
        - Add the ability to chain many methods in `pyAgrum.lib.notebook.flow`.
        - Move from `title` to `caption` as argument for `pyAgrum.lib.notebook.flow`'s methods.
        - Improve graphical representation for `pyAgrum.lib.notebook.showInformation()`.
        - Add a new argument `ncols` to `pyAgrum.lib.notebook.sideBySide` in order to use it as a grid (and not only as
          a line).

## Changelog for 0.22.7

Mostly several quick fixes.

- aGrUM

    - Small bugfix on Markov random field with only one factor.
    - Better behavior for ordered nodes when `gum::MarkovNetwork::addFactor`.
    - Better behavior for `gum::ShaferShenoyMNInference::jointPosterior` when a node of the joint is hard-observed.

- pyAgrum

    - Update and fix documentation (mostly for readthedocs and typehints).
    - Fix and improve ticks when plotted distribution with large domainSize.
    - Better behavior for ordered nodes when `pyAgrum.MarkovNetwork.addFactor`.
    - Better behavior for `pyAgrum.ShaferShenoyMNInference.jointPosterior` when a node of the joint is hard-observed.

## Changelog for 0.22.6

- aGrUM

    - aGrum is now compiled used c++20 (updated cmake).
    - Fix 'distribute' phase in some cases for `gum::ShaferShenoyLIMIDInference` (Thanks to Michael Kontoulis).
    - Remove a lot of allocators as template parameters.
    - `act`: Improve architecture.
    - `act`: Use of `argparse`  instead of the obsolete `optparse`.
    - Worked on CI : Worked on CI : removed some obsolete platform (`MSVC 15`), added some new (`MSVC 22`)
    - use `std::mt19937` instead of `rand()` (thanks to Julien Schueller).

- pyAgrum

    - Drop support for python 3.7 (following [NEP29](https://numpy.org/neps/nep-0029-deprecation_policy.html)).
    - Drop support for 32bits architectures (following [numpy](https://numpy.org/devdocs/release/1.22.0-notes.html)).
    - Use of `pydot` instead of `pydotplus`.
    - Several typos in code and documentation of `gum.lib` (thanks to Benjamin Datko).
    - Improve notebooks (thanks to Clara Charon).

## Changelog for 0.22.5

This release mainly deals with provided a workaround to a pydotplus/pyparsed bug affected several graph visualizations
in pyAgrum.

- pyAgrum

    - workaround to the pydotplus/pyparsed bug : "added an empty node".
    - remove the use of deprecated distutils in act.
    - (approximate) fix labels in x-axis for lined-histogram.

## Changelog for 0.22.4

This release aims to provide a number of new configurations for aGrUM and pyAgrum:

- aGrUM

    - builder for windows used msvc2022 (new option for `act`)
    - aGrUM is now compiled used c++17

- pyAgrum

    - package for MacOS arm64 (M1)
    - package for python 3.10 (limited to 64 bits)
    - customization title of plots in `pyAgrum.lib.dynamicBN`

## Changelog for 0.22.3

- pyAgrum

    - new option in `gum.config` for percent or just number in histograms.
    - better wrappers for `gum.Instantiation.variablesSequence()` and `pyAgrum.MixedGraph.adjacents(id)`.
    - better wrappers for `pyAgrum.MixedGraph.{mixedOrientedPath|mixedUnorientedPath}`.
    - refreshed all `requirements.txt`.
    - better visualisation for `pyagrum.lib.DynamicBN`.
    - improvements in documentations.
    - aarch64 version for linux package

## Changelog for 0.22.2

- aGrUM
    - Minor change in `CSVParser` (`BNLearner` etc.): if a token is surrounded by quote characters, those are not part
      of the
      very token anymore.

- pyAgrum
    - Updated documentations
    - Moved export image utilities in a new `pyAgrum.lib.image`. See the notebook (![Coloured and exported models]
      (<https://webia.lip6.fr/~phw/aGrUM/docs/last/notebooks/colouringAndExportingBNs.ipynb.html>)).
    - Minor change in `CSVParser` (`BNLearner`, etc.) : if a token is surrounded by quote characters, those are not part
      of
      the very token anymore.

## Changelog for 0.22.1

- aGrUM
    - fix issue #69 (no more final destructor),
    - update Coco/R parsers (notations and explicit casts),
    - better `gum::SyntaxError` (access to filename),
    - remove many redundant ';' in testsuites,
    - better option `--stats` for act,
    - `gum.[model].fastPrototype` now accepts multiline specifications.

- pyAgrum
    - better `pyAgrum.SyntaxError` treated as python's SyntaxError,
    - much better annotations for types declaration in python codes,
    - improved documentation,
    - update pyAgrum.causal,
    - `gum.fast[Model]` now accepts multiline specifications,
    - `gum.DiscreteVariable` are now hashable.

## Changelog for 0.22.0

As planned, 0.22.0 is the first version of pyAgrum that does not support python>3.6 (included 2.7).

- aGrUM
    - fix issue #27
    - (act) remove (hopefully) all the codes to support both python. In particular, there is no more options for act to
      choose the targeted version of python.
    - (ci/deploy) removed 2.7 tests and deploy (thanks to @Aspard)
    - better and customized type induction when learning Bayesian networks from CSV.
    - new constructor for `gum::learning::BNLearner` to activate/deactivate the type induction when readed a csv file. (
      thanks to @gonzalesc)

- pyAgrum
    - remove a large part of the codes dedicated to python2 in the wrapper (`wrapper/python/generated-files2`) and in
      `pyAgrum.lib`. To be cont'd.
    - many improvements due to linter (pylint especially) in `pyAgrum.lib`.
    - graphical improvement in `pyAgrum.lib.bn2roc` thanks to Clara Charon.
    - new constructor for `gum.BNLearner` to activate/deactivate the type induction when readed a csv file.

## Changelog for 0.21.0

Contrary to what was said in the 0.20.0 changelog, we decided to remove support for python 2.7 before the 1.0 release.

This tag (0.21.0) is the last version that supports python 2.7. We are already worked and will deliver a 0.22.0 tag as
soon as possible, which will be dedicated to this move and will then be the first tag without python 2.7 support.

The next tag (0.22.0) will be the (new) last minor version before the release of agrum/pyAgrum 1.0.0 (:fist: :smirk: ).

- aGrUM
    - New type for discrete variable (`gum::IntegerVariable`) which represents a set of non-consecutive integers.
    - New syntax for `gum::IntegerVariable` in `gum::*::fastPrototype` : `a{-3|0|3}`.
    - Change in syntax for `gum::MarkovNetwork::fastPrototype` : the link are represented by `--` instead of `-`.
    - New `gum::BNLearner::state()` which gives a view of the activated options in the learner (scores, priors,
      algorithms, constraints, etc.).
    - New `gum::BNLearner::toString()` which gives a stred representation of `gum::BNLearner::state()`.
    - Add a new CI for last gcc (g++11 for now).
    - Code optimizations for hash function for small-sized values.
    - Better hierarchy for exceptions.
    - MLEstimator should lead to an error when divided by 0.

- pyAgrum
    - New type for discrete variable (`pyAgrum.IntegerVariable`) which represents a set of non-consecutive integers.
    - New syntax for `pyAgrum.IntegerVariable` in `pyAgrum.fast*` : `a{-3|0|3}`.
    - Change in syntax for `pyAgrum.fastMN` : the links are represented by `--` instead of `-`.
    - New `pyAgrum.BNLearner.state()` which gives a view of the activated options in the learner (scores, priors,
      algorithms, constraints, etc.).
    - New `pyAgrum.BNLearner.__str__()` which gives a stred representation of `gum::BNLearner::state()`.
    - Documentations and notebooks updated w.r.t. this new features.
    - Added ShapValues for BN in `pyAgrum.lib.explain` (see notebook).
    - Added `pyAgrum.lib.explain.independenceListForPairs()`.
    - Other improvements in `pyAgrum.lib.explain` and the corresponded notebook and documentations.
    - Updated notebooks for classifiers.
    - Better hierarchy for exceptions.
    - Removed unnecessary and obsolete codes by deleted `pyAgrum.lib._utils`.
    - 'Terminology clash' between 'Laplace's adjustment' and 'Smoothing' : use more generic 'Smoothing' everywhere now.
    - MLEstimator should lead to an error when divided by 0.

## Changelog for 0.20.3

- aGrUM
    - Refactoring/fixed MIIC and better heuristic for orientations for constraint-based learning algorithms.
    - Updated guidelines and new convention for `private` methods/attributes.
    - Changed behaviour of `gum::MixedGraph::mixed{Oriented|Unoriented}Path` : no misuse of exception when no path is
      found.

- pyAgrum
    - Refactored MIIC and better heuristic for orientations for constraint-based learning algorithms.
    - Changed behaviour of `pyAgrum.MixedGraph.mixed{Oriented|Unoriented}Path` : no misuse of exception when no path is
      found.
    - Updated new `pyAgrum.Potential`'s methods and documentation.
    - New tool for layout in notebooks : `pyAgrum.notebook.flow`.
    - New gum.config options for background colors in CPT : `potential_color_0` and `potential_color_1`.
    - New module `pyAgrum.lib.explain`.

## Changelog for 0.20.2

- aGrUM
    - Add a check on parameters when builded a `gum::credal::CredalNet` from BNmin and BNmax: 'are Pmin<=Pmax' ?".
    - Fix a bug, and a visualisation of results on decision nodes with deterministic optimal strategy in
      `gum::InfluenceDiagram`.

- pyAgrum
    - Add a check on parameters when builded a `gum.CredalNet` from BNmin and BNmax: 'are Pmin<=Pmax' ?".
    - Fix a bug and add a better visualisation of results on decision nodes with deterministic optimal strategy in `gum.
    InfluenceDiagram`.
    - Add some options for notebook and influence diagrams in `gum.config`. Notably, add a `gum.notebook.
    show_inference_time`
    - Fixes and typos in notebooks
    - Finally, add a worked version of `gum.lib.notebook.exportInference` to create pdf from an inference. With `gum.
    lib.notebook.export`, it is now possible to export all kind of pyAgrum's graphs into pdf from a notebook.
    - new methods: `pyAgrum.Potential.topandas()`,`pyAgrum.Potential.tolatex()`

## Changelog for 0.20.1

- aGrUM
    - Fix an infamous bug: monocycle in DAG (thanks to Guy, GabF and Joanne). This bug did not propagate to graphical
      models (especially BNs).

- pyAgrum
    - new site for tutorials.
    - renamed and reorganized many tutorials
    - sync'ed documentation (readthedocs) with the new URLs for notebooks

## Changelog for 0.20.0

0.20.0 is the last minor release before 1.0.0.

- aGrUM
    - Workaround for OMP with MVSC
    - Refreshed doxygen configuration file
    - Graph methods for `children`and `parents` of sets of nodes.
    - Renamed `core/math/math.h` to `core/math/math_utils.h` to avoid clash names and false warnings from linters
    - work on CIs
    - Fix and typos from F.Keidel
    - Improved API of `gum::BayesNetFragment` (for instance, non-implemented `gum::BayesNetFragment::VariableNodeMap`).
    - Major changes (and typos and bug fixes) in `gum::CN::CredalNetwork`'s API
    - Fixed minor bugs in inference for `gum::CN::CredalNet`
    - (internal) re-organized files and folders for Credal Networks
    - (internal) fixed bug in organization of inline/tpl/source files for `gum::credal::lp::LpInterface`

- pyAgrum
    - (internal) Better logic and automatic generation for the multiple "requirements.txt".
    - Graph methods for `children`and `parents` of sets of nodes.
    - `pyAgrum.notebook.export` and `pyAgrum.notebook.exportInference` to export as png, pdf(, etc.) PGM and inference
      in PGM
    - Fix several tests
    - Fix and typos from F.Keidel
    - Added `gum.Instantiation.addVarsFromModel` and allowed chained `gum.Instantiation.add()`.
    - Fixed some broken links in documentation.
    - Updated `gum.skbn` for non-binary classifier (see notebooks).
    - Improved notebooks for classifiers.
    - Major changes in `gum.CredalNetwork`'s API
    - Specific visualisation for credal networks
    - Graphical visualisation of inference with credal
      networks (![Visual Credal networks](https://gitlab.com/agrumery/aGrUM/-/blob/master/wrappers/pyAgrum/doc/sphinx/_static/fastModelsSource/5-fastCNWithPyAgrum.png "Credal Networks"))
    - Added some example for credal networks in notebooks

## Changelog for 0.19.3

- pyAgrum
    - missed graphical  (not correctly wrapped) methods in `gum::InfluenceDiagram`
    - fix falsely raised exception leaded to incomplete generation of documentation and wheels.
    - `pyAgrum.lib.ipython` improved.
    - pyAgrum's documentation refreshed a bit.
    - `pyAgrum.skbn` improved.
    - several typos in notebooks and testsuites.

## Changelog for 0.19.2

- aGrUM
    - bugfix for `EssentialGraph` (thanks to M.Lasserre).

## Changelog for 0.19.1

- aGrUM
    - bugfix for `InfluenceDiagram` with all-negative utilities (thanks to B.Enderle).
    - [internal] typos and reorganization for `act`'s modules.

## Changelog for 0.19.0

*Mainly* : important changes for Influence Diagram (aGrUM and pyAgrum) and for BayesNet classifiers compliant to
scikit-learn's API (pyAgrum).

- aGrUM
    - new and better inference for Influence Diagrams and LIMIDs (`gum::ShafeShenoyLIMIDInference`).
    - new builder for Influence Diagram `gum::InfluenceDiagram::fastPrototype`.
    - bugfixes.

- pyAgrum
    - wrapper and notebook functions for new inference and new methods for influence diagram.
    - new module `skbn` for BayesNet classifier compatible with sklearn (classification and discretization)  with
      optimized `predict` method and specific structural learning for `fit` (Naïve Bayes, TAN, Chow-Liu tree, and others
      learning aGrUM's algorithms). Several discretization methods are implemented.
    - minor graphical improvements.
    - remove old deprecated class/method (since pyAgrum 0.12.0).
    - Improved documentation (readthebook).
    - bugfixes.

## Changelog for 0.18.2

Mainly bugfixes and internal improvements.

- aGrUM
    - bugs fixed for `gum::MarkovNet` and `gum::ShaferShenoyMNInference`.
    - typo in the name of `odbc` library for mac.

- pyAgrum
    - packages for `python 3.9` (except win32).
    - better error message for `DuplicateElement` in operations between `gum.Potential`.
    - [internal] improvements for builded wheels.
    - deprecated `PyEval_CallObject`.
    - [internal] improvements for `pyAgrum`'s tests.

## Changelog for 0.18.1

- aGrUM
    - Direct access to `gum::<graphicalmodel>::isIndependent(X,Y,Z)`.
    - Direct access to direct access to `ancestors` and `descendants()`.
    - Update API with node names for `putFirst`/`reorganize`/`VI`/`I`.

- pyAgrum
    - bug fixed on wrapped {Edge|Arc}Part (thanks to Arthur Esquerre-Pourtère).
    - bug fixed for some UTF8 names.
    - Direct access to `gum::<graphicalmodel>::isIndependent(X,Y,Z)`.
    - Direct access to `ancestors()` and `descendants()`.
    - Update API with node names for `putFirst`/`reorganize`/`VI`/`I`.

## Changelog for 0.18.0

- aGrUM
    - MarkovNet's model, UAI file format and inference (incremental ShaferShenoy).
    - Bug fix in `MIIC` learning algorithm.
    - Bug fix in `gum::GammaLog2` approximations for very small values.
    - Updated and enhanced `gum::GraphicalBNComparator`.
    - Enhanced API for `gum::MixedGraph` (build a MixedGraph from other graphs).
    - API changes for `gum::MultiDimAggregator` (consistant behavior without parent).
    - new `gum::MultidimmAggegator` : `Sum`.
    - Minor API changes for `gum::Potential` (`normalizeAsCPT`,`minNonZero`,`maxNonOne`).
    - Minor API changes for graphical models (`gum::DAGModel` and `gum::UGModels`).
    - [internal] adopted more classical convention for named pr{otected|ivate} methods and attributes.
    - [internal] Updated sources for MVSC 2019.

- pyAgrum
    - MarkovNet's model, UAI file format and inference (incremental ShaferShenoy).
    - Bug fix in `MIIC` learning algorithm.
    - Updated and enhanced `pyAgrum.GraphicalBNComparator`.
    - Enhanced API for `pyAgrum.MixedGraph` (build a MixedGraph from other graphs).
    - API changes for `pyAgrum::MultiDimAggregator` (consistent behavior without parent).
    - new `pyAgrum::MultidimmAggegator` : `Sum`.
    - Minor API changes for `pyAgrum::Potential` (`normalizeAsCPT`,`minNonZero`,`maxNonOne`).
    - In `gum.lib.bn2roc` : bugfix for ROC, access to significant_digit for `predict`, add Precision-Recall graph.

## Changelog for 0.17.3

- aGrUM
    - improved version of MIIC's learning algorithm.
    - add access to pseudo count with `gum::BNLearner::pseudoCount`.
    - fix a bug in inference with `gum::InfluenceDiagram`.
    - improved API for Influence Diagram : accessor with variable names (instead of only NodeId).
    - VS2019's compiler is now supported by `act/CMakeLists.txt`.
    - reorganized resources for testsuite.

- pyAgrum
    - improved version of MIIC's learning algorithm.
    - add access to pseudo count with `gum.BNLearner.pseudoCount`.
    - fix a bug in inference with `gum.InfluenceDiagram`.
    - improved API for Influence Diagram : accessor with variable names (instead of only NodeId).

## Changelog for 0.17.2

- aGrUM
    - fix a bug in graphChangeGeneratorOnSubDiGraph (thanks @yurivict for the issue).
    - fix a bug in LazyPropagation due to a (rare) improper optimization.
    - improve projection and combination codes for `MultiDim` hierarchy.
    - reorganization of source codes and internal structure.
    - refreshed a bit the c++ examples.
    - preliminary works on undirected graphical models.

- pyAgrum
    - forgotten description for pip packages.
    - typo for special char (':' for instance) with pydotplus.

## Changelog for 0.17.1

(really) minor patch

- aGrUM
    - O3PRMBNReader can now read a BN from an o3prm file with a unique class even it the name of the class is not the
      name of the file.

- pyAgrum
    - internal changes for wheel generations.
    - updated description for packages.
    - optimized loops and inference for ROC and classifiers.
    - minor improvements for pyAgrum's tests (logged and restrictions for python2).

## Changelog for 0.17.0

- pyAgrum
    - Update requirements
    - Improve `classifier.py`
    - Documentation improvements
    - Remove '?' from names in some resources files concerned Asia
    - Updated API for `pyAgrum.Instantiation` (accessor used name of variables)
    - Important internal updates for the relation between `numpy.array` and `pyAgrum.Potential` (with a significant
      speed-up)
    - Add operators between `pyAgrum.Potential` and numbers
    - Fix a bug when used `pydotplus` with `size=None`
    - Fix minor bugs in `pyAgrum.fastBN` and in `pyAgrum.O3prmBNWriter/Reader`
    - Add `pyAgrum.Potential.log2()` method and `pyAgrum.log2(pyAgrum.Potential)` function
    - Add `pyAgrum.BayesNet.clear()` method

- aGrUM
    - Updated API for `gum::Instantiation` (accessor used name of variables)
    - Add operators between `gum::Potential<GUM_SCALAR>` and `GUM_SCALAR`
    - Fix minor bugs in `gum::fastPrototype` and in `gum::O3prmBNWriter/Reader`
    - Add `gum::multiDimDecorator::erase(std::string& name)` (mainly used as `gum::Potential::erase(std::string& name)`)
    - Add `gum::BayesNet<GUM_SCALAR>::clear()` method

## Changelog for 0.16.4

- pyAgrum
    - first version of `pyAgrum.lib.classifier` providing a class `pyAgrum.BNClassifier` wrapping a BN as a classifier
      with a scikitlearn-like API.
    - Fix bug in `pyAgrum.lib.notebook.showPotential` with explicit digit param
    - Add a `pyAgrum.Potential.loopIn()` to iterate inside a Potential
    - Enhanced API for `pyAgrum.InfluenceDiagram`
    - Documentation improvements
    - remove package for python 3.4 and 3.5 (
      following [NEP29](https://numpy.org/neps/nep-0029-deprecation_policy.html)). But 2.7 is still maintained.
    - add package for python 3.8

- aGrUM
    - still worked on CI
    - Enhanced API for `gum::InfluenceDiagram`

## Changelog for 0.16.3

- pyAgrum
    - wrapper for the class `gum::BayesNetFragment`
    - typos in dot methods for Influence Diagrams and Causal Models

## Changelog for 0.16.2

- pyAgrum
    - remove the use of 'f-strings' in `pyAgrum.lib.notebook.py`

## Changelog for 0.16.1

- aGrUM
    - improve the syntax for BN specification used `gum::fastPrototype`
    - improve several CMakeFiles.txt and doxygen documentation
    - add CI for python 2.7
    - refresh `gum::BayesNet::toString()`
    - API change : `gum::MarkovBlanket()::{mb()`->`gum::MarkovBlanket::dag()}`

- pyAgrum
    - `pyAgrum.config` object for customization (see notebook 08-configForPyAgrum)
    - improved the syntax for BN specification used `pyAgrum.fastBN`
    - improved pyAgrum's documentation
    - add `pyAgrum.causal.counterfactual` and `pyAgrum.causal.counterfactualModel` (see notebook
      55-Causality_Counterfactual)
    - improve plots for histograms (particularly : now svg image by default)
    - add `pyAgrum.Potential.fullWithFunction()`
    - add `pyAgrum.{any graph-like}.connectedComponents()`
    - add `pyAgrum.BayesNet.parents()` and `pyAgrum.BayesNet.children()` with the name of node.
    - `pyAgrum.MarkovBlanket(bn,var,level)` build now a level-th order Markov Blanket (by default level=1)
    - add access to constants `pyAgrum.VarType_{Discretized|Labelized|Range}` when tested
      `pyAgrmu.{any discrete variable}.varType()`
    - API change : `pyAgrum.MarkovBlanket().{mb()`->`dag()}`
    - add  `pyAgrum.lib.bn_vs_bn.GraphicalBNComparator.hamming()`

## Changelog for 0.16.0

- aGrUM
    - bug fixed in `gum::BNLearner::learnParameters()` in some cases with ML estimation (without priors). Better
      bootstrap for EM.
    - bug fixed for variables order in the `gum::Potential` generated by `gum::BayesNet::reverseArc()`
    - new `gum::DiGraph::hasDirectedCycle`
    - new methods in `gum::Potential` for random generations and pertubations

- pyAgrum
    - improvements in `pyAgrum.lib.causality` (direct access to backdoor and frontdoor, typos in doCalculus, etc.)
    - `pyAgrum.lib.notebook`'s visualisations of graph can be adapted for dark or light themes (in jupyterlab for
      instance) with `forDarkTheme()`and `forLightTheme()`
    - `pyAgrum.lib.notebook.{show|get}Inference()` can now have colored arcs (see 06-colouringBNs.ipynb in the
      notebooks)
    - improvements on the documentation framework (w.r.t. readthdocs)
    - bug fixed for variables order in the `pyAgrum.Potential` generated by `pyAgrum.BayesNet.reverseArc()`
    - new `pyAgrum.DiGraph.hasDirectedCycle`
    - new methods in `pyAgrum.Potential` for random generations and pertubations
    - better error messages when added arcs in a Bayesian Network
    - API change for joint targets in exact inference : only as set of node ids or names of variable

## Changelog for 0.15.2

- aGrUM
    - fix a (rare) bug in counters for contingency tables for chi2 and G2 (bug found by Bastien Chassagnol)
- pyAgrum
    - fix `fscore` in `gumlib/bn_vs_bn.py`
    - API :  Wherever a list of strings is the type for an argument, a single stred `"x"` can be used instead of `["x"]`
    - workaround for weird bug when displayed matplotlib-generated svg in notebooks (for `gnb.showInference`)
    - fix AUC computation in  `gumlib/bn2roc.py`

## Changelog for 0.15.1

- aGrUM
    - add forgotten `addPossibleEdge` constraint for `LocalSearchWithTabuList` learning algorithm in `BNLearner`.
    - bug fix in exact inference leaded to an erroneous exception.
    - Better iterated random test for sampled inference
- pyAgrum
    - update path for new notebooks location for CI and documentation
    - nightly build with pip : use now `pip install pyAgrum-nightly`
    - API change in `pyAgrum.lib.dynamicBN` : `plotFollowUnrolled(lovars, dbn, T, evs)`
    - add forgotten `addPossibleEdge` constraint for `LocalSearchWithTabuList` learning algorithm in `BNLearner`.
    - bug fix in exact inference leaded to an erroneous exception.
- internal
    - new values for __act__ `-j` option (number of jobs for compilation) : `all`, `except1`, `half`, `halfexcept1`.
    - several bugfixes in CI

## Changelog for 0.15.0

- ![LGPLV3](https://www.gnu.org/graphics/lgplv3-with-text-95x42.png "LGPLV3") new LGPL3 licence for aGrUM/pyAgrum
- aGrUM
    - bug fix with openMP in `BNLearner::setDatabaseWeight`
    - new `BNLearner::recordWeight()` and `BNLearner::databaseWeight()`
    - new `BNLearner::setRecordWeight()`
- pyAgrum
    - nightly builds with pip : `pip install pyAgrum-nightly`
    - bug fix with openMP in `BNLearner::setDatabaseWeight`
    - new `BNLearner.recordWeight()` and `BNLearner.databaseWeight()`
    - new `BNLearner.setRecordWeight()`
    - bug fix and minor API changes in `pyAgrum.causality`

## Changelog for 0.14.3

- aGrUM
    - new constraint for structural learning : `possibleEdge` forces the tested edges to be taken from the sets of
      `possibleEdge`s
    - new methods `BNLearner::addPossibleEdge(const gum::Edge&)` and
      `BNLearner::setPossibleSkeleton(const gum::UndiGraph&)`
    - Fix a bug in `gum::IndepTestG2`
    - Access to scores from BNLearner : `BNLearner::G2()`
- pyAgrum
    - new methods `BNLearner.addPossibleEdge(x,y)` and `BNLearner.setPossibleSkeleton(undigraph)`
    - fix a bug in causality's identification algorithm
    - Access to scores from BNLearner : `BNLearner.G2()`
    - add tests and fix typos in notebooks

## Changelog for 0.14.2

- aGrUM
    - bug fixes in learning (`3off2/miic` and `learnMixedGraph`)
    - removed redundant `gum::BNLearner::setAprioriWeight`
- pyAgrum
    - `pyAgrum.lib.notebook.showInference` can now use `svg` format
    - use of the `svg` format by default for graphs and drawings in `pyAgrum.lib.notebook`
    - refreshed notebooks
    - removed redundant `pyAgrum.BNLearner.setAprioriWeight`
    - added forgotten wrapper for `pyAgrum.BNLearner.useBDeuPrior`
    - changed the representation of causal model (special node for latent variable)
    - extended documentation

## Changelog for 0.14.1

- aGrUM
    - chaintool for compilation with microsfot visual C++ 17 (`act --msvc17` and `act --msvc17_32`)
- pyAgrum
    - fixed a missed importation of the `pyAgrum.causal` module in pypi packages
    - updated sphynx version for pyAgrum's ReadTheDoc

## Changelog for 0.14.0

- aGrUM
    - support for mingw64 + bugfix for mingw (`act --mingw64`)
    - Access to scores from BNLearner : `BNLearner::Chi2` and `BNLearner::logLikelihood`
    - bug fix in `KL[...]::bhattacharya`
    - add `KL[...]::jsd` (Jensen-Shannon divergence)
    - renamed `gum::[...]]KL` classes into `gum::[...]distance` because they provide access to KL but also to Hellinger,
      Bhattacharya distances and Jensen-Shanon divergence.
- pyAgrum
    - `pyAgrum.causality` (do-calculus and causal identification !)
    - `JunctionTreeGenerator` (formerly `JTGenerator`) can now expose the eliminationOrder and can drive the
      triangulation with a partial order of the nodes.
    - Access to scores from BNLearner : `BNLearner::Chi2` and `BNLearner::logLikelihood`
    - bug fix in `pyAgrum.lib.notebook`
    - bug fix in `KL[...]::bhattacharya`
    - add `KL[...]::jsd` (Jensen-Shannon divergence)
    - renamed `pyAgrum.[...]]KL` classes into `pyAgrum::[...]distance` because they provide acces to KL but also to
      Hellinger, Bhattacharya distances and Jensen-Shanon divergence.
    - fix some scratches in pyAgrum documentation

## Changelog for 0.13.6

- aGrUM
    - Compilation issue for clang4 fixed
    - remove all pre-compiled `float` instanciations of aGrUM's templates (and significantly reduce the size of all
      libraries)
    - add the configuration files needed for interactive notebooks on mybinder.org
- pyAgrum
    - minor changes in notebooks

## Changelog for 0.13.5

- aGrUM
    - fix errors for gcc 4.8.2
    - fix issue <https://gitlab.com/agrumery/aGrUM/issues/23>
    - fix act error for python<3.6

## Changelog for 0.13.4

- pyAgrum
    - minor API changes
    - minor changes in documentation
    - BNLearner follows the new learning framework
    - parametric EM !! See notebook <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/16-ParametriceEM.ipynb>
    - New method : JointTargetedInference.jointMutualInformation for any set of variables in the BN
- aGrUM
    - parametric EM !!
    - several internal improvements
    - learning: major update of the scores, independence tests and record counters:
      They can now be used on subsets of databases (e.g., for cross validation), the ids of the nodes need not
      correspond to indices of columns in the database. The interfaces of these classes have been simplified.
    - learning: all the scores have been speeded-up
    - learning: new score fNML has been introduced
    - learning: Dirichlet prior has been improved: the variables in its database need not be in the same order as those
      of the learning database
    - learning: all the score-related testunits have been improved
    - learning: the documentations of the scores have been improved
    - learning: the corrected mutual information of 3off2 has been improved
    - BNLearner: now supports cross validation
    - New method : JointTargetedInference::jointMutualInformation for any set of variables in the BN

## Changelog for 0.13.3

- pyAgrum
    - __pip__ : wheels for mac/windows/linux for python 2.7,3.{4-7}
    - __anaconda__ : compilation for maxOS/anaconda64/python3 should be fixed
    - updated tests
    - updated pyAgrum.lib
    - updated posterior histograms for notebooks (added mean/stdev for `RangeVariable` and `DiscretizedVariable`)
    - new functions for coloured and graphically compared BNs
    - improved documentation
- aGrUM
    - fixed bugs for `DiscreteVariable` with `domainSize()`<=1 (particularly when added in `Potential`)
    - improved `CMakeFiles.txt`
    - improved documentation
    - fixed `UAI` format for read and write
    - `BNLearner.setSliceOrder` with list of list of names (and not only with ids)
    - improved error messages
    - fixed `learnParameters`
    - multi-thread support for learning

## Changelog for 0.13.2

- aGrUM/pyAgrum
    - fixed bugs in `Potential::fillWith`
    - removed unsafe and ambiguous `Potential::fastKL` and kept safe `Potential::KL`

## Changelog for 0.13.1

- aGrUM
    - variable: new methods to set bounds with doubles in `ContinuousVariable`
    - Changed the code of `Instantiation`'s hash functions to make it compliant with windows mingw implementation
- TestUnits
    - fixed bug in `RawDatabaseTable` test unit

## Changelog for 0.13.0

- aGrUM
    - inference: Loopy Belief Propagation (`LBP`)
    - inference: new approximated inference : `Monte-Carlo`/`Importance`/`Weighted Sampling` + the same used LBP as a
      Dirichlet prior (`Loopy...`).
    - learning: new algorithm 3off2 and miic
    - learning: new database handled framework (allows for coped with missed values and with different types of
      variables)
    - learning: possibility to load data from nanodbc databases (e.g., `postgres`, `sqlite`)
    - learning: add a progress Listener/Signaler in `BNDatabaseGenerator`
    - potential: API extension (`findAll`,`argmax`,`argmin`,`fillWith(pot,map)`)
    - variable: new constructor for `LabelizedVariable` with labels as vector of stred + `posLabel(std::string)`
    - variable: new constructor with vector of ticks for `gum::DiscretizedVariable`
    - graph: API extension (`addNodes(n)`)
    - graph: API change (`addNode(id)`->`addNodeWithId(id)`)
    - Changes and bug fixe in in BIF and NET writer/reader
- pyAgrum
    - wheels for python 3.3 and 3.4
    - access to the new learning framework used `BNLearner`
    - access to the new inference algorithms
    - new methods `Instantiation.fromdict` and `Instantiation.todict`
    - `DiscreteVariable.toDiscretized/toLabelized/toRange` copy the variable instead of gived a (not readonly) reference
- O3PRM
    - new syntax for types
    - read and write Bayesian Network with O3PRM syntax
- Documentations
    - agrum : doxygen helps structure and howtos
    - pyAgrum : documentation of a large part of pyAgrum's API, export to <https://pyagrum.readthedocs.io>
    - o3prm : still in progress (see <https://o3prm.lip6.fr>, <https://o3prm.readthedocs.io>)
- act
    - new command guideline for a few easy checks
- many bug fixes

## Changelog for 0.12.1

Minor bug fixes and improvements

## Changelog for 0.12.0

- API
    - new class `EssentialGraph`
    - new class `MarkovBlanket`
    - improved targets in `MarginalTargettedInference`
- pyAgrum
    - update notebooks
    - new swig-based documentation framework
    - transparent background for dot graphs
    - more windows-compliant agrum.lib.bn2csv
- aGrUM
    - PRM bug fixes
    - improved CI in gitlab
    - improved exception messages in BN learning and O3PRM
    - improved act

## Changelog for 0.11.2

- aGrUM
    - a lot of internal changes for CI in gitlab (especially for future automatic generation of wheels)
    - learning: correct identification of stred labels beginned with digits
    - learning: labels from CSV are now alphabetically sorted
    - fix an issue with sql.h
- pyAgrum
    - notebooks as tests (now in wrappers/pyAgrum/notebooks)
    - updated requirements
    - some improvements in doc
    - pyagrum.lib.ipython: emulation of 'pyagrum.lib.notebook' for ipython graphical console (within spyder for
      instance)
    - pyagrum.lib.bn2csv: csv file with labels of variables instead of index (parameter with_labels:boolean)
    - pyagrum.lib.bn2roc: use a csv with labels by default (parameter with_labels:boolean)

## Changelog for 0.11.1

- 2 typos found in pyAgrum.lib.notebook

## Changelog for 0.11.0

- internal
    - worked on continuous integration with gitlab
    - aGrUM/pyAgrum to be compilable with g++-4.8
    - aGrUM/pyAgrum to be compilable with win32
    - pyAgrum wheels generation used act for 'pip' tool
- aGrUM
    - removed some unused data structure (`AVLTree`)
    - fixed bug in `localSearchWithTabuList` learning class
    - Remove wrong parallel estimations for learning (now correct but sequential)
    - worked on docs
    - API change : add `BayesNet::minimalCondSet(NodeSet&,NodeSet&)` (migration from pyAgrum to aGrUM)
    - API change : add JointTargettedInference::evidenceJointImpact()
- pyAgrum
    - API changes : pyAgrum.lib.bn2graph (`BN2dot`, `BNinference2dot`, `proba2histo`)
    - API changes : pyAgrum.lib.pretty_print (`bn2txt`, `cpt2txt`)
    - API changes : pyAgrum.lib.notebook : uniformed parameters evs (first) and targets (second) order.
    - API changes : pyAgrum.lib.notebook : `showEntropy->showInformation`
    - updated sphinx help generation
    - fix `CNMonteCarloSampling` not recognized as `ApproximationScheme`
    - enhanced `showInformation` with Mutual Information on arcs
    - API change : added `BayesNet.minimalCondSet(set_of_targets,set_of_evs)` (as wrapper)
    - API change : added `LazyInference.evidenceJointImpact(set_of_targets,set_of_evs)`

## Changelog for 0.10.4

- Add new approximated inference : `LBP` (aGrUM and pyAgrum)
- Fix bugs in `LazyPropagation` and `ShaferShenoy` inference
- Refresh some codes in learning module
- Update (and simplify) CMakeLists.txt for new swig 3.0.11
- Add some project files (included this CHANGELOG.md)
- Refresh pyAgrum notebooks with matplotlib2

## Changelog for 0.10.3

- Only bug fixes in tests

## Changelog for 0.10.2

- New method for `BayesNet` : `minimalCondSet`
- New method for all inference : `evidenceImpact`
- Potential has a (single) value even if no dimension.
- Bug fix for `LazyPropagation`
- Typos for Visual C++ compiler
- Many internal changes

## Changelog for 0.10.1

- aGrUM
    - Fix GCC compilation
    - `ParamEstimator::setMaxThread` new method
- pyAgrum
    - `VariableElimination` and `ShaferShenoy` inference
    - new `addJointTarget` and `jointPosterior` methods for exact inference
    - `pyAgrum.getPosterior` now uses `VariableElimination`
    - Fix pyAgrum.lib.notebook error for python2
    - pyAgrum now linked with static library aGrUM
    - pyAgrum.so (linux) size significantly reduced

## Changelog for 0.10.0

- aGrUM
    - Improvements in inference : New target/evidence-driven incremental inference scheme used relevant reasoned used by
      Lazy/Shafer-Shenoy/Variable Elimination algorithms. Relevant reasoned leads to a major improvement of the
      inference (
      see [RelevanceReasoning.html](http://www-desir.lip6.fr/~phw/aGrUM/officiel/notebooks/RelevanceReasoning.html)).
- pyAgrum
    - LazyPropagation API follow the new inference scheme (add/removeTarget, add/remove/chgEvidence)
- Installers used pip or anaconda.

## Changelog for 0.9.3

Tag 0.9.3 has not been properly announced. Still, many changes in this release :

- Many bug fixes and API glitch/improvement
    - Many internal reorganisations (compilation, test, jenkins, etc.)
    - Many change in the C++ code in order to be more c++11/14
    - Bug fix in learning
    - Many Doxygen improvements
    - Many refactors and bug fix in PRM
- Improvements
    - dynamic BN in pyAgrum
    - nanodbc support for pyAgrUM
    - O3PRMBNReader in pyAgrum (read a prm to a BN)
    - PRMExplorer in pyAgrum
    - UAI reader/writer for BayesNet
    - Algebra of potentials (operators on Potential)
    - pyAgrum.lib.notebook refactored and simplified
    - updated lrs version for credal networks
- Windows
    - aGrUM/pyAgrum compilation on windows used Visual Studio 2015

## Changelog for 0.9.2

- aGrUM
    - Improvements in Inference
        - old LazyPropagation renamed JunctionTreeInference,
        - Improved LazyPropagation ~30% faster,
        - Bug fix and other improvements for relevance reasoned features.
    - Improvements for Probabilistic Relational Models
        - model refinements : e.g. parameterized classes, specification of CPTs used formula, etc.
        - bug fixes and other improvements in dedicated inference algorithms,
        - improved and fixed documentations
        - new file format for Bayesian network : o3prmBNReader (readed a BN by grounded a system)
    - learning API still improved
        - BNLearner templatized
        - new feature for BNLearner : used a BN to specicfy variables and their modalities,
        - bug fixes and improvement for parameter learning.
    - other bug fixes and improvements in aGrUM architecture
        - aGrUM g++5.1-ready
        - etc.
- pyAgrum
    - small bugs fixed and reorganisation

## Changelog for 0.9.1

- aGrUM
    - Improvement in learning algorithms
    - learning from databases with fewer rows than there are processors
    - method to BNLearner to learn parameters from a BN's DAG
    - static lib compilation for aGrUM
    - bug fixes and other improvements
- pyAgrum
    - Compiled for Python 3 or Python 2 (default is python3, python2 if no python3.). New option for act to choose which
      python : --python={2|3}.
    - gumLib has moved and changed its name (in the pyAgrum package) : pyAgrum.lib
    - Improved API for learning (changeLabel/parameter learning/ etc.)
    - Improved graphs manipulation
    - bug fixes and other improvements

## Changelog for 0.9.0

Aside from many bug fixes and general improvements such as performance optimizations in various areas, some changes are
especially noteworthy:

- Functionality : Structural and parameter learning for Bayesian networks
- Model : Credal Networks, FMDP used Multi-Valued Decision Diagrams
- Language : migration to modern C++(11/14)
- Core : Improvements and optimization of basic data structures in aGrUM/core
