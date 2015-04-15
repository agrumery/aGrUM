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

  void BayesBall::requisiteNodes(const DAG &dag, const Set<NodeId> &query,
                                 const Set<NodeId> &hardEvidence,
                                 Set<NodeId> &requisite) {
    __marks.clear();

    for (auto node : query) {
      __fromChild(node, dag, hardEvidence);
    }

    for (auto node : dag.nodes()) {
      try {
        if (__marks[node].first) {
          requisite.insert(node);
        }
      } catch (NotFound &) {
        // Do nothing
      }
    }
  }

  void BayesBall::__fromChild(NodeId node, const DAG &dag,
                              const Set<NodeId> &hardEvidence) {
    if (not __marks.exists(node)) {
      __marks.insert(node, std::pair<bool, bool>(false, false));
    }

    if ((not hardEvidence.exists(node)) and (not __marks[node].first)) {
      __marks[node].first = true;

      for (auto par : dag.parents(node)) {
        __fromChild(par, dag, hardEvidence);
      }
    }

    if (not __marks[node].second) {
      __marks[node].second = true;

      for (auto chi : dag.children(node)) {
        __fromParent(chi, dag, hardEvidence);
      }
    }
  }

  void BayesBall::__fromParent(NodeId node, const DAG &dag,
                               const Set<NodeId> &hardEvidence) {
    if (!__marks.exists(node)) {
      __marks.insert(node, std::pair<bool, bool>(false, false));
    }

    if (hardEvidence.exists(node) and (not __marks[node].first)) {
      __marks[node].first = true;

      for (auto par : dag.parents(node)) {
        __fromChild(par, dag, hardEvidence);
      }
    } else if (!__marks[node].second) {
      __marks[node].second = true;

      for (auto chi : dag.children(node)) {
        __fromParent(chi, dag, hardEvidence);
      }
    }
  }

} /* namespace gum */
