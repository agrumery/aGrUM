# aGrUM Changelog

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
  * `gum.causality` (do-calculus and causal identification !)
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
  * Improvements in inference : New target/evidence-driven incremental inference scheme using relevant reasoning used by Lazy/Shafer-Shenoy/Variable Elimination algorithms. Relevant reasoning leads to a major improvement of the inference (see http://www-desir.lip6.fr/~phw/aGrUM/officiel/notebooks/RelevanceReasoning.html).
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

##Changelog for 0.9.1

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
