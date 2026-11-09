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


// The free functions of gum::graph wrapped for pyagrum.graph, under private names (_graph_<name>):
// each function is instantiated once per family of graphs, on the base class of the family and on
// the immutable mixed graphs (CPDAG, EssentialGraph: ImmutableMixedGraph<ChainGraph>; MPDAG:
// ImmutableMixedGraph<PDAG>). The families are disjoint hierarchies, hence no ambiguity between the
// overloads:
//   A  (arcs)        : DiGraph (DAG, MixedGraph, PDAG, ChainGraph) + immutable mixed graphs
//   E  (edges)       : UndiGraph (MixedGraph, PDAG, ChainGraph) + immutable mixed graphs
//   AE (both)        : MixedGraph (PDAG, ChainGraph) + immutable mixed graphs
//   K  (marked)      : MarkedGraph (PAG) + MAG
// Only the declarations below are wrapped (the headers are not %include-d): the other functions of
// gum::graph stay out of the module.

%{
#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/algorithms/generic/chordality.h>
#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/generic/pathFinding.h>
#include <agrum/base/graphs/algorithms/generic/reachability.h>
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/base/graphs/algorithms/separationCriteria.h>
%}

%rename(_graph_ancestors) gum::graph::ancestors;
%rename(_graph_descendants) gum::graph::descendants;
%rename(_graph_anteriors) gum::graph::anteriors;
%rename(_graph_isAncestral) gum::graph::isAncestral;
%rename(_graph_isMaximal) gum::graph::isMaximal;
%rename(_graph_degeneracyOrdering) gum::graph::degeneracyOrdering;
%rename(_graph_cpdag) gum::graph::cpdag;
%rename(_graph_consistentExtension) gum::graph::consistentExtension;
%rename(_graph_meekClosure) gum::graph::meekClosure;
%rename(_graph_orientAsDAG) gum::graph::orientAsDAG;
%rename(_graph_separation) gum::graph::separation;

namespace gum {
  namespace graph {
    // A
    template < typename G > gum::Sequence< gum::NodeId > topologicalOrder(const G& g);
    template < typename G > bool hasDirectedCycle(const G& g);
    template < typename G > gum::NodeSet ancestors(const G& g, const gum::NodeSet& ids);
    template < typename G > gum::NodeSet descendants(const G& g, const gum::NodeSet& ids);
    template < typename G >
    std::optional< std::vector< gum::NodeId > > directedPath(const G& g, gum::NodeId n1, gum::NodeId n2);
    template < typename G > bool hasDirectedPath(const G& g, gum::NodeId from, gum::NodeId to);

    // E
    template < typename G > bool hasUndirectedCycle(const G& g);
    template < typename G > bool isChordal(const G& g);
    template < typename G >
    gum::Sequence< gum::NodeId > lexBFS(const G& g, const std::vector< gum::NodeId >& visitFirst);
    template < typename G > gum::Sequence< gum::NodeId > maximumCardinalitySearch(const G& g);
    template < typename G >
    bool isPerfectEliminationOrdering(const G& g, const gum::Sequence< gum::NodeId >& eliminationOrder);
    template < typename G >
    std::optional< std::vector< gum::NodeId > > undirectedPath(const G& g, gum::NodeId n1, gum::NodeId n2);

    // AE
    template < typename G > bool hasPartiallyDirectedCycle(const G& g);
    template < typename G > gum::NodeSet anteriors(const G& g, const gum::NodeSet& ids);
    template < typename G >
    std::optional< std::vector< gum::NodeId > > mixedOrientedPath(const G& g, gum::NodeId n1, gum::NodeId n2);
    gum::Sequence< gum::NodeId > degeneracyOrdering(const gum::MixedGraph& g);
    gum::Sequence< gum::NodeId > degeneracyOrdering(const gum::CPDAG& g);
    gum::Sequence< gum::NodeId > degeneracyOrdering(const gum::MPDAG& g);

    // K
    gum::NodeSet ancestors(const gum::MarkedGraph& g, const gum::NodeSet& x);
    gum::NodeSet ancestors(const gum::MAG& g, const gum::NodeSet& x);
    gum::NodeSet descendants(const gum::MarkedGraph& g, const gum::NodeSet& x);
    gum::NodeSet descendants(const gum::MAG& g, const gum::NodeSet& x);
    gum::NodeSet anteriors(const gum::MarkedGraph& g, const gum::NodeSet& x);
    gum::NodeSet anteriors(const gum::MAG& g, const gum::NodeSet& x);
    bool isAncestral(const gum::MarkedGraph& g);
    bool isAncestral(const gum::MAG& g);
    bool isMaximal(const gum::MarkedGraph& g);
    bool isMaximal(const gum::MAG& g);

    // orientation
    gum::CPDAG cpdag(const gum::DAG& dag);
    std::optional< gum::DAG > consistentExtension(const gum::PDAG& pdag);
    std::optional< gum::DAG > consistentExtension(const gum::CPDAG& cpdag);
    std::optional< gum::DAG > consistentExtension(const gum::MPDAG& mpdag);
    std::optional< gum::PDAG > meekClosure(const gum::PDAG& pdag);
    gum::DAG orientAsDAG(const gum::PDAG& pdag, const gum::NodeProperty< gum::Size >& domainSizes);

    // separation, with the criterion of the kind of graph
    bool separation(const gum::DAG& g, const gum::NodeSet& X, const gum::NodeSet& Y, const gum::NodeSet& Z);
    bool separation(const gum::PDAG& g, const gum::NodeSet& X, const gum::NodeSet& Y, const gum::NodeSet& Z);
    bool separation(const gum::CPDAG& g, const gum::NodeSet& X, const gum::NodeSet& Y, const gum::NodeSet& Z);
    bool separation(const gum::MPDAG& g, const gum::NodeSet& X, const gum::NodeSet& Y, const gum::NodeSet& Z);
    bool separation(const gum::MAG& g, const gum::NodeSet& X, const gum::NodeSet& Y, const gum::NodeSet& Z);
  }   // namespace graph
}   // namespace gum

%define GRAPH_ALGORITHMS_ON(classname)
%template(_graph_topologicalOrder) gum::graph::topologicalOrder< classname >;
%template(_graph_hasDirectedCycle) gum::graph::hasDirectedCycle< classname >;
%template(_graph_ancestors) gum::graph::ancestors< classname >;
%template(_graph_descendants) gum::graph::descendants< classname >;
%template(_graph_directedPath) gum::graph::directedPath< classname >;
%template(_graph_hasDirectedPath) gum::graph::hasDirectedPath< classname >;
%enddef

%define GRAPH_UNDIRECTED_ALGORITHMS_ON(classname)
%template(_graph_hasUndirectedCycle) gum::graph::hasUndirectedCycle< classname >;
%template(_graph_isChordal) gum::graph::isChordal< classname >;
%template(_graph_lexBFS) gum::graph::lexBFS< classname >;
%template(_graph_maximumCardinalitySearch) gum::graph::maximumCardinalitySearch< classname >;
%template(_graph_isPerfectEliminationOrdering) gum::graph::isPerfectEliminationOrdering< classname >;
%template(_graph_undirectedPath) gum::graph::undirectedPath< classname >;
%enddef

%define GRAPH_MIXED_ALGORITHMS_ON(classname)
%template(_graph_hasPartiallyDirectedCycle) gum::graph::hasPartiallyDirectedCycle< classname >;
%template(_graph_mixedOrientedPath) gum::graph::mixedOrientedPath< classname >;
%template(_graph_anteriors) gum::graph::anteriors< classname >;
%enddef

// A
GRAPH_ALGORITHMS_ON(gum::DiGraph)
GRAPH_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::ChainGraph >)
GRAPH_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::PDAG >)
// E
GRAPH_UNDIRECTED_ALGORITHMS_ON(gum::UndiGraph)
GRAPH_UNDIRECTED_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::ChainGraph >)
GRAPH_UNDIRECTED_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::PDAG >)
// AE
GRAPH_MIXED_ALGORITHMS_ON(gum::MixedGraph)
GRAPH_MIXED_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::ChainGraph >)
GRAPH_MIXED_ALGORITHMS_ON(gum::ImmutableMixedGraph< gum::PDAG >)
