
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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_NO_APRIORI_H
#define GUM_LEARNING_A_PRIORI_NO_APRIORI_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /** @class AprioriNoApriori
     * @brief the no a priori class: corresponds to 0 weight-sample
     * @headerfile aprioriNoApriori.h <agrum/learning/database/aprioriNoApriori.h>
     * @ingroup learning_apriori
     */
    template < template < typename > class ALLOC = std::allocator >
    class AprioriNoApriori : public Apriori< ALLOC > {
      public:
      /// the type of the a priori
      using type = AprioriNoAprioriType;

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
      AprioriNoApriori(
         const DatabaseTable< ALLOC >& database,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      AprioriNoApriori(const AprioriNoApriori< ALLOC >& from);

      /// copy constructor with a given allocator
      AprioriNoApriori(const AprioriNoApriori< ALLOC >& from,
                       const allocator_type&            alloc);

      /// move constructor
      AprioriNoApriori(AprioriNoApriori< ALLOC >&& from);

      /// move constructor with a given allocator
      AprioriNoApriori(AprioriNoApriori< ALLOC >&& from,
                       const allocator_type&       alloc);

      /// virtual copy constructor
      virtual AprioriNoApriori< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual AprioriNoApriori< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~AprioriNoApriori();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      AprioriNoApriori< ALLOC >& operator=(const AprioriNoApriori< ALLOC >& from);

      /// move operator
      AprioriNoApriori< ALLOC >& operator=(AprioriNoApriori< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the weight of the a priori (kind of effective sample size)
      virtual void setWeight(const double weight) final;

      /// indicates whether an apriori is of a certain type
      virtual bool isOfType(const std::string& type) final;

      /// returns the type of the apriori
      virtual const std::string& getType() const final;

      /// indicates whether the apriori is potentially informative
      /** Basically, only the NoApriori is uninformative. However, it may happen
       * that, under some circonstances, an apriori, which is usually not equal
       * to the NoApriori, becomes equal to it (e.g., when the weight is equal
       * to zero). In this case, if the apriori can detect this case, it shall
       * inform the classes that use it that it is temporarily uninformative.
       * These classes will then be able to speed-up their code by avoiding to
       * take into account the apriori in their computations. */
      virtual bool isInformative() const final;

      /// adds the apriori to a counting vector corresponding to the idset
      /** adds the apriori to an already created counting vector defined over
       * the union of the variables on both the left and right hand side of the
       * conditioning bar of the idset.
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint variables set. */
      virtual void
         addAllApriori(const IdSet< ALLOC >&                   idset,
                       std::vector< double, ALLOC< double > >& counts) final;

      /** @brief adds the apriori to a counting vectordefined over the right
       * hand side of the idset
       *
       * @warning the method assumes that the size of the vector is exactly
       * the domain size of the joint RHS variables of the idset. */
      virtual void addConditioningApriori(
         const IdSet< ALLOC >&                   idset,
         std::vector< double, ALLOC< double > >& counts) final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriNoApriori_tpl.h>

#endif /* GUM_LEARNING_A_PRIORI_NO_APRIORI_H */
