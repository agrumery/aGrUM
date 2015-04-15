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
 * @brief the class for computing Bayesian Dirichlet (BD) log2 scores
 *
 * @warning This class computes the "general" log2 (BD score). If you wish to
 * reduce the number of hyperparameters, try using ScoreBDeu or ScoreK2.
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores you need in one pass. To do so, use
 * the appropriate addNodeSet methods. These will compute everything you need.
 * Use methods score to retrieve the scores computed. See the Score class for
 * details.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_BD_H
#define GUM_LEARNING_SCORE_BD_H

#include <agrum/core/math/gammaLog2.h>
#include <agrum/learning/scores_and_tests/score.h>
#include <agrum/learning/scores_and_tests/scoreInternalNoApriori.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                          SCORE BD CLASS                           === */
    /* ========================================================================= */
    /** @class ScoreBD
     * @ingroup learning_group
     * @brief The class for computing Bayesian Dirichlet (BD) log2 scores
     *
     * @warning This class computes the "general" log2 (BD score). If you wish to
     * reduce the number of hyperparameters, try using ScoreBDeu or ScoreK2.
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so, use
     * the appropriate addNodeSet methods. These will compute everything you need.
     * Use methods score to retrieve the scores computed. See the Score class for
     * details.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class ScoreBD : public Score<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      ScoreBD(const RowFilter &filter,
              const std::vector<unsigned int> &var_modalities,
              Apriori<IdSetAlloc, CountAlloc> &apriori);

      /// copy constructor
      ScoreBD(const ScoreBD<IdSetAlloc, CountAlloc> &);

      /// move constructor
      ScoreBD(ScoreBD<IdSetAlloc, CountAlloc> &&);

      /// virtual copy factory
      virtual ScoreBD<IdSetAlloc, CountAlloc> *copyFactory() const;

      /// destructor
      virtual ~ScoreBD();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the log2(BDeu score) corresponding to a given nodeset
      float score(unsigned int nodeset_index);

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** @returns true if the apriori is compatible with the score.
       * @throws IncompatibleScoreApriori is raised if the apriori is known to
       * be incompatible with the score. Such a case arises because the score
       * needs an apriori and none is passed in argument. aGrUM will nevertheless
       * allow you to use this apriori with the score, but the learning will
       * certainly fail.
       * @throws PossiblyIncompatibleScoreApriori is raised if the apriori with
       * its current weight is compatible with the score but this may not be the
       * case any more if you change the weight (e.g., if you put the weight of
       * the apriori to 0, the latter will inevitably becomes incompatible).
       * In such a case, you should not modify the weight. aGrUM will nevertheless
       * allow you to do so but the result of learning may become meaningless.
       * @throws InvalidArgument is raised if the apriori is not handled yet by
       * method isAprioriCompatible (the method needs be updated to take it into
       * account). */
      virtual bool isAprioriCompatible() const final;

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** @returns true if the apriori is compatible with the score.
       * @throws IncompatibleScoreApriori is raised if the apriori is known to
       * be incompatible with the score. Such a case arises because the score
       * needs an apriori and none is passed in argument. aGrUM will nevertheless
       * allow you to use this apriori with the score, but the learning will
       * certainly fail.
       * @throws PossiblyIncompatibleScoreApriori is raised if the apriori with
       * its current weight is compatible with the score but this may not be the
       * case any more if you change the weight (e.g., if you put the weight of
       * the apriori to 0, the latter will inevitably becomes incompatible).
       * In such a case, you should not modify the weight. aGrUM will nevertheless
       * allow you to do so but the result of learning may become meaningless.
       * @throws InvalidArgument is raised if the apriori is not handled yet by
       * method isAprioriCompatible (the method needs be updated to take it into
       * account). */
      static bool isAprioriCompatible(const std::string &apriori_type,
                                      float weight = 1.0f);

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** @returns true if the apriori is compatible with the score.
       * @throws IncompatibleScoreApriori is raised if the apriori is known to
       * be incompatible with the score. Such a case arises because the score
       * needs an apriori and none is passed in argument. aGrUM will nevertheless
       * allow you to use this apriori with the score, but the learning will
       * certainly fail.
       * @throws PossiblyIncompatibleScoreApriori is raised if the apriori with
       * its current weight is compatible with the score but this may not be the
       * case any more if you change the weight (e.g., if you put the weight of
       * the apriori to 0, the latter will inevitably becomes incompatible).
       * In such a case, you should not modify the weight. aGrUM will nevertheless
       * allow you to do so but the result of learning may become meaningless.
       * @throws InvalidArgument is raised if the apriori is not handled yet by
       * method isAprioriCompatible (the method needs be updated to take it into
       * account). */
      static bool
      isAprioriCompatible(const Apriori<IdSetAlloc, CountAlloc> &apriori);

      /// returns the internal apriori of the score
      /** Some scores include an apriori. For instance, the K2 score is a BD score
       * with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with a
       * N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i is
       * the domain size of the target variable and q_i is the domain size of the
       * Cartesian product of its parents. The goal of the score's internal apriori
       * classes is to enable to account for these aprioris outside the score,
       * e.g., when performing parameter estimation. It is important to note that,
       * to be meaningfull a structure + parameter learning requires that the same
       * aprioris are taken into account during structure learning and parameter
       * learning. */
      virtual const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
      internalApriori() const noexcept final;

      /// @}

      private:
      /// the log(gamma (n)) function: generalizes log((n-1)!)
      GammaLog2 __gammalog2;

      /// the internal apriori of the score
      ScoreInternalNoApriori<IdSetAlloc, CountAlloc> __internal_apriori;

      /// an empty vector
      const std::vector<unsigned int> __empty_vect;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the template implementation
#include <agrum/learning/scores_and_tests/scoreBD.tcc>

#endif /* GUM_LEARNING_SCORE_BDeu_H */
