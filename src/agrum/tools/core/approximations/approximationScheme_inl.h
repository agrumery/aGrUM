/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Inline implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(const std::string& message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>
// To help IDE parser
#include <agrum/tools/core/approximations/approximationScheme.h>

namespace gum {

  // Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)| If
  // the criterion was disabled it will be enabled
  INLINE void ApproximationScheme::setEpsilon(double eps) {
    if (eps < 0.) { GUM_ERROR(OutOfBounds, "eps should be >=0") }

    eps_         = eps;
    enabled_eps_ = true;
  }

  // Get the value of epsilon
  INLINE double ApproximationScheme::epsilon() const { return eps_; }

  // Disable stopping criterion on epsilon
  INLINE void ApproximationScheme::disableEpsilon() { enabled_eps_ = false; }

  // Enable stopping criterion on epsilon
  INLINE void ApproximationScheme::enableEpsilon() { enabled_eps_ = true; }

  // @return true if stopping criterion on epsilon is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledEpsilon() const { return enabled_eps_; }

  // Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
  INLINE void ApproximationScheme::setMinEpsilonRate(double rate) {
    if (rate < 0) { GUM_ERROR(OutOfBounds, "rate should be >=0") }

    min_rate_eps_         = rate;
    enabled_min_rate_eps_ = true;
  }

  // Get the value of the minimal epsilon rate
  INLINE double ApproximationScheme::minEpsilonRate() const { return min_rate_eps_; }

  // Disable stopping criterion on epsilon rate
  INLINE void ApproximationScheme::disableMinEpsilonRate() { enabled_min_rate_eps_ = false; }

  // Enable stopping criterion on epsilon rate
  INLINE void ApproximationScheme::enableMinEpsilonRate() { enabled_min_rate_eps_ = true; }

  // @return true if stopping criterion on epsilon rate is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMinEpsilonRate() const { return enabled_min_rate_eps_; }

  // stopping criterion on number of iterations
  INLINE void ApproximationScheme::setMaxIter(Size max) {
    if (max < 1) { GUM_ERROR(OutOfBounds, "max should be >=1") }
    max_iter_         = max;
    enabled_max_iter_ = true;
  }

  // @return the criterion on number of iterations
  INLINE Size ApproximationScheme::maxIter() const { return max_iter_; }

  // Disable stopping criterion on max iterations
  INLINE void ApproximationScheme::disableMaxIter() { enabled_max_iter_ = false; }

  // Enable stopping criterion on max iterations
  INLINE void ApproximationScheme::enableMaxIter() { enabled_max_iter_ = true; }

  // @return true if stopping criterion on max iterations is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMaxIter() const { return enabled_max_iter_; }

  // stopping criterion on timeout (in seconds)
  // If the criterion was disabled it will be enabled
  INLINE void ApproximationScheme::setMaxTime(double timeout) {
    if (timeout <= 0.) { GUM_ERROR(OutOfBounds, "timeout should be >0.") }
    max_time_         = timeout;
    enabled_max_time_ = true;
  }

  // returns the timeout (in seconds)
  INLINE double ApproximationScheme::maxTime() const { return max_time_; }

  // get the current running time in second (double)
  INLINE double ApproximationScheme::currentTime() const { return timer_.step(); }

  // Disable stopping criterion on timeout
  INLINE void ApproximationScheme::disableMaxTime() { enabled_max_time_ = false; }

  // Enable stopping criterion on timeout
  INLINE void ApproximationScheme::enableMaxTime() { enabled_max_time_ = true; }

  // @return true if stopping criterion on timeout is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMaxTime() const { return enabled_max_time_; }

  // how many samples between 2 stopping isEnableds
  INLINE void ApproximationScheme::setPeriodSize(Size p) {
    if (p < 1) { GUM_ERROR(OutOfBounds, "p should be >=1") }

    period_size_ = p;
  }

  INLINE Size ApproximationScheme::periodSize() const { return period_size_; }

  // verbosity
  INLINE void ApproximationScheme::setVerbosity(bool v) { verbosity_ = v; }

  INLINE bool ApproximationScheme::verbosity() const { return verbosity_; }

  // history
  INLINE IApproximationSchemeConfiguration::ApproximationSchemeSTATE
         ApproximationScheme::stateApproximationScheme() const {
    return current_state_;
  }

  // @throw OperationNotAllowed if scheme not performed
  INLINE Size ApproximationScheme::nbrIterations() const {
    if (stateApproximationScheme() == ApproximationSchemeSTATE::Undefined) {
      GUM_ERROR(OperationNotAllowed, "state of the approximation scheme is undefined")
    }

    return current_step_;
  }

  // @throw OperationNotAllowed if scheme not performed or verbosity=false
  INLINE const std::vector< double >& ApproximationScheme::history() const {
    if (stateApproximationScheme() == ApproximationSchemeSTATE::Undefined) {
      GUM_ERROR(OperationNotAllowed, "state of the approximation scheme is udefined")
    }

    if (!verbosity()) GUM_ERROR(OperationNotAllowed, "No history when verbosity=false")

    return history_;
  }

  // initialise the scheme
  INLINE void ApproximationScheme::initApproximationScheme() {
    current_state_   = ApproximationSchemeSTATE::Continue;
    current_step_    = 0;
    current_epsilon_ = current_rate_ = -1.0;
    history_.clear();
    timer_.reset();
  }

  // @return true if we are at the beginning of a period (compute error is
  // mandatory)
  INLINE bool ApproximationScheme::startOfPeriod() const {
    if (current_step_ < burn_in_) { return false; }

    if (period_size_ == 1) { return true; }

    return ((current_step_ - burn_in_) % period_size_ == 0);
  }

  // update the scheme w.r.t the new error and incr steps
  INLINE void ApproximationScheme::updateApproximationScheme(unsigned int incr) {
    current_step_ += incr;
  }

  INLINE Size ApproximationScheme::remainingBurnIn() const {
    if (burn_in_ > current_step_) {
      return burn_in_ - current_step_;
    } else {
      return 0;
    }
  }

  // stop approximation scheme by user request.
  INLINE void ApproximationScheme::stopApproximationScheme() {
    if (current_state_ == ApproximationSchemeSTATE::Continue) {
      stopScheme_(ApproximationSchemeSTATE::Stopped);
    }
  }

  // update the scheme w.r.t the new error. Test the stopping criterions that
  // are enabled
  INLINE bool ApproximationScheme::continueApproximationScheme(double error) {
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
      if (current_step_ > max_iter_) {
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

  INLINE void ApproximationScheme::stopScheme_(ApproximationSchemeSTATE new_state) {
    if (new_state == ApproximationSchemeSTATE::Continue) { return; }

    if (new_state == ApproximationSchemeSTATE::Undefined) { return; }

    current_state_ = new_state;
    timer_.pause();

    if (onStop.hasListener()) { GUM_EMIT1(onStop, messageApproximationScheme()); }
  }

}   // namespace gum
