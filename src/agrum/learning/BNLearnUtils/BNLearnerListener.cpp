/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Pierre-Henri WUILLEMIN <pierre-henri.wuillemin@lip6.fr>

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

#include <agrum/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {
  namespace learning {
    BNLearnerListener::BNLearnerListener(genericBNLearner *bnl,
                                         ApproximationScheme &sch)
        : ApproximationSchemeListener(sch), __bnlearner(bnl) {
      bnl->setCurrentApproximationScheme(&sch);
      GUM_CONSTRUCTOR(BNLearnerListener);
    }

    BNLearnerListener::BNLearnerListener(const BNLearnerListener &other)
        : ApproximationSchemeListener(other) {
      GUM_CONS_CPY(BNLearnerListener);
      GUM_ERROR(OperationNotAllowed, "No copy constructor for BNLearnerListener");
    }

    BNLearnerListener::~BNLearnerListener() { GUM_DESTRUCTOR(BNLearnerListener); }

    BNLearnerListener &BNLearnerListener::operator=(const BNLearnerListener &other) {
      GUM_CONS_CPY(BNLearnerListener);
      GUM_ERROR(OperationNotAllowed, "No copy constructor for BNLearnerListener");
    }

    void BNLearnerListener::whenProgress(const void *src, Size pourcent,
                                         double error, double time) {
      __bnlearner->distributeProgress(static_cast<const ApproximationScheme *>(src),
                                      pourcent, error, time);
    }
    void BNLearnerListener::whenStop(const void *src, std::string message) {
      __bnlearner->distributeStop(static_cast<const ApproximationScheme *>(src),
                                  message);
    }
  }
}
