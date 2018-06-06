# aGrUM Changelog

## Changelog for 0.13.0 (in progress)

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
  * variable: new methods to set bounds with doubles in `gum::ContinuousVariable`
  * graph: API extension (`addNodes(n)`)
  * graph: API change (`addNode(id)`->`addNodeWithId(id)`)
  * Changes and bug fixe in in BIF and NET writer/reader
  * Changed the code of instantiations' hash functions to make it compliant with windows mingw implementation
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
* TestUnits
  * fixed bug in RawDatabaseTable test unit
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
