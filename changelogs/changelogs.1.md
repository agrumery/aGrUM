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
