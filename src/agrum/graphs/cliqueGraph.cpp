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
 * @brief source of basic clique graphs (join trees, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <sstream>
#include <agrum/graphs/cliqueGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/cliqueGraph.inl>
#endif //GUM_NOINLINE


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  IMPLEMENTATION OF GUM_CLIQUE_GRAPH                 === */
  /* =========================================================================== */
  /* =========================================================================== */


  /// basic constructor: creates an empty xsclique graph

  CliqueGraph::CliqueGraph ( Size nodes_size,
                             bool nodes_resize_policy,
                             Size edges_size,
                             bool edges_resize_policy ) :
    NodeGraphPart ( nodes_size, nodes_resize_policy ),
    UndiGraph ( nodes_size, nodes_resize_policy,
                edges_size, edges_resize_policy ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( CliqueGraph );
  }


  /// copy constructor

  CliqueGraph::CliqueGraph ( const CliqueGraph& from ) :
    NodeGraphPart ( from ), // needed because NodeGraphPart is a virtual inherited
    UndiGraph ( from ),    // class (see C++ FAQ Lite #25.12 for details)
    __cliques ( from.__cliques ),
    __separators ( from.__separators ) {
    // for debugging purposes
    GUM_CONS_CPY ( CliqueGraph );
  }


  /// destructor

  CliqueGraph::~CliqueGraph() {
    // for debugging purposes
    GUM_DESTRUCTOR ( CliqueGraph );
  }


  /// returns a path from a clique containing node1 to a clique containing node2

  std::vector<NodeId>
  CliqueGraph::containerPath ( const NodeId node1, const NodeId node2 ) const  {
    // get a path from a __clique containing node1 to a __clique containing node2
    std::vector<NodeId> path = undirectedPath ( container ( node1 ),
                               container ( node2 ) );

    // it may happen that the path contains several nodes containing node1 and
    // node2. Hence we shall remove the superfluous nodes
    while ( ( path.size() >= 2 ) &&
            ( clique ( path[path.size() - 2] ).contains ( node2 ) ) )
      path.pop_back();

    while ( ( path.size() >= 2 ) &&
            ( clique ( path[1] ).contains ( node1 ) ) )
      path.erase ( path.begin() );

    return path;
  }


  /// changes the set of nodes included into a given clique and returns the new set
  /// @throw DuplicateElement if node_id already is in clique_id

  void CliqueGraph::addToClique ( const NodeId clique_id,
                                  const NodeId node_id ) {
    // get the current clique set
    NodeSet& clique = __cliques[clique_id];

    // check if the node already exists, in which case throw an exception
    if ( clique.contains ( node_id ) ) {
      GUM_ERROR ( DuplicateElement, "the clique set already contains the node" );
    }

    clique.insert ( node_id );

    // update the __separators adjacent to clique 'id'
    const NodeSet& set = neighbours ( clique_id );

    for ( NodeSetIterator ite = set.begin(); ite != set.end(); ++ite ) {
      if ( __cliques[*ite].contains ( node_id ) )
        __separators[Edge ( *ite, clique_id )].insert ( node_id );
    }
  }


  /// remove a node from a __clique

  void CliqueGraph::eraseFromClique ( const NodeId clique_id,
                                      const NodeId node_id ) {
    // get the current __clique set
    NodeSet& clique = __cliques[clique_id];

    // check if the node does not exist, in which case throw an exception
    if ( clique.contains ( node_id ) ) {
      clique.erase ( node_id );

      // update the __separators adjacent to __clique 'id'
      const NodeSet& set = neighbours ( clique_id );

      for ( NodeSetIterator ite = set.begin(); ite != set.end(); ++ite ) {
        Edge edge ( *ite, clique_id );

        if ( __separators[edge].contains ( node_id ) )
          __separators[edge].erase ( node_id );
      }
    }
  }


  /// DFS function for computing whether the running intersection property holds

  bool CliqueGraph::__runningIntersectionDFS
  ( const NodeId clique, const NodeId from,
    CliqueGraph::__RunningIntersect& infos_DFS ) const  {
    // check that no node in the clique belongs to the set of nodes belonging to
    // other connected components of the cliqueGraph
    const NodeSet& nodes_clique = __cliques[clique];

    for ( NodeSetIterator iter = nodes_clique.begin();
          iter != nodes_clique.end(); ++iter )
      if ( infos_DFS.nodes_other_components.contains ( *iter ) )
        return false;

    // update the structure that keeps track of the cliques that still require
    // chains to access some of their nodes
    for ( NodeSetIterator iter = nodes_clique.begin();
          iter != nodes_clique.end(); ++iter ) {
      if ( !infos_DFS.nodes_DFS_forbidden.contains ( *iter ) )
        infos_DFS.cliques_DFS_chain[clique].erase ( *iter );
    }

    // if the clique had already been visited, no need to see its neighbours
    // or to update the list of visited nodes
    if ( infos_DFS.visited_cliques.contains ( clique ) )
      return true;

    // update the list of nodes visited during the DFS
    for ( NodeSetIterator iter = nodes_clique.begin();
          iter != nodes_clique.end(); ++iter ) {
      if ( !infos_DFS.nodes_DFS_seen.contains ( *iter ) )
        infos_DFS.nodes_DFS_seen.insert ( *iter );
    }

    // update the fact that the clique has been visited
    infos_DFS.visited_cliques.insert ( clique );

    // check the neighbours that are different from "from" and that have not
    // been visited yet
    const NodeSet& neighbour = neighbours ( clique );

    for ( NodeSetIterator iter = neighbour.begin();
          iter != neighbour.end(); ++iter ) {
      const NodeId otherID = *iter;

      if ( otherID != from ) {
        // update the list of forbidden nodes in the DFS, i.e., the nodes that
        // belong to the clique but not to the separator
        const Edge edge ( otherID, clique );
        const NodeSet& from_separ = __separators[edge];

        for ( NodeSetIterator iter_clique = nodes_clique.begin();
              iter_clique != nodes_clique.end(); ++iter_clique ) {
          if ( !from_separ.contains ( *iter_clique ) )
            infos_DFS.nodes_DFS_forbidden.insert ( *iter_clique );
        }

        // check the neighbour
        if ( ! __runningIntersectionDFS ( otherID, clique, infos_DFS ) )
          return false;

        // remove from the forbidden list the nodes that belong to clique
        for ( NodeSetIterator iter_clique = nodes_clique.begin();
              iter_clique != nodes_clique.end(); ++iter_clique )
          infos_DFS.nodes_DFS_forbidden.erase ( *iter_clique );

        // check again the structure that keeps track of the cliques that still
        // require chains to access some of their nodes: the chain may be
        // the neighbour we just encountered
        for ( NodeSetIterator iter = nodes_clique.begin();
              iter != nodes_clique.end(); ++iter ) {
          if ( !infos_DFS.nodes_DFS_forbidden.contains ( *iter ) )
            infos_DFS.cliques_DFS_chain[clique].erase ( *iter );
        }
      }
    }

    // when a node is terminal, i.e., it has at most one neighbour, add its nodes
    // to the nodes forbidden by the DFS. It will prevent non adjacent extremal
    // cliques to contain the same node while this one does not belong to any
    // separator
    if ( neighbours ( clique ).size() <= 1 )
      for ( NodeSetIterator iter_clique = nodes_clique.begin();
            iter_clique != nodes_clique.end(); ++iter_clique )
        if ( !infos_DFS.nodes_DFS_forbidden.contains ( *iter_clique ) )
          infos_DFS.nodes_DFS_forbidden.insert ( *iter_clique );

    // here everything was OK. A priori, the running intersection property holds
    return true;
  }


  /// indicates whether the running intersection property holds

  bool CliqueGraph::hasRunningIntersection() const  {
    // create a RunningIntersect structure and initialize it
    __RunningIntersect infos_DFS;
    infos_DFS.cliques_DFS_chain = __cliques;

    // while there exist unvisited cliques, perform a DFS on them
    for ( const auto DFSnode : nodes() )
      if ( ! infos_DFS.visited_cliques.contains ( DFSnode ) ) {
        // no nodes are forbidden a priori in the DFS
        infos_DFS.nodes_DFS_forbidden.clear();

        // no node has already been seen in the DFS
        infos_DFS.nodes_DFS_seen.clear();

        // here iter_DFS points on a clique that has not been visited yet
        // visit the clique graph from this clique
        if ( ! __runningIntersectionDFS ( DFSnode, DFSnode, infos_DFS ) )
          return false;

        // the nodes that were seen during the DFS belong to a connected component
        // that is different from the connected components of the subsequent DFS
        for ( NodeSetIterator iter_seen = infos_DFS.nodes_DFS_seen.begin();
              iter_seen != infos_DFS.nodes_DFS_seen.end(); ++iter_seen )
          if ( ! infos_DFS.nodes_other_components.contains ( *iter_seen ) )
            infos_DFS.nodes_other_components.insert ( *iter_seen );
      }

    // check that no clique requires an additional chain to guarantee the
    // running intersection property
    for ( NodeProperty<NodeSet>::const_iterator_safe iter =
            infos_DFS.cliques_DFS_chain.beginSafe();
          iter != infos_DFS.cliques_DFS_chain.endSafe(); ++iter )
      if ( ! iter.val().empty() ) return false;

    return true;
  }


  /// checks whether two __clique graphs are equal

  bool CliqueGraph::operator== ( const CliqueGraph& from ) const {
    // check if both graphical structures are identical
    if ( UndiGraph::operator!= ( from ) ) return false;

    // check if the __cliques are identical
    for ( NodeProperty<NodeSet>::const_iterator_safe iter = __cliques.beginSafe();
          iter != __cliques.endSafe(); ++iter )
      if ( iter.val () != from.__cliques[iter.key()] ) return false;

    return true;
  }


  const std::string
  CliqueGraph::toString() const {
    std::stringstream stream;
    stream << "list of nodes:" << std::endl;

    for ( const auto node : nodes() ) {
      stream << " -- node: " << node << std::endl << "    clique:";

      /*const NodeSet& cl = clique( node );

      for ( NodeSetIterator iter = cl.begin(); iter != cl.end(); ++iter )*/
      for ( const auto cliq : clique ( node ) )
        stream << "  " << cliq;

      stream << std::endl;
    }

    stream << "\n\nlist of edges:\n";

    for ( const auto & edge : edges() )
      stream << edge << "  ";

    return stream.str();
  }


  const std::string
  CliqueGraph::toDot() const {
    std::stringstream stream;
    stream << "graph {" << std::endl;
    stream << "  edge [fontsize=8 fontcolor=red];" << std::endl << std::endl;

    for ( const auto node : nodes() ) {
      stream << "  " << node << " [ label=\"";

      /*const NodeSet& cl = clique ( node );

      for ( NodeSetIterator iter = cl.begin(); iter != cl.end(); ++iter )*/
      for ( const auto cliq : clique ( node ) )
        stream << "  " << cliq;

      stream << "\"];" << std::endl;
    }

    stream << std::endl;

    for ( const auto & edge : edges() ) {
      stream << edge << " [ label=\"";

      const NodeSet& sep = separator ( edge );

      for ( NodeSetIterator iter = sep.begin(); iter != sep.end(); ++iter )
        stream << "  " << *iter;

      stream << "\"];" << std::endl;
    }

    stream << std::endl << "}" << std::endl;

    return stream.str();
  }



/// a << operator for CliqueGraph

  std::ostream& operator<< ( std::ostream& stream,
                             const CliqueGraph& graph ) {
    stream << graph.toString();
    return stream;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
