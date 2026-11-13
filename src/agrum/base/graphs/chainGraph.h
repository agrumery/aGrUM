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
 * @brief Chain graphs: mixed graphs without partially directed cycle
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_CHAIN_GRAPH_H
#define GUM_CHAIN_GRAPH_H

#include <agrum/base/graphs/PDAG.h>

namespace gum {

  /** @class ChainGraph
   * @brief Chain graph: a PDAG without partially directed cycle
   *
   * \ingroup graph_group
   *
   * A chain graph is a mixed graph (arcs and undirected edges, at most one connection between two
   * nodes) without partially directed cycle (also called semi-directed cycle), i.e. without any
   * cycle of at least 3 distinct nodes containing at least one arc, in which every arc is followed
   * in its direction and every edge in either direction (Lauritzen and Wermuth 1989; Frydenberg
   * 1990; Andersson et al. 2001; Wienöbst et al. 2021, Sec. 2). Equivalently, the connected
   * components of its undirected part (the chain components) are linked by arcs that form a
   * directed acyclic graph over the components. DAGs and undirected graphs are chain graphs.
   *
   * This class is the graph only: the independence model read from a chain graph depends on its
   * interpretation (LWF, AMP or MVR; Sonntag and Peña 2015). The separation and moralization
   * methods of this class follow the LWF interpretation (c-separation, Bouckaert and Studený 1995;
   * Studený and Bouckaert 1998).
   *
   * addArc and addEdge check the absence of partially directed cycle, in addition to the checks of
   * gum::PDAG.
   *
   * @warning The assignment operators are not virtual: a chain graph assigned through a reference
   * to gum::PDAG or gum::MixedGraph is not checked and may then contain a partially directed cycle.
   * Use the checked constructor ChainGraph(const PDAG&) to convert a PDAG.
   *
   * @par References
   * - S. L. Lauritzen, N. Wermuth. Graphical Models for Associations between Variables, some of
   *   which are Qualitative and some Quantitative. The Annals of Statistics 17(1):31-57, 1989.
   *   doi:10.1214/aos/1176347003.
   * - M. Frydenberg. The chain graph Markov property. Scandinavian Journal of Statistics
   *   17(4):333-353, 1990.
   * - S. A. Andersson, D. Madigan, M. D. Perlman. Alternative Markov Properties for Chain Graphs.
   *   Scandinavian Journal of Statistics 28(1):33-85, 2001. doi:10.1111/1467-9469.00224.
   * - R. R. Bouckaert, M. Studený. Chain graphs: Semantics and expressiveness. ECSQARU 1995, LNCS
   *   946, pp. 69-76, Springer. doi:10.1007/3-540-60112-0_9.
   * - M. Studený, R. R. Bouckaert. On chain graph models for description of conditional
   *   independence structures. The Annals of Statistics 26(4):1434-1495, 1998.
   *   doi:10.1214/aos/1024691250.
   * - D. Sonntag, J. M. Peña. Chain graph interpretations and their relations revisited.
   *   International Journal of Approximate Reasoning 58:39-56, 2015.
   *   doi:10.1016/j.ijar.2014.12.001.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   */
  class GUM_SHARED_PUBLIC ChainGraph: public PDAG {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     * @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table
     */
    ChainGraph(Size nodes_size          = HashTableConst::default_size,
               bool nodes_resize_policy = true,
               Size arcs_size           = HashTableConst::default_size,
               bool arcs_resize_policy  = true,
               Size edges_size          = HashTableConst::default_size,
               bool edges_resize_policy = true);

    /// an undirected graph is a chain graph
    /** @throw InvalidDirectedCycle if g contains an edge from a node to itself */
    explicit ChainGraph(const UndiGraph& g);

    /// a DAG is a chain graph
    explicit ChainGraph(const DAG& g);

    /// copy of a PDAG
    /** @throw InvalidPartiallyDirectedCycle if g contains a partially directed cycle */
    explicit ChainGraph(const PDAG& g);

    /// copy of a mixed graph
    /** @throw InvalidDirectedCycle if g contains a directed cycle or an edge from a node to itself
     * @throw InvalidPartiallyDirectedCycle if g contains a partially directed cycle
     * @throw InvalidArc if a pair of nodes of g is connected by an arc and an edge */
    explicit ChainGraph(const MixedGraph& g);

    /// copy constructor
    ChainGraph(const ChainGraph& g);

    /// move constructor
    ChainGraph(ChainGraph&& g);

    /// destructor
    ~ChainGraph() override;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ChainGraph& operator=(const ChainGraph& g);

    /// move assignment operator
    ChainGraph& operator=(ChainGraph&& g);

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the chain graph
    /**
     * @param tail the id of the tail of the new inserted arc
     * @param head the id of the head of the new inserted arc
     * @warning if the arc already exists, nothing is done. In particular, no exception is raised.
     * @throw InvalidNode if head or tail does not belong to the graph nodes
     * @throw InvalidArc if an edge already connects tail and head
     * @throw InvalidDirectedCycle if the arc creates a directed cycle (head == tail included)
     * @throw InvalidPartiallyDirectedCycle if the arc creates a partially directed cycle, i.e. if
     * there is a path from head to tail following the arcs in their direction and the edges in
     * either direction
     */
    void addArc(NodeId tail, NodeId head) final;

    /// insert a new edge into the chain graph
    /**
     * @param first the id of the first node of the new inserted edge
     * @param second the id of the second node of the new inserted edge
     * @warning if the edge already exists, nothing is done. In particular, no exception is raised.
     * @throw InvalidNode if first or second does not belong to the graph nodes
     * @throw InvalidEdge if an arc already connects first and second
     * @throw InvalidDirectedCycle if first == second
     * @throw InvalidPartiallyDirectedCycle if the edge creates a partially directed cycle, i.e. if
     * there is a path between first and second, in one direction or the other, following the arcs
     * in their direction and the edges in either direction, with at least one arc
     */
    void addEdge(NodeId first, NodeId second) final;

    /// @}

    /// returns true if a path from n1 to n2 exists, following the arcs in their direction and the
    /// edges in either direction, with at least one arc
    /** Linear in the size of the graph: without partially directed cycle, a walk with an arc can
     * be reduced to a path with an arc.
     * @param n1 the id from which the path begins
     * @param n2 the id to which the path ends
     * @return false if n1 == n2
     */
    bool hasMixedReallyOrientedPath(NodeId n1, NodeId n2) const;

    // ############################################################################
    /// @name Separation (LWF interpretation)
    // ############################################################################
    /// @{

    /// the moral graph (LWF): the parents of each chain component are married, then all the
    /// arcs become edges
    UndiGraph moralGraph() const;

    /// the moral graph (LWF) of the smallest ancestral set containing nodes
    /** @param nodes the set of nodeId */
    UndiGraph moralizedAncestralGraph(const NodeSet& nodes) const;

    /// check if node X and node Y are independent given nodes Z (c-separation, LWF)
    bool cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const;

    /// check if nodes X and nodes Y are independent given nodes Z (c-separation, LWF)
    bool cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const;

    /// @}

    /// to friendly display the chain graph in DOT format: one cluster for each chain component
    std::string toDot() const override;

    /// to friendly display the chain graph in DOT format (one cluster for each chain component),
    /// with the options given by the caller
    std::string toDot(const dot::DotOptions& options) const override;
  };

} /* namespace gum */

static_assert(gum::GUM_MixedGraphable< gum::ChainGraph >);

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/chainGraph_inl.h>
#endif   // GUM_NO_INLINE

#endif   /* GUM_CHAIN_GRAPH_H */
