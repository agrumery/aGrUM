%ignore gum::DAG::emptyArcs;
%ignore gum::DAG::eraseArc;
%ignore gum::DAG::eraseChildren;
%ignore gum::DAG::eraseParents;
%ignore gum::DAG::existsArc;
%ignore gum::DAG::ids;
%ignore gum::DAG::sizeArcs;

%feature("docstring") gum::DAG
"
DAG represents a Directed Acyclic Graph.

Available constructors:
    ``DAG() -> DAG``

    ``DAG(src) -> DAG``

Parameters
----------
src : :class: DAG
  the DAG to copy
"

%feature("docstring") gum::DAG::addArc
"
Add an arc from tail to head.

Parameters
----------
tail : int
  the id of the tail node
head : int
  the id of the head node

Warnings
--------
	InvalidDirectedCircle raised if any (directed) cycle is created by this arc.
	
Warnings
--------
	InvalidNode raised if head or tail does not belong to the graph nodes.
"

%feature("docstring") gum::DAG::toDot
"
Returns
-------
str
    a description of the graph

Examples
--------
>>> import pyAgrum as gum
>>> bn = gum.DAG()
>>> a = bn.addNode()
>>> b = bn.addNode()
>>> bn.addArc(a,b)
>>> bn.toDot()
DAG {
	0;
	1;
	0 -> 1;
}
"