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

// this should be removed at some point with a new version of SWIG
using namespace std;
using namespace gum;
%}

%pythoncode %{
  import numpy
%}
 
%include "aGrUM_wrap.i"

/* declaration of code modifiers for 'pythonification' of aGrUM */
%include "pythonize.i"
%include "exceptions.i"

/* declaration of code enhancers for pyAgrum */

%include "core.i"
%include "variables.i"
%include "potential.i"

%include "pylisteners.i"

%include "bayesnet.i"
%include "inference.i"
%include "kl.i"

%include "credalnet.i"


