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
#endif // GUM_NO_INLINE

namespace gum {

  void BayesBall::requisiteNodes(const DAG &dag,
                                 const NodeSet& query,
                                 const NodeSet& hardEvidence,
                                 const NodeSet& softEvidence,
                                 NodeSet& requisite) {
 
    // save the parameters so as to avoid passing them in arguments to
    // the recursive functions
    __dag = &dag;
    __hardEvidence = &hardEvidence;
    __softEvidence = &softEvidence;

    // create the marks (top and bottom) so that we won't have to test their
    // existence in the recursive functions
    __marks.clear();
    __marks.resize ( dag.size () );
    const std::pair<bool, bool> empty_mark (false, false);
    for ( const auto node : dag ) __marks.insert ( node, empty_mark );
    
    // perform the ball bouncing
    for ( const auto node : query ) {
      __fromChild ( node );
    }

    // the requisite nodes are those whose top mark (the first one) is set to true
    for ( const auto node : dag ) {
      if ( __marks[node].first ) {
        requisite.insert(node);
      }
    }
  }
  

  void BayesBall::__fromChild ( NodeId node ) {
    if ( __hardEvidence->exists(node) ) {
      // mark its top so that it will be included in the end into
      // the set of requisite nodes
      __marks[node].first = true;
      return;
    }
      
    if ( not __marks[node].first ) {
      __marks[node].first = true; // top marked

      for (const auto par : __dag->parents(node)) {
        __fromChild(par);
      }
    }

    if (not __marks[node].second) {
      __marks[node].second = true; // bottom marked

      for (const auto chi : __dag->children(node)) {
        __fromParent(chi);
      }
    }
  }

  
  void BayesBall::__fromParent(NodeId node) {
    const bool hard_evidence = __hardEvidence->exists(node);
    const bool evidence = hard_evidence or __softEvidence->exists ( node );
    
    if ( evidence && ! __marks[node].first ) {
      __marks[node].first = true;

      for (const auto par : __dag->parents(node)) {
        __fromChild(par);
      }
    }

    if (! hard_evidence && !__marks[node].second) {
      __marks[node].second = true;

      for (const auto chi : __dag->children(node)) {
        __fromParent(chi);
      }
    }
  }

  
  
} /* namespace gum */
