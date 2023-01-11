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
 * @brief some utils for topology : NodeId, Edge, Arc and consorts ...
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * This file provides two classes, namely Edge and Arc
 * which represent respectively undirected and directed edges. The
 * "directed/undirected" term may be misleading although in practice this
 * will probably result in how these edges will be drawn. In fact, a more
 * appropriate term would be "symmetric/asymmetric edges": an Arc is an edge in
 * which the extremities have different status whereas in an Edge the role of
 *the
 * extremities is symmetric. For instance, in an arrow, one node is near the
 *head
 * and the other one is farther, hence these nodes have different status and
 * swapping the nodes results in reversing the direction of the arrow. Thus, the
 * nodes in an arrow can be thought of as asymmetric and the arrow's graphical
 * representation contains a pointed extremity so as to account for this
 *asymmetry.
 * Conversely, in a Markov Random Field, an edge between two nodes, x and y,
 * means that x and y are probabilistically dependent of one another. This being
 *a
 * symmetrical relation, there is no difference between edge (x,y) and edge
 *(y,x).
 * Thus, it can be represented by an Edge and, graphically, by an undirected
 *edge.
 *
 * @par Usage example:
 * @code
 * // creation of an edge between nodes whose IDs are 3 and 4
 * Edge edge1 (3,4);
 *
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
 * *
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

#include <agrum/agrum.h>

#include <agrum/tools/core/set.h>

namespace gum {

  /** \ingroup graph_group
   * Type for node ids
   */
  using NodeId = Size;

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===                       GENERIC UNDIRECTED EDGES                      ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /** @class Edge
   * @brief The base class for all undirected edges.
   * \ingroup graph_group
   *
   * This class is used as a basis for manipulating any undirected edge in any
   * graph. By undirected edge, we mean a symmetric edge, i.e., an edge in which
   * the order of the nodes is unimportant. For instance, in Markov Random
   * fields, an edge between two nodes, x and y, means that x and y are
   * probabilistically dependent of one another. This being a symmetrical
   *relation,
   * there is no difference between edge (x,y) and edge (y,x). Thus, it can be
   * represented by an undirected edge and, in aGrUM, by an Edge.
   * @par Usage example:
   * @code
   * // creation of an edge between nodes whose IDs are 3 and 4
   * Edge edge1 (3,4);
   *   *
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
  /* ===========================================================================
   */

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
    Edge(const Edge& src);

    /// destructor
    ~Edge();

    ///@}

    // ############################################################################
    /// @name Accessors
    // ############################################################################
    /// @{

    /// returns an extremal node of an edge given the ID of the other one
    GUM_NODISCARD NodeId other(NodeId id) const;

    /// returns one extremal node ID (whichever one it is is unspecified)
    GUM_NODISCARD NodeId first() const;

    /// returns the node ID of the other extremal node ID
    GUM_NODISCARD NodeId second() const;

    ///@}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Edge& operator=(const Edge& src);

    /// checks whether two undirected edges are equal
    /** Two Edge are equal if they have the same extremal nodes, whatever their
     * order. For instance (3,4) == (4,3). */
    bool operator==(const Edge& src) const = default;

    /// @}

    private:
    /// the extremal nodes of the edge (their order is unimportant)
    NodeId n1;
    NodeId n2;
  };

  /* =========================================================================*/
  /* =========================================================================*/
  /* ==                        GENERIC DIRECTED EDGES                       ==*/
  /* =========================================================================*/
  /* =========================================================================*/
  /** @class Arc
   * @brief The base class for all directed edges
   * \ingroup graph_group
   *
   * This class is used as a basis for manipulating all directed edges (i.e.,
   *edges
   * in which the order of the nodes is meaningful). For instance, in an arrow,
   *one
   * node is near the head and the other one is farther, hence these nodes have
   * different status and swapping the nodes results in reversing the direction
   *of
   * the arrow. Thus, the nodes in an arrow can be thought of as asymmetric and
   *the
   * arrow's graphical representation contains a pointed extremity so as to
   *account
   * for this asymmetry. In aGrUM, the latter is taken into account by Arc.
   * @par Usage example:
   * @code
   * // creation of an arc (directed edge) from 3 to 4
   * Arc arc1 (3,4);
   *   *
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
  /* ===========================================================================
   */

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
    Arc(const Arc& src);

    /// destructor
    ~Arc();

    /// @}

    // ############################################################################
    /// @name Accessors
    // ############################################################################
    /// @{

    /// returns the tail of the arc
    GUM_NODISCARD NodeId tail() const;

    /// returns the head of the arc
    GUM_NODISCARD NodeId head() const;

    /// returns an extremal node of an edge given the ID of the other one
    GUM_NODISCARD NodeId other(NodeId id) const;

    /// returns one extremal node ID (whichever one it is is unspecified)
    GUM_NODISCARD NodeId first() const;

    /// returns the node ID of the other extremal node ID
    GUM_NODISCARD NodeId second() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Arc& operator=(const Arc& src);

    /// checks whether two arcs are equal
    /** Two arcs are considered equal if they have the same head and tail
     * (by same we mean they have the same ID). */
    bool operator==(const Arc& src) const = default;

    /// @}

    private:
    /// the extremal nodes of the edge (their order is unimportant)
    NodeId n1;
    NodeId n2;

    /// modifies the tail of the arc
    void _setTail_(NodeId id);

    /// modifies the head of the arc
    void _setHead_(NodeId id);

    /// reverses the direction of the arc
    void operator-();
  };

  ////////////////////////////////////////////////////////////////
  // we need to provide hash functions for some Edge and Arc

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <>
  class HashFunc< Edge >: public HashFuncBase< Edge > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Edge& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()(const Edge& key) const final;
  };


  template <>
  class HashFunc< Arc >: public HashFuncBase< Arc > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Arc& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()(const Arc& key) const final;
  };

#endif   // DOXYGEN_SHOULD_SKIP_THIS

  /** \ingroup graph_group
   * @{
   * Some typdefs and define for shortcuts ...
   */
  using EdgeSet = Set< Edge >;
  using NodeSet = Set< NodeId >;
  using ArcSet  = Set< Arc >;

  using ArcSetIterator  = ArcSet::const_iterator;
  using EdgeSetIterator = EdgeSet::const_iterator;
  using NodeSetIterator = NodeSet::const_iterator;

  inline const NodeSet emptyNodeSet;
  /** @} */

  /** \ingroup graph_group
   * @{
   * @brief Property on graph elements
   **/
  template < class VAL >
  using NodeProperty = HashTable< NodeId, VAL >;
  template < class VAL >
  using EdgeProperty = HashTable< Edge, VAL >;
  template < class VAL >
  using ArcProperty = HashTable< Arc, VAL >;

  /// @}

  /// to friendly display an edge
  std::ostream& operator<<(std::ostream& stream, const Edge& edge);

  /// to friendly display an arc
  std::ostream& operator<<(std::ostream& stream, const Arc& arc);

} /* namespace gum */

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::HashFunc< gum::NodeSet >;
#endif

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/graphElements_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // GUM_GRAPH_ELEMENTS_H
