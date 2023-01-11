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
 * @brief the smooth a priori: adds a weight w to all the counts
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_SMOOTHING_H
#define GUM_LEARNING_PRIOR_SMOOTHING_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/learning/priors/prior.h>

namespace gum {

  namespace learning {

    /** @class SmoothingPrior
     * @brief the smooth a priori: adds a weight w to all the counts
     * @headerfile smoothingPrior.h <agrum/tools/database/smoothingPrior.h>
     * @ingroup learning_priors
     */
    class SmoothingPrior: public Prior {
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
      explicit SmoothingPrior(const DatabaseTable&                    database,
                              const Bijection< NodeId, std::size_t >& nodeId2columns
                              = Bijection< NodeId, std::size_t >());

      /// copy constructor
      SmoothingPrior(const SmoothingPrior& from);

      /// move constructor
      SmoothingPrior(SmoothingPrior&& from) noexcept;

      /// virtual copy constructor
      SmoothingPrior* clone() const override;

      /// destructor
      virtual ~SmoothingPrior();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      SmoothingPrior& operator=(const SmoothingPrior& from);

      /// move operator
      SmoothingPrior& operator=(SmoothingPrior&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

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
      virtual bool isInformative() const final;

      /// adds the prior to a counting vector corresponding to the idset
      /** adds the prior to an already created counting vector defined over
       * the union of the variables on both the left and right hand side of the
       * conditioning bar of the idset.
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint variables set. */
      virtual void addJointPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

      /** @brief adds the prior to a counting vectordefined over the right
       * hand side of the idset
       *
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint RHS variables of the idset. */
      virtual void addConditioningPseudoCount(const IdCondSet&       idset,
                                              std::vector< double >& counts) final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/priors/smoothingPrior_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_SMOOTHING_H */
