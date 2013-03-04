%define DOCSTRING
"pyAgrum is a Python wrapper for the C++ aGrUM library. It provides a high-level
interface to the part of agrum allowing to create, handle and make computations
into Bayesian Networks. The module is a straightforward application of the SWIG
interface generator. Custom-written code was added to make the interface similar
to OpenBayes, a free Bayesian Network library for Python."
%enddef
%module(docstring=DOCSTRING, directors="1") pyAgrum
%feature("autodoc", "1");

%{
#define SWIG_FILE_WITH_INIT
%}

%pythoncode %{
  import numpy
%}

/* adding extensions */
%{
#include "extensions/PythonBNListener.h"
#include "extensions/PythonLoadListener.h"
#include "extensions/PythonApproximationListener.h"
%}

/* declaration of code modifiers for 'pythonification' of aGrUM */
%include "exceptions.i"
%include "pythonize.i"

/* declaration of code enhancers for pyAgrum */
%include "core.i"
%include "variables.i"
%include "potential.i"

%include "pylisteners.i"

%include "bayesnet.i"
%include "inference.i"
%include "kl.i"

%include "aGrUM_wrap.i"

%pythoncode %{
Potential = Potential_double
ListPotentials = ListPotentials_double
BayesNet = BayesNet_double
LazyPropagation = LazyPropagation_double
GibbsInference = GibbsInference_double
BruteForceKL = BruteForceKL_double
GibbsKL = GibbsKL_double
%}

