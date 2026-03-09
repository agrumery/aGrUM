/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


%feature("docstring") gum::CausalModel
"
A causal model pairing an observational Bayesian network with a causal DAG.

A CausalModel extends an observational BayesNet by adding latent (hidden)
variables that represent unobserved common causes between observed variables.
The causal DAG includes both observed and latent nodes, while the observational
BN contains only the observed ones.

CausalModel(bn) -> CausalModel
    Parameters:
        - **bn** (*pyagrum.BayesNet*) -- the observational Bayesian network.

CausalModel(bn, latents, keepArcs=False) -> CausalModel
    Parameters:
        - **bn** (*pyagrum.BayesNet*) -- the observational Bayesian network.
        - **latents** (*list of (str, list of str)*) -- description of latent
          variables. Each entry is a pair ``(name, children)`` where ``name``
          is the latent variable name and ``children`` is the list of observed
          variable names it affects.
        - **keepArcs** (*bool*) -- if True, existing arcs between the children
          of each latent variable are preserved. Default is False (arcs between
          affected children are removed as they are assumed to be explained by
          the latent confounder).

Examples
--------
>>> import pyagrum as gum
>>> import pyagrum.causal as csl
>>> bn = gum.BayesNet.fastPrototype('X->Y;X->Z;Y->Z')
>>> cm = csl.CausalModel(bn)

Create a model with a latent confounder U between X and Y:

>>> cm = csl.CausalModel(bn, [('U', ['X', 'Y'])], keepArcs=False)
"

%feature("docstring") gum::CausalModel::addLatentVariable
"
Add a latent (hidden) variable to the causal model.

The latent variable is added as a common cause of the specified children.
By default, any existing arc between two affected children is removed, as
it is assumed to be explained by the new latent confounder.

Parameters
----------
name : str
	Name of the new latent variable.
children : list of str
	Names of the observed variables that are children of this latent variable.
keepArcs : bool, optional
	If True, preserve existing arcs between the specified children.
	Default is False.
"

%feature("docstring") gum::CausalModel::existsArc
"
Check whether an arc exists in the causal DAG.

Parameters
----------
x : int or str
	Tail of the arc (NodeId or variable name).
y : int or str
	Head of the arc (NodeId or variable name).

Returns
-------
bool
	True if the arc x→y exists in the causal DAG.
"

%feature("docstring") gum::CausalModel::assumeSpurious
"
Mark an arc in the causal DAG as spurious.

A spurious arc x→y means the observed correlation between x and y is believed
to be explained by a latent common cause rather than a direct causal effect.
This changes the structural interpretation but does not remove the arc.

Parameters
----------
x : int or str
	Tail of the arc (NodeId or variable name).
y : int or str
	Head of the arc (NodeId or variable name).
"

%feature("docstring") gum::CausalModel::assumeNonSpurious
"
Mark an arc in the causal DAG as non-spurious (a genuine causal effect).

Parameters
----------
x : int or str
	Tail of the arc (NodeId or variable name).
y : int or str
	Head of the arc (NodeId or variable name).
"

%feature("docstring") gum::CausalModel::isAssumedSpurious
"
Check whether the arc x→y is assumed spurious.

Parameters
----------
x : int or str
	Tail of the arc (NodeId or variable name).
y : int or str
	Head of the arc (NodeId or variable name).

Returns
-------
bool
	True if the arc x→y is marked as spurious.
"

%feature("docstring") gum::CausalModel::backDoor
"
Find a backdoor adjustment set between cause and effect.

Returns the first valid backdoor adjustment set found. A backdoor set Z
blocks all spurious (non-causal) paths between cause and effect while
leaving all directed causal paths open, enabling estimation of the causal
effect P(effect | do(cause)) via standard conditioning on Z.

Parameters
----------
cause : int or str
	The treatment variable (NodeId or variable name).
effect : int or str
	The outcome variable (NodeId or variable name).

Returns
-------
set of int
	A valid backdoor adjustment set as NodeIds. Empty if no backdoor set exists.

See Also
--------
pyagrum.causal.DoorCriteria.enumerateBackdoorSets : enumerate all valid sets.
"

%feature("docstring") gum::CausalModel::frontDoor
"
Find a frontdoor adjustment set between cause and effect.

Returns the first valid frontdoor adjustment set found. A frontdoor set Z
intercepts all directed paths from cause to effect and enables estimation of
P(effect | do(cause)) even in the presence of unobserved confounders.

Parameters
----------
cause : int or str
	The treatment variable (NodeId or variable name).
effect : int or str
	The outcome variable (NodeId or variable name).

Returns
-------
set of int
	A valid frontdoor adjustment set as NodeIds. Empty if no frontdoor set exists.

See Also
--------
pyagrum.causal.DoorCriteria.enumerateFrontdoorSets : enumerate all valid sets.
"

%feature("docstring") gum::CausalModel::observationalBN
"
Return the observational Bayesian network underlying the causal model.

Warning
-------
Do not use this BN for causal inference. It represents the observational
distribution only. Use :func:`pyagrum.causal.causalImpact` for
interventional queries.

Returns
-------
pyagrum.BayesNet
	The observational BN (observed variables only).
"

%feature("docstring") gum::CausalModel::causalDAG
"
Return the full causal DAG, including latent variables.

Returns
-------
pyagrum.DAG
	The causal DAG (observed + latent nodes).
"

%feature("docstring") gum::CausalModel::names
"
Return the names of all variables in the causal model (observed and latent).

Returns
-------
set of str
	The set of all variable names.
"

%feature("docstring") gum::CausalModel::idFromName
"
Return the NodeId of a variable by name.

Parameters
----------
name : str
	The variable name.

Returns
-------
int
	The NodeId of the variable.

Raises
------
pyagrum.NotFound
	If no variable with that name exists in the causal model.
"

%feature("docstring") gum::CausalModel::nameFromId
"
Return the name of a variable by NodeId.

Parameters
----------
id : int
	The NodeId of the variable.

Returns
-------
str
	The variable name.

Raises
------
pyagrum.NotFound
	If no variable with that NodeId exists in the causal model.
"

%feature("docstring") gum::CausalModel::id2name
"
Return the full NodeId ↔ name bijection.

Parameters
----------
includeLatentVariable : bool, optional
	If True, include latent variables in the mapping. Default is False.

Returns
-------
pyagrum.Bijection
	A bijection between NodeIds and variable names.
"

%feature("docstring") gum::CausalModel::latentVariablesIds
"
Return the NodeIds of all latent (hidden) variables in the causal model.

Returns
-------
set of int
	NodeIds of latent variables.
"

%feature("docstring") gum::CausalModel::latentVariablesNames
"
Return the names of all latent (hidden) variables in the causal model.

Returns
-------
set of str
	Names of latent variables.
"

%feature("docstring") gum::CausalModel::parents
"
Return the parents of a variable in the causal DAG.

Parameters
----------
x : int or str
	The variable (NodeId or name).

Returns
-------
set of int
	NodeIds of the variable's parents in the causal DAG.
"

%feature("docstring") gum::CausalModel::children
"
Return the children of a variable in the causal DAG.

Parameters
----------
x : int or str
	The variable (NodeId or name).

Returns
-------
set of int
	NodeIds of the variable's children in the causal DAG.
"

%feature("docstring") gum::CausalModel::connectedComponents
"
Return the connected components of the causal DAG (treating arcs as undirected).

Returns
-------
dict of int → set of int
	A mapping from component index to the set of NodeIds in that component.
"

%feature("docstring") gum::CausalModel::inducedCausalSubModel
"
Return the causal sub-model induced by a subset of observed nodes.

The sub-model is restricted to the specified nodes, preserving the relevant
portion of the causal DAG and latent structure.

Parameters
----------
cm : pyagrum.causal.CausalModel
	The original causal model.
subset : set of int
	NodeIds of the observed variables to keep.

Returns
-------
pyagrum.causal.CausalModel
	The induced causal sub-model.
"

%feature("docstring") gum::CausalModel::toDot
"
Return a Graphviz dot string representing the causal model.

Observed nodes are shown with default styling. Latent nodes are displayed
with a distinct background colour. Their names are hidden by default.

Parameters
----------
SHOW_LATENT_NAMES : bool, optional
	If True, display the names of latent nodes in the graph. Default is False.
NODE_BG : str, optional
	Background colour for latent nodes (hex or CSS colour name).
	Default is '#404040'.
NODE_FG : str, optional
	Text colour for latent nodes. Default is 'white'.
EDGE_COL : str, optional
	Edge colour. Default is '#4A4A4A'.

Returns
-------
str
	A dot-format string representation of the causal model.

Examples
--------
>>> import pyagrum as gum
>>> import pyagrum.causal as csl
>>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
>>> cm = csl.CausalModel(bn)
>>> print(cm.toDot())
"
