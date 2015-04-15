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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DAG_2_BN_LEARNER_H
#define GUM_LEARNING_DAG_2_BN_LEARNER_H

#include <vector>

#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                          BN CREATOR CLASS                         === */
    /* ========================================================================= */
    /** @class DAG2BNLearner
     * @brief A class that, given a structure and a parameter estimator returns
     * a full Bayes net
     * @ingroup learning_group
     */
    class DAG2BNLearner {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DAG2BNLearner();

      /// copy constructor
      DAG2BNLearner(const DAG2BNLearner &from);

      /// move constructor
      DAG2BNLearner(DAG2BNLearner &&from);

      /// destructor
      ~DAG2BNLearner();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// create a BN
      template <typename GUM_SCALAR = float, typename PARAM_ESTIMATOR,
                typename CELL_TRANSLATORS>
      static BayesNet<GUM_SCALAR> createBN(PARAM_ESTIMATOR &estimator,
                                           const DAG &dag,
                                           const std::vector<std::string> &names,
                                           const std::vector<unsigned int> &modal,
                                           const CELL_TRANSLATORS &translator);

      /// @}

      private:
      /// copy a potential into another whose variables' sequence differs
      /** The variables of both potential should be the same, only their
       * order differs */
      template <typename GUM_SCALAR = float>
      static void __probaVarReordering(gum::Potential<GUM_SCALAR> &pot,
                                       const gum::Potential<float> &other_pot);
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/paramUtils/DAG2BNLearner.tcc>

#endif /* GUM_LEARNING_DAG_2_BN_LEARNER_H */
