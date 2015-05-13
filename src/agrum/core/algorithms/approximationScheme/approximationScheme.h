/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

/**
 * @file
 * @brief This file contains general scheme for iteratively convergent algorithms
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
  * @see ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_APPROXIMATION_SCHEME_H
#define GUM_APPROXIMATION_SCHEME_H

#include <math.h>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/timer.h>
#include <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>

namespace gum {

  /// Approximation Scheme
  /** The approximation scheme is assumed to be used like this
   *      @code
   *      initApproximationScheme();
   *
   *      // this loop can stop with ApproximationSchemeSTATE::Epsilon,
   ApproximationSchemeSTATE::Rate, ApproximationSchemeSTATE::Limit
   *      do {
   *      // compute new values and a GUM_SCALAR error representing the progress in
   this step.
   *        updateApproximationScheme();
   *        compute state of the approximation
   *        if (startOfPeriod()) w.r.t to the state of approximation
   *      compute epsilon
   } while ( continueApproximationScheme( epsilon ));
   // end of loop

   if ( verbosity() ) {
     switch ( stateApproximationScheme() ) {
       case ApproximationSchemeSTATE::Continue: // should not be possible
      break;
       case ApproximationSchemeSTATE::Epsilon: GUM_TRACE( "stop with
   epsilon="<<epsilon() );
       break;
       case ApproximationSchemeSTATE::Rate: GUM_TRACE( "stop with
   rate="<<minEpsilonRate() );
       break;
       case ApproximationSchemeSTATE::Limit: GUM_TRACE( "stop with max
   iteration="<<maxIter() );
       break;
       case ApproximationSchemeSTATE::TimeLimit: GUM_TRACE( "stop with
   timemout="<<currentTime() );
       break;
     }
   }
   // equivalent to
   if (verbosity()) GUM_TRACE(messageApproximationScheme());
   @endcode
   */
  class ApproximationScheme : public IApproximationSchemeConfiguration {
    public:
    /// Constructors and Destructors
    /// @{
    ApproximationScheme(bool v = false)
        : _current_state(ApproximationSchemeSTATE::Undefined),

          _eps(5e-2), _enabled_eps(true),

          _min_rate_eps(1e-2), _enabled_min_rate_eps(true),

          _max_time(1.), _enabled_max_time(false),

          _max_iter((Size)10000), _enabled_max_iter(true),

          _burn_in((Size)0), _period_size((Size)1), _verbosity(v) {
      GUM_CONSTRUCTOR(ApproximationScheme);
    };

    ~ApproximationScheme() { GUM_DESTRUCTOR(ApproximationScheme); };
    /// @}

    /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if eps<0
    void setEpsilon(double eps) {
      if (eps < 0.) {
        GUM_ERROR(OutOfLowerBound, "eps should be >=0");
      }

      _eps = eps;
      _enabled_eps = true;
    };

    /// Get the value of epsilon
    double epsilon(void) const { return _eps; }

    /// Disable stopping criterion on epsilon
    void disableEpsilon() { _enabled_eps = false; }
    /// Enable stopping criterion on epsilon
    void enableEpsilon() { _enabled_eps = true; }
    /// @return true if stopping criterion on epsilon is enabled, false otherwise
    bool isEnabledEpsilon() const { return _enabled_eps; }
    /// @}

    /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if rate<0
    void setMinEpsilonRate(double rate) {
      if (rate < 0) {
        GUM_ERROR(OutOfLowerBound, "rate should be >=0");
      }

      _min_rate_eps = rate;
      _enabled_min_rate_eps = true;
    };

    /// Get the value of the minimal epsilon rate
    double minEpsilonRate(void) const { return _min_rate_eps; };

    /// Disable stopping criterion on epsilon rate
    void disableMinEpsilonRate() { _enabled_min_rate_eps = false; }
    /// Enable stopping criterion on epsilon rate
    void enableMinEpsilonRate() { _enabled_min_rate_eps = true; }
    /// @return true if stopping criterion on epsilon rate is enabled, false
    /// otherwise
    bool isEnabledMinEpsilonRate() const { return _enabled_min_rate_eps; }
    /// @}

    /// stopping criterion on number of iterations
    /// @{
    /// If the criterion was disabled it will be enabled
    /// @param max The maximum number of iterations
    /// @throw OutOfLowerBound if max<=1
    void setMaxIter(Size max) {
      if (max < 1) {
        GUM_ERROR(OutOfLowerBound, "max should be >=1");
      }

      _max_iter = max;
      _enabled_max_iter = true;
    };

    /// @return the criterion on number of iterations
    Size maxIter(void) const { return _max_iter; };

    /// Disable stopping criterion on max iterations
    void disableMaxIter() { _enabled_max_iter = false; }
    /// Enable stopping criterion on max iterations
    void enableMaxIter() { _enabled_max_iter = true; }
    /// @return true if stopping criterion on max iterations is enabled, false
    /// otherwise
    bool isEnabledMaxIter() const { return _enabled_max_iter; }
    /// @}

    /// stopping criterion on timeout
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if timeout<=0.0
    /** timeout is time in second (double).
    */
    void setMaxTime(double timeout) {
      if (timeout <= 0.) {
        GUM_ERROR(OutOfLowerBound, "timeout should be >0.");
      }

      _max_time = timeout;
      _enabled_max_time = true;
    };

    /// returns the timeout (in seconds)
    double maxTime(void) const { return _max_time; };

    /// get the current running time in second (double)
    double currentTime(void) const { return _timer.step(); }

    /// Disable stopping criterion on timeout
    void disableMaxTime() { _enabled_max_time = false; }
    /// Enable stopping criterion on timeout
    void enableMaxTime() { _enabled_max_time = true; }
    /// @return true if stopping criterion on timeout is enabled, false otherwise
    bool isEnabledMaxTime() const { return _enabled_max_time; }
    /// @}

    /// how many samples between 2 stopping isEnableds
    /// @{
    /// @throw OutOfLowerBound if p<1
    void setPeriodSize(Size p) {
      if (p < 1) {
        GUM_ERROR(OutOfLowerBound, "p should be >=1");
      }

      _period_size = p;
    };

    Size periodSize(void) const { return _period_size; };
    /// @}

    /// size of burn in on number of iteration
    /// @{

    /// @throw OutOfLowerBound if b<1
    void setBurnIn(Size b) {
      if (b < 1) {
        GUM_ERROR(OutOfLowerBound, "b should be >=1");
      }

      _burn_in = b;
    };

    Size burnIn(void) const { return _burn_in; };
    /// @}

    /// verbosity
    /// @{
    void setVerbosity(bool v) { _verbosity = v; };

    bool verbosity(void) const { return _verbosity; };
    /// @}

    /// history
    /// @{

    ApproximationSchemeSTATE stateApproximationScheme() const {
      return _current_state;
    }

    /// @throw OperationNotAllowed if scheme not performed
    Size nbrIterations() const {
      if (stateApproximationScheme() == ApproximationSchemeSTATE::Undefined) {
        GUM_ERROR(OperationNotAllowed,
                  "state of the approximation scheme is undefined");
      }

      return _current_step;
    };

    /// @throw OperationNotAllowed if scheme not performed or verbosity=false
    const std::vector<double> &history() const {
      if (stateApproximationScheme() == ApproximationSchemeSTATE::Undefined) {
        GUM_ERROR(OperationNotAllowed,
                  "state of the approximation scheme is udefined");
      }

      if (verbosity() == false) {
        GUM_ERROR(OperationNotAllowed, "No history when verbosity=false");
      }

      return _history;
    }
    /// @}
    /// @{

    /// initialise the scheme
    void initApproximationScheme() {
      _current_state = ApproximationSchemeSTATE::Continue;
      _current_step = 0;
      _current_epsilon = _current_rate = -1.0;
      _history.clear();
      _timer.reset();
    };

    /// @return true if we are at the beginning of a period (compute error is
    /// mandatory)
    bool startOfPeriod() {
      if (_current_step < _burn_in) {
        return false;
      }

      if (_period_size == 1) {
        return true;
      }

      return ((_current_step - _burn_in) % _period_size == 0);
    }

    /// update the scheme w.r.t the new error and incr steps
    void updateApproximationScheme(unsigned int incr = 1) { _current_step += incr; }

    Size remainingBurnIn() {
      if (_burn_in > _current_step) {
        return _burn_in - _current_step;
      } else {
        return 0;
      }
    }

    /// stop approximation scheme by user request.
    void stopApproximationScheme() {
      if (_current_state == ApproximationSchemeSTATE::Continue) {
        _stopScheme(ApproximationSchemeSTATE::Stopped);
      }
    }

    /// update the scheme w.r.t the new error. Test the stopping criterions that are
    /// enabled
    /// @throw OperationNotAllowed if stat!=ApproximationSchemeSTATE::Continue
    /// @return false if state become != ApproximationSchemeSTATE::Continue
    bool continueApproximationScheme(double error) {
      // For coherence, we fix the time used in the method

      double timer_step = _timer.step();

      if (_enabled_max_time) {
        if (timer_step > _max_time) {
          _stopScheme(ApproximationSchemeSTATE::TimeLimit);
          return false;
        }
      }

      if (!startOfPeriod()) {
        return true;
      }

      if (_current_state != ApproximationSchemeSTATE::Continue) {
        GUM_ERROR(OperationNotAllowed,
                  "state of the approximation scheme is not correct : " +
                      messageApproximationScheme());
      }

      if (verbosity()) {
        _history.push_back(error);
      }

      if (_enabled_max_iter) {
        if (_current_step > _max_iter) {
          _stopScheme(ApproximationSchemeSTATE::Limit);
          return false;
        }
      }

      _last_epsilon = _current_epsilon;
      _current_epsilon = error; // eps rate isEnabled needs it so affectation was
                                // moved from eps isEnabled below

      if (_enabled_eps) {
        if (_current_epsilon <= _eps) {
          _stopScheme(ApproximationSchemeSTATE::Epsilon);
          return false;
        }
      }

      if (_last_epsilon >= 0.) {
        if (_current_epsilon > .0) { // ! _current_epsilon can be 0. AND epsilon
                                     // isEnabled can be disabled !
          _current_rate =
              std::fabs((_current_epsilon - _last_epsilon) / _current_epsilon);
        }
        // limit with current eps ---> 0 is | 1 - ( last_eps / 0 ) | ---> infinity
        // the else means a return false if we isEnabled the rate below, as we would
        // have returned false if epsilon isEnabled was enabled
        else {
          _current_rate = _min_rate_eps;
        }

        if (_enabled_min_rate_eps) {
          if (_current_rate <= _min_rate_eps) {
            _stopScheme(ApproximationSchemeSTATE::Rate);
            return false;
          }
        }
      }

      if (stateApproximationScheme() == ApproximationSchemeSTATE::Continue) {
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress, (_current_step * 100) / _max_iter, _current_epsilon,
                    timer_step);
        }

        return true;
      } else {
        return false;
      }
    };

    ///  @}
    private:
    void _stopScheme(ApproximationSchemeSTATE new_state) {
      if (new_state == ApproximationSchemeSTATE::Continue) {
        return;
      }

      if (new_state == ApproximationSchemeSTATE::Undefined) {
        return;
      }

      _current_state = new_state;
      _timer.pause();

      if (onStop.hasListener()) {
        GUM_EMIT1(onStop, messageApproximationScheme());
      }
    }

    protected:
    /// current state of approximationScheme
    double _current_epsilon, _last_epsilon, _current_rate;
    Size _current_step;
    Timer _timer;
    ApproximationSchemeSTATE _current_state;

    /// history for trace (if verbosity=true)
    std::vector<double> _history;

    /// Threshold for convergence
    double _eps;
    /// boolean for threshold convergence isEnabled
    bool _enabled_eps;

    /// Threshold for rate of epsilon
    double _min_rate_eps;
    /// boolean for threshold rate of epsilon isEnabled
    bool _enabled_min_rate_eps;

    // timeout
    double _max_time;
    // boolean for timeout isEnabled
    bool _enabled_max_time;

    /// max iterations as a stopping criterion
    Size _max_iter;
    // boolean for max iterations stopping criterion isEnabled
    bool _enabled_max_iter;

    /// nbr of iterations before checking stopping criteria
    Size _burn_in;

    /// checking criteria every _period_size iterations
    Size _period_size;

    bool _verbosity;
  };
} // namespace gum
#endif // GUM_APPROXIMATION_SCHEME_H
