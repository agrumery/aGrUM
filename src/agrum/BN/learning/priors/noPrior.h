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
 * @brief the no a priorclass: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_NO_prior_H
#define GUM_LEARNING_PRIOR_NO_prior_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/learning/priors/prior.h>

namespace gum {

  namespace learning {

    /** @class NoPrior
     * @brief the no a priorclass: corresponds to 0 weight-sample
     * @headerfile noPrior.h <agrum/BN/learning/priors/noPrior.h>
     * @ingroup learning_priors
     */
    class NoPrior: public Prior {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param database the database from which learning is performed. This is
       * useful to get access to the random variables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable.
       * This enables estimating from a database in which variable A corresponds
       * to the 2nd column the parameters of a BN in which variable A has a
       * NodeId of 5. An empty nodeId2Columns bijection means that the mapping
       * is an identity, i.e., the value of a NodeId is equal to the index of
       * the column in the DatabaseTable.
       */
      NoPrior(const DatabaseTable&                    database,
              const Bijection< NodeId, std::size_t >& nodeId2columns
              = Bijection< NodeId, std::size_t >());

      /// copy constructor
      NoPrior(const NoPrior& from);

      /// move constructor
      NoPrior(NoPrior&& from);

      /// virtual copy constructor
      virtual NoPrior* clone() const;

      /// destructor
      virtual ~NoPrior();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      NoPrior& operator=(const NoPrior& from);

      /// move operator
      NoPrior& operator=(NoPrior&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the weight of the a prior(kind of effective sample size)
      void setWeight(const double weight) final;

      /// returns the type of the prior
      PriorType getType() const final;

      /// indicates whether the prior is potentially informative
      /** Basically, only the NoPrior is uninformative. However, it may happen
       * that, under some circonstances, an prior, which is usually not equal
       * to the NoPrior, becomes equal to it (e.g., when the weight is equal
       * to zero). In this case, if the prior can detect this case, it shall
       * inform the classes that use it that it is temporarily uninformative.
       * These classes will then be able to speed-up their code by avoiding to
       * take into account the prior in their computations. */
      bool isInformative() const final;

      /// adds the prior to a counting vector corresponding to the idset
      /** adds the prior to an already created counting vector defined over
       * the union of the variables on both the left and right hand side of the
       * conditioning bar of the idset.
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint variables set. */
      void addJointPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

      /** @brief adds the prior to a counting vectordefined over the right
       * hand side of the idset
       *
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint RHS variables of the idset. */
      void addConditioningPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/priors/noPrior_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_NO_prior_H */
