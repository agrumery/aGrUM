%feature("docstring") gum::StructuralComparator::compare
"
Use to compare the edges/arcs of two structure of the same type and same sizes (either DiGraph, UndiGraph or MixedGraph).

Could be use to compare a BN and its learned version.

Parameters
----------
ref :
	the structure of reference
test :
	the structure we want to test
"

%feature("docstring") gum::StructuralComparator::precision_skeleton
"
Rate of true postive over labelized edges.

Returns
-------
double
	the precision of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::recall_skeleton
"
Rate of true postive over labelized edges.

Returns
-------
double
	the recall of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::f_score_skeleton
"
Harmonic mean between recall and precision.

Returns
-------
double
	the tarmonic mean of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::precision
"
Rate of true postive over postively labelized arcs/edges.

Returns
-------
double
	the precision of the tested graph
"

%feature("docstring") gum::StructuralComparator::recall
"
Rate of true postive over labelized arcs/edges.

Returns
-------
double
	the recall of the tested graph
"

%feature("docstring") gum::StructuralComparator::f_score
"
Harmonic mean between recall and precision.

Returns
-------
double
	the harmonic mean of the tested graph
"