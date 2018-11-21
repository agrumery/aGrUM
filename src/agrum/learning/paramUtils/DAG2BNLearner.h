/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DAG_2_BN_LEARNER_H
#define GUM_LEARNING_DAG_2_BN_LEARNER_H

#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>
#include <agrum/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class DAG2BNLearner
     * @brief A class that, given a structure and a parameter estimator returns
     * a full Bayes net
     * @headerfile DAG2BNLearner.h <agrum/learning/paramUtils/DAG2BNLearner.h>
     * @ingroup learning_param_utils
     */
    template < template < typename > class ALLOC = std::allocator >
    class DAG2BNLearner : private ALLOC< NodeId > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DAG2BNLearner(const allocator_type& alloc = allocator_type());

      /// copy constructor
      DAG2BNLearner(const DAG2BNLearner< ALLOC >& from);

      /// copy constructor with a given allocator
      DAG2BNLearner(const DAG2BNLearner< ALLOC >& from,
                     const allocator_type&          alloc);

      /// move constructor
      DAG2BNLearner(DAG2BNLearner< ALLOC >&& from);

      /// move constructor with a given allocator
      DAG2BNLearner(DAG2BNLearner< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual DAG2BNLearner< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual DAG2BNLearner< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~DAG2BNLearner();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DAG2BNLearner< ALLOC >& operator=(const DAG2BNLearner< ALLOC >& from);

      /// move operator
      DAG2BNLearner< ALLOC >& operator=(DAG2BNLearner< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// create a BN
      template < typename GUM_SCALAR = double >
      static BayesNet< GUM_SCALAR > createBN(ParamEstimator< ALLOC >& estimator,
                                             const DAG&                dag);

      /// returns the allocator used by the score
      allocator_type getAllocator() const;

      /// @}

      private:
      /// copy a potential into another whose variables' sequence differs
      /** The variables of both potential should be the same, only their
       * order differs */
      template < typename GUM_SCALAR = double >
      static void
        __probaVarReordering(gum::Potential< GUM_SCALAR >&       pot,
                             const gum::Potential< GUM_SCALAR >& other_pot);
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/paramUtils/DAG2BNLearner_tpl.h>

#endif /* GUM_LEARNING_DAG_2_BN_LEARNER_H */
