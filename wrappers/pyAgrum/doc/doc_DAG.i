%feature("docstring") gum::DAG
"
DAG represents a Directed Acyclic Graph.

DAG() -> DAG
    default constructor

DAG(src) -> DAG
    Parameters:
        * **src** (*DAG*) -- the DAG to copy
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
pyAgrum.InvalidDirectedCircle
	If any (directed) cycle is created by this arc
pyAgrum.InvalidNode
	If head or tail does not belong to the graph nodes
"
