/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (c) 2012 -2024 Pierre-Henri WUILLEMIN(_@LIP6)
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
#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>

namespace gum {
  namespace learning {
    BNLearnerListener::BNLearnerListener(IBNLearner* bnl, ApproximationScheme& sch) :
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

    void BNLearnerListener::whenStop(const void* src, const std::string& message) {
      _bnlearner_->distributeStop(static_cast< const ApproximationScheme* >(src), message);
    }
  }   // namespace learning
}   // namespace gum
