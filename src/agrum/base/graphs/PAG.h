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
 * @brief Partial Ancestral Graph (PAG) — graph type for FCI output.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_PAG_H
#define GUM_PAG_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/undiGraph.h>

namespace gum {

  /** @enum EdgeMark
   * @brief Endpoint marks used in PAG edges.
   *
   * Convention: marks_[Arc(x,y)] stores the mark at the y-endpoint when
   * traversing from x.  For edge x-y:
   *   - x→y : marks_[Arc(x,y)] = Arrowhead, marks_[Arc(y,x)] = Tail
   *   - x↔y : both = Arrowhead
   *   - xo-oy : both = Circle
   */
  enum class EdgeMark : uint8_t { Circle = 0, Tail = 1, Arrowhead = 2 };

  /* =========================================================================
   */
  /* ===                   PARTIAL ANCESTRAL GRAPH (PAG)                   ===
   */
  /* =========================================================================
   */
  /** @class PAG
   * @brief Partial Ancestral Graph: undirected topology with endpoint marks.
   *
   * Extends UndiGraph: the edge topology is stored in the base class; each
   * edge carries two EdgeMark values (one per endpoint) stored in marks_.
   *
   * marks_[Arc(x,y)] = mark at the y-endpoint of edge x-y.
   *
   * Edge types represented:
   *   - o-o (Circle-Circle)   : completely uncertain
   *   - o-> (Circle-Arrowhead): possibly directed
   *   - ->  (Tail-Arrowhead)  : definitely directed
   *   - <-> (Arrowhead-Arrowhead) : bidirected (latent confounder)
   *   - --  (Tail-Tail)       : definitely undirected
   *
   * @ingroup graph_group
   */
  class PAG: public UndiGraph {
    public:
    // ########################################################################
    /// @name Constructors / Destructors
    // ########################################################################
    /// @{

    PAG();
    PAG(const PAG&);
    PAG(PAG&&) noexcept;
    ~PAG() override;

    PAG& operator=(const PAG&);
    PAG& operator=(PAG&&) noexcept;

    /// @}

    // ########################################################################
    /// @name Edge operations (override UndiGraph/EdgeGraphPart virtuals)
    // ########################################################################
    /// @{

    /// add edge with Circle-Circle marks
    void addEdge(NodeId x, NodeId y) override;

    /// add edge with explicit endpoint marks
    void addEdge(NodeId x, NodeId y, EdgeMark markAtX, EdgeMark markAtY);

    /// remove edge and its two mark entries
    void eraseEdge(const Edge& e) override;

    /// clear all edges (and their marks)
    void clearEdges() override;

    /// remove node and clean up marks for all its adjacent edges
    void eraseNode(NodeId id) override;

    /// remove all nodes, edges, and marks
    void clear() override;

    /// @}

    // ########################################################################
    /// @name Mark accessors
    // ########################################################################
    /// @{

    /// mark at dst-endpoint when traversing from src
    EdgeMark markAt(NodeId src, NodeId dst) const;

    /// set mark at dst-endpoint when traversing from src
    void setMarkAt(NodeId src, NodeId dst, EdgeMark m);

    /// @}

    // ########################################################################
    /// @name Endpoint-mark predicates
    // ########################################################################
    /// @{

    /// true if mark at dst (from src) is Arrowhead
    bool isArrowhead(NodeId src, NodeId dst) const;

    /// true if mark at dst (from src) is Tail
    bool isTail(NodeId src, NodeId dst) const;

    /// true if mark at dst (from src) is Circle
    bool isCircle(NodeId src, NodeId dst) const;

    /// true if edge x-y is definitely directed x→y (Tail@x, Arrowhead@y)
    bool isDefinitelyDirected(NodeId x, NodeId y) const;

    /// true if edge x-y is bidirected x↔y (Arrowhead on both endpoints)
    bool isBidirected(NodeId x, NodeId y) const;

    /// true if z is a definite collider on path x-z-y (Arrowhead at z from both sides)
    bool isDefCollider(NodeId x, NodeId z, NodeId y) const;

    /// @}

    // ########################################################################
    /// @name Bulk operations
    // ########################################################################
    /// @{

    /// set all endpoint marks to m (used between FCI phases to reset to Circle)
    void reorientAllWith(EdgeMark m);

    /// @}

    // ########################################################################
    /// @name Conversion / display
    // ########################################################################
    /// @{

    /// approximate conversion for learnDAG/learnPDAG:
    ///   Tail-Arrowhead → arc; Arrowhead-Arrowhead → two arcs;
    ///   anything else → undirected edge
    MixedGraph toMixedGraph() const;

    std::string toString() const override;
    std::string toDot() const override;

    /// @}

    private:
    /// marks_[Arc(x,y)] = mark at y-endpoint of edge x-y
    ArcProperty< EdgeMark > marks_;
  };

  std::ostream& operator<<(std::ostream&, const PAG&);

} /* namespace gum */

#endif /* GUM_PAG_H */
