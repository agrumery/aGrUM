/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


// to help IDE parser
#include <agrum/learning/BNLearner.h>

#include <agrum/learning/BNLearnerListener.h>


namespace gum {


  namespace learning {


    /// learn a Bayes Net from a file
    template <typename GUM_SCALAR>
    BayesNet<GUM_SCALAR> BNLearner::learnBN () {
      // create the score, the apriori and the estimator
      __createApriori ();
      __createScore ();
      __createParamEstimator ();
     
      return
        DAG2BNLearner::createBN
        <ParamEstimator<>,
         DBRowTranslatorSetDynamic<CellTranslatorUniversal>,
         GUM_SCALAR> ( *__param_estimator,
                       __learnDAG (),
                       __score_database.names (),
                       __score_database.modalities (),
                       __score_database.rawTranslators () );
    }


  } /* namespace learning */


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

