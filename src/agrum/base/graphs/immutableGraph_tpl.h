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


/** @file
 * @brief Template implementation of the read-only bases of the immutable graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#pragma once

#include <utility>

#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/graphs/immutableGraph.h>

namespace gum {

  // ============================================================================
  // ImmutableGraph
  // ============================================================================

  template < typename GRAPH >
  ImmutableGraph< GRAPH >::ImmutableGraph(GRAPH&& g) : graph_(std::move(g)) {
    GUM_CONSTRUCTOR(ImmutableGraph);
  }

  template < typename GRAPH >
  ImmutableGraph< GRAPH >::ImmutableGraph(const ImmutableGraph< GRAPH >& g) : graph_(g.graph_) {
    GUM_CONS_CPY(ImmutableGraph);
  }

  template < typename GRAPH >
  ImmutableGraph< GRAPH >::ImmutableGraph(ImmutableGraph< GRAPH >&& g) :
      graph_(std::move(g.graph_)) {
    GUM_CONS_MOV(ImmutableGraph);
  }

  template < typename GRAPH >
  ImmutableGraph< GRAPH >::~ImmutableGraph() {
    GUM_DESTRUCTOR(ImmutableGraph);
  }

  template < typename GRAPH >
  ImmutableGraph< GRAPH >& ImmutableGraph< GRAPH >::operator=(const ImmutableGraph< GRAPH >& g) {
    if (this != &g) {
      graph_ = g.graph_;
      GUM_OP_CPY(ImmutableGraph);
    }
    return *this;
  }

  template < typename GRAPH >
  ImmutableGraph< GRAPH >& ImmutableGraph< GRAPH >::operator=(ImmutableGraph< GRAPH >&& g) {
    if (this != &g) {
      graph_ = std::move(g.graph_);
      GUM_OP_MOV(ImmutableGraph);
    }
    return *this;
  }

  template < typename GRAPH >
  INLINE const GRAPH& ImmutableGraph< GRAPH >::graph() const {
    return graph_;
  }

  template < typename GRAPH >
  INLINE const NodeGraphPart& ImmutableGraph< GRAPH >::nodes() const {
    return graph_.nodes();
  }

  template < typename GRAPH >
  INLINE const EdgeSet& ImmutableGraph< GRAPH >::edges() const {
    return graph_.edges();
  }

  template < typename GRAPH >
  INLINE Size ImmutableGraph< GRAPH >::size() const {
    return graph_.size();
  }

  template < typename GRAPH >
  INLINE Size ImmutableGraph< GRAPH >::sizeEdges() const {
    return graph_.sizeEdges();
  }

  template < typename GRAPH >
  INLINE bool ImmutableGraph< GRAPH >::empty() const {
    return graph_.empty();
  }

  template < typename GRAPH >
  INLINE bool ImmutableGraph< GRAPH >::existsNode(NodeId node) const {
    return graph_.existsNode(node);
  }

  template < typename GRAPH >
  INLINE bool ImmutableGraph< GRAPH >::existsEdge(NodeId first, NodeId second) const {
    return graph_.existsEdge(first, second);
  }

  template < typename GRAPH >
  INLINE std::string ImmutableGraph< GRAPH >::nameFromId(NodeId id) const {
    return graph_.nameFromId(id);
  }

  template < typename GRAPH >
  INLINE std::optional< NodeId >
         ImmutableGraph< GRAPH >::idFromName(const std::string& name) const {
    return graph_.idFromName(name);
  }

  template < typename GRAPH >
  INLINE bool ImmutableGraph< GRAPH >::hasName(NodeId id) const {
    return graph_.hasName(id);
  }

  template < typename GRAPH >
  INLINE NodeProperty< NodeId > ImmutableGraph< GRAPH >::connectedComponents() const {
    return graph_.connectedComponents();
  }

  template < typename GRAPH >
  UndiGraph ImmutableGraph< GRAPH >::skeleton() const {
    if constexpr (requires { graph_.skeleton(); }) {   // MarkedGraph
      return graph_.skeleton();
    } else {   // the mixed graphs: their edges and their arcs
      UndiGraph skeleton;
      for (const auto node: graph_.nodes()) {
        skeleton.addNodeWithId(node);
        if (graph_.hasName(node)) skeleton.setName(node, graph_.nameFromId(node));
      }
      for (const auto& edge: graph_.edges())
        skeleton.addEdge(edge.first(), edge.second());
      for (const auto& arc: graph_.arcs())
        skeleton.addEdge(arc.tail(), arc.head());
      return skeleton;
    }
  }

  template < typename GRAPH >
  INLINE bool ImmutableGraph< GRAPH >::operator==(const ImmutableGraph< GRAPH >& g) const {
    return graph_ == g.graph_;
  }

  template < typename GRAPH >
  INLINE std::string ImmutableGraph< GRAPH >::toString() const {
    return graph_.toString();
  }

  // ============================================================================
  // ImmutableMixedGraph
  // ============================================================================

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >::ImmutableMixedGraph(GRAPH&& g, DAG&& extension) :
      ImmutableGraph< GRAPH >(std::move(g)), extension_(std::move(extension)) {
    GUM_CONSTRUCTOR(ImmutableMixedGraph);
  }

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >::ImmutableMixedGraph(const ImmutableMixedGraph< GRAPH >& g) :
      ImmutableGraph< GRAPH >(g), extension_(g.extension_) {
    GUM_CONS_CPY(ImmutableMixedGraph);
  }

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >::ImmutableMixedGraph(ImmutableMixedGraph< GRAPH >&& g) :
      ImmutableGraph< GRAPH >(std::move(g)), extension_(std::move(g.extension_)) {
    GUM_CONS_MOV(ImmutableMixedGraph);
  }

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >::~ImmutableMixedGraph() {
    GUM_DESTRUCTOR(ImmutableMixedGraph);
  }

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >&
      ImmutableMixedGraph< GRAPH >::operator=(const ImmutableMixedGraph< GRAPH >& g) {
    if (this != &g) {
      ImmutableGraph< GRAPH >::operator=(g);
      extension_ = g.extension_;
      GUM_OP_CPY(ImmutableMixedGraph);
    }
    return *this;
  }

  template < typename GRAPH >
  ImmutableMixedGraph< GRAPH >&
      ImmutableMixedGraph< GRAPH >::operator=(ImmutableMixedGraph< GRAPH >&& g) {
    if (this != &g) {
      ImmutableGraph< GRAPH >::operator=(std::move(g));
      extension_ = std::move(g.extension_);
      GUM_OP_MOV(ImmutableMixedGraph);
    }
    return *this;
  }

  template < typename GRAPH >
  INLINE const ArcSet& ImmutableMixedGraph< GRAPH >::arcs() const {
    return this->graph_.arcs();
  }

  template < typename GRAPH >
  INLINE const NodeSet& ImmutableMixedGraph< GRAPH >::parents(NodeId node) const {
    return this->graph_.parents(node);
  }

  template < typename GRAPH >
  INLINE const NodeSet& ImmutableMixedGraph< GRAPH >::children(NodeId node) const {
    return this->graph_.children(node);
  }

  template < typename GRAPH >
  INLINE const NodeSet& ImmutableMixedGraph< GRAPH >::neighbours(NodeId node) const {
    return this->graph_.neighbours(node);
  }

  template < typename GRAPH >
  INLINE Size ImmutableMixedGraph< GRAPH >::sizeArcs() const {
    return this->graph_.sizeArcs();
  }

  template < typename GRAPH >
  INLINE bool ImmutableMixedGraph< GRAPH >::existsArc(NodeId tail, NodeId head) const {
    return this->graph_.existsArc(tail, head);
  }

  template < typename GRAPH >
  INLINE DAG ImmutableMixedGraph< GRAPH >::consistentExtension() const {
    return extension_;
  }

  template < typename GRAPH >
  INLINE bool
      ImmutableMixedGraph< GRAPH >::dSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    return extension_.dSeparation(X, Y, Z);
  }

  template < typename GRAPH >
  INLINE bool ImmutableMixedGraph< GRAPH >::dSeparation(const NodeSet& X,
                                                        const NodeSet& Y,
                                                        const NodeSet& Z) const {
    return extension_.dSeparation(X, Y, Z);
  }

  template < typename GRAPH >
  std::string ImmutableMixedGraph< GRAPH >::toDot() const {
    return toDot(dot::DotOptions());
  }

  template < typename GRAPH >
  std::string ImmutableMixedGraph< GRAPH >::toDot(const dot::DotOptions& options) const {
    return this->graph_.toDot(dot::withCompelledArcColor(options, this->graph_.arcs()));
  }

}   // namespace gum
