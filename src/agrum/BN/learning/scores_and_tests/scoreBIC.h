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
 * @brief the class for computing BIC scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LEARNING_SCORE_BIC_H
#define GUM_LEARNING_SCORE_BIC_H

#include <string>

#include <agrum/agrum.h>
#include <agrum/BN/learning/scores_and_tests/score.h>
#include <agrum/BN/learning/priors/noPrior.h>

namespace gum {

  namespace learning {

    /** @class ScoreBIC
     * @brief the class for computing BIC scores
     * @headerfile scoreBIC.h <agrum/BN/learning/scores_and_tests/scoreBIC.h>
     * @ingroup learning_scores
     *
     * @warning If you pass an prior to the score, this one will be added
     * into the log-likelihood part of the score.
     */
    class ScoreBIC: public Score {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of the score
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
      ScoreBIC(const DBRowGeneratorParser&                                 parser,
               const Prior&                                                prior,
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
      ScoreBIC(const DBRowGeneratorParser&             parser,
               const Prior&                            prior,
               const Bijection< NodeId, std::size_t >& nodeId2columns
               = Bijection< NodeId, std::size_t >());

      /// copy constructor
      ScoreBIC(const ScoreBIC& from);

      /// move constructor
      ScoreBIC(ScoreBIC&& from);

      /// virtual copy constructor
      virtual ScoreBIC* clone() const;

      /// destructor
      virtual ~ScoreBIC();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ScoreBIC& operator=(const ScoreBIC& from);

      /// move operator
      ScoreBIC& operator=(ScoreBIC&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// indicates whether the prior is compatible (meaningful) with the score
      /** The combination of some scores and priors can be meaningless. For
       * instance, adding a Dirichlet prior to the K2 score is not very
       * meaningful since K2 corresonds to a BD score with a 1-smoothing
       * prior.
       * aGrUM allows you to perform such combination, but you can check with
       * method isPriorCompatible () whether the result the score will give
       * you is meaningful or not.
       * @returns a non empty string if the prior is compatible with the
       * score.*/
      virtual std::string isPriorCompatible() const final;

      /// returns the internal prior of the score
      /** Some scores include an prior. For instance, the K2 score is a BD
       * score with a Laplace Prior ( smoothing(1) ). BDeu is a BD score with
       * a N'/(r_i * q_i) prior, where N' is an effective sample size and r_i
       * is the domain size of the target variable and q_i is the domain size of
       * the Cartesian product of its parents. The goal of the score's internal
       * prior classes is to enable to account for these priors outside the
       * score, e.g., when performing parameter estimation. It is important to
       * note that, to be meaningful, a structure + parameter learning requires
       * that the same priors are taken into account during structure learning
       * and parameter learning. */
      virtual const Prior& internalPrior() const final;

      /// returns the size of the database w.r.t. a given idset
      /** Usually, the size of the database should be a constant. But some
       * a priorviolate this condition. This is the case, for instance, of
       * the Laplace a priori: this one adds ones to each cells of the
       * contingency tables. As all those tables have not necessarily the same
       * size, the number of ones added differ from one contingency table
       * to the other. As a result, the size of the database, including the a
       * priori, depends on the variables of the contingency tables (the idset
       * passed in argument of this function). */
      double N(const IdCondSet& idset);

      /// @}


      /// indicates whether the prior is compatible (meaningful) with the score
      /** @returns a non empty string if the prior is compatible with the score.
       */
      static std::string isPriorCompatible(PriorType prior_type, double weight = 1.0f);

      /// indicates whether the prior is compatible (meaningful) with the score
      /** a non empty string if the prior is compatible with the score. */
      static std::string isPriorCompatible(const Prior& prior);


      protected:
      /// returns the score for a given IdCondSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double score_(const IdCondSet& idset) final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the internal prior of the score
      NoPrior _internal_prior_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/scores_and_tests/scoreBIC_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_SCORE_BIC_H */
