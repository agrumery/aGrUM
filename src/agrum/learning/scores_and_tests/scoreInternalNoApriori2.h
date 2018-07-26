/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief the internal apriori for the scores without apriori (e.g., BD)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORE_INTERNAL_NO_APRIORI2_H
#define GUM_LEARNING_SCORE_INTERNAL_NO_APRIORI2_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/scores_and_tests/scoreInternalApriori2.h>

namespace gum {

  namespace learning {

    /** @class ScoreInternalNoApriori2
     * @brief the internal apriori for the scores without apriori (e.g., BD)
     * @headerfile scoreInternalNoApriori2.h <agrum/learning/scores_and_tests/scoreInternalNoApriori2.h>
     * @ingroup learning_scores
     *
     * Some scores include an apriori. For instance, the K2 score is a BD score
     * with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with a
     * N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i is
     * the domain size of the target variable and q_i is the domain size of the
     * Cartesian product of its parents. The goal of the score's internal
     * apriori classes is to enable to account for these aprioris outside the
     * score, e.g., when performing parameter estimation. It is important to
     * note that, to be meaningful, a structure + parameter learning requires
     * that the same aprioris are taken into account during structure learning
     * and parameter learning.
     */
    template < template < typename > class ALLOC = std::allocator >
    class ScoreInternalNoApriori2 : public ScoreInternalApriori2< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoreInternalNoApriori2(const DatabaseTable< ALLOC >& database,
               const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 nodeId2columns =
                   Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
               const allocator_type& alloc = allocator_type());

      /// copy constructor
      ScoreInternalNoApriori2(const ScoreInternalNoApriori2< ALLOC >& from);

      /// copy constructor with a given allocator
      ScoreInternalNoApriori2(const ScoreInternalNoApriori2< ALLOC >& from,
                            const allocator_type&                 alloc);

      /// move constructor
      ScoreInternalNoApriori2(ScoreInternalNoApriori2< ALLOC >&& from);

      /// move constructor with a given allocator
      ScoreInternalNoApriori2(ScoreInternalNoApriori2< ALLOC >&& from,
                            const allocator_type&            alloc);

      /// virtual copy constructor
      virtual ScoreInternalNoApriori2< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual ScoreInternalNoApriori2< ALLOC >*
        clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~ScoreInternalNoApriori2();

      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ScoreInternalNoApriori2< ALLOC >&
        operator=(const ScoreInternalNoApriori2< ALLOC >& from);

      /// move operator
      ScoreInternalNoApriori2< ALLOC >&
        operator=(ScoreInternalNoApriori2< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// adds the apriori to a counting vector corresponding to the idset
      /** adds the apriori to an already created counting vector defined over
       * the union of the variables on both the left and right hand side of the
       * conditioning bar of the idset.
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint variables set. */
      virtual void 
      addAllApriori(const IdSet2< ALLOC >& idset,
                    std::vector< double, ALLOC< double > >& counts ) final;

      /** @brief adds the apriori to a counting vectordefined over the right
       * hand side of the idset
       *
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint RHS variables of the idset. */
      virtual void 
      addConditioningApriori(const IdSet2< ALLOC >& idset,
                             std::vector< double, ALLOC< double > >& counts )
        final;

      /// indicates whether the apriori is potentially informative
      virtual bool isInformative() const final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */


extern template class gum::learning::ScoreInternalNoApriori2<>;


/// include the template implementation
#include <agrum/learning/scores_and_tests/scoreInternalNoApriori2_tpl.h>

#endif /* GUM_LEARNING_SCORE_INTERNAL_NO_APRIORI2_H */
