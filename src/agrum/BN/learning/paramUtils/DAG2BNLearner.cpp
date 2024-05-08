/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/paramUtils/DAG2BNLearner_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DAG2BNLearner::DAG2BNLearner() { GUM_CONSTRUCTOR(DAG2BNLearner); }

    /// copy constructor
    DAG2BNLearner::DAG2BNLearner(const DAG2BNLearner& from) : ApproximationScheme(from) {
      GUM_CONS_CPY(DAG2BNLearner);
    }

    /// move constructor
    DAG2BNLearner::DAG2BNLearner(DAG2BNLearner&& from) : ApproximationScheme(std::move(from)) {
      GUM_CONS_MOV(DAG2BNLearner);
    }

    /// virtual copy constructor
    DAG2BNLearner* DAG2BNLearner::clone() const { return new DAG2BNLearner(*this); }

    /// destructor
    DAG2BNLearner::~DAG2BNLearner() { GUM_DESTRUCTOR(DAG2BNLearner); }

    /// copy operator
    DAG2BNLearner& DAG2BNLearner::operator=(const DAG2BNLearner& from) {
      ApproximationScheme::operator=(from);
      return *this;
    }

    /// move operator
    DAG2BNLearner& DAG2BNLearner::operator=(DAG2BNLearner&& from) {
      ApproximationScheme::operator=(std::move(from));
      return *this;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
