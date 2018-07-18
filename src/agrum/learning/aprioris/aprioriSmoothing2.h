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
 * @brief the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_SMOOTHING2_H
#define GUM_LEARNING_A_PRIORI_SMOOTHING2_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/aprioris/apriori2.h>

namespace gum {

  namespace learning {

    /** @class AprioriSmoothing2
     * @brief the smooth a priori: adds a weight w to all the countings
     * @headerfile aprioriSmoothing2.h <agrum/learning/database/aprioriSmoothing2.h>
     * @ingroup learning_apriori
     */
    template < template < typename > class ALLOC = std::allocator >
    class AprioriSmoothing2 : public Apriori2< ALLOC > {
      public:

      /// the type of the a priori
      using type = AprioriSmoothingType;

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<NodeId>;
      
      

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
      AprioriSmoothing2( const DatabaseTable<ALLOC>& database,
                         const Bijection<NodeId,std::size_t,
                         ALLOC<std::size_t>>& nodeId2columns =
                         Bijection<NodeId,std::size_t,ALLOC<std::size_t>> (),
                         const allocator_type& alloc = allocator_type ());

      /// copy constructor
      AprioriSmoothing2(const AprioriSmoothing2< ALLOC >& from);

      /// copy constructor with a given allocator
      AprioriSmoothing2(const AprioriSmoothing2< ALLOC >& from,
                        const allocator_type& alloc);

      /// move constructor
      AprioriSmoothing2(AprioriSmoothing2< ALLOC >&& from);

      /// move constructor with a given allocator
      AprioriSmoothing2(AprioriSmoothing2< ALLOC >&& from,
                        const allocator_type& alloc);

      /// virtual copy constructor
      virtual AprioriSmoothing2<ALLOC>* clone () const;

      /// virtual copy constructor with a given allocator
      virtual AprioriSmoothing2<ALLOC>*
      clone (const allocator_type& alloc) const;

      /// destructor
      virtual ~AprioriSmoothing2();

      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      AprioriSmoothing2< ALLOC >&
      operator=(const AprioriSmoothing2< ALLOC >& from);

      /// move operator
      AprioriSmoothing2< ALLOC >&
      operator=(AprioriSmoothing2< ALLOC >&& from );

      /// @}
      
      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{
      
      /// indicates whether an apriori is of a certain type
      virtual bool isOfType(const std::string& type) final;

      /// returns the type of the apriori
      virtual const std::string& getType() const final;

      /// returns the apriori vector all the variables in the idset
      /** @returns the apriori vector over the union of the variables on the
       * left and right hand side of the conditioning bar */
      virtual std::vector< double, ALLOC<double> >
      getAllApriori( const IdSet2<ALLOC>& idset ) final;

      /// returns the apriori vector over only the conditioning set of an idset
      virtual std::vector< double, ALLOC<double> >
      getConditioningApriori( const IdSet2<ALLOC>& idset ) final;


      /// @}
     
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriSmoothing2_tpl.h>

#endif /* GUM_LEARNING_A_PRIORI_SMOOTHING2_H */
