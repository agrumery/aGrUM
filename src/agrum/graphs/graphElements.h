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

/*!
  \ingroup graph_group

  \page graph_page How to use the graph classes?

  The basic idea of the implementation of graphs in aGrUM is to leave it "as
  pure as possible". Hence graphs consist of integers (@ref gum::NodeId) and binary
  relations between them (Edge, Arc).

  \section graph_sec_1 graph hierarchy
  Precisely a graph contains a compact representation of a set of
  @ref gum::NodeId, and can contain a set of @ref gum::Arc and a set of
  @ref gum::Edge.

  In aGrUM,
  - 3 classes wrap these 3 sets : @ref gum::NodeGraphPart, @ref gum::ArcGraphPart
  and @ref gum::EdgeGraphPart
  (resp. wrapping @ref gum::NodeSet, @ref gum::ArcSet and @ref gum::EdgeSet,
  3 typedefs of Set),
  - @ref gum::UndiGraph inherits from @ref gum::EdgeGraphPart and
  @ref gum::NodeGraphPart,
  - @ref gum::DiGraph inherits from @ref gum::ArcGraphPart and
  @ref gum::NodeGraphPart and
  - @ref gum::MixedGraph inherits from @ref gum::DiGraph and
  @ref gum::UndiGraph.

  (as shown in the inheritance diagram for @ref gum::MixedGraph, the class
  hierarchy for graphs in aGrUM uses multiple inheritance.)

  This pattern of inheritance allows aGrUM to have a quite standardized API for
  all basic operations in graphs: manipulating and iterating over nodes, edges
  and arcs.

  \section graph_sec_2 IDs, iterators and properties
  Such representation of graphs has two major drawbacks that aGrUM has to face:
  - how to certify that all node ids are unique in a graph?
  - how to iterate on nodes, arcs, edges in a graph ?
  - how to put properties (name/position/integer/flag) on nodes, edges and arcs?

  <h4>ID factory</h4>
  For aGrUM, each node has (is) an id. IDs begin from 1 (id>0).

  For the uniqueness of ids, addNode() in NodeGraphPart contains an internal
  idFactory which guarantees this property.

  There are two ways to bypass the idFactory :

  - use the copy constructor or the @ref gum::NodeGraphPart::populateNodes().
  These very two methods copy a set of
  nodes and ensure that the ids are the same as those of their argument
  (@ref gum::NodeGraphPart::populateNodes() clears
  the @ref gum::NodeGraphPart before copying).

  - use @ref gum::NodeGraphPart::addNode(gum::NodeId id) which allow the user
  to give a specific id to a new node. <i> This method
  should be used very carefully !! It may throw exception (id=0 or id already
  used) and it may deteriorate the compacity of
  the @ref gum::NodeGraphPart. </i>

  Good programmers should treat with respect the idFactory.

  <h4>Iterators</h4>

  @ref gum::NodeSet, @ref gum::EdgeSet and @ref gum::ArcSet have their own
  iterators (see @ref gum::NodeSetIterator etc.). However aGrUM gives specialized
  iterators for graphs. These iterators are herited from the different 3 graphs
  part and may be (or not) a <i>typedef</i> for set iterators. We encourage to
  use these iterators when iterating on a graph.

  For a graph <tt>gr</tt> of type <tt>G</tt> (<tt>G</tt> is @ref gum::DiGraph,
  @ref gum::DAG, @ref gum::UndiGraph or @ref gum::MixedGraph, etc.) :

  @code
  G gr;

  // iterate on nodes
  for(const auto node : gr.nodes()) {
    ... // node is a gum::NodeId
  }

  // iterate on edges (if possible)
  for(const auto & edge : gr.edges()) {
    ... // edge is a const gum::Edge&
  }

  // iterate on arcs (if possible)
  for(const auto & arc =gr.arcs()) {
    ... // arc is a const gum::Arc&
  }
  @endcode

  <h4>Properties</h4>

  Properties are the way to put (dynamic) informations within nodes, edges and
  arcs. Properties are just HashTable in which keys are @ref gum::NodeId, Edge
  or Arc. NodeProperty, ArcProperty and EdgeProperty are the names of these classes.
  @code
  gum::UndiGraph g;
  gum::NodeProperty<bool> is_id_odd=g.nodesProperty( false );

  g.addNode();
  g.addNode();
  g.addNode();

  for ( auto nodeId : g.nodes()) {
    is_id_odd.set( nodeId, nodeId % 2 == 0 );
  }

  std::cout<<is_id_odd<<std::cout<<std::endl;

  for ( auto i : is_id_odd)
    std::cout<<i.first()<<" : "<<i.second()<<std::endl;
  @endcode

  Equivalently, you could have written :
  @code
  bool is_it_odd(const gum::NodeId& i) {
    return i%2==0;
  }

  gum::UndiGraph g;
  g.addNode();
  g.addNode();
  g.addNode();

  gum::NodeProperty<bool> is_id_odd=g.nodesProperty(is_it_odd);

  std::cout<<is_id_odd<<std::endl<<std::endl;

  for (const auto & elt :is_id_odd) {
    std::cout<<i.first<<" : "<<i.second<<std::endl;
  }
  @endcode

  Here is a code for \subpage exemple_recherche_chemin "searching directed path
  in a DiGraph" g from \ref gum::NodeId n1 to \ref gum::NodeId n2.

  \page exemple_recherche_chemin Example: Search of a directed path in a DiGraph
  g from n1 to n2
  \ingroup graph_group

  @code

  gum::List<gum::NodeId> nodeFIFO;
  // mark[node] contains 0 if not visited
  // mark[node]=predecessor if visited
  gum::NodeProperty<gum::NodeId> mark=g.nodesProperty((gum::NodeId) 0 );
  gum::NodeId current;

  mark[n1]=n1;
  nodeFIFO.pushBack( n1 );
  while ( ! nodeFIFO.empty() ) {
    current=nodeFIFO.front();nodeFIFO.popFront();

    const gum::NodeSet& set=;
    for ( const auto new_one : g.children( current )) {
        if ( mark[new_one]!=0 ) continue; // if this node is already marked, continue
        mark[new_one]=current;
        if ( new_one==n2 ) break; // if we reach n2, stop.

        nodeFIFO.pushBack( new_one );
    }
  }

  if ( mark[n2] ==0 ) GUM_ERROR( gum::NotFound,"no path found" );
  @endcode
*/

/** @file
 * @brief some utils for topology : NodeId, Edge, Arc and consorts ...
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides two classes, namely Edge and Arc
 * which represent respectively undirected and directed edges. The
 * "directed/undirected" term may be misleading although in practice this
 * will probably result in how these edges will be drawn. In fact, a more
 * appropriate term would be "symmetric/asymmetric edges": an Arc is an edge in
 * which the extremities have different status whereas in an Edge the role of the
 * extremities is symmetric. For instance, in an arrow, one node is near the head
 * and the other one is farther, hence these nodes have different status and
 * swapping the nodes results in reversing the direction of the arrow. Thus, the
 * nodes in an arrow can be thought of as asymmetric and the arrow's graphical
 * representation contains a pointed extremity so as to account for this asymmetry.
 * Conversely, in a Markov Random Field, an edge between two nodes, x and y,
 * means that x and y are probabilistically dependent of one another. This being a
 * symmetrical relation, there is no difference between edge (x,y) and edge (y,x).
 * Thus, it can be represented by an Edge and, graphically, by an undirected edge.
 *
 * @par Usage example:
 * @code
 * // creation of an edge between nodes whose IDs are 3 and 4
 * Edge edge1 (3,4);
 *
 * // print that the edge is "undirected"
 * if (!edge1.isDirected ()) cerr << "undirected edge" << endl;
 *
 * // copy the edge into another edge
 * Edge edge2 (edge1), edge3 = edge4;
 *
 * // compare two edges
 * if (Edge(3,4) == Edge(4,3)) cerr << "ok, this is symmetric" << endl;
 * if (Edge(3,4) != Edge(5,4)) cerr << "different edges" << endl;
 *
 * // get the extremities of the edge
 * cerr << edge1.first() << " = 3 and " << edge1.second() << " = 4\n";
 * cerr << "edge1 = (3," << edge1.other (3) << ")" << endl;
 *
 * // display the edge in a console
 * cerr << edge1 << endl;
 *
 * // creation of an arc (directed edge) from 3 to 4
 * Arc arc1 (3,4);
 *
 * // print that this edge is "directed"
 * if (arc1.isDirected ()) cerr << "directed edge" << endl;
 *
 * // compare two arcs
 * if (Arc(3,4) != Arc(4,3)) cerr << "ok, this is asymmetric" << endl;
 *
 * // get the extremities of the edge
 * cerr << arc1.tail() << " = 3 and " << arc1.head() << " = 4\n";
 * cerr << "arc1 = (3 -> " << edge1.other (3) << ")" << endl;
 *
 * // display an arc in a console
 * cerr << arc1 << endl;
 * @endcode
 */
#ifndef GUM_GRAPH_ELEMENTS_H
#define GUM_GRAPH_ELEMENTS_H

#include <iostream>

#include <agrum/config.h>

#include <agrum/core/set.h>

namespace gum {

  /** \ingroup graph_group
   * Type for node ids
   */
  typedef unsigned int NodeId;

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                       GENERIC UNDIRECTED EDGES                      === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Edge
   * @brief The base class for all undirected edges.
   * \ingroup graph_group
   *
   * This class is used as a basis for manipulating any undirected edge in any
   * graph. By undirected edge, we mean a symmetric edge, i.e., an edge in which
   * the order of the nodes is unimportant. For instance, in Markov Random
   * fields, an edge between two nodes, x and y, means that x and y are
   * probabilistically dependent of one another. This being a symmetrical relation,
   * there is no difference between edge (x,y) and edge (y,x). Thus, it can be
   * represented by an undirected edge and, in aGrUM, by an Edge.
   * @par Usage example:
   * @code
   * // creation of an edge between nodes whose IDs are 3 and 4
   * Edge edge1 (3,4);
   *
   * // print that the edge is "undirected"
   * if (!edge1.isDirected ()) cerr << "undirected edge" << endl;
   *
   * // copy the edge into another edge
   * Edge edge2 (edge1), edge3 = edge4;
   *
   * // compare two edges
   * if (Edge(3,4) == Edge(4,3)) cerr << "ok, this is symmetric" << endl;
   * if (Edge(3,4) != Edge(5,4)) cerr << "different edges" << endl;
   *
   * // get the extremities of the edge
   * cerr << edge1.first() << " = 3 and " << edge1.second() << " = 4\n";
   * cerr << "edge1 = (3," << edge1.other (3) << ")" << endl;
   *
   * // display the edge in a console
   * cerr << edge1 << endl;
   * @endcode
   */
  /* =========================================================================== */

  class Edge {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructs a new edge (aN1,aN2)
    /** @param aN1 the ID of the first extremal node
     * @param aN2 the ID of the second extremal node */
    Edge(NodeId aN1, NodeId aN2);

    /// copy constructor
    Edge(const Edge &src);

    /// destructor
    ~Edge();

    ///@}

    // ############################################################################
    /// @name Accessors
    // ############################################################################
    /// @{

    /// returns a Boolean (false) indicating that the edge is undirected
    bool isDirected() const;

    /// returns an extremal node of an edge given the ID of the other one
    NodeId other(NodeId id) const;

    /// returns one extremal node ID (whichever one it is is unspecified)
    NodeId first() const;

    /// returns the node ID of the other extremal node ID
    NodeId second() const;

    ///@}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Edge &operator=(const Edge &src);

    /// checks whether two undirected edges are equal
    /** Two Edge are equal if they have the same extremal nodes, whetever their
     * order. For instance (3,4) == (4,3). */
    bool operator==(const Edge &src) const;

    /// checks whether two undirected edges are different
    /** Two Edge are different if at least one extremal node of an edge is not
     * an extremal node of the other edge. For instance, (4,5) != (5,6). */
    bool operator!=(const Edge &src) const;

    /// @}

    private:
    /// the extremal nodes of the edge (their order is unimportant)
    NodeId n1, n2;
  };

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                        GENERIC DIRECTED EDGES                       === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Arc
   * @brief The base class for all directed edges
   * \ingroup graph_group
   *
   * This class is used as a basis for manipulating all directed edges (i.e., edges
   * in which the order of the nodes is meaningful). For instance, in an arrow, one
   * node is near the head and the other one is farther, hence these nodes have
   * different status and swapping the nodes results in reversing the direction of
   * the arrow. Thus, the nodes in an arrow can be thought of as asymmetric and the
   * arrow's graphical representation contains a pointed extremity so as to account
   * for this asymmetry. In aGrUM, the latter is taken into account by Arc.
   * @par Usage example:
   * @code
   * // creation of an arc (directed edge) from 3 to 4
   * Arc arc1 (3,4);
   *
   * // print that this edge is "directed"
   * if (arc1.isDirected ()) cerr << "directed edge" << endl;
   *
   * // compare two arcs
   * if (Arc(3,4) != Arc(4,3)) cerr << "ok, this is asymmetric" << endl;
   *
   * // get the extremities of the edge
   * cerr << arc1.tail() << " = 3 and " << arc1.head() << " = 4\n";
   * cerr << "arc1 = (3 -> " << edge1.other (3) << ")" << endl;
   *
   * // display an arc in a console
   * cerr << arc1 << endl;
   * @endcode
   */
  /* =========================================================================== */

  class Arc {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. Creates tail -> head.
    /** @warning the order in which the nodes are passed is important */
    Arc(NodeId tail, NodeId head);

    /// copy constructor
    Arc(const Arc &src);

    /// destructor
    ~Arc();

    /// @}

    // ############################################################################
    /// @name Accessors
    // ############################################################################
    /// @{

    /// returns the tail of the arc
    NodeId tail() const;

    /// returns the head of the arc
    NodeId head() const;

    /// returns a Boolean (true) indicating that the arc is directed
    bool isDirected() const;

    /// returns an extremal node of an edge given the ID of the other one
    NodeId other(NodeId id) const;

    /// returns one extremal node ID (whichever one it is is unspecified)
    NodeId first() const;

    /// returns the node ID of the other extremal node ID
    NodeId second() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Arc &operator=(const Arc &src);

    /// checks whether two arcs are equal
    /** Two arcs are considered equal if they have the same head and tail
     * (by same we mean they have the same ID). */
    bool operator==(const Arc &src) const;

    /// check if two arcs are different
    /** Two arcs are considered different if they have different head and/or tail
     * (by different we mean they have different ID). */
    bool operator!=(const Arc &src) const;

    /// @}

    private:
    /// the extremal nodes of the edge (their order is unimportant)
    NodeId n1, n2;

    /// modifies the tail of the arc
    void __setTail(NodeId id);

    /// modifies the head of the arc
    void __setHead(NodeId id);

    /// reverses the direction of the arc
    void operator-();
  };

////////////////////////////////////////////////////////////////
// we need to provide hash functions for some Edge and Arc

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <> class HashFunc<Edge> : public HashFuncSmallKeyPair<NodeId, NodeId> {
    public:
    /**
     * @throw HashSize
     */
    Size operator()(const Edge &key) const;

    private:
    mutable std::pair<NodeId, NodeId> pair;
  };

  template <> class HashFunc<Arc> : public HashFuncSmallKeyPair<NodeId, NodeId> {
    public:
    /**
     * @throw HashSize
     */
    Size operator()(const Arc &key) const;

    private:
    mutable std::pair<NodeId, NodeId> pair;
  };

#endif // DOXYGEN_SHOULD_SKIP_THIS

  /** \ingroup graph_group
   * @{
   * Some typdefs and define for shortcuts ...
   */
  typedef Set<NodeId> NodeSet;
  typedef Set<Edge> EdgeSet;
  typedef Set<Arc> ArcSet;

  typedef ArcSet::const_iterator ArcSetIterator;
  typedef EdgeSet::const_iterator EdgeSetIterator;
  typedef NodeSet::const_iterator NodeSetIterator;
  /** @} */

  /** \ingroup graph_group
   * @{
   * @brief Property on graph elements
   *
   * @{
   * @deprecated C++11 allows now partial specification of template using "using".
   *The
   * old definitions Property VAL::onNodes are now obsolete. Please use NodeProperty
   *VAL
   **/

  template <class VAL> struct Property {
    typedef HashTable<NodeId, VAL> GUM_DEPRECATED(onNodes);
    typedef HashTable<Edge, VAL> GUM_DEPRECATED(onEdges);
    typedef HashTable<Arc, VAL> GUM_DEPRECATED(onArcs);
  };
  /// @}

  template <class VAL> using NodeProperty = HashTable<NodeId, VAL>;
  template <class VAL> using EdgeProperty = HashTable<Edge, VAL>;
  template <class VAL> using ArcProperty = HashTable<Arc, VAL>;

  /// @}

  /// to friendly display an edge
  std::ostream &operator<<(std::ostream &stream, const Edge &edge);

  /// to friendly display an arc
  std::ostream &operator<<(std::ostream &stream, const Arc &arc);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/graphElements.inl>
#endif /* GUM_NO_INLINE */

#endif // GUM_GRAPHELEMENTS_H
