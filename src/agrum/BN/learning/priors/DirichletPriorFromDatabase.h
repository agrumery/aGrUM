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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_DIRICHLET_FROM_DATABASE_H
#define GUM_LEARNING_PRIOR_DIRICHLET_FROM_DATABASE_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/stattests/recordCounter.h>
#include <agrum/BN/learning/priors/prior.h>

namespace gum {

  namespace learning {

    /** @class DirichletPriorFromDatabase
     * @brief A dirichlet priori: computes its N'_ijk from a database
     * @headerfile DirichletPriorFromDatabase.h <agrum/tools/database/DirichletPriorFromDatabase.h>
     * @ingroup learning_priors
     */
    class DirichletPriorFromDatabase: public Prior {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param learning_db the database from which learning is performed.
       * This is useful to get access to the random variables
       * @param prior_parser the parser used to parse the prior database
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in learning_db.
       * This enables estimating from a database in which variable A corresponds
       * to the 2nd column the parameters of a BN in which variable A has a
       * NodeId of 5. An empty nodeId2Columns bijection means that the mapping
       * is an identity, i.e., the value of a NodeId is equal to the index of
       * the column in the DatabaseTable.
       *
       * @throws DatabaseError The prior database may differ from the learning
       * database, i.e., the prior may have more nodes than the learning one.
       * However, a check is performed to ensure that the variables within the
       * prior database that correspond to those in the learning database
       * (they have the same names) are exactly identical. If this is not the
       * case, then a DatabaseError exception is raised. */
      DirichletPriorFromDatabase(const DatabaseTable&                    learning_db,
                                 const DBRowGeneratorParser&             prior_parser,
                                 const Bijection< NodeId, std::size_t >& nodeId2columns
                                 = Bijection< NodeId, std::size_t >());

      /// copy constructor
      DirichletPriorFromDatabase(const DirichletPriorFromDatabase& from);

      /// move constructor
      DirichletPriorFromDatabase(DirichletPriorFromDatabase&& from) noexcept;

      /// virtual copy constructor
      virtual DirichletPriorFromDatabase* clone() const;

      /// destructor
      virtual ~DirichletPriorFromDatabase();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DirichletPriorFromDatabase& operator=(const DirichletPriorFromDatabase& from);

      /// move operator
      DirichletPriorFromDatabase& operator=(DirichletPriorFromDatabase&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the type of the prior
      PriorType getType() const final;

      /// indicates whether the prior is potentially informative
      /** Basically, only the NoPrior is uninformative. However, it may happen
       * that, under some circumstances, an prior, which is usually not equal
       * to the NoPrior, becomes equal to it (e.g., when the weight is equal
       * to zero). In this case, if the prior can detect this case, it shall
       * inform the classes that use it that it is temporarily uninformative.
       * These classes will then be able to speed-up their code by avoiding to
       * take into account the prior in their computations. */
      bool isInformative() const final;

      /// sets the weight of the a prior(kind of effective sample size)
      void setWeight(double weight) final;

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
      void addConditioningPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the record counter used to parse the prior database
      RecordCounter _counter_;

      // the internal weight is equal to weight_ / nb rows of prior database
      // this internal weight is used to ensure that assigning a weight of 1
      // to the prior is equivalent to adding just one row to the learning
      // database
      double _internal_weight_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/priors/DirichletPriorFromDatabase_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_DIRICHLET_FROM_DATABASE_H */
