
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief the internal apriori for the K2 score = Laplace Apriori
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_K2_H
#define GUM_LEARNING_A_PRIORI_K2_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>

namespace gum {

  namespace learning {

    /** @class AprioriK2
     * @brief the internal apriori for the K2 score = Laplace Apriori
     * @headerfile aprioriK2.h <agrum/learning/database/aprioriK2.h>
     * @ingroup learning_apriori
     *
     * K2 is a BD score with a Laplace apriori (i.e., a smoothing of 1).
     *
     * It is important to note that, to be meaningful a structure + parameter
     * learning requires that the same aprioris are taken into account during
     * structure learning and parameter learning.
     */
    template < template < typename > class ALLOC = std::allocator >
    class AprioriK2 : public AprioriSmoothing< ALLOC > {
      public:
      /// the type of the a priori
      using type = AprioriSmoothingType;

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;


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
       * @param alloc the allocator used to allocate the structures within the
       * RecordCounter.*/
      AprioriK2(const DatabaseTable< ALLOC >& database,
                const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                   nodeId2columns =
                      Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                const allocator_type& alloc = allocator_type());

      /// copy constructor
      AprioriK2(const AprioriK2< ALLOC >& from);

      /// copy constructor with a given allocator
      AprioriK2(const AprioriK2< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      AprioriK2(AprioriK2< ALLOC >&& from);

      /// move constructor with a given allocator
      AprioriK2(AprioriK2< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual AprioriK2< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual AprioriK2< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~AprioriK2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      AprioriK2< ALLOC >& operator=(const AprioriK2< ALLOC >& from);

      /// move operator
      AprioriK2< ALLOC >& operator=(AprioriK2< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// dummy set weight function: in K2, weights are always equal to 1
      virtual void setWeight(const double weight) final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriK2_tpl.h>

#endif /* GUM_LEARNING_A_PRIORI_K2_H */
