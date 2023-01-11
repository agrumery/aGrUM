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
 * @brief Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_DIGRAPH_H
#define GUM_DIGRAPH_H

#include <iostream>
#include <sstream>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/sequence.h>

#include <agrum/tools/graphs/parts/arcGraphPart.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===           BASE CLASS FOR MANIPULATING ALL DIRECTED GRAPHS           ===
   */
  /* ===========================================================================
   */
  /** @class DiGraph
   * @brief Base class for all oriented graphs
   *
   * \ingroup graph_group
   *
   *
   * This is the base class for graphs containing directed edges (so-called
   *arcs)
   * *
   * @par Usage example:
   * @code
   * // creating empty graphs
   * DiGraph g1,g2;
   *
   * // adding nodes and arcs to g1
   * NodeId i1=g1.addNode();
   * NodeId i2=g1.addNode();
   * NodeId i3=g1.addNode();
   * g1.addArc( i1,i2 );
   * g1.addArc( i1,i3 );
   * g1.addArc( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.addArc( i1+i2+i3,i1 );
   *
   * // copying graphs
   * DiGraph g3 = g1;
   * g2 = g1;
   * DiGraph g4=g1;
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
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( const auto node : g3.nodes() )
   *   cerr << node<< endl;
   *
   * for ( const auto & arc : g3.arcs())
   *   cerr << arc << endl;
   *
   * const ArcSet& a=g3.parents( 3 );
   *
   * for ( const auto & par : g3.parents(3))
   *   cerr << "  -  "<< par;
   *
   * cerr<<endl;
   *
   * // remove all the arcs that are parent of a given node
   * g3.eraseParents( 2 );
   * @endcode
   */
  /* ===========================================================================
   */
  class DiGraph: public virtual NodeGraphPart, public ArcGraphPart {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    explicit DiGraph(Size nodes_size          = HashTableConst::default_size,
                     bool nodes_resize_policy = true,
                     Size arcs_size           = HashTableConst::default_size,
                     bool arcs_resize_policy  = true);

    /// copy constructor
    /** @param g the DiGraph to copy */
    DiGraph(const DiGraph& g);

    /// destructor
    virtual ~DiGraph();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the DiGraph to copy */
    DiGraph& operator=(const DiGraph& g);

    /// tests whether two DiGraphs are identical (same nodes, same arcs)
    /** @param g the DiGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator==(const DiGraph& g) const;

    /// tests whether two DiGraphs are different
    /** @param g the DiGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator!=(const DiGraph& g) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the directed graph
    /** @param tail the id of the tail of the new inserted arc
     * @param head the id of the head of the new inserted arc
     * @warning if the arc already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if head or tail does not belong to the graph nodes */
    virtual void addArc(const NodeId tail, const NodeId head);

    /// remove a node and its adjacent arcs from the graph
    /** @param id the id of the node to be removed
     * @warning if the node does not exist, nothing is done. In particular, no
     * exception is raised.*/
    virtual void eraseNode(const NodeId id);

    /// removes all the nodes and arcs from the graph
    virtual void clear();

    /// to friendly display the content of the graph
    virtual std::string toString() const;

    /// to friendly display the content of the graph in the DOT syntax
    /** @param name The graph name in the dot syntax. Default is G.
     * @return Returns a string describing the graph in the dot syntax */
    virtual std::string toDot() const;

    /**
     * Build and return a topological order
     *
     * @throw InvalidDirectedCycle Raised if this DiGraph contains cycles.
     */
    Sequence< NodeId > topologicalOrder() const;
    /// @}

    /** checks whether there exists a directed path from \e from to \e to
     *
     * If from==to, this function checks if a directed cycle containing \e from
     * exists.
     * @param from
     * @param to
     * @return true if a directed path exists
     *
     */
    bool hasDirectedPath(NodeId from, NodeId to);
  };

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream&, const DiGraph&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/diGraph_inl.h>
#endif   // GUM_NOINLINE

#endif /* GUM_DIGRAPH_H */
