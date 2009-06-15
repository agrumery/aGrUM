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
namespace gum {

// Default constructor.
template<typename T_DATA> INLINE
BayesBalls<T_DATA>::BayesBalls():
  __bayesNet(0), __query(0), __softEvidence(0), __hardEvidence(0)
{
  GUM_CONSTRUCTOR( BayesBalls );
}

// Destructor.
template<typename T_DATA> INLINE
BayesBalls<T_DATA>::~BayesBalls()
{
  GUM_DESTRUCTOR( BayesBalls );
}

// Returns the sequence of requisite nodes for a Bayes Net given a set of
// query nodes, the list of nodes with soft and hard evidences.
//
// @return Returns a pointer over a sequence, delete it after use.
template<typename T_DATA> INLINE
Sequence<NodeId>*
BayesBalls<T_DATA>::requisiteNodes(const BayesNet<T_DATA>& bayesNet,
                                   const Sequence<NodeId>& query,
                                   const Sequence<NodeId>& softEvidence,
                                   const Sequence<NodeId>& hardEvidence)
{
  // Initialization.
  __bayesNet = &bayesNet;
  __query = &query;
  __softEvidence = &softEvidence;
  __hardEvidence = &hardEvidence;
  __visited = new Sequence<NodeId>();
  // Initiating Baye's ball propagation.
  for (Sequence<NodeId>::iterator iter = query.begin(); iter != query.end(); ++iter) {
    __fromChild(*iter);
  }
  return __visited;
}

/// Call this when a node receive the ball from one of his child.
template<typename T_DATA> INLINE
void
BayesBalls<T_DATA>::__fromChild(NodeId node)
{
  if (!__visited->exists(node)) __visited->insert(node);
  if (!__marks.exists(node)) __marks.insert(node, std::pair<bool, bool>(false, false));
  if (!__hardEvidence->exists(node)) {
    if (!__marks[node].first) {
      __marks[node].first = true;
      for (ArcSetIterator iter = __bayesNet->dag().parents(node).begin(); iter != __bayesNet->dag().parents(node).end(); ++iter) {
        __fromChild(iter->tail());
      }
    }
    if (!__marks[node].second) {
      __marks[node].second = true;
      for (ArcSetIterator iter = __bayesNet->dag().children(node).begin(); iter != __bayesNet->dag().children(node).end(); ++iter) {
        __fromParent(iter->head());
      }
    }
  }
}

/// Call this when a node receive the ball from one of this parents.
template<typename T_DATA> INLINE
void
BayesBalls<T_DATA>::__fromParent(NodeId node)
{
  if (!__visited->exists(node)) __visited->insert(node);
  if (!__marks.exists(node)) __marks.insert(node, std::pair<bool, bool>(false, false));
  if (__hardEvidence->exists(node) and (!__marks[node].first)) {
    __marks[node].first = true;
    for (ArcSetIterator iter = __bayesNet->dag().parents(node).begin(); iter != __bayesNet->dag().parents(node).end(); ++iter) {
      __fromChild(iter->tail());
    }
  } else if (!__marks[node].second) {
    __marks[node].second = true;
    for (ArcSetIterator iter = __bayesNet->dag().children(node).begin(); iter != __bayesNet->dag().children(node).end(); ++iter) {
      __fromParent(iter->head());
    }
  }
}

} /* namespace gum */
// ============================================================================
