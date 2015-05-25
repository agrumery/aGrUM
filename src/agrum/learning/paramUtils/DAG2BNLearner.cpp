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

#include <agrum/learning/paramUtils/DAG2BNLearner.h>

namespace gum {

  namespace learning {

    /// default constructor
    DAG2BNLearner::DAG2BNLearner() { GUM_CONSTRUCTOR(learning::DAG2BNLearner); }

    /// copy constructor
    DAG2BNLearner::DAG2BNLearner(const DAG2BNLearner &from) {
      GUM_CONS_CPY(learning::DAG2BNLearner);
    }

    /// move constructor
    DAG2BNLearner::DAG2BNLearner(DAG2BNLearner &&from) {
      GUM_CONS_MOV(learning::DAG2BNLearner);
    }

    /// destructor
    DAG2BNLearner::~DAG2BNLearner() { GUM_DESTRUCTOR(learning::DAG2BNLearner); }

  } /* namespace learning */

} /* namespace gum */
