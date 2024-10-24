%feature("docstring") gum::Arc
"
pyAgrum.Arc is the representation of an arc between two nodes represented by int : the head and the tail.

Arc(tail, head) -> Arc
    Parameters:
        - **tail** (int) -- the tail
        - **head** (int) -- the head

Arc(src) -> Arc
    Parameters:
        - **src** (*Arc*) -- the pyAgrum.Arc to copy
"

%feature("docstring") gum::Arc::tail
"
Returns
-------
int
  the id of the tail node
"

%feature("docstring") gum::Arc::head
"
Returns
------
int
  the id of the head node
"

%feature("docstring") gum::Arc::other
"
Parameters
----------
id : int
  the nodeId of the head or the tail


Returns
-------
int
  the nodeId of the other node
"


%feature("docstring") gum::Arc::first
"
Returns
-------
int
  the nodeId of the first node of the arc (the tail)
"


%feature("docstring") gum::Arc::second
"
Returns
-------
int
  the nodeId of the second node of the arc (the head)
"
