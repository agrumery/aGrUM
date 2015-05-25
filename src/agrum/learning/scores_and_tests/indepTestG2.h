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
 * @brief the class for computing G2 scores
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the independence tests you need in one pass.
 * To do so, use the appropriate addNodeSet methods. These will compute
 * everything you need. Use method score to retrieve the scores related to
 * the independence test that were computed. See the IndependenceTest class for
 * details.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_INDEP_TEST_G2_H
#define GUM_LEARNING_INDEP_TEST_G2_H

#include <agrum/core/math/chi2.h>
#include <agrum/learning/scores_and_tests/independenceTest.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                       INDEP TEST G2 CLASS                         === */
    /* ========================================================================= */
    /** @class ScoreG2
     * @brief the class for computing G2 independence test scores
     * @ingroup learning_group
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the independence tests you need in one pass.
     * To do so, use the appropriate addNodeSet methods. These will compute
     * everything you need. Use method score to retrieve the scores related to
     * the independence test that were computed. See the IndependenceTest class for
     * details.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class IndepTestG2 : public IndependenceTest<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      IndepTestG2(const RowFilter &filter,
                  const std::vector<unsigned int> &var_modalities);

      /// destructor
      ~IndepTestG2();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the score corresponding to a given nodeset
      /** This method computes
       * sum_X sum_Y sum_Z #XYZ * log ( ( #XYZ * #Z ) / ( #XZ * #YZ ) ),
       * where #XYZ, #XZ, #YZ, #Z correspond to the number
       * of occurences of (X,Y,Z), (X,Z), (Y,Z) and Z respectively in the
       * database. Then, it computes the critical value alpha for the chi2 test
       * and returns ( #sum - alpha ) / alpha, where #sum corresponds to the
       * summations mentioned above. Therefore, any positive result should
       * reflect a dependence whereas negative results should reflect
       * independences. */
      float score(unsigned int nodeset_index);

      /// @}

      private:
      /// a chi2 distribution for computing critical values
      Chi2 __chi2;

      /// an empty vector of ids
      const std::vector<unsigned int, IdSetAlloc> __empty_set;
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/scores_and_tests/indepTestG2.tcc>

#endif /* GUM_LEARNING_INDEP_TEST_G2_H */
