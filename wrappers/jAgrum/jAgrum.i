%module jAgrum

%include "../swig/aGrUM_wrap.i"

%include <agrum/tools/graphs/DAG.h>

%{
// WORKAROUND for a bug in swig C++ parser
#define DummyString gum::DummyString
%}