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
 * @brief Header of the BayesBall class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYESBALLS_H
#define GUM_BAYESBALLS_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/core/list.h>

#include <agrum/BN/IBayesNet.h>

namespace gum {
  /**
   * @class BayesBall BayesBall.h <agrum/BN/inference/BayesBall.h>
   * @brief Implementation of Shachter's Bayes Balls algorithm.
   * @ingroup bn_group
   *
   */
  class BayesBall {
    public:
    /// Default constructor.
    BayesBall();

    /// Destructor.
    ~BayesBall();

    /**
     * Fill requisite with the requisite nodes in dag given a query and hard
     * evidences.
     */
    void requisiteNodes(const DAG &dag, const Set<NodeId> &query,
                        const Set<NodeId> &hardEvidence, Set<NodeId> &requisite);

    private:
    /// Call this when a node receive the ball from one of his child.
    void __fromChild(NodeId node, const DAG &dag, const Set<NodeId> &hardEvidence);

    /// Call this when a node reveive the ball from one of this parents.
    void __fromParent(NodeId node, const DAG &dag, const Set<NodeId> &hardEvidence);

    /// Top and bottom flags for each nodes.
    // HashTable< NodeId, std::pair<bool, bool> > __marks;
    NodeProperty<std::pair<bool, bool>> __marks;
  };
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/BN/inference/BayesBall.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_BAYESBALLS_H */
