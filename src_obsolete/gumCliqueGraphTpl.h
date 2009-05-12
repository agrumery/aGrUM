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
 * @brief Basic class for all graphs of cliques (join trees, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_CLIQUE_GRAPH_TPL_H
#define GUM_CLIQUE_GRAPH_TPL_H

#include <iostream>
#include <agrum/gum_utils.h>
#include <agrum/gumId.h>
#include <agrum/gumNode.h>
#include <agrum/gumGraphElements.h>
#include <agrum/gumGraph.h>


/* ============================================================================== */
/* ===                            GRAPHS OF CLIQUES                           === */
/* ============================================================================== */
/** @class gumCliqueGraphTpl
 * @brief Basic graph of cliques
 *
 * gumCliqueGraphTpl<Clique,Edge,Node> is a graph the nodes of which are Cliques, i.e.,
 * sets of Nodes. Cliques are linked by Edges.
 */
/* ============================================================================== */
template <typename Clique, typename Edge, typename Node>

class gumCliqueGraphTpl : public gumUndiGraphTpl<Clique,Edge> {
  public:

    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty clique graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    // ==============================================================================
    explicit gumCliqueGraphTpl( gumSize nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                             bool nodes_resize_policy    = true,
                             gumSize edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                             bool edges_resize_policy    = true );

    // ==============================================================================
    /// copy constructor (by default, deep copy of nodes and edges lists)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Edge and Node instances are created
     * and inserted into the newly created gumCliqueGraphTpl. Thus, the latter and the
     * copied gumCliqueGraphTpl (from) are completely distinct in terms of memory
     * locations. If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only
     * Edge and Node pointers are copied. As a result, all the edges/nodes
     * in the new gumCliqueGraphTpl are shared with those of \e from. Thus, modifying the
     * content of an edge/node in one graph will also modify it in the other
     * graph. However, both graphs are distinct in the sense that removing an
     * edge or node from one graph does not remove it from the other one. */
    // ==============================================================================
    gumCliqueGraphTpl( const gumCliqueGraphTpl<Clique,Edge,Node>& from,
                    gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumCliqueGraphTpl<Clique,Edge,Node> ();

    /// @}


    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new graph shares its edges/nodes with \e from)
    // ==============================================================================
    gumCliqueGraphTpl<Clique,Edge,Node>&
    shallowCopy( const gumCliqueGraphTpl<Clique,Edge,Node>& from );

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    using gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges;
    using gumUndiGraphTpl<Clique,Edge>::endEdges;
    using gumUndiGraphTpl<Clique,Edge>::getNodes;

    // ==============================================================================
    /// adds a copy of \e arc to the list of edges of the graph.
    /** @return As this is a copy that is inserted into the list, the method
     * returns a reference to this new copy. The insertion is achieved only if it
     * does not break the running intersection property. In this case, it throws an
     * exception
     * @throw gumDuplicateElement exception is thrown if the edge list already
     * contains an edge with the same extremities */
    // ==============================================================================
    virtual Edge& insertEdge( const Edge& edge );

    // ==============================================================================
    /// removes an edge (and its separator) from the clique graph
    /** If the clique graph does not contain the edge, then the function does nothing.
     * In particular, it does not throw any exception. */
    // ==============================================================================
    virtual void eraseEdge( const Edge &edge );

    // ==============================================================================
    /// removes an edge (and its separator) from the clique graph
    /** If the clique graph does not contain the edge, then the function does nothing.
     * In particular, it does not throw any exception. */
    // ==============================================================================
    virtual void eraseEdge( const gumId &id1, const gumId &id2 );

    // ==============================================================================
    /// adds a new clique to the graph
    // ==============================================================================
    virtual Clique& insertNode( const Clique& clique,
                                const gumNodeList<Node>& list,
                                gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// removes a given clique from the clique graph
    // ==============================================================================
    virtual void eraseNode( const Node &node );

    // ==============================================================================
    /// removes a given clique from the clique graph
    // ==============================================================================
    virtual void eraseNode( const gumId &id );

    // ==============================================================================
    /// returns the set of nodes included into a given clique
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumNodeList<Node>& getClique( const Clique& clique ) const;

    // ==============================================================================
    /// returns the set of nodes included into a clique specified by its ID
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumNodeList<Node>& getClique( const gumId& id )
    const;

    // ==============================================================================
    /// returns the id of a clique containing the node the id of which is in argument
    // ==============================================================================
    gumId getContainerNode( const gumId& id ) const;

    // ==============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    // ==============================================================================
    const gumNodeList<Node>& setClique( const Clique& clique,
                                        const gumNodeList<Node>& new_clique,
                                        gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    // ==============================================================================
    const gumNodeList<Node>& setClique( const gumId& id,
                                        const gumNodeList<Node>& new_clique,
                                        gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    /** @throw gumDuplicateElement exception is thrown if the clique set already
     * contains the node (i.e., it already contains a node with the same ID) */
    // ==============================================================================
    const gumNodeList<Node>& addToClique( const Clique& clique,
                                          const gumRefPtr<Node>& node,
                                          gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// changes the set of nodes included into a given clique and returns the new set
    /** @throw gumDuplicateElement exception is thrown if the clique set already
     * contains the node (i.e., it already contains a node with the same ID) */
    // ==============================================================================
    const gumNodeList<Node>& addToClique( const gumId& id,
                                          const gumRefPtr<Node>& node,
                                          gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// remove a node from a clique
    /** If the node cannot be found in the clique set, then the function does nothing.
     * In particular, it does not throw any exception. */
    // ==============================================================================
    const gumNodeList<Node>& eraseFromClique( const Clique& clique, const Node& node );

    // ==============================================================================
    /// remove a node from a clique
    /** If the node cannot be found in the clique set, then the function does nothing.
     * In particular, it does not throw any exception. */
    // ==============================================================================
    const gumNodeList<Node>& eraseFromClique( const gumId& id, const Node& node );

    // ==============================================================================
    /// remove a node from a clique
    /** If the node cannot be found in the clique set, then the function does nothing.
    * In particular, it does not throw any exception. */
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumNodeList<Node>& eraseFromClique( const gumId& id, const gumId& node );

    // ==============================================================================
    /// returns the separator included in a given edge
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumNodeList<Node>& getSeparator( const Edge& edge )
    const;

    // ==============================================================================
    /// returns the separator included in an edge specified by its extremities
    /**
    * @throw gumNotFound
    */
    // ==============================================================================
    const gumNodeList<Node>& getSeparator( const gumId &node1, const gumId &node2 )
    const;

    // ==============================================================================
    /// returns a possible path from node1 to node2 in the clique graph
    /** @param node1 the id of the clique from which the path begins
    * @param node2 the id of the clique to which the path ends
    * @throw gumNotFound
    */
    // ==============================================================================
    std::vector<gumId> getPath( const gumId &node1, const gumId &node2 )
    const;

    // ==============================================================================
    /// returns a path from a clique containing node1 to a clique containing node2
    // ==============================================================================
    std::vector<gumId> getContainerPath( const gumId &node1, const gumId &node2 )
    const;

    // ==============================================================================
    /// indicates whether the running intersection property holds
    /** The function works properly even if the graph contains cycles. */
    // ==============================================================================
    const bool hasRunningIntersection() const ;

    // ==============================================================================
    /// indicates whether the graph is a join tree
    // ==============================================================================
    const bool isJoinTree() const ;

    // ==============================================================================
    /// removes all the nodes from the graph (as well as their adjacent edges/arcs)
    // ==============================================================================
    virtual void clearNodes();

    // ==============================================================================
    /// removes all the edges from the graph
    // ==============================================================================
    virtual void clearEdges();
    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Edge/Node instances are
     * created and inserted into the newly created gumCliqueGraphTpl. Thus, the latter
     * and the copied gumCliqueGraphTpl (from) are completely distinct in terms of memory
     * locations. */
    // ==============================================================================
    gumCliqueGraphTpl<Clique,Edge,Node>&
    operator= ( const gumCliqueGraphTpl<Clique,Edge,Node>& from );

    // ==============================================================================
    /// checks whether two clique graphs are different
    // ==============================================================================
    const bool operator!= ( const gumCliqueGraphTpl<Clique,Edge,Node> &from ) const;

    // ==============================================================================
    /// checks whether two clique graphs are equal
    // ==============================================================================
    const bool operator== ( const gumCliqueGraphTpl<Clique,Edge,Node> &from ) const;
    /// @}



  private:
    /// the set of nodes contained into the cliques
    gumHashTable< gumId, gumNodeList<Node> > cliques;

    /// the set of nodes contained into the separators
    gumHashTable< std::pair<gumId,gumId>, gumNodeList<Node> > separators;

    // ==============================================================================
    /// function used to update the separators when clique/edges are modified
    // ==============================================================================
    void updateSeparator( const gumId& id1, const gumId& id2 );

    /// structure used for the computation of the running intersection property

    struct gumRunningIntersect {
      /// structure indicating for each clique whether it has been examined by a DFS
      gumHashTable<gumId,bool> visited_cliques;

      /// structure indicating the nodes that belong to other connected components
      /** These nodes must not be found in the current connected component if the
       * running intersection holds */
      gumHashTable<gumId,bool> nodes_other_components;

      /// the nodes that are currently forbidden by separators in the DFS
      gumHashTable<gumId,bool> nodes_DFS_forbidden;

      /// list of the nodes examined during the current DFS
      gumHashTable<gumId,bool> nodes_DFS_seen;

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
      gumHashTable< gumId,gumNodeList<Node> > cliques_DFS_chain;
    };

    // ==============================================================================
    /// function used for the computation of the running intersection property
    // ==============================================================================
    const bool runningIntersectionDFS( const gumId& clique, const gumId& from,
                                       gumRunningIntersect& infos_DFS ) const ;

    // ==============================================================================
    /// function used to mark the path from one clique to another
    // ==============================================================================
    const bool getPathMark( gumHashTable<gumId,char>& marks,
                            std::vector<gumId>& path,
                            const gumId& node, const gumId& from,
                            const gumId& looked ) const ;

};





// ================================================================================
/// for friendly displaying the content of clique graphs
// ================================================================================
template <typename Clique, typename Edge, typename Node>
std::ostream& operator<< ( std::ostream&, const gumCliqueGraphTpl<Clique,Edge,Node>& );



// always include the .tcc as it contains only templates
#include "gumCliqueGraphTpl.tcc"

typedef gumCliqueGraphTpl<gumNode, gumEdge, gumNode> gumJunctionTree;

#endif /* GUM_CLIQUE_GRAPH_TPL_H */
