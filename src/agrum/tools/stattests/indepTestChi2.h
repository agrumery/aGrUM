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
 * @brief the class for computing Chi2 scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LEARNING_INDEP_TEST_CHI2_H
#define GUM_LEARNING_INDEP_TEST_CHI2_H

#include <vector>

#include <agrum/tools/core/math/chi2.h>
#include <agrum/tools/stattests/independenceTest.h>
#include <agrum/BN/learning/priors/noPrior.h>

namespace gum {

  namespace learning {

    /** @class IndepTestChi2
     * @brief the class for computing Chi2 independence test scores
     * @headerfile indepTestChi2.h <agrum/BN/learning/scores_and_tests/indepTestChi2.h>
     * @ingroup learning_scores
     */
    class IndepTestChi2: public IndependenceTest {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation
       * of the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database.
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      IndepTestChi2(const DBRowGeneratorParser&                                 parser,
                    const Prior&                                                external_prior,
                    const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                    const Bijection< NodeId, std::size_t >&                     nodeId2columns
                    = Bijection< NodeId, std::size_t >());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of the score
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      IndepTestChi2(const DBRowGeneratorParser&             parser,
                    const Prior&                            prior,
                    const Bijection< NodeId, std::size_t >& nodeId2columns
                    = Bijection< NodeId, std::size_t >());

      /// copy constructor
      IndepTestChi2(const IndepTestChi2& from);

      /// move constructor
      IndepTestChi2(IndepTestChi2&& from);

      /// virtual copy constructor
      virtual IndepTestChi2* clone() const;

      /// destructor
      virtual ~IndepTestChi2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      IndepTestChi2& operator=(const IndepTestChi2& from);

      /// move operator
      IndepTestChi2& operator=(IndepTestChi2&& from);

      /// @}

      // ##########################################################################
      /// @name Statistics
      // ##########################################################################

      /// @{

      /// get the pair <chi2 statistic,pvalue> for a test var1 indep var2 given
      /// rhs_ids
      std::pair< double, double >
         statistics(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids = {});

      /// @}


      protected:
      /// returns the score for a given IdCondSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double score_(const IdCondSet& idset) final;

      /// compute the pair <chi2 statistic,pvalue>
      std::pair< double, double > statistics_(const IdCondSet& idset);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the domain sizes of the variables
      std::vector< std::size_t > _domain_sizes_;

      /// a chi2 distribution for computing critical values
      Chi2 _chi2_;

      /// an empty conditioning set
      const std::vector< Idx > _empty_set_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/stattests/indepTestChi2_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_INDEP_TEST_CHI2_H */
