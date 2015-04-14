%define DOCSTRING
"pyAgrum is a Python wrapper for the C++ aGrUM library. It provides a high-level
interface to the part of agrum allowing to create, handle and make computations
into Bayesian Networks. The module is a straightforward application of the SWIG
interface generator."
%enddef
%module(docstring=DOCSTRING, directors="1") pyAgrum
%feature("autodoc", "1");

%{
#define SWIG_FILE_WITH_INIT

// this should be removed at some point with a new version of SWIG
using namespace std;

#include <cmath>
%}

%pythoncode %{
  import numpy
%}


//////////////////////////////////////////////////////////////////
/* declaration of code modifiers for 'pythonification' of aGrUM */
//////////////////////////////////////////////////////////////////
%include "pythonize.i"
%include "exceptions.i"


//////////////////////////////////////////////////////////////////
/* declaration of code enhancers for pyAgrum */
//////////////////////////////////////////////////////////////////
%include "core.i"
%include "variables.i"

%include "potential.i"
%include "utility.i"

%include "graphs.i"

%include "pylisteners.i"

%include "bayesnet.i"
%include "inference.i"
%include "learning.i"
%include "kl.i"

%include "credalnet.i"

%include "influenceDiagram.i"
%include "influenceDiagramInference.i"


//////////////////////////////////////////////////////////////////
/* extraction of the API for all wrappers */
//////////////////////////////////////////////////////////////////
%include "aGrUM_wrap.i"


%pythoncode %{
Potential = Potential_double
UtilityTable = UtilityTable_double

randomDistribution = randomDistribution_double

SimpleBayesNet = IBayesNet_double
BayesNet = BayesNet_double
LazyPropagation = LazyPropagation_double
GibbsInference = GibbsInference_double

BruteForceKL = BruteForceKL_double
GibbsKL = GibbsKL_double

CredalNet = CredalNet_double
CNMonteCarloSampling = CNMonteCarloSampling_double
CNLoopyPropagation = CNLoopyPropagation_double

DiscretizedVariable = DiscretizedVariable_double

InfluenceDiagram = InfluenceDiagram_double
InfluenceDiagramInference = InfluenceDiagramInference_double

BNLearner =  BNLearner_double
%}

