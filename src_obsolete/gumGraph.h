/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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

/** @file
 * @brief Base classes for graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides four classes to manipulate graphs: gumMixedGraphTpl that
 * support graphs with both directed and undirected edges, gumUndiGraphTpl that
 * represent undirected graphs, gumDiGraphTpl that represent directed graphs and
 * gumDAGTpl that represent directed acyclic graphs.
 *
 * Copying these graphs through usual functions like operator= will result in
 * completely fresh copies to be generated, i.e., the new edges, arcs and nodes are
 * not shared in memory with those copied. However, for optimization issues, an
 * alternative copy procedure is available, the so-called "shallow copy". In this
 * mode, only pointers on edges/arcs/nodes are copied. As a result, this creates a
 * completely distinct new graph containing pointers toward the same
 * edges/arcs/nodes as the original graph. Thus, as both graphs are distinct, we
 * can remove or add elements to one graph without modifying the other graph. As
 * the graphs contain pointers, creating new graphs is fast as we do not need to
 * create new edges/arcs/nodes. This kind of copy should be used with caution and
 * probably only for temporary usage. Once a graph has been copied through the
 * shallowCopy member, the other shallow_ methods can be used. Using the latter
 * without having performed a shallow copy will usually produce erroneous results
 * (but they will not crash the application).
 *
 * @par Usage example:
 * @code
 * // creating empty graphs
 * gumMixedGraphTpl<gumNode, gumEdge, gumArc> mixed1, mixed2;
 *
 * // adding nodes, edges and arcs to mixed1
 * mixed1.insertNode (gumNode (1, "node1"));
 * gumNode node2 (2, "node 2");
 * mixed1.insertNode (node2);
 * mixed1.insertNode (gumNode (3, "node3"));
 * mixed1.insertEdge (1,2);
 * mixed1.insertArc (1,3);
 * mixed1.insertArc (2,3);
 *
 * // copying graphs
 * gumMixedGraphTpl<gumNode, gumEdge, gumArc> mixed3 = mixed1;
 * mixed2 = mixed1;
 *
 * // displaying the content of a graph
 * cerr << mixed1 << endl;
 *
 * // check if a graph has no node
 * if (mixed1.emptyNodes ()) cerr << "graph mixed1 is empty" << endl;
 *
 * // remove all the nodes (as well as their adjacent edges and arcs)
 * mixed1.clearNodes ();
 *
 * // remove all the edges from a graph
 * mixed2.clearEdges ();
 *
 * // remove some arc
 * mixed2.eraseArc (1,3);
 * mixed2.eraseArc (gumArc (2,3));
 *
 * // parse a graph
 * for (gumNodeList<gumNode>::iterator iter = mixed3.beginNodes ();
 *        iter != mixed3.endNodes (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = mixed3.beginArcs ();
 *        iter != mixed3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = mixed3.beginEdges ();
 *        iter != mixed3.endEdges (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = mixed3.beginParentArcs (3);
 *        iter != mixed3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 *
 * // remove all the arcs adjacent to a given node
 * mixed3.eraseneighbourArcs (2);
 * @endcode
 */


#ifndef GUM_GRAPH_H
#define GUM_GRAPH_H


#include <iostream>
#include <utility>

#include <agrum/gum_utils.h>

#include <agrum/gumId.h>
#include <agrum/gumNodeList.h>
#include <agrum/gumEdgeList.h>


template <typename Node, typename Edge, typename Arc> class gumMixedGraphTpl;

template <typename Node, typename Arc>  class gumDiGraphTpl;

template <typename Node, typename Arc>  class gumDAGTpl;

template <typename Node, typename Edge> class gumUndiGraphTpl;

/// for friendly displaying the content of mixed directed/undirected graphs
template <typename Node, typename Edge, typename Arc>
std::ostream& operator<< ( std::ostream&, const gumMixedGraphTpl<Node,Edge,Arc>& );

/// for friendly displaying the content of directed graphs
template <typename Node, typename Arc>
std::ostream& operator<< ( std::ostream&, const gumDiGraphTpl<Node,Arc>& );

/// for friendly displaying the content of directed acyclic graphs
template <typename Node, typename Arc>
std::ostream& operator<< ( std::ostream&, const gumDAGTpl<Node,Arc>& );

/// for friendly displaying the content of undirected graphs
template <typename Node, typename Edge>
std::ostream& operator<< ( std::ostream&, const gumUndiGraphTpl<Node,Edge>& );




/* ============================================================================== */
/* ===       BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS      === */
/* ============================================================================== */
/** @class gumMixedGraphTpl
 * @brief Base class for all graphs
 *
 * This is the base class for graphs containing both directed edges (so-called arcs)
 * and undirected edges (so-called edges).
 *
 * Copying gumMixedGraphTpl through usual functions like operator= will result in a
 * completely fresh copy to be generated, i.e., the new edges, arcs and nodes are
 * not shared in memory with those copied. However, for optimization issues, an
 * alternative copy procedure is available, the so-called "shallow copy". In this
 * mode, only pointers on edges/arcs/nodes are copied. As a result, this creates a
 * completely distinct new gumMixedGraphTpl containing pointers toward the same
 * edges/arcs/nodes as the original gumMixedGraphTpl. Thus, as both gumMixedGraphTpl are
 * distinct, we can remove or add elements to one gumMixedGraphTpl without modifying
 * the other gumMixedGraphTpl. As the gumMixedGraphTpl contain pointers, creating new
 * gumMixedGraphTpl is fast as we do not need to create new edges/arcs/nodes. This kind
 * of copy should be used with caution and probably only for temporary usage. Once a
 * gumMixedGraphTpl has been copied through the shallowCopy member, the other shallow_
 * methods can be used. Using the latter without having performed a shallow copy
 * will usually produce erroneous results (but they will not crash the application).
 *
 * @par Usage example:
 * @code
 * // creating empty graphs
 * gumMixedGraphTpl<gumNode, gumEdge, gumArc> mixed1, mixed2;
 *
 * // adding nodes, edges and arcs to mixed1
 * mixed1.insertNode (gumNode (1, "node1"));
 * gumNode node2 (2, "node 2");
 * mixed1.insertNode (node2);
 * mixed1.insertNode (gumNode (3, "node3"));
 * mixed1.insertEdge (1,2);
 * mixed1.insertArc (1,3);
 * mixed1.insertArc (2,3);
 *
 * // copying graphs
 * gumMixedGraphTpl<gumNode, gumEdge, gumArc> mixed3 = mixed1;
 * mixed2 = mixed1;
 *
 * // displaying the content of a graph
 * cerr << mixed1 << endl;
 *
 * // check if a graph has no node
 * if (mixed1.emptyNodes ()) cerr << "graph mixed1 is empty" << endl;
 *
 * // remove all the nodes (as well as their adjacent edges and arcs)
 * mixed1.clearNodes ();
 *
 * // remove all the edges from a graph
 * mixed2.clearEdges ();
 *
 * // remove some arc
 * mixed2.eraseArc (1,3);
 * mixed2.eraseArc (gumArc (2,3));
 *
 * // parse a graph
 * for (gumNodeList<gumNode>::iterator iter = mixed3.beginNodes ();
 *        iter != mixed3.endNodes (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = mixed3.beginArcs ();
 *        iter != mixed3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = mixed3.beginEdges ();
 *        iter != mixed3.endEdges (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = mixed3.beginParentArcs (3);
 *        iter != mixed3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 *
 * // remove all the arcs adjacent to a given node
 * mixed3.eraseneighbourArcs (2);
 * @endcode
 */
/* ============================================================================== */
template <typename Node, typename Edge, typename Arc>

class gumMixedGraphTpl {
  public:

    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty mixed directed-undirected graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    // ==============================================================================
    explicit gumMixedGraphTpl( gumSize nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                            bool nodes_resize_policy    = true,
                            gumSize edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                            bool edges_resize_policy    = true,
                            gumSize arcs_size  = GUM_HASHTABLE_DEFAULT_SIZE,
                            bool arcs_resize_policy     = true );

    // ==============================================================================
    /// copy constructor (by default, deep copy of nodes, arcs and edges lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Edge, Arc, and Node instances are created
     * and inserted into the newly created gumMixedGraphTpl. Thus, the latter and the
     * copied gumMixedGraphTpl (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Edge, Arc and Node pointers are copied. As a result, all the edges/arcs/nodes
     * in the new gumMixedGraphTpl are shared with those of \e from. Thus, modifying the
     * content of an edge/arc/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an edge,
     * arc or node from one graph does not remove it from the other one. */
    // ==============================================================================
    gumMixedGraphTpl( const gumMixedGraphTpl<Node,Edge,Arc>& from,
                   gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumMixedGraphTpl();

    /// @}



    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new graph shares its edges/arc/nodes with \e from)
    // ==============================================================================
    gumMixedGraphTpl<Node,Edge,Arc>& shallowCopy
    ( const gumMixedGraphTpl<Node,Edge,Arc>& from );

    // ==============================================================================
    /// checks whether two lists of edges are similar
    // ==============================================================================
    const bool shallowEqual( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const ;

    // ==============================================================================
    /// checks whether two lists of edges contain different elements
    // ==============================================================================
    const bool shallowDifferent( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const ;

    // ==============================================================================
    /// adds a shallow copy of a node into the graph
    /** @throw gumDuplicateElement exception is thrown if there already exists a
     * node with the same id in the graph. */
    // ==============================================================================
    virtual const gumRefPtr<Node>& shallowInsertNode( const gumRefPtr<Node>& node );

    // ==============================================================================
    /// returns a pointer to a given node (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the node cannot be found */
    // ==============================================================================
    const gumRefPtr<Node>& shallowGetNode( const gumId& id )
    const ;

    // ==============================================================================
    /// returns a pointer to a given node (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the node cannot be found */
    // ==============================================================================
    const gumRefPtr<Node>& shallowGetNode( const Node& node )
    const ;

    // ==============================================================================
    /// adds a shallow copy of an edge into the edge list
    /** @throw gumDuplicateElement exception is thrown if there already exists a
     * similar edge in the list. */
    // ==============================================================================
    virtual const gumRefPtr<Arc>& shallowinsertArc( const gumRefPtr<Arc>& arc );

    // ==============================================================================
    /// returns a pointer to a given arc (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the arc cannot be found */
    // ==============================================================================
    const gumRefPtr<Arc>& shallowGetArc( const gumId& node1, const gumId& node2 )
    const ;

    // ==============================================================================
    /// returns a pointer to a given arc (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the arc cannot be found */
    // ==============================================================================
    const gumRefPtr<Arc>& shallowGetArc( const Arc& arc )
    const ;

    // ==============================================================================
    /// adds a shallow copy of an edge into the edge list
    // ==============================================================================
    virtual const gumRefPtr<Edge>& shallowInsertEdge( const gumRefPtr<Edge>& edge );

    // ==============================================================================
    /// returns a smart pointer on the edge the extremities of which are node1,node2
    /** @throw gumNotFound exception is thrown if the edge cannot be found */
    // ==============================================================================
    const gumRefPtr<Edge>& shallowGetEdge( const gumId& node1, const gumId& node2 )
    const ;

    // ==============================================================================
    /// returns a pointer to a given edge (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the edge cannot be found */
    // ==============================================================================
    const gumRefPtr<Edge>& shallowGetEdge( const Edge& edge )
    const ;

    /// @}



    // ##############################################################################
    /// @name Fine tuning
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns the size of the nodes hashtable
    /** The method runs in constant time. */
    // ==============================================================================
    const gumSize& capacityNodes() const ;

    // ==============================================================================
    /// enables the user to change dynamically the nodes list resizing policy
    // ==============================================================================
    void setResizePolicyNodes( const bool new_policy ) ;

    // ==============================================================================
    /// returns the current list of nodes resizing policy
    // ==============================================================================
    const bool getResizePolicyNodes() const ;

    // ==============================================================================
    /// changes the size of the nodes hashtable
    // ==============================================================================
    void resizeNodes( gumSize new_size ) ;

    // ==============================================================================
    /// returns the size of the edges hashtable
    /** The method runs in constant time. */
    // ==============================================================================
    const gumSize& capacityEdges() const ;

    // ==============================================================================
    /// enables the user to change dynamically the edges list resizing policy.
    // ==============================================================================
    void setResizePolicyEdges( const bool new_policy ) ;

    // ==============================================================================
    /// returns the current list of edges resizing policy.
    // ==============================================================================
    const bool getResizePolicyEdges() const ;

    // ==============================================================================
    /// changes the size of the edges hashtable
    // ==============================================================================
    void resizeEdges( gumSize new_size ) ;

    // ==============================================================================
    /// returns the size of the arcs hashtable
    /** The method runs in constant time. */
    // ==============================================================================
    const gumSize& capacityArcs() const ;

    // ==============================================================================
    /// enables the user to change dynamically the arcs list resizing policy.
    // ==============================================================================
    void setResizePolicyArcs( const bool new_policy ) ;

    // ==============================================================================
    /// returns the current list of arcs resizing policy.
    // ==============================================================================
    const bool getResizePolicyArcs() const ;

    // ==============================================================================
    /// changes the size of the hashtables within the arcs list
    // ==============================================================================
    void resizeArcs( gumSize new_size ) ;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for nodes
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// removes all the nodes from the graph (as well as their adjacent edges/arcs)
    // ==============================================================================
    virtual void clearNodes();

    // ==============================================================================
    /// returns the list of nodes of the graph
    // ==============================================================================
    const gumNodeList<Node>& getNodes() const ;

    // ==============================================================================
    /// indicates whether the graph contains some nodes
    // ==============================================================================
    const bool emptyNodes() const ;

    // ==============================================================================
    /// checks whether a given node belongs to the graph
    // ==============================================================================
    const bool existsNode( const gumId& id ) const ;

    // ==============================================================================
    /// checks whether a given node belongs to the graph
    // ==============================================================================
    const bool existsNode( const Node& node ) const ;

    // ==============================================================================
    /// returns the number of nodes in the graph
    // ==============================================================================
    const gumSize& getNbrNodes() const ;

    // ==============================================================================
    /// adds a new node in the graph (actually only a copy is added to the graph)
    /** Note that node is not actually inserted into the graph: it is rather a copy
     * that is inserted.
     * @return as a copy of the node passed in argument is inserted into the
     * graph rather than the node itself, the function returns a reference to this
     * copy
     * @throw gumDuplicateElement exception is thrown if a node with the same ID
     * already exists. */
    // ==============================================================================
    virtual Node& insertNode( const Node& node );

    // ==============================================================================
    /// removes a node specified by its ID from the list of nodes
    /** If no such element can be found, nothing is done (in particular, it does
     * not throw any exception). */
    // ==============================================================================
    virtual void eraseNode( const gumId& id );

    // ==============================================================================
    /// removes a given node from the graph
    /** If no such element can be found, nothing is done (in particular, it does
     * not throw any exception). */
    // ==============================================================================
    virtual void eraseNode( const Node& node );

    // ==============================================================================
    /// returns the node the ID of which is passed in argument
    /** throws gumNotFound exception is thrown if the graph contains no
     * node with this ID. */
    // ==============================================================================
    Node& getNode( const gumId& id ) ;
    const Node& getNode( const gumId& id ) const ;

    // ==============================================================================
    /// creates a gumNodeList of Assent from the list of nodes of the graph
    /** @param f a function that maps a node into a Assent node.
     * @param size the size of the hashtable resulting nodelist. When equal to 0,
     * a default size is computed that is a good trade-off between space consumption
     * and efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting nodelist may
     * not be similar to that of the list of nodes of the graph. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumNodeList<Assent>
    nodeListMapNodes( Assent( *f )( Node ), gumSize size = 0 ) const;
    template <typename Assent> gumNodeList<Assent>
    nodeListMapNodes( Assent( *f )( Node& ), gumSize size = 0 ) const;
    template <typename Assent> gumNodeList<Assent>
    nodeListMapNodes( Assent( *f )( const Node& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Assent from the list of nodes of the graph
    /** @param f a function that maps a node into a Assent
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting hashtable may
     * not be similar to that of the list of nodes of the graph. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMapNodes( Assent( *f )( Node ), gumSize size = 0 ) const;
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMapNodes( Assent( *f )( Node& ), gumSize size = 0 ) const;
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMapNodes( Assent( *f )( const Node& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Assent from the list of nodes of the graph
    /** @param val the value taken by all the elements of the resulting hashtable
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting hashtable may
     * not be similar to that of the list of nodes of the graph. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMapNodes( const Assent& val, gumSize size = 0 ) const;

    // ==============================================================================
    /// a method to create a gumList of Assent from the list of nodes of the graph
    /** @param f a function that maps a node into a Assent
     * @warning the order of the Assent elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Assent> gumList<Assent>
    listMapNodes( Assent( *f )( Node ) ) const;
    template <typename Assent> gumList<Assent>
    listMapNodes( Assent( *f )( Node& ) ) const;
    template <typename Assent> gumList<Assent>
    listMapNodes( Assent( *f )( const Node& ) ) const;

    // ==============================================================================
    /// a method to create a gumList of Assent from the list of nodes of the graph
    /** @param val the value taken by all the elements of the resulting list
     * @warning the order of the Assent elements in the resulting list is
     *  arbitrary */
    // ==============================================================================
    template <typename Assent>
    gumList<Assent> listMapNodes( const Assent& val ) const;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for edges
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// removes all the edges from the graph
    // ==============================================================================
    virtual void clearEdges();

    // ==============================================================================
    /// returns the list of edges of the graph
    // ==============================================================================
    const gumEdgeList<Edge>& getEdges() const ;

    // ==============================================================================
    /// indicates whether the graph contains some edges
    // ==============================================================================
    const bool emptyEdges() const ;

    // ==============================================================================
    /// checks whether a given edge exists
    // ==============================================================================
    const bool existsEdge( const gumId& node1, const gumId& node2 ) const ;

    // ==============================================================================
    /// checks whether a given edge exists
    // ==============================================================================
    const bool existsEdge( const Edge& edge ) const ;

    // ==============================================================================
    /// returns the number of edges in the graph
    // ==============================================================================
    const gumSize& getNbrEdges() const ;

    // ==============================================================================
    /// removes a given edge from the graph
    /** If the edge cannot be found, it does nothing (in particular, it throws no
     * exception). */
    // ==============================================================================
    virtual void eraseEdge( const gumId& id1, const gumId& id2 );

    // ==============================================================================
    /// removes a given edge from the graph
    /** If the edge cannot be found, it does nothing (in particular, it throws no
     * exception). */
    // ==============================================================================
    virtual void eraseEdge( const Edge &edge );

    // ==============================================================================
    /// returns a given edge
    /** @throw gumNotFound exception is thrown if the edge cannot be found. */
    // ==============================================================================
    const Edge& getEdge( const gumId& node1, const gumId& node2 )
    const ;

    // ==============================================================================
    /// returns a given edge
    /** @throw gumNotFound exception is thrown if the edge cannot be found. */
    // ==============================================================================
    Edge& getEdge( const gumId& node1, const gumId& node2 )
    ;

    // ==============================================================================
    /// adds an undirected edge to the graph (only a copy is added to the graph)
    /** @throw gumInvalidEdge exception is thrown if at least one extremal node
     * of the edge does not belong to the graph.
     * @throw gumDuplicateElement exception is thrown if an arc with the same
     * extremities already exists. */
    // ==============================================================================
    virtual Edge& insertEdge( const Edge& edge );

    // ==============================================================================
    /// returns the list of the edges adjacent to a given node
    /** If the node has no neighbours, the function returns an empty list. */
    // ==============================================================================
    const gumList<gumRefPtr<Edge> >& getneighbourEdges( const gumId& node )
    const ;

    // ==============================================================================
    /// removes all the edges adjacent to the given node
    // ==============================================================================
    virtual void eraseneighbourEdges( const gumId& node );

    // ==============================================================================
    /// creates a hashtable of Border from the list of edges of the graph
    /** @param f a function that maps an edge into a Border
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the edges in the resulting hashtable may
     * not be similar to that of list of edges of the graph. Hence iterators on the
     * former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
    hashMapEdges( Border( *f )( Edge ), gumSize size = 0 ) const;
    template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
    hashMapEdges( Border( *f )( Edge& ), gumSize size = 0 ) const;
    template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
    hashMapEdges( Border( *f )( const Edge& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Border from the list of edges of the graph
    /** @param val the value taken by all the elements of the resulting list
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the edges in the resulting hashtable may
     * not be similar to that of the list of edges of the graph. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
    hashMapEdges( const Border& val, gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a list of Border from an edge list
    /** @param f a function that maps an edge into a Border
     * @warning the order of the Border elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Border> gumList<Border>
    listMapEdges( Border( *f )( Edge ) ) const;
    template <typename Border> gumList<Border>
    listMapEdges( Border( *f )( Edge& ) ) const;
    template <typename Border> gumList<Border>
    listMapEdges( Border( *f )( const Edge& ) ) const;

    // ==============================================================================
    /// creates a list of Border from an edge list
    /** @param val the value taken by all the elements of the resulting list
     * @warning the order of the Border elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Border> gumList<Border>
    listMapEdges( const Border& val ) const;

    // ==============================================================================
    /// creates a gumEdgeList of Border from the list of edges of the graph
    /** @param f a function that maps an edge into a Border.
     * @param size the size of the hashtable resulting edgelist. When equal to 0,
     * a default size is computed that is a good trade-off between space consumption
     * and efficiency of new elements insertions
     * @warning by default, the order of the edges in the resulting edgelist may
     * not be similar to that of the edge list passed in argument. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Border> gumEdgeList<Border>
    edgeListMapEdges( Border( *f )( Edge ), gumSize size = 0 ) const;
    template <typename Border> gumEdgeList<Border>
    edgeListMapEdges( Border( *f )( Edge& ), gumSize size = 0 ) const;
    template <typename Border> gumEdgeList<Border>
    edgeListMapEdges( Border( *f )( const Edge& ), gumSize size = 0 ) const;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for arcs
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// removes all the arcs from the graph
    // ==============================================================================
    virtual void clearArcs();

    // ==============================================================================
    /// returns the list of arcs of the graph
    // ==============================================================================
    const gumArcList<Arc>& getArcs() const ;

    // ==============================================================================
    /// indicates whether the graph contains some arcs
    // ==============================================================================
    const bool emptyArcs() const ;

    // ==============================================================================
    /// checks whether a given edge exists
    // ==============================================================================
    const bool existsArc( const gumId& node1, const gumId& node2 ) const ;

    // ==============================================================================
    /// checks whether a given edge exists
    // ==============================================================================
    const bool existsArc( const Arc& arc ) const ;

    // ==============================================================================
    /// returns the number of arcs in the graph
    // ==============================================================================
    const gumSize& getNbrArcs() const ;

    // ==============================================================================
    /// returns the arc in the graph specified by its tail and head
    /** @throw gumNotFound exception is thrown if the arc cannot be found. */
    // ==============================================================================
    Arc& getArc( const gumId& node1, const gumId& node2 ) ;
    const Arc& getArc( const gumId& node1, const gumId& node2 )
    const ;

    // ==============================================================================
    /// adds a copy of \e arc to the list of arcs of the graph.
    /** @return As this is a copy that is inserted into the list, the method
     * returns a reference to this new copy
     * @throw gumDuplicateElement exception is thrown if the list already contains
     * an arc with the same head and tail. */
    // ==============================================================================
    virtual Arc& insertArc( const Arc& arc );

    // ==============================================================================
    /// removes an arc specified by its tail and head from the graph
    /** The iterators that pointed on the removed arc are resetted to rend.
     * If the arc cannot be found, the function does nothing (in particular, it
     * does not throw any exception). */
    // ==============================================================================
    virtual void eraseArc( const gumId& node1, const gumId& node2 );

    // ==============================================================================
    /// removes a given arc from the graph
    /** The iterators that pointed on the removed arc are resetted to rend.
     * If the arc cannot be found, the function does nothing (in particular, it
     * does not throw any exception). */
    // ==============================================================================
    virtual void eraseArc( const Arc& edge );

    // ==============================================================================
    /// removes all the arcs adjacent to the given node
    // ==============================================================================
    virtual void eraseneighbourArcs( const gumId& node );

    // ==============================================================================
    /// removes all the arcs outgoing from the given node
    // ==============================================================================
    virtual void eraseChildrenArcs( const gumId& node );

    // ==============================================================================
    /// removes all the arcs ingoing into the given node
    // ==============================================================================
    virtual void eraseParentArcs( const gumId& node );

    // ==============================================================================
    /// returns the list of the arcs out-going from a given node
    /** If the node has no children, the function returns an empty list.
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumList<gumRefPtr<Arc> >& getChildrenArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns the list of the arcs in-going to a given node
    /** If the node has no parent, the function returns an empty list. */
    // ==============================================================================
    const gumList<gumRefPtr<Arc> >& getParentArcs( const gumId& node )
    const ;

    // ==============================================================================
    /// creates a hashtable of Curve from the list of arcs of the graph
    /** @param f a function that maps an arc into a Curve
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the arcs in the resulting hashtable may
     * not be similar to that of list of arcs of the graph. Hence iterators on the
     * former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Curve> gumHashTable<std::pair<gumId,gumId>,Curve>
    hashMapArcs( Curve( *f )( Arc ), gumSize size = 0 ) const;
    template <typename Curve> gumHashTable<std::pair<gumId,gumId>,Curve>
    hashMapArcs( Curve( *f )( Arc& ), gumSize size = 0 ) const;
    template <typename Curve> gumHashTable<std::pair<gumId,gumId>,Curve>
    hashMapArcs( Curve( *f )( const Arc& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Curve from the list of arcs of the graph
    /** @param val the value taken by all the elements of the resulting list
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the arcs in the resulting hashtable may
     * not be similar to that of the list of arcs of the graph. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Curve> gumHashTable<std::pair<gumId,gumId>,Curve>
    hashMapArcs( const Curve& val, gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a list of Curve from an arc list
    /** @param f a function that maps an arc into a Curve
     * @warning the order of the Curve elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Curve> gumList<Curve>
    listMapArcs( Curve( *f )( Arc ) ) const;
    template <typename Curve> gumList<Curve>
    listMapArcs( Curve( *f )( Arc& ) ) const;
    template <typename Curve> gumList<Curve>
    listMapArcs( Curve( *f )( const Arc& ) ) const;

    // ==============================================================================
    /// creates a list of Curve from an arc list
    /** @param val the value taken by all the elements of the resulting list
     * @warning the order of the Curve elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Curve> gumList<Curve>
    listMapArcs( const Curve& val ) const;

    // ==============================================================================
    /// creates a gumArcList of Curve from the list of arcs of the graph
    /** @param f a function that maps an arc into a Curve.
     * @param size the size of the hashtable resulting arclist. When equal to 0,
     * a default size is computed that is a good trade-off between space consumption
     * and efficiency of new elements insertions
     * @warning by default, the order of the arcs in the resulting arclist may
     * not be similar to that of the arcs list passed in argument. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Curve> gumArcList<Curve>
    arcListMapArcs( Curve( *f )( Arc ), gumSize size = 0 ) const;
    template <typename Curve> gumArcList<Curve>
    arcListMapArcs( Curve( *f )( Arc& ), gumSize size = 0 ) const;
    template <typename Curve> gumArcList<Curve>
    arcListMapArcs( Curve( *f )( const Arc& ), gumSize size = 0 ) const;

    /// @}




    // ##############################################################################
    /// @name Iterators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns an iterator pointing to the end of the list of nodes
    // ==============================================================================
    const gumNodeListIterator<Node>& endNodes() const ;

    // ==============================================================================
    /// returns the iterator pointing to the rend (beginning) of the list of nodes
    // ==============================================================================
    const gumNodeListIterator<Node>& rendNodes() const ;

    // ==============================================================================
    /// returns the iterator pointing to the beginning of the list of nodes
    // ==============================================================================
    gumNodeListIterator<Node> beginNodes() const;

    // ==============================================================================
    /// returns the iterator pointing to the the last element of the list of nodes
    // ==============================================================================
    gumNodeListIterator<Node> rbeginNodes() const;

    // ==============================================================================
    /// returns an iterator pointing to the end of the list of edges
    // ==============================================================================
    const gumEdgeListIterator<Edge>& endEdges() const ;

    // ==============================================================================
    /// returns the iterator pointing to the rend (beginning) of the list of edges
    // ==============================================================================
    const gumEdgeListIterator<Edge>& rendEdges() const ;

    // ==============================================================================
    /// returns the iterator pointing to the beginning of the list of edges
    // ==============================================================================
    gumEdgeListIterator<Edge> beginEdges() const;

    // ==============================================================================
    /// returns the iterator pointing to the the last element of the list of edges
    // ==============================================================================
    gumEdgeListIterator<Edge> rbeginEdges() const;

    // ==============================================================================
    /// returns an iterator pointing to the first neighbour of a given node
    // ==============================================================================
    gumEdgeListIterator<Edge> beginneighbourEdges( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the last neighbour of a given node */
    // ==============================================================================
    gumEdgeListIterator<Edge> rbeginneighbourEdges( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the end of the list of arcs
    // ==============================================================================
    const gumArcListIterator<Arc>& endArcs() const ;

    // ==============================================================================
    /// returns the iterator pointing to the rend (beginning) of the list of arcs
    // ==============================================================================
    const gumArcListIterator<Arc>& rendArcs() const ;

    // ==============================================================================
    /// returns the iterator pointing to the beginning of the list of arcs
    // ==============================================================================
    gumArcListIterator<Arc> beginArcs() const;

    // ==============================================================================
    /// returns the iterator pointing to the the last element of the list of arcs
    // ==============================================================================
    gumArcListIterator<Arc> rbeginArcs() const;

    // ==============================================================================
    /// returns an iterator pointing to the first parent of a given node
    // ==============================================================================
    gumArcListIterator<Arc> beginParentArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the last parent of a given node
    // ==============================================================================
    gumArcListIterator<Arc> rbeginParentArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the first child of a given node
    // ==============================================================================
    gumArcListIterator<Arc> beginChildrenArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the last parent of a given node
    // ==============================================================================
    gumArcListIterator<Arc> rbeginChildrenArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the first adjacent arc of a node
    /** Actually, this corresponds to the first parent. */
    // ==============================================================================
    gumArcListIterator<Arc> beginneighbourArcs( const gumId& node ) const;

    // ==============================================================================
    /// returns an iterator pointing to the last adjacent arc of a node
    /** Actually, this corresponds to the last of its children */
    // ==============================================================================
    gumArcListIterator<Arc> rbeginneighbourArcs( const gumId& node ) const;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Edge/Arc/Node instances are
     * created and inserted into the newly created gumMixedGraphTpl. Thus, the latter
     * and the copied gumMixedGraphTpl (from) are completely distinct in terms of memory
     * locations. */
    // ==============================================================================
    gumMixedGraphTpl<Node,Edge,Arc>& operator=( const gumMixedGraphTpl<Node,Edge,Arc>& from );

    // ==============================================================================
    /// checks whether two graphs are identical
    // ==============================================================================
    const bool operator== ( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const;

    // ==============================================================================
    /// checks whether two graphs are different
    // ==============================================================================
    const bool operator!= ( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const;


    /// @}



    // ##############################################################################
    /// @name graph specific members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// checks whether the graph contains cycles
    // ==============================================================================
    const bool hasCycle() const ;
    /// @}



  protected:
    /// the list of nodes of the graph
    gumNodeList<Node> nodes;

    /// the list of undirected edges of the graph
    gumEdgeList<Edge> edges;

    /// the list of directed edges of the graph
    gumArcList<Arc> arcs;

    // ==============================================================================
    /// function used by function hasCycle ()
    // ==============================================================================
    const bool hasCycleMark( gumHashTable<gumId,bool>& marks,
                             const gumId& node, const gumId& from ) const ;

};






/* ============================================================================== */
/* ===               BASE CLASS FOR MANIPULATING DIRECTED GRAPHS              === */
/* ============================================================================== */
/** @class gumDiGraphTpl
 * @brief Base class for all directed graphs
 *
 * This is the base class for graphs containing only directed edges (the so-called
 * arcs). By directed edges, we mean in fact asymmetric edges, that is, edges in
 * which there is a difference between edge (x,y) and edge (y,x).
 *
 * Copying gumDiGraphTpl through usual functions like operator= will result in a
 * completely fresh copy to be generated, i.e., the new arcs and nodes are
 * not shared in memory with those copied. However, for optimization issues, an
 * alternative copy procedure is available, the so-called "shallow copy". In this
 * mode, only pointers on arcs/nodes are copied. As a result, this creates a
 * completely distinct new gumDiGraphTpl containing pointers toward the same
 * arcs/nodes as the original gumDiGraphTpl. Thus, as both gumDiGraphTpl are
 * distinct, we can remove or add elements to one gumDiGraphTpl without modifying
 * the other gumDiGraphTpl. As the gumDiGraphTpl contain pointers, creating new
 * gumDiGraphTpl is fast as we do not need to create new arcs/nodes. This kind
 * of copy should be used with caution and probably only for temporary usage. Once a
 * gumDiGraphTpl has been copied through the shallowCopy member, the other shallow_
 * methods can be used. Using the latter without having performed a shallow copy
 * will usually produce erroneous results (but they will not crash the application).
 *
 * @par Usage example:
 * @code
 * // creating empty graphs
 * gumDiGraphTpl<gumNode, gumArc> digraph1, digraph2;
 *
 * // adding nodes and arcs to digraph1
 * digraph1.insertNode (gumNode (1, "node1"));
 * gumNode node2 (2, "node 2");
 * digraph1.insertNode (node2);
 * digraph1.insertNode (gumNode (3, "node3"));
 * digraph1.insertArc (gumArc (1,2));
 * digraph1.insertArc (1,3);
 * digraph1.insertArc (2,3);
 *
 * // copying graphs
 * gumDiGraphTpl<gumNode, gumArc> digraph3 = digraph1;
 * digraph2 = digraph1;
 *
 * // displaying the content of a graph
 * cerr << digraph1 << endl;
 *
 * // check if a graph has no node
 * if (digraph1.emptyNodes ()) cerr << "graph digraph1 is empty" << endl;
 *
 * // remove all the arcs from a graph
 * digraph2.clearArcs ();
 *
 * // remove all the nodes (as well as their adjacent arcs)
 * digraph1.clearNodes ();
 *
 * // remove some arcs
 * digraph2.eraseArc (1,3);
 * digraph2.eraseArc (gumArc (2,3));
 *
 * // parse a graph
 * for (gumNodeList<gumNode>::iterator iter = digraph3.beginNodes ();
 *        iter != digraph3.endNodes (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = digraph3.beginArcs ();
 *        iter != digraph3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = digraph3.beginParentArcs (3);
 *        iter != digraph3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 *
 * // remove all the arcs adjacent to a given node
 * digraph3.eraseneighbourArcs (2);
 * @endcode
 */
/* ============================================================================== */
template <typename Node, typename Arc>

class gumDiGraphTpl : private gumMixedGraphTpl<Node,gumEdge,Arc> {
  public:

    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty directed graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    // ==============================================================================
    explicit gumDiGraphTpl( gumSize nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                         bool nodes_resize_policy    = true,
                         gumSize arcs_size  = GUM_HASHTABLE_DEFAULT_SIZE,
                         bool arcs_resize_policy     = true );

    // ==============================================================================
    /// copy constructor (by default, deep copy of nodes and arcs lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Arc and Node instances are created
     * and inserted into the newly created gumDiGraphTpl. Thus, the latter and the
     * copied gumDiGraphTpl (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Arc and Node pointers are copied. As a result, all the arcs/nodes
     * in the new gumDiGraphTpl are shared with those of \e from. Thus, modifying the
     * content of an arc/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an
     * arc or node from one graph does not remove it from the other one. */
    // ==============================================================================
    gumDiGraphTpl( const gumDiGraphTpl<Node,Arc>& from,
                gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumDiGraphTpl();

    /// @}



    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new graph shares its edges/arc/nodes with \e from)
    // ==============================================================================
    gumDiGraphTpl<Node,Arc>& shallowCopy( const gumDiGraphTpl<Node,Arc>& from );

    // ==============================================================================
    /// checks whether two lists of edges are similar
    // ==============================================================================
    const bool shallowEqual( const gumDiGraphTpl<Node,Arc>& from ) const;

    // ==============================================================================
    /// checks whether two lists of edges contain different elements
    // ==============================================================================
    const bool shallowDifferent( const gumDiGraphTpl<Node,Arc>& from ) const;

    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowInsertNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowGetNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowinsertArc;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowGetArc;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for nodes
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,gumEdge,Arc>::capacityNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::clearNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::emptyNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::existsNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::setResizePolicyNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getResizePolicyNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::resizeNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getNbrNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::insertNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::eraseNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::nodeListMapNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::hashMapNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::listMapNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getNodes;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for arcs
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,gumEdge,Arc>::capacityArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::resizeArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::clearArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::emptyArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::existsArc;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::setResizePolicyArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getResizePolicyArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getNbrArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getArc;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::insertArc;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::eraseArc;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::eraseneighbourArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::eraseChildrenArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::eraseParentArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getChildrenArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getParentArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::hashMapArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::listMapArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::arcListMapArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::getArcs;

    /// @}



    // ##############################################################################
    /// @name Iterators
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,gumEdge,Arc>::endNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rendNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::beginNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rbeginNodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::endArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rendArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::beginArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rbeginArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::beginParentArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rbeginParentArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::beginChildrenArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rbeginChildrenArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::beginneighbourArcs;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::rbeginneighbourArcs;

    /// @}



    // ##############################################################################
    /// @name graph specific members
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,gumEdge,Arc>::hasCycle;

    // ==============================================================================
    /// checks whether the graph contains directed cycles
    // ==============================================================================
    const bool hasCircuit() const ;

    // ==============================================================================
    /// checks whether the graph directed cycles passing through arc (x,y)
    /** @throw gumIdError exception is throw if at least one of the extremities of
     * the arc does not belong to the graph. */
    // ==============================================================================
    const bool hasCircuit( const gumId& x, const gumId& y ) const ;

    // ==============================================================================
    /// returns the mixed edges/arcs graph corresponding to the current graph
    // ==============================================================================
    const gumMixedGraphTpl<Node,gumEdge,Arc>& getMixedGraph() const ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Arc/Node instances are
     * created and inserted into the newly created gumDiGraphTpl. Thus, the latter
     * and the copied gumDiGraphTpl (from) are completely distinct in terms of memory
     * locations. */
    // ==============================================================================
    gumDiGraphTpl<Node,Arc>& operator=( const gumDiGraphTpl<Node,Arc>& from );

    // ==============================================================================
    /// checks whether two graphs are identical
    // ==============================================================================
    const bool operator== ( const gumDiGraphTpl<Node,Arc>& from ) const;

    // ==============================================================================
    /// checks whether two graphs are different
    // ==============================================================================
    const bool operator!= ( const gumDiGraphTpl<Node,Arc>& from ) const;
    /// @}



  protected:
    using gumMixedGraphTpl<Node,gumEdge,Arc>::nodes;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::arcs;



  private:
    // ==============================================================================
    /// function used by function hasCircuit
    // ==============================================================================
    const bool hasCircuitMark( gumHashTable<gumId,char>& marks, const gumId& node,
                               const gumId& from ) const ;

};






/* ============================================================================== */
/* ===              BASE CLASS FOR MANIPULATING UNDIRECTED GRAPHS             === */
/* ============================================================================== */
/** @class gumUndiGraphTpl
 * @brief Base class for all undirected graphs
 *
 * This is the base class for graphs containing only undirected edges. By
 * undirected edges, we mean in fact symmetric edges, that is, edges in which
 * there is no semantic difference between edge (x,y) and edge (y,x).
 *
 * Copying gumUndiGraphTpl through usual functions like operator= will result in a
 * completely fresh copy to be generated, i.e., the new edges and nodes are
 * not shared in memory with those copied. However, for optimization issues, an
 * alternative copy procedure is available, the so-called "shallow copy". In this
 * mode, only pointers on edges/nodes are copied. As a result, this creates a
 * completely distinct new gumUndiGraphTpl containing pointers toward the same
 * edges/nodes as the original gumUndiGraphTpl. Thus, as both gumUndiGraphTpl are
 * distinct, we can remove or add elements to one gumUndiGraphTpl without modifying
 * the other gumUndiGraphTpl. As the gumUndiGraphTpl contain pointers, creating new
 * gumUndiGraphTpl is fast as we do not need to create new edges/nodes. This kind
 * of copy should be used with caution and probably only for temporary usage. Once a
 * gumUndiGraphTpl has been copied through the shallowCopy member, the other shallow_
 * methods can be used. Using the latter without having performed a shallow copy
 * will usually produce erroneous results (but they will not crash the application).
 *
 * @par Usage example:
 * @code
 * // creating empty graphs
 * gumUndiGraphTpl<gumNode, gumEdge> undigraph1, undigraph2;
 *
 * // adding nodes, edges and arcs to undigraph1
 * undigraph1.insertNode (gumNode (1, "node1"));
 * gumNode node2 (2, "node 2");
 * undigraph1.insertNode (node2);
 * undigraph1.insertNode (gumNode (3, "node3"));
 * undigraph1.insertEdge (1,2);
 * undigraph1.insertEdge (gumEdge (1,3));
 * undigraph1.insertEdge (2,3);
 *
 * // copying graphs
 * gumUndiGraphTpl<gumNode, gumEdge> undigraph3 = undigraph1;
 * undigraph2 = undigraph1;
 *
 * // displaying the content of a graph
 * cerr << undigraph1 << endl;
 *
 * // check if a graph has no node
 * if (undigraph1.emptyNodes ()) cerr << "graph undigraph1 is empty" << endl;
 *
 * // remove all the nodes (as well as their adjacent edges)
 * undigraph1.clearNodes ();
 *
 * // remove some arc
 * undigraph2.eraseEdge (1,3);
 * undigraph2.eraseEdge (gumEdge (2,3));
 *
 * // remove all the edges from a graph
 * undigraph2.clearEdges ();
 *
 * // parse a graph
 * for (gumNodeList<gumNode>::iterator iter = undigraph3.beginNodes ();
 *        iter != undigraph3.endNodes (); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = undigraph3.beginEdges ();
 *        iter != undigraph3.endEdges (); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = undigraph3.beginneighbourEdges (3);
 *        iter != undigraph3.endEdges (); ++iter)
 *   cerr << *iter << endl;
 *
 * // remove all the edges adjacent to a given node
 * undigraph3.eraseneighbourEdges (2);
 * @endcode
 */
/* ============================================================================== */
template <typename Node, typename Edge>

class gumUndiGraphTpl : private gumMixedGraphTpl<Node,Edge,gumArc> {
  public:

    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty undirected graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    // ==============================================================================
    explicit gumUndiGraphTpl( gumSize nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                           bool nodes_resize_policy    = true,
                           gumSize edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                           bool edges_resize_policy    = true );

    // ==============================================================================
    /// copy constructor (by default, deep copy of nodes and edges lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Edge and Node instances are created
     * and inserted into the newly created gumUndiGraphTpl. Thus, the latter and the
     * copied gumUndiGraphTpl (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Edge and Node pointers are copied. As a result, all the edges/nodes
     * in the new gumUndiGraphTpl are shared with those of \e from. Thus, modifying the
     * content of an edge/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an edge
     * or node from one graph does not remove it from the other one. */
    // ==============================================================================
    gumUndiGraphTpl( const gumUndiGraphTpl<Node,Edge>& from,
                  gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumUndiGraphTpl();

    /// @}



    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new graph shares its edges/nodes with \e from)
    // ==============================================================================
    gumUndiGraphTpl<Node,Edge>& shallowCopy( const gumUndiGraphTpl<Node,Edge>& from );

    // ==============================================================================
    /// checks whether two lists of edges are similar
    // ==============================================================================
    const bool shallowEqual( const gumUndiGraphTpl<Node,Edge>& from ) const ;

    // ==============================================================================
    /// checks whether two lists of edges contain different elements
    // ==============================================================================
    const bool shallowDifferent( const gumUndiGraphTpl<Node,Edge>& from ) const ;

    using gumMixedGraphTpl<Node,Edge,gumArc>::shallowInsertNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::shallowGetNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::shallowInsertEdge;
    using gumMixedGraphTpl<Node,Edge,gumArc>::shallowGetEdge;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for nodes
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,Edge,gumArc>::capacityNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::clearNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::emptyNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::existsNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::setResizePolicyNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getResizePolicyNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::resizeNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getNbrNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::insertNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::eraseNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getNode;
    using gumMixedGraphTpl<Node,Edge,gumArc>::nodeListMapNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::hashMapNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::listMapNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getNodes;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for edges
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,Edge,gumArc>::capacityEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::resizeEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::clearEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::emptyEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::existsEdge;
    using gumMixedGraphTpl<Node,Edge,gumArc>::setResizePolicyEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getResizePolicyEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getNbrEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getEdge;
    using gumMixedGraphTpl<Node,Edge,gumArc>::insertEdge;
    using gumMixedGraphTpl<Node,Edge,gumArc>::eraseEdge;
    using gumMixedGraphTpl<Node,Edge,gumArc>::eraseneighbourEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getneighbourEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::hashMapEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::listMapEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::edgeListMapEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::getEdges;

    /// @}



    // ##############################################################################
    /// @name Iterators
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,Edge,gumArc>::endNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::rendNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::beginNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::rbeginNodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::endEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::rendEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::beginEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::rbeginEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::beginneighbourEdges;
    using gumMixedGraphTpl<Node,Edge,gumArc>::rbeginneighbourEdges;

    /// @}



    // ##############################################################################
    /// @name graph specific members
    // ##############################################################################
    /// @{

    using gumMixedGraphTpl<Node,Edge,gumArc>::hasCycle;

    // ==============================================================================
    /// returns the mixed edges/arcs graph corresponding to the current graph
    // ==============================================================================
    const gumMixedGraphTpl<Node,Edge,gumArc>& getMixedGraph() const ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Edge/Node instances are
     * created and inserted into the newly created gumUndiGraphTpl. Thus, the latter
     * and the copied gumUndiGraphTpl (from) are completely distinct in terms of memory
     * locations. */
    // ==============================================================================
    gumUndiGraphTpl<Node,Edge>& operator= ( const gumUndiGraphTpl<Node,Edge>& from );

    // ==============================================================================
    /// checks whether two graphs are identical
    // ==============================================================================
    const bool operator== ( const gumUndiGraphTpl<Node,Edge>& from ) const;

    // ==============================================================================
    /// checks whether two graphs are different
    // ==============================================================================
    const bool operator!= ( const gumUndiGraphTpl<Node,Edge>& from ) const;
    /// @}


  protected:
    using gumMixedGraphTpl<Node,Edge,gumArc>::nodes;
    using gumMixedGraphTpl<Node,Edge,gumArc>::edges;
};





/* ============================================================================== */
/* ===           BASE CLASS FOR MANIPULATING DIRECTED ACYCLIC GRAPHS          === */
/* ============================================================================== */
/** @class gumDAGTpl
 * @brief Base class for all directed acyclic graphs
 *
 * This is the base class for graphs containing only directed edges (the so-called
 * arcs) which, in addition, do not contain directed cycles (circuits according to
 * Berge's terminology. By directed edges, we mean in fact asymmetric edges, that
 * is, edges in which the order of the nodes is semantically meaningful, and in
 * which there is a difference between edge (x,y) and edge (y,x). These are thus
 * represented by arrows instead of links.
 *
 * Copying gumDAGTpl through usual functions like operator= will result in a completely
 * fresh copy to be generated, i.e., the new arcs and nodes are not shared in memory
 * with those copied. However, for optimization issues, an alternative copy
 * procedure is available, the so-called "shallow copy". In this mode, only pointers
 * on arcs/nodes are copied. As a result, this creates a completely distinct new
 * gumDAGTpl containing pointers toward the same arcs/nodes as the original gumDAGTpl.
 * Thus, as both gumDAGTpl are distinct, we can remove or add elements to one gumDAGTpl
 * without modifying the other gumDAGTpl. As the gumDAGTpl contain pointers, creating new
 * gumDAGTpl is fast as we do not need to create new arcs/nodes. This kind of copy
 * should be used with caution and probably only for temporary usage. Once a gumDAGTpl
 * has been copied through the shallowCopy member, the other shallow_ methods can be
 * used. Using the latter without having performed a shallow copy will usually
 * produce erroneous results (but they will not crash the application).
 *
 * @par Usage example:
 * @code
 * // creating empty graphs
 * gumDAGTpl<gumNode, gumArc> DAG1, DAG2;
 *
 * // adding nodes and arcs to DAG1
 * DAG1.insertNode (gumNode (1, "node1"));
 * gumNode node2 (2, "node 2");
 * DAG1.insertNode (node2);
 * DAG1.insertNode (gumNode (3, "node3"));
 * DAG1.insertArc (gumArc (1,2));
 * DAG1.insertArc (1,3);
 * DAG1.insertArc (2,3);
 *
 * // copying graphs
 * gumDAGTpl<gumNode, gumArc> DAG3 = DAG1;
 * DAG2 = DAG1;
 *
 * // displaying the content of a graph
 * cerr << DAG1 << endl;
 *
 * // check if a graph has no node
 * if (DAG1.emptyNodes ()) cerr << "graph DAG1 is empty" << endl;
 *
 * // remove all the arcs from a graph
 * DAG2.clearArcs ();
 *
 * // remove all the nodes (as well as their adjacent arcs)
 * DAG1.clearNodes ();
 *
 * // remove some arcs
 * DAG2.eraseArc (1,3);
 * DAG2.eraseArc (gumArc (2,3));
 *
 * // parse a graph
 * for (gumNodeList<gumNode>::iterator iter = DAG3.beginNodes ();
 *        iter != DAG3.endNodes (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = DAG3.beginArcs ();
 *        iter != DAG3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = DAG3.beginParentArcs (3);
 *        iter != DAG3.endArcs (); ++iter)
 *   cerr << *iter << endl;
 *
 * // remove all the arcs adjacent to a given node
 * DAG3.eraseneighbourArcs (2);
 * @endcode
 */
/* ============================================================================== */
template <typename Node, typename Arc>

class gumDAGTpl : public gumDiGraphTpl<Node,Arc> {
  public:

    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty DAG
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    // ==============================================================================
    explicit gumDAGTpl( gumSize nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                     bool nodes_resize_policy    = true,
                     gumSize arcs_size  = GUM_HASHTABLE_DEFAULT_SIZE,
                     bool arcs_resize_policy     = true );

    // ==============================================================================
    /// copy constructor (by default, deep copy of nodes and arcs lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Arc and Node instances are created
     * and inserted into the newly created gumDiGraphTpl. Thus, the latter and the
     * copied gumDiGraphTpl (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Arc and Node pointers are copied. As a result, all the arcs/nodes
     * in the new gumDiGraphTpl are shared with those of \e from. Thus, modifying the
     * content of an arc/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an
     * arc or node from one graph does not remove it from the other one. */
    // ==============================================================================
    gumDAGTpl( const gumDAGTpl<Node,Arc>& from,
            gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumDAGTpl();

    /// @}


    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new graph shares its edges/arc/nodes with \e from)
    // ==============================================================================
    gumDAGTpl<Node,Arc>& shallowCopy( const gumDAGTpl<Node,Arc>& from );

    // ==============================================================================
    /// adds a shallow copy of an edge into the edge list
    /** @throw gumDuplicateElement exception is thrown if there already exists a
     * similar edge in the list. */
    // ==============================================================================
    virtual const gumRefPtr<Arc>& shallowinsertArc( const gumRefPtr<Arc>& arc );

    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowInsertNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowGetNode;
    using gumMixedGraphTpl<Node,gumEdge,Arc>::shallowGetArc;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers specific for arcs
    // ##############################################################################
    /// @{

    using gumDiGraphTpl<Node,Arc>::hasCircuit;

    // ==============================================================================
    /// adds a copy of \e arc to the list of arcs of the graph.
    /** @return As this is a copy that is inserted into the list, the method
     * returns a reference to this new copy
     * @throw gumDuplicateElement exception is thrown if the list already contains
     * an arc with the same head and tail.
     * @throw gumCircuit exception is thrown if the addition of the new edge
     * implies the existence of a directed cycle. */
    // ==============================================================================
    virtual Arc& insertArc( const Arc& arc );
    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Arc/Node instances are
     * created and inserted into the newly created gumDiGraphTpl. Thus, the latter
     * and the copied gumDiGraphTpl (from) are completely distinct in terms of memory
     * locations. */
    // ==============================================================================
    gumDAGTpl<Node,Arc>& operator=( const gumDAGTpl<Node,Arc>& from );

    /// @}

  protected:
    using gumDiGraphTpl<Node,Arc>::nodes;
    using gumDiGraphTpl<Node,Arc>::arcs;

};





// always include the .tcc as it contains only templates
#include "gumGraph.tcc"



#endif /* GUM_GRAPH_H */
