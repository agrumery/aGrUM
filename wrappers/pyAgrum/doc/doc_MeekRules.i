/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

%feature("docstring") gum::MeekRules
"
MeekRules class applies the Meek rules to a mixed graph or PDAG (Partially Directed Acyclic Graph) in order to propagate orientation constraints and obtain a CPDAG (Completed Partially Directed Acyclic Graph) or a DAG (Directed Acyclic Graph).

The Meek Rules help complete the orientation of edges in a mixed graph, resulting in a CPDAG that reflects the possible causal relationships among variables while accounting for the observed conditional independencies in the data. The CPDAG is a more refined and directed version of the original PDAG (Partially Directed Acyclic Graph) obtained from the data.
"

%feature("docstring") gum::MeekRules::propagates
"
Propagates orientation constraints in the graph according to the Meek rules. The Meek rules are applied iteratively until no more orientation constraints can be propagated. This may lead to double orientation that are not resolved by the Meek rules. Then this method returns just a pyAgrum.MixedGraph.

Parameters
----------
mg : pyAgrum.MixedGraph
    The graph to be completed.

Returns
-------
pyAgrum.MixedGraph
    The completed graph, with possible double orientations.
"

%feature("docstring") gum::MeekRules::propagateToCPDAG
"
Propagates orientation constraints in the graph according to the Meek rules. The Meek rules are applied iteratively until no more orientation constraints can be propagated. And then arbitratily resolves double orientations. This method returns a `pyAgrum.PDAG` with the properties of CPDAG.

The arbitrary resolution of double orientations is not deterministic. It depends on the order of the edges in the graph. `pyAgrum.MeekRules.Choices` returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG`.

Parameters
----------
mg : pyAgrum.MixedGraph
    The graph to be completed.

Returns
-------
pyAgrum.PDAG
    The completed `pyAgrum.PDAG` (CPDAG).
"

%feature("docstring") gum::MeekRules::propagateToDAG
"
Calls `propagateToCPDAG` and then orients the remaining edges arbitrarily according to some heuristics. `pyAgrum.MeekRules.Choices` returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToDAG`.

Parameters
----------
mg : `pyAgrum.MixedGraph` or `pyAgrum.PDAG`
    The graph to be completed.

Returns
-------
`pyAgrum.DAG`
    The completed `pyAgrum.DAG`.
"

%feature("docstring") gum::MeekRules::choices
"
Returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG` or `pyAgrum.MeekRules.progagatesToDAG`.

Returns
-------
list of tuple
    The list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG` or `pyAgrum.MeekRules.progagatesToDAG`. Each tuple represents a `pyAgrum.Edge`.
"
