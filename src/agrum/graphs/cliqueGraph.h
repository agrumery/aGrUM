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

#include <agrum/config.h>

#include <agrum/graphs/undiGraph.h>

namespace gum {
  /* =========================================================================== */
  /* ===                           GRAPHS OF CLIQUES                         === */
  /* =========================================================================== */
  /** @class CliqueGraph
   * @brief Basic graph of cliques
   *
   * \ingroup graph_group
   *
   *
   * A CliqueGraph is an undirected graph the nodes of which are Cliques, i.e.,
   * sets of NodeIds. Cliques are linked by Edges. These edges contain separators
   * that are actually the intersection of the two Cliques at the extermities of
   * the edge. */
  /* =========================================================================== */

  class CliqueGraph : public UndiGraph {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates an empty clique graph
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */

    explicit CliqueGraph(Size nodes_size = HashTableConst::default_size,
                         bool nodes_resize_policy = true,
                         Size edges_size = HashTableConst::default_size,
                         bool edges_resize_policy = true);

    /// copy constructor
    /** @param from the CliqueGraph that will be copied into \e this */

    CliqueGraph(const CliqueGraph &from);

    /// destructor

    virtual ~CliqueGraph();

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// inserts a new edge between two cliques
    /** @param first the id of one extremity of the new edge to be inserted
     * @param second the id of the other extremity of the new edge to be inserted
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if first and/or second do not belong to the
     * graph nodes */
    GUM_DEPRECATED(virtual void insertEdge(const NodeId first, const NodeId second));
    virtual void addEdge(const NodeId first, const NodeId second);

    /// removes an edge (and its separator) from the clique graph
    /** @param edge the edge to be removed
     * @warning if the edge does not exist, nothing is done. In particular, no
     * exception is thrown. */

    virtual void eraseEdge(const Edge &edge);

    /// removes all edges and their separators

    virtual void clearEdges();

    /// adds a new clique to the graph
    /** @return the id chosen for the new clique */

    GUM_DEPRECATED(NodeId insertNode(const NodeSet &clique));
    GUM_DEPRECATED(NodeId insertNode());
    NodeId addNode(const NodeSet &clique);
    virtual NodeId addNode();

    /// try to add a new clique to the graph
    /** @throws DuplicateElement exception is thrown if the id of the clique
     * already exists within the clique graph */

    GUM_DEPRECATED(virtual void insertNode(const NodeId id, const NodeSet &clique));
    GUM_DEPRECATED(virtual void insertNode(const NodeId id));
    virtual void addNode(const NodeId id, const NodeSet &clique);
    virtual void addNode(const NodeId id);

    /// removes a given clique from the clique graph
    /** If the CliqueGraph does not contain the node, then nothing is done. In
     * particular, no exception is raised. */

    virtual void eraseNode(const NodeId node);

    /** @brief removes all the cliques and separators from the graph (as well as
     * their adjacent edges) */

    virtual void clear();

    /// returns the set of nodes included into a given clique
    /** @throw NotFound exception is raised if the clique does not belong to
     * the clique graph */

    const NodeSet &clique(const NodeId idClique) const;

    /** @brief returns the id of a clique containing the node the id of which is
     * in argument
     * @warning note that this method is time consuming as the clique graph does
     * not contain a priori information about which clique could contain idNode.
     * As a consequence, it searches the cliques until it finds one that actually
     * contains idNode.
     * @throws NotFound exception is thrown if no clique contains idNode */

    NodeId container(const NodeId idNode) const;

    /** @brief changes the set of nodes included into a given clique and returns
     * the new set
     * @throws NotFound exception is thrown if idClique is not a clique of
     * the clique graph */

    virtual void setClique(const NodeId idClique, const NodeSet &new_clique);

    /** @brief changes the set of nodes included into a given clique and returns
     * the new set
     *
     * @throws NotFound exception is thrown if clique_id does not exist
     * @throw DuplicateElement exception is thrown if clique_id set already
     * contains the node */

    virtual void addToClique(const NodeId clique_id, const NodeId node_id);

    /// remove a node from a clique
    /** If node_id cannot be found in the clique set, then the function does
     * nothing. In particular, it does not throw any exception.
     * @throws NotFound exception is thrown if clique_id does not exist */

    virtual void eraseFromClique(const NodeId clique_id, const NodeId node_id);

    /// returns the separator included in a given edge
    /** @throw NotFound exception is thrown if the edge does not belong to the
     * clique graph */

    const NodeSet &separator(const Edge &edge) const;

    /// returns the separator included in an edge specified by its extremities
    /** @throw NotFound exception is thrown if the edge does not belong to the
     * clique graph */

    const NodeSet &separator(const NodeId clique1, const NodeId clique) const;

    /// returns a path from a clique containing node1 to a clique containing node2
    /** @throws NotFound such path cannot be found */

    std::vector<NodeId> containerPath(const NodeId node1, const NodeId node2) const;

    /// indicates whether the running intersection property holds
    /** The function works properly even if the graph contains cycles. */

    bool hasRunningIntersection() const;

    /// indicates whether the graph is a join tree

    bool isJoinTree() const;

    /// friendly displays the content of the CliqueGraph

    virtual const std::string toString() const;

    /// friendly displays the content of the CliqueGraph in DOT format

    virtual const std::string toDot() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator

    CliqueGraph &operator=(const CliqueGraph &from);

    /// checks whether two clique graphs are different

    bool operator!=(const CliqueGraph &from) const;

    /// checks whether two clique graphs are equal

    bool operator==(const CliqueGraph &from) const;

    /// @}

    private:
    /// the set of nodes contained into the cliques
    NodeProperty<NodeSet> __cliques;

    /// the set of nodes contained into the separators
    EdgeProperty<NodeSet> __separators;

    /// function used to update the separators when a clique is modified

    void __updateSeparators(const NodeId clique1);

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
       * In a DFS, when a chain reaches a clique, we remove from
       * \c cliques_DFS_chain the nodes that are not forbidden by the DFS,
       * i.e., the nodes that are reachable by the chain starting from the root of
       * the DFS. These are the nodes that belong to the separators. Hence, after
       * completing all the DFS, there remain in \c cliques_DFS_chain only the
       * nodes that are accessible by no chain but that are found in several parts
       * of the clique graph. In such a case, this is a violation of the running
       * intersection property. Hence, for the latter to hold, after completion of
       * all the DFS, \c cliques_DFS_chain must contain only empty sets. */
      NodeProperty<NodeSet> cliques_DFS_chain;
    };

    /// function used for the computation of the running intersection property

    bool __runningIntersectionDFS(const NodeId clique, const NodeId from,
                                  __RunningIntersect &infos_DFS) const;
  };

  typedef CliqueGraph JunctionTree;

  /// for friendly displaying the content of clique graphs

  std::ostream &operator<<(std::ostream &, const CliqueGraph &);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/cliqueGraph.inl>
#endif // GUM_NOINLINE

#endif /* GUM_CLIQUE_GRAPH_H */
