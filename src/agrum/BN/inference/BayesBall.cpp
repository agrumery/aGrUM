/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/**
 * @file
 * @brief Implementation of the BayesBall class.
 */

#include <agrum/BN/inference/BayesBall.h>


#ifdef GUM_NO_INLINE
#include <agrum/BN/inference/BayesBall.inl>
#endif  // GUM_NO_INLINE

namespace gum {

  void BayesBall::requisiteNodes( const DAG& dag,
                                  const NodeSet& query,
                                  const NodeSet& hardEvidence,
                                  const NodeSet& softEvidence,
                                  NodeSet& requisite ) {
    // for the moment, no node is requisite
    requisite.clear ();
    
    // create the marks (top = first and bottom = second )
    NodeProperty<std::pair<bool, bool>> marks ( dag.size() );
    const std::pair<bool, bool> empty_mark( false, false );

    // indicate that we will send the ball to all the query nodes (as children):
    // in list nodes_to_visit, the first element is the next node to send the
    // ball to and the Boolean indicates whether we shall reach it from one of
    // its children (true) or from one parent (false)
    List<std::pair<NodeId, bool>> nodes_to_visit;
    for ( const auto node : query ) {
      nodes_to_visit.insert( std::pair<NodeId, bool>( node, true ) );
    }

    // perform the bouncing ball until there is no node in the graph to send
    // the ball to
    while ( ! nodes_to_visit.empty() ) {
      // get the next node to visit
      NodeId node = nodes_to_visit.front().first;

      // if the marks of the node do not exist, create them
      if ( ! marks.exists( node ) )
        marks.insert( node, empty_mark );

      // bounce the ball toward the neighbors
      if ( nodes_to_visit.front().second ) {  // visit from a child
        nodes_to_visit.popFront();
        requisite.insert ( node );

        if ( hardEvidence.exists( node ) ) {
          continue;
        }

        if ( not marks[node].first ) {
          marks[node].first = true;  // top marked
          for ( const auto par : dag.parents( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( par, true ) );
          }
        }

        if ( not marks[node].second ) {
          marks[node].second = true;  // bottom marked
          for ( const auto chi : dag.children( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( chi, false ) );
          }
        }
      }
      else {  // visit from a parent
        nodes_to_visit.popFront();

        const bool is_hard_evidence = hardEvidence.exists( node );
        const bool is_evidence = is_hard_evidence or softEvidence.exists( node );

        if ( is_evidence && ! marks[node].first ) {
          marks[node].first = true;
          requisite.insert ( node );

          for ( const auto par : dag.parents( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( par, true ) );
          }
        }

        if ( ! is_hard_evidence && ! marks[node].second ) {
          marks[node].second = true;

          for ( const auto chi : dag.children( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( chi, false ) );
          }
        }
      }
    }
  }


} /* namespace gum */
