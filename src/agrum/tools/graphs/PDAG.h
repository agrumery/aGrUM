/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Base classes for partially directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MAG_H
#define GUM_MAG_H

#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/DAG.h>

namespace gum {

  /* =================================================================== */
  /* ===      BASE CLASS FOR MANIPULATING PARTIALLY DIRECTED ACYCLIC GRAPHS */
  /* =================================================================== */
  /** @class PDAG
   * @brief Base class for partially directed acyclic graphs
   *
   * \ingroup graph_group
   *
   *
   * This is the base class for partially directed acyclic graph : addArc and addEdge may throw a
   * DirectedCycle if any (mixed) cycle is created by this arc/edge.
   *
   * @par exemple de code
   * @code
   * // creating empty graphs
   * gum::PDAG g1,g2;
   *
   * // adding nodes and arcs to g1
   * gum::NodeId i1=g1.addNode();
   * gum::NodeId i2=g1.addNode();
   * gum::NodeId i3=g1.addNode();
   * g1.addArc( i1,i2 );
   * g1.addArc( i1,i3 );
   * g1.addEdge( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.addArc( i1+i2+i3,i1 );
   *
   * // throw an InvalidMixedCycle
   * // g1.addArc( i3,i1 );
   *
   * // throw an InvalidMixedCycle
   * // g1.addEdge( i3,i1 );
   *
   * // copying graphs
   * gum::PDAG g3 = g1;
   * g2 = g1;
   * gum::PDAG g4=g1;
   *
   * // check if a graph has no node
   * if ( g1.empty() ) cerr << "graph g1 is empty" << endl;
   *
   * // remove all the nodes (as well as their adjacent arcs)
   * g1.clear();
   *
   * // remove some arc
   * g4.eraseArc( Arc ( i1,i3 ) );
   *
   * // remove some edge
   * g4.eraseEdge( Edge ( i1,i3 ) );
   *
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( const auto node : g3.nodes() ) // type of node = gum::NodeId
   *   cerr << node << endl;
   *
   * for ( const auto& arc= g3.arcs()) // type of arc : gum::Arc&
   *   cerr << iter << endl;
   *
   * for ( const auto& arc= g3.edge()) // type of edge : gum::Edge&
   *   cerr << iter << endl;
   *
   * for ( const auto node :g3.parents( gum::NodeId(3) ))
   *   cerr << "  -  "<<*iter;
   *
   * for ( const auto node :g3.neighbours( gum::NodeId(3) ))
   *   cerr << "  -  "<<*iter;
   *
   * cerr<<endl;
   *
   * // remove all the arcs that are parent of a given node
   * g3.eraseParents( gum::NodeId(2) );
   *
   * @endcode
   */
  /* ====================================================================== */
  class PDAG: public MixedGraph {
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
     * @param arcs_resize_policy the resizing policy of this hash table.
     */
    PDAG(Size nodes_size          = HashTableConst::default_size,
         bool nodes_resize_policy = true,
         Size arcs_size           = HashTableConst::default_size,
         bool arcs_resize_policy  = true,
         Size edges_size          = HashTableConst::default_size,
         bool edges_resize_policy = true);

    explicit PDAG(const UndiGraph& g);
    explicit PDAG(const DAG& g);

    /// copy constructor
    /** @param g the PDAG to copy */
    PDAG(const PDAG& g);

    /// destructor
    virtual ~PDAG();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the PDAG to copy */
    PDAG& operator=(const PDAG& g);

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the directed graph
    /**
     * @param tail the id of the tail of the new inserted arc
     * @param head the id of the head of the new inserted arc
     * @warning if the arc already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if head or tail does not belong to the graph nodes
     * @throw InvalidMixedCycle if any (mixed) cycle is created by this
     * arc.
     * @warning Unfortunately, this means that addArc is not in constant
     * time anymore.
     */
    void addArc(NodeId tail, NodeId head) final;
    /// @}

    /// insert a new edge into the partially directed graph
    /**
     * @param first the id of the first node of the new inserted edge
     * @param second the id of the second node of the new inserted edge
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if first or second does not belong to the graph nodes
     * @throw InvalidMixedCycle if any (mixed) cycle is created by this
     * edge.
     * @warning Unfortunately, this means that addEdge is not in constant
     * time anymore.
     */
    void addEdge(NodeId first, NodeId second) final;
    /// @}

    /** build a UndiGraph by moralizing the PDAG
     *
     * @return the moralized graph
     */
    UndiGraph moralGraph() const;

    /** build a UndiGraph by moralizing the Ancestral Graph of a set of Nodes
     *
     * @param nodes the set of nodeId
     * @return the moralized ancestral graph
     */
    UndiGraph moralizedAncestralGraph(const NodeSet& nodes) const;

    /** @brief returns true if a mixed edge/directed arc path from node1 to node2 in the
     * arc/edge set exists. Really mean that we need at least one arc in the path (undirected path
     * are not OK).
     *
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     */
    bool hasMixedReallyOrientedPath(NodeId n1, NodeId n2) const;

    /** check if node X and node Y are independent given nodes Z (in the sense of
     * c-separation)*/
    bool cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const;

    /** check if nodes X and nodes  Y are independent given Z (in the sense of
     * d-separation)*/
    bool cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const;

    /// to friendly display mixed graph in DOT format
    std::string toDot() const override;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/PDAG_inl.h>
#endif   // GUM_NOINLINE

#endif /* GUM_MAG_H */
