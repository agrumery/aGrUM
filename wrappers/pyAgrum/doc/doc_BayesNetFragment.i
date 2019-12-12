%feature("docstring") gum::BayesNetFragment
"
BayesNetFragment represents a part of a Bayesian Network (subset of nodes). By default, the arcs and the CPTs are the same as the BN but local CPTs can be build to express different local dependencies. All the non local CPTs are not copied. Therefore a BayesNetFragment is a light object. 

BayesNetFragment(BayesNet bn) -> BayesNetFragment
    Parameters:
      * **bn** (*pyAgrum.BayesNet*) -- the bn refered by the fragment
"

%feature("docstring") gum::BayesNetFragment::checkConsistency
"
If a variable is added to the fragment but not its parents, there is no CPT consistant for this variable. This function checks the consistency for a variable of for all.

Parameters
----------

n : int, str (optional)
	the id or the name of the variable. If no argument, the function checks all the variables.

Returns
-------
boolean
	True if the variable(s) is consistant.

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::installAscendants
"
Add the variable and all its ascendants in the fragment. No inconsistant node are created.

Parameters
----------
n : int, str
	the id or the name of the variable.

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::installCPT
"
Install a local CPT for a node. Doing so, it changes the parents of the node in the fragment.

Parameters
----------
n : int, str
	the id or the name of the variable.
pot : Potential
  the Potential to install

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::installMarginal
"
Install a local marginal for a node. Doing so, it removes the parents of the node in the fragment.

Parameters
----------
n : int, str
	the id or the name of the variable.
pot : Potential
  the Potential (marginal) to install

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::installNode
"
Add a node to the fragment. The arcs that can be added between installed nodes are created.
No specific CPT are created. Then either the parents of the node are already in the fragment 
and the node is consistant, or the parents are not in the fragment and the node is not consistant.

Parameters
----------
n : int, str
	the id or the name of the variable.

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::isInstalledNode
"
Check if a node is in the fragment

Parameters
----------
n : int, str
	the id or the name of the variable.
"

%feature("docstring") gum::BayesNetFragment::uninstallCPT
"
Remove a local CPT. The fragment can become inconsistant.

Parameters
----------
n : int, str
	the id or the name of the variable.

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::uninstallNode
"
Remove a node from the fragment. The fragment can become inconsistant.

Parameters
----------
n : int, str
	the id or the name of the variable.

Raises
------
gum.NotFound
  if the node is not found.
"

%feature("docstring") gum::BayesNetFragment::toBN
"
Create a BayesNet from a fragment.

Raises
------
gum.OperationNotAllowed
  if the fragment is not consistent.
"