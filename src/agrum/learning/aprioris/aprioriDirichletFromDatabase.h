
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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H
#define GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/scores_and_tests/recordCounter.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /** @class AprioriDirichletFromDatabase
     * @brief A dirichlet priori: computes its N'_ijk from a database
     * @headerfile aprioriDirichletFromDatabase.h <agrum/learning/database/aprioriDirichletFromDatabase.h>
     * @ingroup learning_apriori
     */
    template < template < typename > class ALLOC = std::allocator >
    class AprioriDirichletFromDatabase : public Apriori< ALLOC > {
      public:
      /// the type of the a priori
      using type = AprioriDirichletType;

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param learning_db the database from which learning is performed.
       * This is useful to get access to the random variables
       * @param apriori_parser the parser used to parse the apriori database
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in learning_db.
       * This enables estimating from a database in which variable A corresponds
       * to the 2nd column the parameters of a BN in which variable A has a
       * NodeId of 5. An empty nodeId2Columns bijection means that the mapping
       * is an identity, i.e., the value of a NodeId is equal to the index of
       * the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * RecordCounter.
       *
       * @throws DatabaseError The apriori database may differ from the learning
       * database, i.e., the apriori may have more nodes than the learning one.
       * However, a check is performed to ensure that the variables within the
       * apriori database that correspond to those in the learning database
       * (they have the same names) are exactly identical. If this is not the
       * case, then a DatabaseError exception is raised. */
      AprioriDirichletFromDatabase(
         const DatabaseTable< ALLOC >&        learning_db,
         const DBRowGeneratorParser< ALLOC >& apriori_parser,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      AprioriDirichletFromDatabase(
         const AprioriDirichletFromDatabase< ALLOC >& from);

      /// copy constructor with a given allocator
      AprioriDirichletFromDatabase(
         const AprioriDirichletFromDatabase< ALLOC >& from,
         const allocator_type&                        alloc);

      /// move constructor
      AprioriDirichletFromDatabase(AprioriDirichletFromDatabase< ALLOC >&& from);

      /// move constructor with a given allocator
      AprioriDirichletFromDatabase(AprioriDirichletFromDatabase< ALLOC >&& from,
                                   const allocator_type&                   alloc);

      /// virtual copy constructor
      virtual AprioriDirichletFromDatabase< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual AprioriDirichletFromDatabase< ALLOC >*
         clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~AprioriDirichletFromDatabase();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      AprioriDirichletFromDatabase< ALLOC >&
         operator=(const AprioriDirichletFromDatabase< ALLOC >& from);

      /// move operator
      AprioriDirichletFromDatabase< ALLOC >&
         operator=(AprioriDirichletFromDatabase< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

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

      /// sets the weight of the a priori (kind of effective sample size)
      virtual void setWeight(const double weight) final;

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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the record counter used to parse the apriori database
      RecordCounter< ALLOC > __counter;

      // the internal weight is equal to _weight / nb rows of apriori database
      // this internal weight is used to ensure that assigning a weight of 1
      // to the apriori is equivalent to adding just one row to the learning
      // database
      double __internal_weight;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriDirichletFromDatabase_tpl.h>

#endif /* GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H */
