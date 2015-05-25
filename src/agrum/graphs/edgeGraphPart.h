/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef GUM_EDGE_GRAPH_PART_H
#define GUM_EDGE_GRAPH_PART_H

#include <algorithm>
#include <utility>

#include <agrum/config.h>

#include <agrum/core/signal/signaler.h>

#include <agrum/graphs/graphElements.h>

namespace gum {

  /** @class EdgeGraphPart
   * @brief Classes for undirected edge sets
   *
   * \ingroup graph_group
   *
   * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
   *
   * @par Usage example:
   * @code
   * EdgeGraphPart edges1,edges2;
   *
   * // insert elements into edges1
   * edges1.addEdge( 2,3 );
   * Edge edge( 5,3 );
   * edges1.addEdge( 5,3 );
   *
   * // copy edges1 into edges2
   * edges2=edges1;
   * std::cerr<<"edges2:"<<edges2.toString()<<std::endl;
   *
   * // remove some elements from edges1
   * edges1.eraseEdge( Edge (2,3) );
   * edges1.eraseEdge( edge );
   *
   * if ( edges1.empty() ) std::cerr<<" edges1 is empty"<<std::endl;
   *
   * // checks whether a given edge exists
   * if ( edges2.exists( edge ) )
   *   std::cerr << "set contains " << edge << endl;
   *
   * if ( edges2.exists( 5,3 ) )
   *   std::cerr << "set contains " << edge << endl;
   *
   * std::cerr<<edges2.toString()<<std::endl;
   * std::cerr<<edges2.neighbours( 5 )<<std::endl;
   * @endcode
   */

  class EdgeGraphPart {
    public:
    typedef EdgeSetIterator EdgeIterator;

    Signaler2<NodeId, NodeId> onEdgeAdded;
    Signaler2<NodeId, NodeId> onEdgeDeleted;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit EdgeGraphPart(Size edges_size = HashTableConst::default_size,
                           bool edges_resize_policy = true);

    /// copy constructor
    /** @param s the EdgeGraphPart to copy */
    EdgeGraphPart(const EdgeGraphPart &s);

    /// destructor
    virtual ~EdgeGraphPart();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param s the EdgeGraphPart to copy */
    EdgeGraphPart &operator=(const EdgeGraphPart &s);

    /// tests whether two EdgeGraphParts contain the same edges
    /** @param p the EdgeGraphPart that we compare with this */
    bool operator==(const EdgeGraphPart &p) const;

    ///  tests whether two EdgeGraphParts contain different edges
    /** @param p the EdgeGraphPart that we compare with this */
    bool operator!=(const EdgeGraphPart &p) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new edge into the EdgeGraphPart
    /** @param n1 the id of one extremity of the new edge to be inserted
     * @param n2 the id of the other extremity of the new edge to be inserted
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised. */
    GUM_DEPRECATED(virtual void insertEdge(const NodeId n1, const NodeId n2));

    /// insert a new edge into the EdgeGraphPart
    /** @param n1 the id of one extremity of the new edge to be inserted
     * @param n2 the id of the other extremity of the new edge to be inserted
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised. */
    virtual void addEdge(const NodeId n1, const NodeId n2);

    /// removes an edge from the EdgeGraphPart
    /** @param edge the edge to be removed
     * @warning if the edge does not exist, nothing is done. In particular, no
     * exception is thrown. However, the signal onEdgeDeleted is fired
     * only if a node is effectively removed. */
    virtual void eraseEdge(const Edge &edge);

    /// indicates whether a given edge exists
    /** @param edge the edge we test whether or not it belongs to the
     * EdgeGraphPart */
    bool existsEdge(const Edge &edge) const;

    /// indicates whether a given edge exists
    /** @param n1 the id of one extremity of the edge we test the existence in
     * the EdgeGraphPart
     * @param n2 the id of the other extremity of the edge we test the existence
     * in the EdgeGraphPart */
    bool existsEdge(const NodeId n1, const NodeId n2) const;

    /// indicates wether the EdgeGraphPart contains any edge
    bool emptyEdges() const;

    /// removes all the edges from the EdgeGraphPart
    virtual void clearEdges();

    /// indicates the number of edges stored within the EdgeGraphPart
    Size sizeEdges() const;

    /// returns the set of edges stored within the EdgeGraphPart
    const EdgeSet &edges() const;

    /// returns the set of edges adjacent to a given node
    /** Note that the set of edges returned may be empty if no edge within the
     * EdgeGraphPart is adjacent the given node.
     * @param id the node to which the edges are adjacent */
    const NodeSet &neighbours(const NodeId id) const;

    /// erase all the edges adjacent to a given node
    /** @param id the node the adjacent edges of which will be removed
     * @warning if no edge is adjacent to id, nothing is done. In particular, no
     * exception is thrown.
     * @warning although this method is not virtual, it calls method
     * eraseEdge( const Edge& edge ) and, as such, has a "virtual" behaviour */
    void eraseNeighbours(const NodeId id);

    /// same function as eraseNeighbours but without any virtual call to an erase
    /** @param id the node whose ingoing arcs will be removed */
    void unvirtualizedEraseNeighbours(const NodeId id);

    /// returns an iterator to parse the set of edges of the EdgeGraphPart
    /// @deprecated Please use edges().begin() or better :
    /// @code for(const auto& edge : graph.edges()) @endcode
    GUM_DEPRECATED(const EdgeGraphPart::EdgeIterator beginEdges() const);

    /// returns the end iterator to parse the set of edges
    /// @deprecated Please use edges().end() or better :
    /// @code for(const auto& edge : graph.edges()) @endcode
    GUM_DEPRECATED(const EdgeGraphPart::EdgeIterator &endEdges() const);

    /// to friendly display the content of the EdgeGraphPart
    const std::string toString() const;

    /** @brief a method to create a hashMap of VAL from a set of edges
     * (using for every edge, say x, the VAL f(x))
     * @param f a function assigning a VAL to any edge
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of edges. If you do not specify this parameter, the method
     * will assign it for you. */
    template <typename VAL>
    EdgeProperty<VAL> edgesProperty(VAL (*f)(const Edge &), Size size = 0) const;

    /** @brief a method to create a hashMap of VAL from a set of edges
     * (using for every edge, say x, the VAL a)
     * @param a the default value assigned to each edge in the returned Property
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of edges. If you do not specify this parameter, the method
     * will assign it for you. */
    template <typename VAL>
    EdgeProperty<VAL> edgesProperty(const VAL &a, Size size = 0) const;

    /** @brief a method to create a list of VAL from a set of edges
     * (using for every edge, say x, the VAL f(x))
     * @param f a function assigning a VAL to any edge */
    template <typename VAL> List<VAL> listMapEdges(VAL (*f)(const Edge &)) const;

    /// returns a possible path from node1 to node2 in the edge set
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    const std::vector<NodeId> undirectedPath(const NodeId node1,
                                             const NodeId node2) const;

    /// @}

    private:
    /// the set of all the edges contained within the EdgeGraphPart
    EdgeSet __edges;

    /// for each node, the set of its adjacent edges
    mutable NodeProperty<NodeSet *> __neighbours;

    /** @brief when the EdgeGraphPart contains no edge adjacent to a given node,
     * this function adds an empty set entry to __neighbours[id]
     * @param id the node whose __neighbours[id] is checked */
    void __checkNeighbours(const NodeId id) const;
  };

  /// for friendly displaying the content of an edge set
  std::ostream &operator<<(std::ostream &, const EdgeGraphPart &);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/edgeGraphPart.inl>
#endif // GUM_NOINLINE

#include <agrum/graphs/edgeGraphPart.tcc>

#endif // GUM_EDGEGRAPHPART_H
