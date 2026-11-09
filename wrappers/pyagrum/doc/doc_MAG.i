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


%feature("docstring") gum::MAG
"
MAG represents a maximal ancestral graph: the independence model of a DAG over its observed variables, the latent
variables being marginalized and the selected ones conditioned on.

A MAG is a marked graph without circle (edges ``->``, ``<->`` and ``--``) which is ancestral and maximal; its
separation is the m-separation. A MAG is immutable: it is built by ``MAG.fromDAG``, ``MAG.fromPAG`` or the checked
conversion ``MAG.fromMarkedGraph``.

References
----------
T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics 30(4):962-1030, 2002.

J. Zhang. Causal reasoning with ancestral graphs. Journal of Machine Learning Research 9:1437-1474, 2008.

J. Zhang. On the completeness of orientation rules for causal discovery in the presence of latent confounders and
selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.

Examples
--------
.. code-block:: python

  import pyagrum as gum
  dag = gum.fastDAG('A->X<-T->Y<-B')
  mag = gum.MAG.fromDAG(dag, {dag.idFromName('T')})  # T latent: A->X<->Y<-B
"

%feature("docstring") gum::MAG::fromDAG
"
The MAG of a DAG over its observed nodes (Richardson and Spirtes 2002, Theorem 4.2).

Two observed nodes are adjacent iff no set of observed nodes (with the selected ones) d-separates them; the edge is
``a--b`` if each one is an ancestor of the other or of a selected node, ``a->b`` if only a is, ``a<->b`` if none is.

Parameters
----------
dag : pyagrum.DAG
    the DAG
latents : set[int], optional
    its latent nodes (marginalized), as a set (not a list)
selection : set[int], optional
    its selected nodes (conditioned on), as a set (not a list)

Returns
-------
pyagrum.MAG
    the MAG over the other nodes (same ids and names)

Raises
------
pyagrum.InvalidArgument
    if a node is both latent and selected, or is not in the DAG
"

%feature("docstring") gum::MAG::fromPAG
"
A representative MAG of the Markov equivalence class represented by a PAG (Zhang 2008, Artificial Intelligence,
Theorem 2): the edges ``o->`` and ``--o`` become ``->``, and the edges ``o-o`` are oriented into a DAG without
unshielded collider.

Parameters
----------
pag : pyagrum.PAG
    the PAG

Returns
-------
pyagrum.MAG

Raises
------
pyagrum.InvalidArgument
    if the PAG is not valid
"

%feature("docstring") gum::MAG::fromMarkedGraph
"
Checked conversion of a marked graph.

Parameters
----------
g : pyagrum.MarkedGraph
    a marked graph

Returns
-------
pyagrum.MAG

Raises
------
pyagrum.InvalidArgument
    if the graph has a circle, or is not ancestral, or is not maximal
"

%feature("docstring") gum::MAG::mSeparation
"
Test the m-separation (the separation of the MAG: the d-separation of the DAGs it represents).

Parameters
----------
X : int or set[int]
    a node or a set of nodes
Y : int or set[int]
    a node or a set of nodes
Z : int or set[int], optional
    the conditioning nodes (none by default)

Returns
-------
bool
    True if X and Y are m-separated by Z

Raises
------
pyagrum.InvalidArgument
    if X and Y are not disjoint

See Also
--------
pyagrum.graph.separation : the separation criterion of each kind of graph
"

%feature("docstring") gum::ImmutableGraph< gum::MarkedGraph >::graph
"
Returns
-------
pyagrum.MarkedGraph
    a copy of the marked graph of the MAG
"

%feature("docstring") gum::MAG::toDot
"
Parameters
----------
options : pyagrum.DotOptions, optional
    what is given to the drawing beyond the look of ``pyagrum.config`` (size, label, colors of some nodes and edges...)

Returns
-------
str
    a representation of the MAG in the DOT format (see pyagrum.MarkedGraph.toDot)
"

%feature("docstring") gum::MAG::parents
"
Parameters
----------
node : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that y->node
"

%feature("docstring") gum::MAG::children
"
Parameters
----------
node : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that node->y
"

%feature("docstring") gum::MAG::spouses
"
Parameters
----------
node : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that node<->y
"

%feature("docstring") gum::MAG::neighbours
"
Parameters
----------
node : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that node--y (undirected edge)
"

%feature("docstring") gum::MAG::adjacents
"
Parameters
----------
node : int
    the id of a node

Returns
-------
set[int]
    the nodes linked to node by an edge, whatever its marks
"

%feature("docstring") gum::MAG::markAt
"
Parameters
----------
src : int
dst : int

Returns
-------
pyagrum.EdgeMark
    the mark at the dst-end of the edge src-dst
"

