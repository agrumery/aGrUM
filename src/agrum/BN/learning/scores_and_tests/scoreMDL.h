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
 * @brief the class for computing MDL scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LEARNING_SCORE_MDL_H
#define GUM_LEARNING_SCORE_MDL_H

#include <agrum/agrum.h>
#include <agrum/BN/learning/scores_and_tests/scoreBIC.h>


namespace gum {

  namespace learning {

    /** @class ScoreMDL
     * @brief the class for computing MDL scores
     * @headerfile scoreMDL.h <agrum/BN/learning/scores_and_tests/scoreMDL.h>
     * @ingroup learning_scores
     *
     * @warning If you pass a prior to the score, this one will be added
     * into the log-likelihood part of the score.
     */
    using ScoreMDL = ScoreBIC;


  } /* namespace learning */

} /* namespace gum */


#endif /* GUM_LEARNING_SCORE_MDL_H */
