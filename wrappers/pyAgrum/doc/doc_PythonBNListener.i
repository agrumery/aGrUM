%feature("docstring") PythonBNListener
"
Listener for Bayesian Network's modifications. This listener is notified when the structure of the BN is changed.

PythonBNListener(bn:gum.BayesNet,vnm:gum.VariableNodeMap) -> PythonBNListener
    default constructor

Note
----
    This class est mainly automatically instantiated using the method gum.BayesNet.addStructureListener.

Parameters
----------
bn : BaysNet
    The bayes net to listen to
vnm : VarNodeMap
    A translation unit between id of node and name of variable (usually : bn.variableNodeMap()).
"


%feature("docstring") PythonBNListener::setWhenArcAdded
"
Add the listener in parameter to the list of existing ones for adding an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when when an arc (i,j) is added
"

%feature("docstring") PythonBNListener::setWhenArcDeleted
"
Add the listener in parameter to the list of existing ones for deleting an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when when an arc (i,j) is removed
"

%feature("docstring") PythonBNListener::setWhenNodeAdded
"
Add the listener in parameter to the list of existing ones for adding a node.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,s:str) called when a node of id i and name s is added.
"

%feature("docstring") PythonBNListener::setWhenNodeDeleted
"
Add the listener in parameter to the list of existing ones for deleting an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int) called when a node of id i and name s is removed.
"
