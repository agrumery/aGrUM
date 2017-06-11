%feature("docstring") gum::Edge
"
pyAgrum.Edge is the representation of an arc between two nodes represented by `int`s : the first and the second.

Available constructors :
    ``Edge(aN1,aN2) -> Edge``

    ``Edge(src) -> Edge``

Parameters
----------
aN1 : int
  the nodeId of the first node
aN2 : int
  the nodeId of the secondnode
src: Edge
  the Edge to copy
"

%feature("docstring") gum::Edge::other
"
Parameters
----------
id : int
  the nodeId of one of the nodes of the Edge


Returns
------
int
  the nodeId of the other node
"


%feature("docstring") gum::Edge::first
"
Returns
------
int
  the nodeId of the first node of the arc (the tail)
"


%feature("docstring") gum::Edge::second
"
Returns
------
int
  the nodeId of the second node of the arc (the head)
"
