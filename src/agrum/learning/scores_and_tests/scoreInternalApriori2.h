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
 * @brief the base class for all the score's internal aprioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORE_INTERNAL_APRIORI2_H
#define GUM_LEARNING_SCORE_INTERNAL_APRIORI2_H

#include <agrum/agrum.h>
#include <vector>
#include <agrum/core/bijection.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/scores_and_tests/idSet2.h>

namespace gum {

  namespace learning {

    /** @class ScoreInternalApriori
     * @brief the base class for all the score's internal aprioris
     * @headerfile scoreInternalApriori2.h <agrum/learning/scores_and_tests/scoreInternalApriori2.h>
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
    class ScoreInternalApriori2 : private ALLOC< NodeId > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoreInternalApriori2(
        const DatabaseTable< ALLOC >& database,
        const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
          nodeId2columns =
            Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
        const allocator_type& alloc = allocator_type());

      /// virtual copy constructor
      virtual ScoreInternalApriori2< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual ScoreInternalApriori2< ALLOC >*
        clone(const allocator_type& alloc) const = 0;

      /// destructor
      virtual ~ScoreInternalApriori2();

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
        addAllApriori(const IdSet2< ALLOC >&                  idset,
                      std::vector< double, ALLOC< double > >& counts) = 0;

      /** @brief adds the apriori to a counting vectordefined over the right
       * hand side of the idset
       *
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint RHS variables of the idset. */
      virtual void
        addConditioningApriori(const IdSet2< ALLOC >&                  idset,
                               std::vector< double, ALLOC< double > >& counts) = 0;

      /// indicates whether the apriori is potentially informative
      /** Basically, only the NoApriori is uninformative. However, it may happen
       * that, under some circonstances, an apriori, which is usually not equal
       * to the NoApriori, becomes equal to it. In this case, if the apriori can
       * detect this case, it shall informs the classes that use it that it is
       * temporarily uninformative. These classes will then be able to speed-up
       * their code by avoiding to take into account the apriori in their
       * computations. */
      virtual bool isInformative() const;

      /// returns the allocator used by the internal apriori
      allocator_type getAllocator() const;

      /// @}

      protected:
      /// a reference to the database in order to have access to its variables
      const DatabaseTable< ALLOC >* _database;

      /** @brief a mapping from the NodeIds of the variables to the indices of
       * the columns in the database */
      Bijection< NodeId, std::size_t, ALLOC< std::size_t > > _nodeId2columns;


      /// copy constructor
      ScoreInternalApriori2(const ScoreInternalApriori2< ALLOC >& from);

      /// copy constructor with a given allocator
      ScoreInternalApriori2(const ScoreInternalApriori2< ALLOC >& from,
                            const allocator_type&                 alloc);

      /// move constructor
      ScoreInternalApriori2(ScoreInternalApriori2< ALLOC >&& from);

      /// move constructor with a given allocator
      ScoreInternalApriori2(ScoreInternalApriori2< ALLOC >&& from,
                            const allocator_type&            alloc);

      /// copy operator
      ScoreInternalApriori2< ALLOC >&
        operator=(const ScoreInternalApriori2< ALLOC >& from);

      /// move operator
      ScoreInternalApriori2< ALLOC >&
        operator=(ScoreInternalApriori2< ALLOC >&& from);
    };

  } /* namespace learning */

} /* namespace gum */


extern template class gum::learning::ScoreInternalApriori2<>;


/// include the template implementation
#include <agrum/learning/scores_and_tests/scoreInternalApriori2_tpl.h>

#endif /* GUM_LEARNING_SCORE_INTERNAL_APRIORI2_H */
