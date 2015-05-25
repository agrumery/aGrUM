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
 * @brief the internal apriori for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORE_INTERNAL_BDEU_APRIORI_H
#define GUM_LEARNING_SCORE_INTERNAL_BDEU_APRIORI_H

#include <vector>

#include <agrum/config.h>
#include <agrum/learning/scores_and_tests/scoreInternalApriori.h>

namespace gum {

  namespace learning {

    /** @class ScoreInternalBDeuApriori
     * @brief the internal apriori for the BDeu score (N' / (r_i * q_i)
     * @ingroup learning_group
     *
     * Some scores include an apriori. For instance, the K2 score is a BD score
     * with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with a
     * N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i is the
     * domain size of the target variable and q_i is the domain size of the
     * Cartesian product of its parents. The goal of the score's internal apriori
     * classes is to enable to account for these aprioris outside the score, e.g.,
     * when performing parameter estimation. It is important to note that, to be
     * meaningfull a structure + parameter learning requires that the same
     * aprioris are taken into account during structure learning and parameter
     * learning.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class ScoreInternalBDeuApriori
        : public ScoreInternalApriori<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoreInternalBDeuApriori();

      /// virtual copy constructor
      virtual ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> *
      copyFactory() const final;

      /// copy constructor
      ScoreInternalBDeuApriori(
          const ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      ScoreInternalBDeuApriori(
          ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> &&from);

      /// destructor
      virtual ~ScoreInternalBDeuApriori();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert the internal score apriori into a set of countings
      virtual void insertScoreApriori(
          const std::vector<unsigned int> &modalities,
          std::vector<std::vector<float, CountAlloc>> &counts,
          const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                      unsigned int> *> &target_nodesets,
          const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                      unsigned int> *> &conditioning_nodesets) final;

      /// sets the effective sample size of the internal apriori
      void setEffectiveSampleSize(float ess);

      /// @}

      private:
      /// the effective sample size of the internal apriori
      float __ess{1.0f};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/scores_and_tests/scoreInternalBDeuApriori.tcc>

#endif /* GUM_LEARNING_SCORE_INTERNAL_BDEU_APRIORI_H */
