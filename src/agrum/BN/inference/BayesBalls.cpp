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
 * @brief Implementation of the BayesBalls class.
 */
// ============================================================================
#include <agrum/BN/inference/BayesBalls.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/BN/inference/BayesBalls.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {

void
BayesBalls::requisiteNodes(const DAG& dag,
                                   const Set<NodeId>& query,
                                   const Set<NodeId>& hardEvidence,
                                   Set<NodeId>& requisite)
{
  __marks.clear();
  for (Set<NodeId>::iterator iter = query.begin(); iter != query.end(); ++iter) {
    __fromChild(*iter, dag, hardEvidence);
  }
  for (DAG::NodeIterator node = dag.beginNodes(); node != dag.endNodes(); ++node) {
    try {
      if (__marks[*node].first) {
        requisite.insert(*node);
      }
    } catch (NotFound&) {
      // Do nothing
    }
  }
}

void
BayesBalls::__fromChild(NodeId node, const DAG& dag, const Set<NodeId>& hardEvidence)
{
  if (not __marks.exists(node)) {
    __marks.insert(node, std::pair<bool, bool>(false, false));
  }
  if ((not hardEvidence.exists(node)) and (not __marks[node].first)) {
    __marks[node].first = true;
    for (ArcSetIterator iter = dag.parents(node).begin(); iter != dag.parents(node).end(); ++iter) {
      __fromChild(iter->tail(), dag, hardEvidence);
    }
  }
  if (not __marks[node].second) {
    __marks[node].second = true;
    for (ArcSetIterator iter = dag.children(node).begin(); iter != dag.children(node).end(); ++iter) {
      __fromParent(iter->head(), dag, hardEvidence);
    }
  }
}

void
BayesBalls::__fromParent(NodeId node, const DAG& dag, const Set<NodeId>& hardEvidence)
{
  if (!__marks.exists(node)) {
    __marks.insert(node, std::pair<bool, bool>(false, false));
  }
  if (hardEvidence.exists(node) and (not __marks[node].first)) {
    __marks[node].first = true;
    for (ArcSetIterator iter = dag.parents(node).begin(); iter != dag.parents(node).end(); ++iter) {
      __fromChild(iter->tail(), dag, hardEvidence);
    }
  } else if (!__marks[node].second) {
    __marks[node].second = true;
    for (ArcSetIterator iter = dag.children(node).begin(); iter != dag.children(node).end(); ++iter) {
      __fromParent(iter->head(), dag, hardEvidence);
    }
  }
}

} /* namespace gum */
// ============================================================================
