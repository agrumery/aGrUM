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


/** @file
 * @brief A dirichlet priori: computes its N'_ijk from a bayesian network
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_DIRICHLET_FROM_BN_H
#define GUM_LEARNING_PRIOR_DIRICHLET_FROM_BN_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/inference/lazyPropagation.h>

#include <agrum/BN/learning/priors/prior.h>

namespace gum::learning {


  /** @class DirichletPriorFromBN
   * @brief A dirichlet priori: computes its N'_ijk from a database
   * @headerfile DirichletPriorFromBN.h <agrum/tools/database/DirichletPriorFromBN.h>
   * @ingroup learning_priors
   */
  template < typename GUM_SCALAR >
  class DirichletPriorFromBN: public Prior {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    /** @param bn the prior bayesian network from which computation is performed.
     * This is useful to get access to the random variables
     * @param nodeId2priorId a mapping from the ids of the nodes in the graphical model to the
     * corresponding id in the prior BN. An empty nodeId2Columns bijection means that the mapping
     * is an identity.
     */
    DirichletPriorFromBN(const DatabaseTable& learning_db, const BayesNet< GUM_SCALAR >* priorbn);

    /// copy constructor
    DirichletPriorFromBN(const DirichletPriorFromBN& from);

    /// move constructor
    DirichletPriorFromBN(DirichletPriorFromBN&& from) noexcept;

    /// virtual copy constructor
    DirichletPriorFromBN* clone() const final;

    /// destructor
    virtual ~DirichletPriorFromBN();

    /// @}


    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    DirichletPriorFromBN& operator=(const DirichletPriorFromBN& from);

    /// move operator
    DirichletPriorFromBN& operator=(DirichletPriorFromBN&& from);

    /// @}


    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// returns the type of the prior
    PriorType getType() const final;

    /// indicates whether the prior is potentially informative
    /** Basically, only the NoPrior is uninformative. However, it may happen
     * that, under some circumstances, a prior, which is usually not equal
     * to the NoPrior, becomes equal to it (e.g., when the weight is equal
     * to zero). In this case, if the prior can detect this case, it shall
     * inform the classes that use it that it is temporarily uninformative.
     * These classes will then be able to speed-up their code by avoiding to
     * take into account the prior in their computations. */
    bool isInformative() const final;

    /// sets the weight of the a prior(kind of virtual sample size)
    void setWeight(double weight) final;

    /// adds the prior to a counting vector corresponding to the idset
    /** adds the prior to an already created counting vector defined over
     * the union of the variables on both the left and right hand side of the
     * conditioning bar of the idset.
     * @warning the method assumes that the size of the vector is exactly
     * the domain size of the joint variables set. */
    void addJointPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

    /** @brief adds the prior to a counting vector defined over the right
     * hand side of the idset
     *
     * @warning the method assumes that the size of the vector is exactly
     * the domain size of the joint RHS variables of the idset. */
    void addConditioningPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

    /// @}

    private:
    const BayesNet< GUM_SCALAR >* _prior_bn_;

    void _addCountsForJoint_(Instantiation&         Ijoint,
                             const Set< NodeId >&   joint,
                             std::vector< double >& counts);
  };
  /* namespace learning */

}   // namespace gum::learning

#include <agrum/BN/learning/priors/DirichletPriorFromBN_tpl.h>

#endif /* GUM_LEARNING_PRIOR_DIRICHLET_FROM_BN_H */
