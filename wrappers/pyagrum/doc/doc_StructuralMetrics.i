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


%feature("docstring") gum::StructuralMetrics
"
Compares two graphs of the same type and size, computing precision, recall,
F-score, SHD and SID metrics at both the skeleton level (ignoring orientation)
and the arc/edge level (including orientation).

After calling :meth:`compare`, the confusion-matrix accessors
(:meth:`tp`, :meth:`fp`, :meth:`fn`, :meth:`tn` and their ``_skeleton``
variants) become available, as do all derived metrics.

Examples
--------
>>> import pyagrum as gum
>>> ref = gum.fastBN('A->B->C;A->C')
>>> test = gum.fastBN('A->B;C->B;A->C')
>>> comp = gum.StructuralMetrics()
>>> comp.compare(ref, test)
>>> comp.precision()
>>> comp.sid(ref, test)

StructuralMetrics() -> StructuralMetrics
    default constructor
"


%feature("docstring") gum::StructuralMetrics::compare
"
Compare two graphs of the same type and equal size, populating the internal
confusion matrix used by all metric accessors.

The following overloads are available:

* ``compare(DiGraph, DiGraph)`` — nodes matched by NodeId (no alignment).
* ``compare(UndiGraph, UndiGraph)`` — nodes matched by NodeId (no alignment).
* ``compare(PDAG, PDAG)`` — nodes matched by NodeId (no alignment).
* ``compare(BayesNet, BayesNet)`` — nodes matched by variable name; comparison
  runs on the corresponding essential graphs (CPDAGs).
* ``compare(BayesNet, PDAG)`` — ref BN is converted to its essential graph first.
* ``compare(PDAG, BayesNet)`` — test BN is converted to its essential graph first.

For a DAG-level comparison of two BayesNets, align NodeIds manually (e.g. by
building an aligned BN by variable name) and call ``compare(DiGraph, DiGraph)``
with ``bn.dag()``. SID (see :meth:`sid`) is the only metric that natively
operates on the DAGs of BayesNets.

Parameters
----------
ref :
    Reference graph (DiGraph, UndiGraph, PDAG, or BayesNet).
test :
    Graph to evaluate against the reference (same type as ref, or PDAG when
    ref is a BayesNet and vice-versa).

Raises
------
pyagrum.OperationNotAllowed
    If both graphs are empty, or if their sizes differ.
pyagrum.InvalidNode
    If a node present in ref is absent from test (for BayesNet overloads,
    matching is done by variable name).
"


%feature("docstring") gum::StructuralMetrics::sid
"
Compute the Structural Intervention Distance (SID) between two DAGs
(Peters & Bühlmann, 2015).

SID counts the number of ordered pairs (i, j) for which the
parent-adjustment set for the causal effect of i on j in the reference graph
is incorrectly reproduced by the test graph.

Properties:

* ``sid(G, G) == 0``
* If G ⊆ H (all parents of G are parents of H), then ``sid(G, H) == 0``.
* SID is asymmetric: ``sid(G, H) != sid(H, G)`` in general.

Two overloads are available:

* ``sid(DAG, DAG)`` — operates directly on DAG objects.
* ``sid(BayesNet, BayesNet)`` — nodes matched by variable name; the comparison
  runs on the underlying DAGs after name-based alignment.

Parameters
----------
ref :
    Reference DAG or BayesNet.
test :
    DAG or BayesNet to evaluate against the reference.

Returns
-------
float
    The SID value (non-negative integer returned as float).

Raises
------
pyagrum.OperationNotAllowed
    If the graphs have different sizes.
pyagrum.InvalidNode
    If a variable present in ref is absent from test (BayesNet overload only).
"


%feature("docstring") gum::StructuralMetrics::tp
"
Number of true positives at the arc/edge level (correctly predicted arcs or
edges, orientation included for directed graphs).

Returns
-------
float
    True-positive count.
"

%feature("docstring") gum::StructuralMetrics::fp
"
Number of false positives at the arc/edge level (extra or misoriented arcs/edges
in the test graph).

Returns
-------
float
    False-positive count.
"

%feature("docstring") gum::StructuralMetrics::fn
"
Number of false negatives at the arc/edge level (arcs/edges present in ref but
missing from test).

Returns
-------
float
    False-negative count.
"

%feature("docstring") gum::StructuralMetrics::tn
"
Number of true negatives at the arc/edge level (pairs correctly absent from
both ref and test).

Returns
-------
float
    True-negative count.
"

%feature("docstring") gum::StructuralMetrics::tp_skeleton
"
Number of true positives at the skeleton level (undirected edges correctly
predicted, ignoring orientation).

Returns
-------
float
    True-positive count for the skeleton.
"

%feature("docstring") gum::StructuralMetrics::fp_skeleton
"
Number of false positives at the skeleton level (extra undirected edges in
the test skeleton).

Returns
-------
float
    False-positive count for the skeleton.
"

%feature("docstring") gum::StructuralMetrics::fn_skeleton
"
Number of false negatives at the skeleton level (undirected edges present in
the ref skeleton but missing from the test skeleton).

Returns
-------
float
    False-negative count for the skeleton.
"

%feature("docstring") gum::StructuralMetrics::tn_skeleton
"
Number of true negatives at the skeleton level (pairs correctly absent from
both ref and test skeletons).

Returns
-------
float
    True-negative count for the skeleton.
"

%feature("docstring") gum::StructuralMetrics::precision_skeleton
"
Precision at the skeleton level: TP / (TP + FP).

Returns
-------
float
    Skeleton precision in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::recall_skeleton
"
Recall at the skeleton level: TP / (TP + FN).

Returns
-------
float
    Skeleton recall in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::f_score_skeleton
"
F1-score at the skeleton level: harmonic mean of skeleton precision and recall.

Returns
-------
float
    Skeleton F1-score in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::shd_skeleton
"
Structural Hamming Distance at the skeleton level: number of edge insertions
and deletions needed to transform the test skeleton into the ref skeleton.

Returns
-------
float
    Skeleton SHD (non-negative integer returned as float).
"

%feature("docstring") gum::StructuralMetrics::precision
"
Precision at the arc/edge level (orientation included): TP / (TP + FP).

Returns
-------
float
    Precision in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::recall
"
Recall at the arc/edge level (orientation included): TP / (TP + FN).

Returns
-------
float
    Recall in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::f_score
"
F1-score at the arc/edge level: harmonic mean of precision and recall.

Returns
-------
float
    F1-score in [0, 1].
"

%feature("docstring") gum::StructuralMetrics::shd
"
Structural Hamming Distance: number of arc/edge insertions, deletions and
reversals needed to transform the test graph into the ref graph.

Returns
-------
float
    SHD (non-negative integer returned as float).
"
