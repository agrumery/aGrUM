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
/** @file
 * @brief Basic class for all graphs of cliques (join trees, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_CLIQUE_GRAPH_H
#define GUM_CLIQUE_GRAPH_H

#include <iostream>
#include <agrum/core/utils.h>
#include <agrum/graphs/undiGraph.h>


namespace gum {
  

  /* ============================================================================ */
  /* ===                           GRAPHS OF CLIQUES                          === */
  /* ============================================================================ */
  /** @class CliqueGraph
   * @brief Basic graph of cliques
   *
   * \ingroup graph_group
   *
   *
   * CliqueGraph is a undirected graph the nodes of which are Cliques, i.e.,
   * sets of NodeIds. Cliques are linked by Edges, edges support separators
   * which are intersection of Cliques.
   */
  /* ============================================================================ */
  class CliqueGraph : public UndiGraph {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// basic constructor: creates an empty clique graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    // ============================================================================
    explicit CliqueGraph( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                          bool nodes_resize_policy    = true,
                          Size edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                          bool edges_resize_policy    = true );
    
    // ============================================================================
    /// copy constructor (by default, deep copy of nodes and edges lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Edge and NodeId instances are created
     * and inserted into the newly created CliqueGraph. Thus, the latter and the
     * copied CliqueGraph (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Edge and NodeId pointers are copied. As a result, all the edges/nodes
     * in the new CliqueGraph are shared with those of \e from. Thus, modifying the
     * content of an edge/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an
     * edge or node from one graph does not remove it from the other one. */
    // ============================================================================
    CliqueGraph( const CliqueGraph& from );
    
    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~CliqueGraph();

    /// @}


    // ============================================================================
    /// adds a copy of \e edge to the list of edges of the graph.
    /** @return As this is a copy that is inserted into the list, the method
     * returns a reference to this new copy. The insertion is achieved only if it
     * does not break the running intersection property. In this case, it throws an
     * exception
     * @throw DuplicateElement exception is thrown if the edge list already
     * contains an edge with the same extremities */
    // ============================================================================
    virtual void insertEdge( const Edge& edge );
    virtual void insertEdge( const NodeId& first,const NodeId& second );
    
    // ============================================================================
    /// removes an edge (and its separator) from the clique graph
    /** If the clique graph does not contain the edge, then the function does
     * nothing. In particular, it does not throw any exception. */
    // ============================================================================
    virtual void eraseEdge( const Edge &edge );
    
    // ============================================================================
    /// removes an edge (and its separator) from the clique graph
    /** If the clique graph does not contain the edge, then the function does
     * nothing. In particular, it does not throw any exception. */
    // ============================================================================
    virtual void eraseEdge( const NodeId &id1, const NodeId &id2 );
    
    // ============================================================================
    /// adds a new clique to the graph
    // ============================================================================
    NodeId insertNode( const NodeSet& clique );

    // ============================================================================
    /// adds a new clique to the graph
    // ============================================================================
    void insertNode( const NodeId id,  const NodeSet& clique );

    // ============================================================================
    /// removes a given clique from the clique graph
    // ============================================================================
    virtual void eraseNode( const NodeId node );

    // ============================================================================
    /// returns the set of nodes included into a given clique
    /**
     * @throw NotFound
     */
    // ============================================================================
    const NodeSet& clique( const NodeId idClique ) const;

    // ============================================================================
    /// returns the id of a clique containing the node the id of which is in argument
    // ============================================================================
    NodeId container( const NodeId idNode ) const;

    // ============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    // ============================================================================
    void setClique( const NodeId idClique,const NodeSet& new_clique );

    // ============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    /** @throw DuplicateElement exception is thrown if the clique set already
     * contains the node (i.e., it already contains a node with the same ID) */
    // ============================================================================
    void addToClique( const NodeId clique_id,const NodeId node_id );

    // ============================================================================
    /// remove a node from a clique
    /** If the node cannot be found in the clique set, then the function does
     * nothing. In particular, it does not throw any exception. */
    // ============================================================================
    void eraseFromClique( const NodeId clique_id,const NodeId node_id );

    // ============================================================================
    /// returns the separator included in a given edge
    /**
     * @throw NotFound
     */
    // ============================================================================
    const NodeSet& separator( const Edge& edge )  const;

    // ============================================================================
    /// returns the separator included in an edge specified by its extremities
    /**
     * @throw NotFound
     */
    // ============================================================================
    const NodeSet& separator( const NodeId clique1, const NodeId clique ) const;

    // ============================================================================
    /// returns a path from a clique containing node1 to a clique containing node2
    // ============================================================================
    std::vector<NodeId> containerPath( const NodeId node1, const NodeId node2 ) const;
    // ============================================================================
    /// indicates whether the running intersection property holds
    /** The function works properly even if the graph contains cycles. */
    // ============================================================================
    bool hasRunningIntersection() const ;

    // ============================================================================
    /// indicates whether the graph is a join tree
    // ============================================================================
    bool isJoinTree() const ;

    // ============================================================================
    /// removes all the nodes from the graph (as well as their adjacent edges/arcs)
    // ============================================================================
    virtual void clear();

    // ============================================================================
    /// removes all edges and separators
    // ============================================================================
    virtual void clearEdges();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Edge/NodeId instances are
     * created and inserted into the newly created CliqueGraph. Thus, the latter
     * and the copied CliqueGraph (from) are completely distinct in terms of
     * memory locations. */
    // ============================================================================
    CliqueGraph&  operator= ( const CliqueGraph& from );

    // ============================================================================
    /// checks whether two clique graphs are different
    // ============================================================================
    bool operator!= ( const CliqueGraph &from ) const;

    // ============================================================================
    /// checks whether two clique graphs are equal
    // ============================================================================
    bool operator== ( const CliqueGraph &from ) const;

    /// @}


    virtual const std::string toString() const;
    virtual const std::string toDot() const;


  private:
    /// the set of nodes contained into the cliques
    Property<NodeSet>::onNodes __cliques;

    /// the set of nodes contained into the separators
    Property< NodeSet >::onEdges __separators;

    // ============================================================================
    /// function used to update the separators when a clique is modified
    // ============================================================================
    void __updateSeparators( const NodeId clique1 );
    /// structure used for the computation of the running intersection property

    struct __RunningIntersect {
      /** @brief structure indicating for each clique whether it has been
       * examined by a DFS (Depth First Search) */
      NodeSet visited_cliques;

      /// structure indicating the nodes that belong to other connected components
      /** These nodes must not be found in the current connected component if the
       * running intersection holds */
      NodeSet nodes_other_components;

      /// the nodes that are currently forbidden by separators in the DFS
      NodeSet nodes_DFS_forbidden;
      /// set of the nodes examined during the current DFS

      NodeSet nodes_DFS_seen;
      /** @brief for each clique, the list of its nodes that require accessing the
       * clique through a chain
       *
       * At the beginning, all nodes in a clique require a chain to be accessed.
       * In a DFS, when a chain reaches a clique, we remove from \c cliques_DFS_chain
       * the nodes that are are not forbidden by the DFS, i.e., the nodes that are
       * reachable by the chain starting from the root of the DFS. These are the
       * nodes that belong to the separators. Hence, after completing all the DFS,
       * there remain in \c cliques_DFS_chain only the nodes that are accessible by
       * no chain but that are found in several parts of the clique graph. In such
       * a case, this is a violation of the running intersection property. Hence,
       * for the latter to hold, after completion of all the DFS,
       * \c cliques_DFS_chain must contain only empty sets. */
      Property<NodeSet>::onNodes cliques_DFS_chain;
    };

    // ============================================================================
    /// function used for the computation of the running intersection property
    // ============================================================================
    bool __runningIntersectionDFS( const NodeId clique, const NodeId from,
                                   __RunningIntersect& infos_DFS ) const ;
  };


  
  typedef CliqueGraph JunctionTree;

  
  // ============================================================================
  /// for friendly displaying the content of clique graphs
  // ============================================================================
  std::ostream& operator<< ( std::ostream&, const CliqueGraph& );

  
} /* namespace gum */
  
  

#ifndef GUM_NO_INLINE
#include <agrum/graphs/cliqueGraph.inl>
#endif //GUM_NOINLINE


#endif /* GUM_CLIQUE_GRAPH_H */
