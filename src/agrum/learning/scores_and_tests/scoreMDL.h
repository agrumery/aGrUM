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
 * @brief the class for computing MDL scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_MDL_H
#define GUM_LEARNING_SCORE_MDL_H

#include <agrum/agrum.h>
#include <agrum/learning/scores_and_tests/scoreBIC.h>


namespace gum {

  namespace learning {

    /** @class ScoreMDL
     * @brief the class for computing MDL scores
     * @headerfile scoreMDL.h <agrum/learning/scores_and_tests/scoreMDL.h>
     * @ingroup learning_scores
     *
     * @warning If you pass an apriori to the score, this one will be added
     * into the log-likelihood part of the score.
     */
    template < template < typename > class ALLOC = std::allocator >
    using ScoreMDL = ScoreBIC< ALLOC >;


  } /* namespace learning */

} /* namespace gum */


#endif /* GUM_LEARNING_SCORE_MDL_H */