/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (c) 2012  Pierre-Henri WUILLEMIN(_at_LIP6)
<pierre-henri.wuillemin@lip6.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
s
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <agrum/BN/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {
  namespace learning {
    BNLearnerListener::BNLearnerListener(genericBNLearner* bnl, ApproximationScheme& sch) :
        ApproximationSchemeListener(sch), _bnlearner_(bnl) {
      bnl->setCurrentApproximationScheme(&sch);
      GUM_CONSTRUCTOR(BNLearnerListener);
    }

    BNLearnerListener::BNLearnerListener(const BNLearnerListener& other) :
        ApproximationSchemeListener(other) {
      GUM_CONS_CPY(BNLearnerListener);
      GUM_ERROR(OperationNotAllowed, "No copy constructor for BNLearnerListener")
    }

    BNLearnerListener::~BNLearnerListener() {
      GUM_DESTRUCTOR(BNLearnerListener);
      ;
    }

    BNLearnerListener& BNLearnerListener::operator=(const BNLearnerListener& other) {
      GUM_CONS_CPY(BNLearnerListener);
      GUM_ERROR(OperationNotAllowed, "No copy constructor for BNLearnerListener")
    }

    void
       BNLearnerListener::whenProgress(const void* src, Size pourcent, double error, double time) {
      _bnlearner_->distributeProgress(static_cast< const ApproximationScheme* >(src),
                                      pourcent,
                                      error,
                                      time);
    }
    void BNLearnerListener::whenStop(const void* src, std::string message) {
      _bnlearner_->distributeStop(static_cast< const ApproximationScheme* >(src), message);
    }
  }   // namespace learning
}   // namespace gum
