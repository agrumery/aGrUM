/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Outlined implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(const std::string& message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/approximations/approximationScheme.h>

namespace gum {

  ApproximationScheme::ApproximationScheme(bool verbosity) :
      current_state_(ApproximationSchemeSTATE::Undefined), eps_(5e-2), enabled_eps_(true),
      min_rate_eps_(1e-2), enabled_min_rate_eps_(true), max_time_(1.), enabled_max_time_(false),
      max_iter_(static_cast< Size >(10000)), enabled_max_iter_(true),
      burn_in_(static_cast< Size >(0)), period_size_(static_cast< Size >(1)),
      verbosity_(verbosity) {
    GUM_CONSTRUCTOR(ApproximationScheme);
  }

  ApproximationScheme::~ApproximationScheme() { GUM_DESTRUCTOR(ApproximationScheme); }

  bool ApproximationScheme::continueApproximationScheme(double error) {
    // For coherence, we fix the time used in the method

    double timer_step = timer_.step();

    if (enabled_max_time_) {
      if (timer_step > max_time_) {
        stopScheme_(ApproximationSchemeSTATE::TimeLimit);
        return false;
      }
    }

    if (!startOfPeriod()) { return true; }

    if (current_state_ != ApproximationSchemeSTATE::Continue) {
      GUM_ERROR(
          OperationNotAllowed,
          "state of the approximation scheme is not correct : " << messageApproximationScheme());
    }

    if (verbosity()) { history_.push_back(error); }

    if (enabled_max_iter_) {
      if (current_step_ >= max_iter_) {
        stopScheme_(ApproximationSchemeSTATE::Limit);
        return false;
      }
    }

    last_epsilon_    = current_epsilon_;
    current_epsilon_ = error;   // eps rate isEnabled needs it so affectation was
    // moved from eps isEnabled below

    if (enabled_eps_) {
      if (current_epsilon_ <= eps_) {
        stopScheme_(ApproximationSchemeSTATE::Epsilon);
        return false;
      }
    }

    if (last_epsilon_ >= 0.) {
      if (current_epsilon_ > .0) {
        // ! current_epsilon_ can be 0. AND epsilon
        // isEnabled can be disabled !
        current_rate_ = std::fabs((current_epsilon_ - last_epsilon_) / current_epsilon_);
      }
      // limit with current eps ---> 0 is | 1 - ( last_eps / 0 ) | --->
      // infinity the else means a return false if we isEnabled the rate below,
      // as we would have returned false if epsilon isEnabled was enabled
      else {
        current_rate_ = min_rate_eps_;
      }

      if (enabled_min_rate_eps_) {
        if (current_rate_ <= min_rate_eps_) {
          stopScheme_(ApproximationSchemeSTATE::Rate);
          return false;
        }
      }
    }

    if (stateApproximationScheme() == ApproximationSchemeSTATE::Continue) {
      if (onProgress.hasListener()) {
        GUM_EMIT3(onProgress, current_step_, current_epsilon_, timer_step);
      }

      return true;
    } else {
      return false;
    }
  }
}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/base/core/approximations/approximationScheme_inl.h>
#endif
