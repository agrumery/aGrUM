/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief d-separation analysis (as described in Koller & Friedman 2009)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_D_SEPARATION_ALGORITHM_H
#define GUM_D_SEPARATION_ALGORITHM_H


#include <agrum/BN/IBayesNet.h>
#include <agrum/agrum.h>


namespace gum {

  /** @class dSeparationAlgorithm
   * @brief the d-separation algorithm as described in Koller & Friedman (2009)
   * @ingroup bn_inference
   */
  class dSeparationAlgorithm {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    dSeparationAlgorithm();

    /// copy constructor
    dSeparationAlgorithm(const dSeparationAlgorithm& from);

    /// move constructor
    dSeparationAlgorithm(dSeparationAlgorithm&& from);

    /// destructor
    ~dSeparationAlgorithm();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    dSeparationAlgorithm& operator=(const dSeparationAlgorithm& from);

    /// move operator
    dSeparationAlgorithm& operator=(dSeparationAlgorithm&& from);

    /// @}


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
    void requisiteNodes(const DAG&     dag,
                        const NodeSet& query,
                        const NodeSet& hardEvidence,
                        const NodeSet& softEvidence,
                        NodeSet&       requisite) const;

    /** @brief update a set of potentials, keeping only those d-connected with
     * query variables given evidence */
    template < typename GUM_SCALAR, class TABLE >
    void relevantPotentials(const IBayesNet< GUM_SCALAR >& bn,
                            const NodeSet&                 query,
                            const NodeSet&                 hardEvidence,
                            const NodeSet&                 softEvidence,
                            Set< const TABLE* >&           potentials);

    /// @}
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/dSeparationAlgorithm_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/BN/algorithms/dSeparationAlgorithm_tpl.h>


#endif /* GUM_D_SEPARATION_ALGORITHM_H */
