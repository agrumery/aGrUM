# aGrUM Changelog
 
## Changelog for 0.10.3
* Only bug fixes in tests

## Changelog for 0.10.2
* New method for BayesNet : minimalCondSet
* New method for all inference : evidenceImpact
* Potential has a (single) value even if no dimension.
* Bug fix for lazyPropagation
* Typos for Visual C++ compiler
* Many internal changes

## Changelog for 0.10.1
* aGrUM
  * Fix GCC compilation
  * ParamEstimator::setMaxThread new method
* pyAgrum
  * VariableElimination and ShaferShenoy inference
  * new addJointTarget and jointPosterior methods for exact inference
  * pyAgrum.getPosterior now uses VariableElimination
  * Fix pyAgrum.lib.notebook error for python2
  * pyAgrum now linked with static library aGrUM
  * pyAgrum.so (linux) size significantly reduced

## Changelog for 0.10.0
* aGrUM
  * Improvements in inference : New target/evidence-driven incremental inference scheme using relevant reasoning used by Lazy/Shafer-Shenoy/Variable Elimination algorithms. Relevant reasoning leads to a major improvement of the inference (see http://www-desir.lip6.fr/~phw/aGrUM/officiel/notebooks/RelevanceReasoning.html).
* pyAgrum
  * LazyPropagation API follow the new inference scheme (add/removeTarget, add/remove/chgEvidence)
installers using pip or anaconda (See https://forge.lip6.fr/projects/pyagrum/wiki/Install)

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