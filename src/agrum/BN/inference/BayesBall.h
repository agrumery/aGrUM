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
 * @brief The BayesBall algorithm (as described by Schachter).
 *
 * @author Lionel TORTI and Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYESBALLS_H
#define GUM_BAYESBALLS_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/list.h>
#include <agrum/core/sequence.h>

#include <agrum/BN/IBayesNet.h>

namespace gum {
  /**
   * @class BayesBall BayesBall.h <agrum/BN/inference/BayesBall.h>
   * @brief Implementation of Shachter's Bayes Balls algorithm.
   * @ingroup bn_inference
   *
   */
  class BayesBall {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
  private:
    /// Default constructor.
    BayesBall();

    /// Destructor.
    ~BayesBall();

    /// @}
  public:

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief Fill the 'requisite' nodeset with the requisite nodes in dag
     * given a query and evidence.
     *
     * Requisite nodes are those that are d-connected to at least one of the
     * query nodes given a set of hard and soft evidence
     */
    static void requisiteNodes( const DAG&     dag,
                         const NodeSet& query,
                         const NodeSet& hardEvidence,
                         const NodeSet& softEvidence,
                         NodeSet&       requisite );

    /** @brief update a set of potentials, keeping only those d-connected with
     * query variables given evidence */
    template <typename GUM_SCALAR, template <typename> class TABLE>
    static void relevantPotentials( const IBayesNet<GUM_SCALAR>&   bn,
                             const NodeSet&                 query,
                             const NodeSet&                 hardEvidence,
                             const NodeSet&                 softEvidence,
                             Set<const TABLE<GUM_SCALAR>*>& potentials );

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/BN/inference/BayesBall_inl.h>
#endif  // GUM_NO_INLINE

#include <agrum/BN/inference/BayesBall_tpl.h>

#endif /* GUM_BAYESBALLS_H */
