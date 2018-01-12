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

Raises
------
gum.InvalidDirectedCircle
	If any (directed) cycle is created by this arc
gum.InvalidNode
	If head or tail does not belong to the graph nodes
"