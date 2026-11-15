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
 * @brief Graphs whose edges carry a mark at each of their ends (tail, arrowhead or circle).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MARKED_GRAPH_H
#define GUM_MARKED_GRAPH_H

#include <cstdint>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/graphs/undiGraph.h>

namespace gum {

  namespace dot {
    class DotOptions;
  }   // namespace dot

  /** @enum EdgeMark
   * @brief The mark at an end of an edge of a marked graph.
   *
   * An edge x-y carries one mark at x and one mark at y:
   *   - x->y : Tail at x, Arrowhead at y (x is an ancestor of y) ;
   *   - x<->y : Arrowhead at both ends (none is an ancestor of the other) ;
   *   - x--y : Tail at both ends ;
   *   - a Circle is an end whose mark is not determined (in a PAG, it varies across the equivalence
   *     class).
   */
  enum class EdgeMark : uint8_t { Circle = 0, Tail = 1, Arrowhead = 2 };

  /**
   * @class MarkedGraph
   * @brief A graph whose edges carry a mark (EdgeMark) at each of their ends.
   * @ingroup graph_group
   *
   * There is at most one edge between two nodes: its kind (->, <->, --, o->, o-o...) is given by
   * its two marks. The marked graphs generalize the directed, bidirected and undirected edges of
   * the ancestral graphs (Richardson and Spirtes 2002) and the circles of the partial ancestral
   * graphs (Zhang 2008).
   *
   * A marked graph is not an undirected graph: it stores its edges as an undirected graph does
   * (NodeGraphPart and EdgeGraphPart), but no algorithm of undirected graphs applies to it. Its
   * skeleton is given by skeleton(). The directed view (parents, children, spouses) is computed
   * from the marks, in O(degree).
   *
   * Convention: markAt(x, y) is the mark at the y-end of the edge x-y.
   *
   * @par References
   * - T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics
   *   30(4):962-1030, 2002. doi:10.1214/aos/1031689015.
   * - J. Zhang. On the completeness of orientation rules for causal discovery in the presence of
   *   latent confounders and selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
   *   doi:10.1016/j.artint.2008.08.001.
   */
  class GUM_SHARED_PUBLIC MarkedGraph: public virtual NodeGraphPart, public EdgeGraphPart {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    explicit MarkedGraph(Size nodes_size          = HashTableConst::default_size,
                         bool nodes_resize_policy = true,
                         Size edges_size          = HashTableConst::default_size,
                         bool edges_resize_policy = true);
    MarkedGraph(const MarkedGraph& g);
    MarkedGraph(MarkedGraph&& g);
    ~MarkedGraph() override;

    MarkedGraph& operator=(const MarkedGraph& g);
    MarkedGraph& operator=(MarkedGraph&& g);

    /// same nodes, same edges and same marks
    bool operator==(const MarkedGraph& g) const;

    /// @}

    // ############################################################################
    /// @name Edges and marks
    // ############################################################################
    /// @{

    /// adds the edge x-y with a circle at both ends; does nothing if the edge already exists
    /** @throw InvalidNode if x or y is not a node
     * @throw InvalidEdge if x == y */
    void addEdge(NodeId x, NodeId y) override;

    /// adds the edge x-y with these marks, or updates its marks if it already exists
    /** @throw InvalidNode if x or y is not a node
     * @throw InvalidEdge if x == y */
    void addEdge(NodeId x, NodeId y, EdgeMark markAtX, EdgeMark markAtY);

    void eraseEdge(const Edge& edge) override;
    void clearEdges() override;
    void eraseNode(NodeId id) override;
    void clear() override;
    /// removes all the nodes, hence all the edges and their marks (alias for clear())
    void clearNodes() override;

    /// the mark at the dst-end of the edge src-dst
    /** @throw NotFound if there is no edge src-dst */
    EdgeMark markAt(NodeId src, NodeId dst) const;

    /// sets the mark at the dst-end of the edge src-dst
    /** @throw NotFound if there is no edge src-dst */
    void setMarkAt(NodeId src, NodeId dst, EdgeMark mark);

    /// sets all the marks to mark
    void reorientAllWith(EdgeMark mark);

    /// @}

    // ############################################################################
    /// @name Predicates on the marks
    // ############################################################################
    /// @{

    /// true if the mark at dst (on the edge src-dst) is an arrowhead
    /** @throw NotFound if there is no edge src-dst */
    bool isArrowhead(NodeId src, NodeId dst) const;

    /// true if the mark at dst (on the edge src-dst) is a tail
    /** @throw NotFound if there is no edge src-dst */
    bool isTail(NodeId src, NodeId dst) const;

    /// true if the mark at dst (on the edge src-dst) is a circle
    /** @throw NotFound if there is no edge src-dst */
    bool isCircle(NodeId src, NodeId dst) const;

    /// true if the edge is x->y (tail at x, arrowhead at y)
    /** @throw NotFound if there is no edge x-y */
    bool isDefinitelyDirected(NodeId x, NodeId y) const;

    /// true if the edge is x<->y
    /** @throw NotFound if there is no edge x-y */
    bool isBidirected(NodeId x, NodeId y) const;

    /// true if the edge is x--y (tail at both ends)
    /** @throw NotFound if there is no edge x-y */
    bool isUndirected(NodeId x, NodeId y) const;

    /// true if z is a collider on the path x-z-y (arrowheads at z on both edges)
    /** @throw NotFound if there is no edge x-z or no edge z-y */
    bool isDefCollider(NodeId x, NodeId z, NodeId y) const;

    /// @}

    // ############################################################################
    /// @name Directed view (computed from the marks)
    // ############################################################################
    /// @{

    /// the nodes y such that y->x
    NodeSet parents(NodeId x) const;

    /// the nodes y such that x->y
    NodeSet children(NodeId x) const;

    /// the nodes y such that x<->y
    NodeSet spouses(NodeId x) const;

    /// the nodes y such that x--y
    NodeSet undirectedNeighbours(NodeId x) const;

    /// @}

    // ############################################################################
    /// @name Views and display
    // ############################################################################
    /// @{

    /// the undirected graph of the adjacencies (with the names of the nodes)
    UndiGraph skeleton() const;

    /// the connected components of the skeleton: each node is mapped to the root of its component
    NodeProperty< NodeId > connectedComponents() const;

    std::string toString() const override;

    /// a dot representation (marks drawn as graphviz arrows: normal, odot or none)
    virtual std::string toDot() const;

    /// a dot representation, with the options given by the caller
    /** The edges with an arrowhead at both ends get the color of the configuration key
     * `dot.bidirectedEdgeColor`, the edges with a circle the color of
     * `dot.circleMarkEdgeColor` (an empty value means the color of the arcs). */
    virtual std::string toDot(const dot::DotOptions& options) const;

    /// @}

    private:
    /// _marks_[Arc(x, y)] = the mark at the y-end of the edge x-y
    ArcProperty< EdgeMark > _marks_;

    /// erases the marks of the edge x-y
    void _eraseMarks_(NodeId x, NodeId y);

    // erases the edges without their marks: hidden (eraseNeighbours erases both)
    using EdgeGraphPart::unvirtualizedEraseNeighbours;
  };

  GUM_SHARED_PUBLIC std::ostream& operator<<(std::ostream& stream, const MarkedGraph& g);

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/markedGraph_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_MARKED_GRAPH_H
