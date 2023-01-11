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


#ifndef GUM_EDGE_GRAPH_PART_H
#define GUM_EDGE_GRAPH_PART_H


#include <agrum/agrum.h>
#include <algorithm>
#include <utility>

#include <agrum/tools/core/signal/signalers.h>

#include <agrum/tools/graphs/graphElements.h>

namespace gum {

  /** @class EdgeGraphPart
   * @brief Classes for undirected edge sets
   *
   * \ingroup graph_group
   *
   * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
    using EdgeIterator = EdgeSetIterator;

    Signaler2< NodeId, NodeId > onEdgeAdded;
    Signaler2< NodeId, NodeId > onEdgeDeleted;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit EdgeGraphPart(Size edges_size          = HashTableConst::default_size,
                           bool edges_resize_policy = true);

    /// copy constructor
    /** @param s the EdgeGraphPart to copy */
    EdgeGraphPart(const EdgeGraphPart& s);

    /// destructor
    virtual ~EdgeGraphPart();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param s the EdgeGraphPart to copy */
    EdgeGraphPart& operator=(const EdgeGraphPart& s);

    /// tests whether two EdgeGraphParts contain the same edges
    /** @param p the EdgeGraphPart that we compare with this */
    bool operator==(const EdgeGraphPart& p) const;
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
    virtual void addEdge(NodeId n1, NodeId n2);

    /// removes an edge from the EdgeGraphPart
    /** @param edge the edge to be removed
     * @warning if the edge does not exist, nothing is done. In particular, no
     * exception is thrown. However, the signal onEdgeDeleted is fired
     * only if a node is effectively removed. */
    virtual void eraseEdge(const Edge& edge);

    /// indicates whether a given edge exists
    /** @param edge the edge we test whether or not it belongs to the
     * EdgeGraphPart */
    bool existsEdge(const Edge& edge) const;

    /// indicates whether a given edge exists
    /** @param n1 the id of one extremity of the edge we test the existence in
     * the EdgeGraphPart
     * @param n2 the id of the other extremity of the edge we test the existence
     * in the EdgeGraphPart */
    bool existsEdge(NodeId n1, NodeId n2) const;

    /// indicates wether the EdgeGraphPart contains any edge
    bool emptyEdges() const;

    /// removes all the edges from the EdgeGraphPart
    virtual void clearEdges();

    /// indicates the number of edges stored within the EdgeGraphPart
    Size sizeEdges() const;

    /// returns the set of edges stored within the EdgeGraphPart
    const EdgeSet& edges() const;

    /// returns the set of node neighbours to a given node
    /** Note that the set of nodes returned may be empty if no edge within the
     * EdgeGraphPart is adjacent the given node.
     * @param id the node to which the edges are adjacent */
    const NodeSet& neighbours(NodeId id) const;

    /// erase all the edges adjacent to a given node
    /** @param id the node the adjacent edges of which will be removed
     * @warning if no edge is adjacent to id, nothing is done. In particular, no
     * exception is thrown.
     * @warning although this method is not virtual, it calls method
     * eraseEdge( const Edge& edge ) and, as such, has a "virtual" behaviour */
    void eraseNeighbours(NodeId id);

    /// same function as eraseNeighbours but without any virtual call to an
    /// erase
    /** @param id the node whose ingoing arcs will be removed */
    void unvirtualizedEraseNeighbours(NodeId id);

    /// to friendly display the content of the EdgeGraphPart
    virtual std::string toString() const;

    /** @brief a method to create a hashMap of VAL from a set of edges
     * (using for every edge, say x, the VAL f(x))
     * @param f a function assigning a VAL to any edge
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of edges. If you do not specify this parameter, the
     * method
     * will assign it for you. */
    template < typename VAL >
    EdgeProperty< VAL > edgesProperty(VAL (*f)(const Edge&), Size size = 0) const;

    /** @brief a method to create a hashMap of VAL from a set of edges
     * (using for every edge, say x, the VAL a)
     * @param a the default value assigned to each edge in the returned Property
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of edges. If you do not specify this parameter, the
     * method
     * will assign it for you. */
    template < typename VAL >
    EdgeProperty< VAL > edgesProperty(const VAL& val, Size size = 0) const;

    /** @brief a method to create a list of VAL from a set of edges
     * (using for every edge, say x, the VAL f(x))
     * @param f a function assigning a VAL to any edge */
    template < typename VAL >
    List< VAL > listMapEdges(VAL (*f)(const Edge&)) const;

    /// returns a possible path from node1 to node2 in the edge set
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    std::vector< NodeId > undirectedPath(NodeId node1, NodeId node2) const;
    /**
     * return true if n1 and n2 are connected (by an undirected path) in the graph.
     * @param n1 NodeId
     * @param n2 NodeId
     * @return bool
     */
    bool hasUndirectedPath(NodeId n1, NodeId n2) const;

    /**
     * return true if n1 and n2 are connected (by an undirected path not using the
     * nodes of except) in the graph.
     * @param n1 NodeId
     * @param n2 NodeId
     * @param except NodeSet
     * @warning n1 in except has no repercussion. However n2 in except naturally
     * leads to 'false'
     * @return bool
     */
    bool hasUndirectedPath(NodeId n1, NodeId n2, const NodeSet& except) const;
    /**
     * return true if n1 and n2 are connected (by an undirected path not using the
     * nodes of except) in the graph.
     * @param n1 NodeSet
     * @param n2 NodeSet
     * @param except NodeSet
     * @return bool
     */
    bool hasUndirectedPath(const NodeSet& n1, const NodeSet& n2, const NodeSet& except) const;
    /// @}

    private:
    /// the set of all the edges contained within the EdgeGraphPart
    EdgeSet _edges_;

    /// for each node, the set of its adjacent edges
    NodeProperty< NodeSet* > _neighbours_;

    /** @brief when the EdgeGraphPart contains no edge adjacent to a given node,
     * this function adds an empty set entry to  _neighbours_[id]
     * @param id the node whose  _neighbours_[id] is checked */
    void _checkNeighbours_(NodeId id);

    void _clearEdges_();
  };

  /// for friendly displaying the content of an edge set
  std::ostream& operator<<(std::ostream&, const EdgeGraphPart&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/edgeGraphPart_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/tools/graphs/parts/edgeGraphPart_tpl.h>

#endif   // GUM_EDGE_GRAPH_PART_H
