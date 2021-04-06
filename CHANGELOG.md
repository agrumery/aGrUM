# aGrUM Changelog

## Changelog for 0.20.0

0.20.0 is the last minor release before 1.0.0. 

* aGrUM
  * Workaround for OMP with MVSC
  * Refreshing doxygen configuration file
  * Graph methods for `children`and `parents` of sets of nodes.
  * Renaming `core/math/math.h` to `core/math/math_utils.h` to avoid clash names and false warnings from linters
  * work on CIs
  * Fix and typos from F.Keidel
  * Improving API of `gum::BayesNetFragment` (for instance, non-implemented `gum::BayesNetFragment::VariableNodeMap`).
  * Major changes (and typos and bug fixes) in `gum::CN::CredalNetwork`'s API
  * Fixing minor bugs in inference for `gum::CN::CredalNet`
  * (internal) re-organizing files and folders for Credal Networks
  * (internal) fixing bug in organization of inline/tpl/source files for `gum::credal::lp::LpInterface`

* pyAgrum
  * (internal) Better logic and automatic generation for the multiple "requirements.txt".
  * Graph methods for `children`and `parents` of sets of nodes.
  * `pyAgrum.notebook.export` and `pyAgrum.notebook.exportInference` to export as png, pdf(, etc.) PGM and inference in PGM
  * Fix several tests
  * Fix and typos from F.Keidel
  * Adding `gum.Instantiation.addVarsFromModel` and allowing chained `gum.Instantiation.add()`.
  * Fixing some broken links in documentation.
  * Updating `gum.skbn` for non-binary classifier (see notebooks).
  * Improving notebooks for classifiers.
  * Major changes in `gum.CredalNetwork`'s API
  * Specific visualisation for credal networks
  * Graphical visualisation of inference with credal networks (![Visual Credal networks](https://gitlab.com/agrumery/aGrUM/-/blob/master/wrappers/pyAgrum/doc/sphinx/_static/fastModelsSource/5-fastCNWithPyAgrum.png "Credal Networks"))
  * Adding some example for credal networks in notebooks

## Changelog for 0.19.3

* pyAgrum
  * missing graphical  (not correctly wrapped) methods in `gum::InfluenceDiagram` 
  * fix falsely raised exception leading to incomplete generation of documentation and wheels.
  * ` pyAgrum.lib.ipython` improved.
  * pyAgrum's documentation refreshed a bit.
  * `pyAgrum.skbn` improved.
  * several typos in notebooks and testsuites.

## Changelog for 0.19.2

* aGrUM
  * bugfix for `EssentialGraph` (thanks to M.Lasserre).

## Changelog for 0.19.1

* aGrUM
  * bugfix for `InfluenceDiagram` with all-negative utilities (thanks to B.Enderle).
  * [internal] typos and reorganization for `act`'s modules.

## Changelog for 0.19.0

*Mainly* : important changes for Influence Diagram (aGrUM and pyAgrum) and for BayesNet classifiers compliant to scikit-learn's API (pyAgrum). 

* aGrUM
  * new and better inference for Influence Diagrams and LIMIDs (`gum::ShafeShenoyLIMIDInference`).
  * new builder for Influence Diagram `gum::InfluenceDiagram::fastPrototype`.
  * bugfixes.
  
* pyAgrum
  * wrapper and notebook functions for new inference and new methods for influence diagram.
  * new module `skbn` for BayesNet classifier compatible with sklearn (classification and discretization)  with optimized `predict` method and specific structural learning for `fit` (Naïve Bayes, TAN, Chow-Liu tree, and others learning aGrUM's algorithms). Several discretization methods are implemented.
  * minor graphical improvements.
  * remove old deprecated class/method (since pyAgrum 0.12.0).
  * Improving documentation (readthebook).
  * bugfixes.

## Changelog for 0.18.2

Mainly bugfixes and internal improvements.

* aGrUM
  * bugs fixed for `gum::MarkovNet` and `gum::ShaferShenoyMNInference`.
  * typo in the name of `odbc` library for mac.

* pyAgrum
  * packages for `python 3.9` (except win32).
  * better error message for `DuplicateElement` in operations between `gum.Potential`.
  * [internal] improvements for building wheels.
  * deprecated `PyEval_CallObject`.
  * [internal] improvements for `pyAgrum`'s tests.


## Changelog for 0.18.1

* aGrUM
  * Direct access to `gum::<graphicalmodel>::isIndependent(X,Y,Z)`.
  * Direct access to direct access to `ancestors` and `descendants()`.
  * Update API with  node names for `putFirst`/`reorganize`/`VI`/`I`.

* pyAgrum
  * bug fixed on wrapped {Edge|Arc}Part (thanks to Arthur Esquerre-Pourtère).
  * bug fixed for some UTF8 names.
  * Direct access to `gum::<graphicalmodel>::isIndependent(X,Y,Z)`.
  * Direct access to `ancestors()` and `descendants()`.
  * Update API with  node names for `putFirst`/`reorganize`/`VI`/`I`.

## Changelog for 0.18.0

* aGrUM
  * MarkovNet's model, UAI file format and inference (incremental ShaferShenoy).
  * Bug fix in `MIIC` learning algorithm.
  * Bug fix in `gum::GammaLog2` approximations for very small values.
  * Updating and enhancing ` gum::GraphicalBNComparator`.
  * Enhancing API for `gum::MixedGraph` (build a MixedGraph from other graphs).
  * API changes for `gum::MultiDimAggregator` (consistant behavior without parent).
  * new `gum::MultidimmAggegator` : `Sum`.
  * Minor API changes for `gum::Potential` (`normalizeAsCPT`,`minNonZero`,`maxNonOne`).
  * Minor API changes for graphical models (`gum::DAGModel` and `gum::UGModels`).
  * [internal] adopting more classical convention for naming pr{otected|ivate} methods and attributes.
  * [internal] Updating sources for MVSC 2019.

* pyAgrum
  * MarkovNet's model, UAI file format and inference (incremental ShaferShenoy).
  * Bug fix in `MIIC` learning algorithm.
  * Updating and enhancing `pyAgrum.GraphicalBNComparator`.
  * Enhancing API for `pyAgrum.MixedGraph` (build a MixedGraph from other graphs).
  * API changes for `pyAgrum::MultiDimAggregator` (consistent behavior without parent).
  * new `pyAgrum::MultidimmAggegator` : `Sum`.
  * Minor API changes for `pyAgrum::Potential` (`normalizeAsCPT`,`minNonZero`,`maxNonOne`).
  * In `gum.lib.bn2roc` : bugfix for ROC, access to significant_digit for `predict`, add Precision-Recall graph. 

## Changelog for 0.17.3

* aGrUM
  * improved version of MIIC's learning algorithm.
  * add access to pseudo count with `gum::BNLearner::pseudoCount`.
  * fix a bug in inference with `gum::InfluenceDiagram`.
  * improved API for Influence Diagram : accessor with variable names (instead of only NodeId).
  * VS2019's compiler is now supported by `act/CMakeLists.txt`.
  * reorganizing resources for testsuite.

* pyAgrum
  * improved version of MIIC's learning algorithm.
  * add access to pseudo count with `gum.BNLearner.pseudoCount`.
  * fix a bug in inference with `gum.InfluenceDiagram`.
  * improved API for Influence Diagram : accessor with variable names (instead of only NodeId).

## Changelog for 0.17.2

* aGrUM
  * fix a bug in graphChangeGeneratorOnSubDiGraph (thanks @yurivict for the issue).
  * fix a bug in LazyPropagation due to a (rare) improper optimization.
  * improve projection and combination codes for `MultiDim` hierarchy.
  * reorganization of source codes and internal structure.
  * refreshing a bit the c++ examples.
  * preliminary works on undirected graphical models.

* pyAgrum
  * forgotten description for pip packages.
  * typo for special char (':' for instance) with pydotplus.


## Changelog for 0.17.1
(really) minor patch

* aGrUM
  * O3PRMBNReader can now read a BN from an o3prm file with a unique class even it the name of the class is not the name of the file.

* pyAgrum
  * internal changes for wheel generations.
  * updating description for packages.
  * optimizing loops and inference for ROC and classifiers.
  * minor improvements for pyAgrum's tests (logging and restrictions for python2).

## Changelog for 0.17.0

* pyAgrum
  * Update requirements
  * Improve `classifier.py`
  * Documentation improvements
  * Remove '?' from names in some resources files concerning Asia
  * Updating API for `pyAgrum.Instantiation` (accessor using name of variables)
  * Important internal updates for the relation between `numpy.array` and `pyAgrum.Potential` (with a significant speed-up)
  * Add operators between `pyAgrum.Potential` and numbers
  * Fix a bug when using `pydotplus` with `size=None`
  * Fix minor bugs in `pyAgrum.fastBN` and in `pyAgrum.O3prmBNWriter/Reader`
  * Add `pyAgrum.Potential.log2()` method and `pyAgrum.log2(pyAgrum.Potential)` function
  * Add `pyAgrum.BayesNet.clear()` method

* aGrUM
  * Updating API for `gum::Instantiation` (accessor using name of variables)
  * Add operators between `gum::Potential<GUM_SCALAR>` and `GUM_SCALAR`
  * Fix minor bugs in `gum::fastPrototype` and in `gum::O3prmBNWriter/Reader`
  * Add `gum::multiDimDecorator::erase(std::string& name)` (mainly used as `gum::Potential::erase(std::string& name)`)
  * Add `gum::BayesNet<GUM_SCALAR>::clear()` method
  
## Changelog for 0.16.4

* pyAgrum
  * first version of `pyAgrum.lib.classifier` providing a class `pyAgrum.BNClassifier` wrapping a BN as a classifier with a scikitlearn-like API.
  * Fix bug in `pyAgrum.lib.notebook.showPotential` with explicit digit param
  * Add a `pyAgrum.Potential.loopIn()` to iterate inside a Potential
  * Enhanced API for `pyAgrum.InfluenceDiagram`
  * Documentation improvements
  * remove package for python 3.4 and 3.5 (following [NEP29](https://numpy.org/neps/nep-0029-deprecation_policy.html)). But 2.7 is still maintained.
  * add package for python 3.8

* aGrUM
  * still working on CI
  * Enhanced API for `gum::InfluenceDiagram`
  
## Changelog for 0.16.3

* pyAgrum
  * wrapper for the class `gum::BayesNetFragment`
  * typos in dot methods for Influence Diagrams and Causal Models

## Changelog for 0.16.2

* pyAgrum
  * remove the use of 'f-strings' in `pyAgrum.lib.notebook.py`

## Changelog for 0.16.1

* aGrUM
  * improve the syntax for BN specification using `gum::fastPrototype`
  * improve several CMakeFiles.txt and doxygen documentation
  * add CI for python 2.7
  * refresh `gum::BayesNet::toString()`
  * API change : `gum::MarkovBlanket()::{mb()`->`gum::MarkovBlanket::dag()}`

* pyAgrum
  * `pyAgrum.config` object for customization (see notebook 08-configForPyAgrum)
  * improving the syntax for BN specification using `pyAgrum.fastBN`
  * improving pyAgrum's documentation
  * add `pyAgrum.causal.counterfactual` and `pyAgrum.causal.counterfactualModel` (see notebook 55-Causality_Counterfactual)
  * improve plots for histograms (particularly : now svg image by default)
  * add `pyAgrum.Potential.fullWithFunction()`
  * add `pyAgrum.{any graph-like}.connectedComponents()`
  * add `pyAgrum.BayesNet.parents()` and `pyAgrum.BayesNet.children()` with the name of node.
  * `pyAgrum.MarkovBlanket(bn,var,level)` build now a level-th order Markov Blanket (by default level=1)
  * add access to constants `pyAgrum.VarType_{Discretized|Labelized|Range}` when testing `pyAgrmu.{any discrete variable}.varType()`
  * API change : `pyAgrum.MarkovBlanket().{mb()`->`dag()}`
  * add  `pyAgrum.lib.bn_vs_bn.GraphicalBNComparator.hamming()`

## Changelog for 0.16.0

* aGrUM
  * bug fixed in `gum::BNLearner::learnParameters()` in some cases with ML estimation (without priors). Better bootstrap for EM.
  * bug fixed for variables order in the `gum::Potential` generated by `gum::BayesNet::reverseArc()`
  * new `gum::DiGraph::hasDirectedCycle`
  * new methods in `gum::Potential` for random generations and pertubations

* pyAgrum
  * improvements in `pyAgrum.lib.causality` (direct access to backdoor and frontdoor, typos in doCalculus, etc.)
  * `pyAgrum.lib.notebook`'s visualisations of graph can be adapted for dark or light themes (in jupyterlab for instance) with `forDarkTheme()`and `forLightTheme()`
  * `pyAgrum.lib.notebook.{show|get}Inference()` can now have colored arcs (see 06-colouringBNs.ipynb in the notebooks)
  * improvements on the documentation framework (w.r.t. readthdocs)
  * bug fixed for variables order in the `pyAgrum.Potential` generated by `pyAgrum.BayesNet.reverseArc()`
  * new `pyAgrum.DiGraph.hasDirectedCycle`
  * new methods in `pyAgrum.Potential` for random generations and pertubations
  * better error messages when adding arcs in a Bayesian Network
  * API change for joint targets in exact inference : only as set of node ids or names of variable

## Changelog for 0.15.2

* aGrUM
  * fix a (rare) bug in counters for contingency tables for chi2 and G2 (bug found by Bastien Chassagnol)  
* pyAgrum
  * fix `fscore` in `gumlib/bn_vs_bn.py`
  * API :  Wherever a list of strings is the type for an argument, a single string `"x"` can be used instead of `["x"]`
  * workaround for weird bug when displaying matplotlib-generated svg in notebooks (for `gnb.showInference`)
  * fix AUC computation in  `gumlib/bn2roc.py`

## Changelog for 0.15.1

* aGrUM
  * add forgotten `addPossibleEdge` constraint for `LocalSearchWithTabuList` learning algorithm in `BNLearner`.
  * bug fix in exact inference leading to an erroneous exception.
  * Better iterated random test for sampling inference
* pyAgrum
  * update path for new notebooks location for CI and documentation
  * nightly build with pip : use now `pip install pyAgrum-nightly`
  * API change in `pyAgrum.lib.dynamicBN` : `plotFollowUnrolled(lovars, dbn, T, evs)`
  * add forgotten `addPossibleEdge` constraint for `LocalSearchWithTabuList` learning algorithm  in `BNLearner`.
  * bug fix in exact inference leading to an erroneous exception.
* internal
  * new values for **act** `-j` option (number of jobs for compilation) : `all`, `except1`, `half`, `halfexcept1`.
  * several bugfixes in CI

## Changelog for 0.15.0

* ![LGPLV3](https://www.gnu.org/graphics/lgplv3-with-text-95x42.png "LGPLV3") new LGPL3 licence for aGrUM/pyAgrum
* aGrUM
  * bug fix with openMP in `BNLearner::setDatabaseWeight`
  * new `BNLearner::recordWeight()` and `BNLearner::databaseWeight()`
  * new `BNLearner::setRecordWeight()`
* pyAgrum
  * nightly builds with pip : `pip install pyAgrum-nightly`
  * bug fix with openMP in `BNLearner::setDatabaseWeight`
  * new `BNLearner.recordWeight()` and `BNLearner.databaseWeight()`
  * new `BNLearner.setRecordWeight()`
  * bug fix and minor API changes in `pyAgrum.causality`

## Changelog for 0.14.3

* aGrUM
  * new constraint for structural learning : `possibleEdge` forces the tested edges to be taken from the sets of `possibleEdge`s
  * new methods `BNLearner::addPossibleEdge(const gum::Edge&)` and `BNLearner::setPossibleSkeleton(const gum::UndiGraph&)`
  * Fix a bug in `gum::IndepTestG2`
  * Access to scores from BNLearner : `BNLearner::G2()`
* pyAgrum
  * new methods `BNLearner.addPossibleEdge(x,y)` and `BNLearner.setPossibleSkeleton(undigraph)`
  * fix a bug in causality's identification algorithm
  * Access to scores from BNLearner : `BNLearner.G2()`
  * add tests and fix typos in notebooks
  
## Changelog for 0.14.2

* aGrUM
  * bug fixes in learning (`3off2/miic` and `learnMixedGraph`)
  * removing redundant `gum::BNLearner::setAprioriWeight`
* pyAgrum
  * `pyAgrum.lib.notebook.showInference` can now use `svg` format
  * use of the `svg` format by default for graphs and drawings in `pyAgrum.lib.notebook`
  * refreshing notebooks
  * removing redundant `pyAgrum.BNLearner.setAprioriWeight`
  * adding forgotten wrapper for `pyAgrum.BNLearner.useAprioriBDeu`
  * changing the representation of causal model (special node for latent variable)
  * extending documentation

## Changelog for 0.14.1

* aGrUM
  * chaintool for compilation with microsfot visual C++ 17 (`act --msvc17` and `act --msvc17_32`)
* pyAgrum
  * fixing a missing importation of the `pyAgrum.causal` module in pypi packages
  * updating sphynx version for pyAgrum's ReadTheDoc
  
## Changelog for 0.14.0

* aGrUM
  * support for mingw64 + bugfix for mingw (`act --mingw64`)
  * Access to scores from BNLearner : `BNLearner::Chi2` and `BNLearner::logLikelihood`
  * bug fix in `KL[...]::bhattacharya`
  * add `KL[...]::jsd` (Jensen-Shannon divergence)
  * renaming `gum::[...]]KL` classes into `gum::[...]distance` because they provide access to KL but also to Hellinger, Bhattacharya distances and Jensen-Shanon divergence.
* pyAgrum
  * `pyAgrum.causality` (do-calculus and causal identification !)
  * `JunctionTreeGenerator` (formerly `JTGenerator`) can now expose the eliminationOrder and can drive the triangulation with a partial order of the nodes.
  * Access to scores from BNLearner : `BNLearner::Chi2` and `BNLearner::logLikelihood`
  * bug fix in `pyAgrum.lib.notebook`
  * bug fix in `KL[...]::bhattacharya`
  * add `KL[...]::jsd` (Jensen-Shannon divergence)
  * renaming `pyAgrum.[...]]KL` classes into `pyAgrum::[...]distance` because they provide acces to KL but also to Hellinger, Bhattacharya distances and Jensen-Shanon divergence.
  * fix some scratches in pyAgrum documentation

## Changelog for 0.13.6

* aGrUM
  * Compilation issue for clang4 fixed
  * remove all pre-compiled `float` instanciations of aGrUM's templates (and significantly reduce the size of all libraries)
  * add the configuration files needed for interactive notebooks on mybinder.org
* pyAgrum
  * minor changes in notebooks

## Changelog for 0.13.5

* aGrUM
  * fix errors for gcc 4.8.2
  * fix issue <https://gitlab.com/agrumery/aGrUM/issues/23>
  * fix act error for python<3.6
  
## Changelog for 0.13.4

* pyAgrum
  * minor API changes
  * minor changes in documentation
  * BNLearner follows the new learning framework
  * parametric EM !! See notebook <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/16-ParametriceEM.ipynb>
  * New method : JointTargetedInference.jointMutualInformation for any set of variables in the BN
* aGrUM
  * parametric EM !!
  * several internal improvements
  * learning: major update of the scores, independence tests and record counters:
    They can now be used on subsets of databases (e.g., for cross validation), the ids of the nodes need not correspond to indices of columns in the database. The interfaces of these classes have been simplified.
  * learning: all the scores have been speeded-up
  * learning: new score fNML has been introduced
  * learning: Dirichlet apriori has been improved: the variables in its database need not be in the same order as those of the learning database
  * learning: all the score-related testunits have been improved
  * learning: the documentations of the scores have been improved
  * learning: the corrected mutual information of 3off2 has been improved
  * BNLearner: now supports cross validation
  * New method : JointTargetedInference::jointMutualInformation for any set of variables in the BN

## Changelog for 0.13.3

* pyAgrum
  * **pip** : wheels for mac/windows/linux for python 2.7,3.{4-7}
  * **anaconda** : compilation for maxOS/anaconda64/python3 should be fixed
  * updating tests
  * updating pyAgrum.lib
  * updating posterior histograms for notebooks (adding mean/stdev for `RangeVariable` and `DiscretizedVariable`)
  * new functions for colouring and graphically comparing BNs
  * improved documentation
* aGrUM
  * fixed bugs for `DiscreteVariable` with `domainSize()`<=1 (particularly when added in `Potential`)
  * improved `CMakeFiles.txt`
  * improved documentation
  * fixing `UAI` format for read and write
  * `BNLearner.setSliceOrder` with list of list of names (and not only with ids)
  * improved error messages
  * fixing `learnParameters`
  * multi-thread support for learning

## Changelog for 0.13.2

* aGrUM/pyAgrum
  * fixed bugs in `Potential::fillWith`
  * removed unsafe and ambiguous `Potential::fastKL` and kept safe `Potential::KL`

## Changelog for 0.13.1

* aGrUM
  * variable: new methods to set bounds with doubles in `ContinuousVariable`
  * Changed the code of `Instantiation`'s hash functions to make it compliant with windows mingw implementation
* TestUnits
  * fixed bug in `RawDatabaseTable` test unit

## Changelog for 0.13.0

* aGrUM
  * inference: Loopy Belief Propagation (`LBP`)
  * inference: new approximated inference : `Monte-Carlo`/`Importance`/`Weighted Sampling` + the same using LBP as a Dirichlet prior (`Loopy...`).
  * learning: new algorithm 3off2 and miic
  * learning: new database handling framework (allows for coping with missing values and with different types of variables)
  * learning: possibility to load data from nanodbc databases (e.g., `postgres`, `sqlite`)
  * learning: add a progress Listener/Signaler in `BNDatabaseGenerator`
  * potential: API extension (`findAll`,`argmax`,`argmin`,`fillWith(pot,map)`)
  * variable: new constructor for `LabelizedVariable` with labels as vector of string + `posLabel(std::string)`
  * variable: new constructor with vector of ticks for `gum::DiscretizedVariable`
  * graph: API extension (`addNodes(n)`)
  * graph: API change (`addNode(id)`->`addNodeWithId(id)`)
  * Changes and bug fixe in in BIF and NET writer/reader
* pyAgrum
  * wheels for python 3.3 and 3.4
  * access to the new learning framework using `BNLearner`
  * access to the new inference algorithms
  * new methods `Instantiation.fromdict` and `Instantiation.todict`
  * `DiscreteVariable.toDiscretized/toLabelized/toRange` copy the variable instead of giving a (not readonly) reference
* O3PRM
  * new syntax for types
  * read and write Bayesian Network with O3PRM syntax
* Documentations
  * agrum : doxygen helps structure and howtos
  * pyAgrum : documentation of a large part of pyAgrum's API, export to <https://pyagrum.readthedocs.io>
  * o3prm : still in progress (see <https://o3prm.lip6.fr>, <https://o3prm.readthedocs.io>)
* act
  * new command guideline for a few easy checks
* many bug fixes

## Changelog for 0.12.0

* API
  * new class `EssentialGraph`
  * new class `MarkovBlanket`
  * improved targets in `MarginalTargettedInference`
* pyAgrum
  * update notebooks
  * new swig-based documentation framework
  * transparent background for dot graphs
  * more windows-compliant agrum.lib.bn2csv
* aGrUM
  * PRM bug fixes
  * improved CI in gitlab
  * improved exception messages in BN learning and O3PRM
  * improving act

## Changelog for 0.11.2

* aGrUM
  * a lot of internal changes for CI in gitlab (especially for future automatic generation of wheels)
  * learning: correct identification of string labels beginning with digits
  * learning: labels from CSV are now alphabetically sorted
  * fix an issue with sql.h
* pyAgrum
  * notebooks as tests (now in wrappers/pyAgrum/notebooks)
  * updating requirements
  * some improvements in doc
  * pyagrum.lib.ipython: emulation of 'pyagrum.lib.notebook' for ipython graphical console (within spyder for instance)
  * pyagrum.lib.bn2csv: csv file with labels of variables instead of index (parameter with_labels:boolean)
  * pyagrum.lib.bn2roc: use a csv with labels by default (parameter with_labels:boolean)

## Changelog for 0.11.1

* 2 typos found in pyAgrum.lib.notebook

## Changelog for 0.11.0

* internal
  * working on continuous integration with gitlab
  * aGrUM/pyAgrum to be compilable with g++-4.8
  * aGrUM/pyAgrum to be compilable with win32
  * pyAgrum wheels generation using act for 'pip' tool
* aGrUM
  * removing some unused data structure (`AVLTree`)
  * fixing bug in `localSearchWithTabuList` learning class
  * Remove wrong parallel estimations for learning (now correct but sequential)
  * working on docs
  * API change : add `BayesNet::minimalCondSet(NodeSet&,NodeSet&)` (migration from pyAgrum to aGrUM)
  * API change : add JointTargettedInference::evidenceJointImpact()
* pyAgrum
  * API changes : pyAgrum.lib.bn2graph (`BN2dot`, `BNinference2dot`, `proba2histo`)
  * API changes : pyAgrum.lib.pretty_print (`bn2txt`, `cpt2txt`)
  * API changes : pyAgrum.lib.notebook : uniforming parameters evs (first) and targets (second) order.
  * API changes : pyAgrum.lib.notebook : `showEntropy->showInformation`
  * updating sphinx help generation
  * fix `CNMonteCarloSampling` not recognized as `ApproximationScheme`
  * enhancing `showInformation` with Mutual Information on arcs
  * API change : adding `BayesNet.minimalCondSet(set_of_targets,set_of_evs)` (as wrapper)
  * API change : adding `LazyInference.evidenceJointImpact(set_of_targets,set_of_evs)`

## Changelog for 0.10.4

* Add new approximated inference : `LBP` (aGrUM and pyAgrum)
* Fix bugs in `LazyPropagation` and `ShaferShenoy` inference
* Refresh some codes in Learning module
* Update (and simplify) CMakeLists.txt for new swig 3.0.11
* Add some project files (including this CHANGELOG.md)
* Refresh pyAgrum notebooks with matplotlib2

## Changelog for 0.10.3

* Only bug fixes in tests

## Changelog for 0.10.2

* New method for `BayesNet` : `minimalCondSet`
* New method for all inference : `evidenceImpact`
* Potential has a (single) value even if no dimension.
* Bug fix for `LazyPropagation`
* Typos for Visual C++ compiler
* Many internal changes

## Changelog for 0.10.1

* aGrUM
  * Fix GCC compilation
  * `ParamEstimator::setMaxThread` new method
* pyAgrum
  * `VariableElimination` and `ShaferShenoy` inference
  * new `addJointTarget` and `jointPosterior` methods for exact inference
  * `pyAgrum.getPosterior` now uses `VariableElimination`
  * Fix pyAgrum.lib.notebook error for python2
  * pyAgrum now linked with static library aGrUM
  * pyAgrum.so (linux) size significantly reduced

## Changelog for 0.10.0

* aGrUM
  * Improvements in inference : New target/evidence-driven incremental inference scheme using relevant reasoning used by Lazy/Shafer-Shenoy/Variable Elimination algorithms. Relevant reasoning leads to a major improvement of the inference (see [RelevanceReasoning.html](http://www-desir.lip6.fr/~phw/aGrUM/officiel/notebooks/RelevanceReasoning.html)).
* pyAgrum
  * LazyPropagation API follow the new inference scheme (add/removeTarget, add/remove/chgEvidence)
* Installers using pip or anaconda.

## Changelog for 0.9.3

Tag 0.9.3 has not been properly announced. Still, many changes in this release :

* Many bug fixes and API glitch/improvement
  * Many internal reorganisations (compilation, test, jenkins, etc.)
  * Many change in the C++ code in order to be more c++11/14
  * Bug fix in learning
  * Many Doxygen improvements
  * Many refactors and bug fix in PRM
* Improvements
  * dynamic BN in pyAgrum
  * nanodbc support for pyAgrUM
  * O3PRMBNReader in pyAgrum (read a prm to a BN)
  * PRMExplorer in pyAgrum
  * UAI reader/writer for BayesNet
  * Algebra of potentials (operators on Potential)
  * pyAgrum.lib.notebook refactored and simplified
  * updating lrs version for credal networks
* Windows
  * aGrUM/pyAgrum compilation on windows using Visual Studio 2015

## Changelog for 0.9.2

* aGrUM
  * Improvements in Inference
    * old LazyPropagation renamed JunctionTreeInference,
    * Improved LazyPropagation ~30% faster,
    * Bug fix and other improvements for relevance reasoning features.
  * Improvements for Probabilistic Relational Models
    * model refinements : e.g. parameterized classes, specification of CPTs using formula, etc.
    * bug fixes and other improvements in dedicated inference algorithms,
    * improving and fixing documentations
    * new file format for Bayesian network : o3prmBNReader (reading a BN by grounding a system)
  * Learning API still improved
    * BNLearner templatized
    * new feature for BNLearner : using a BN to specicfy variables and their modalities,
    * bug fixes and improvement for parameter learning.
  * other bug fixes and improvements in aGrUM architecture
    * aGrUM g++5.1-ready
    * etc.
* pyAgrum
  * small bugs fixed and reorganisation

## Changelog for 0.9.1

* aGrUM
  * Improvement in learning algorithms
  * learning from databases with fewer rows than there are processors
  * method to BNLearner to learn parameters from a BN's DAG
  * static lib compilation for aGrUM
  * bug fixes and other improvements
* pyAgrum
  * Compiled for Python 3 or Python 2 (default is python3, python2 if no python3.). New option for act to choose which python : --python={2|3}.
  * gumLib has moved and changed its name (in the pyAgrum package) : pyAgrum.lib
  * Improving API for learning (changeLabel/parameter learning/ etc.)
  * Improving graphs manipulation
  * bug fixes and other improvements

## Changelog for 0.9.0

Aside from many bug fixes and general improvements such as performance optimizations in various areas, some changes are especially noteworthy:

* Functionality : Structural and parameter learning for Bayesian networks
* Model : Credal Networks, FMDP using Multi-Valued Decision Diagrams
* Language : migration to modern C++(11/14)
* Core : Improvements and optimization of basic data structures in aGrUM/core
