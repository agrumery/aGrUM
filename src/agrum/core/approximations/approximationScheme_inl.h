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
 * @brief Inline implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN
 */

// To help IDE parser
#include <agrum/core/approximations/approximationScheme.h>

namespace gum {

  // Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)| If
  // the criterion was disabled it will be enabled
  INLINE void ApproximationScheme::setEpsilon( double eps ) {
    if ( eps < 0. ) {
      GUM_ERROR( OutOfLowerBound, "eps should be >=0" );
    }

    _eps = eps;
    _enabled_eps = true;
  }

  // Get the value of epsilon
  INLINE double ApproximationScheme::epsilon( void ) const { return _eps; }

  // Disable stopping criterion on epsilon
  INLINE void ApproximationScheme::disableEpsilon() { _enabled_eps = false; }

  // Enable stopping criterion on epsilon
  INLINE void ApproximationScheme::enableEpsilon() { _enabled_eps = true; }

  // @return true if stopping criterion on epsilon is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledEpsilon() const {
    return _enabled_eps;
  }

  // Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
  INLINE void ApproximationScheme::setMinEpsilonRate( double rate ) {
    if ( rate < 0 ) {
      GUM_ERROR( OutOfLowerBound, "rate should be >=0" );
    }

    _min_rate_eps = rate;
    _enabled_min_rate_eps = true;
  }

  // Get the value of the minimal epsilon rate
  INLINE double ApproximationScheme::minEpsilonRate( void ) const {
    return _min_rate_eps;
  }

  // Disable stopping criterion on epsilon rate
  INLINE void ApproximationScheme::disableMinEpsilonRate() {
    _enabled_min_rate_eps = false;
  }

  // Enable stopping criterion on epsilon rate
  INLINE void ApproximationScheme::enableMinEpsilonRate() {
    _enabled_min_rate_eps = true;
  }

  // @return true if stopping criterion on epsilon rate is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMinEpsilonRate() const {
    return _enabled_min_rate_eps;
  }

  // stopping criterion on number of iterations
  INLINE void ApproximationScheme::setMaxIter( Size max ) {
    if ( max < 1 ) {
      GUM_ERROR( OutOfLowerBound, "max should be >=1" );
    }
    _max_iter = max;
    _enabled_max_iter = true;
  }

  // @return the criterion on number of iterations
  INLINE Size ApproximationScheme::maxIter( void ) const { return _max_iter; }

  // Disable stopping criterion on max iterations
  INLINE void ApproximationScheme::disableMaxIter() { _enabled_max_iter = false; }

  // Enable stopping criterion on max iterations
  INLINE void ApproximationScheme::enableMaxIter() { _enabled_max_iter = true; }

  // @return true if stopping criterion on max iterations is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMaxIter() const {
    return _enabled_max_iter;
  }

  // stopping criterion on timeout If the criterion was disabled it will be
  // enabled
  INLINE void ApproximationScheme::setMaxTime( double timeout ) {
    if ( timeout <= 0. ) {
      GUM_ERROR( OutOfLowerBound, "timeout should be >0." );
    }
    _max_time = timeout;
    _enabled_max_time = true;
  }

  // returns the timeout (in seconds)
  INLINE double ApproximationScheme::maxTime( void ) const { return _max_time; }

  // get the current running time in second (double)
  INLINE double ApproximationScheme::currentTime( void ) const {
    return _timer.step();
  }

  // Disable stopping criterion on timeout
  INLINE void ApproximationScheme::disableMaxTime() { _enabled_max_time = false; }

  // Enable stopping criterion on timeout
  INLINE void ApproximationScheme::enableMaxTime() { _enabled_max_time = true; }

  // @return true if stopping criterion on timeout is enabled, false
  // otherwise
  INLINE bool ApproximationScheme::isEnabledMaxTime() const {
    return _enabled_max_time;
  }

  // how many samples between 2 stopping isEnableds
  INLINE void ApproximationScheme::setPeriodSize( Size p ) {
    if ( p < 1 ) {
      GUM_ERROR( OutOfLowerBound, "p should be >=1" );
    }

    _period_size = p;
  }

  INLINE Size ApproximationScheme::periodSize( void ) const {
    return _period_size;
  }

  // size of burn in on number of iteration
  INLINE void ApproximationScheme::setBurnIn( Size b ) {
    _burn_in = b;
  }

  INLINE Size ApproximationScheme::burnIn( void ) const { return _burn_in; }

  // verbosity
  INLINE void ApproximationScheme::setVerbosity( bool v ) { _verbosity = v; }

  INLINE bool ApproximationScheme::verbosity( void ) const { return _verbosity; }

  // history
  INLINE IApproximationSchemeConfiguration::ApproximationSchemeSTATE
         ApproximationScheme::stateApproximationScheme() const {
    return _current_state;
  }

  // @throw OperationNotAllowed if scheme not performed
  INLINE Size ApproximationScheme::nbrIterations() const {
    if ( stateApproximationScheme() == ApproximationSchemeSTATE::Undefined ) {
      GUM_ERROR( OperationNotAllowed,
                 "state of the approximation scheme is undefined" );
    }

    return _current_step;
  }

  // @throw OperationNotAllowed if scheme not performed or verbosity=false
  INLINE const std::vector<double>& ApproximationScheme::history() const {
    if ( stateApproximationScheme() == ApproximationSchemeSTATE::Undefined ) {
      GUM_ERROR( OperationNotAllowed,
                 "state of the approximation scheme is udefined" );
    }

    if ( verbosity() == false ) {
      GUM_ERROR( OperationNotAllowed, "No history when verbosity=false" );
    }

    return _history;
  }

  // initialise the scheme
  INLINE void ApproximationScheme::initApproximationScheme() {
    _current_state = ApproximationSchemeSTATE::Continue;
    _current_step = 0;
    _current_epsilon = _current_rate = -1.0;
    _history.clear();
    _timer.reset();
  }

  // @return true if we are at the beginning of a period (compute error is
  // mandatory)
  INLINE bool ApproximationScheme::startOfPeriod() {
    if ( _current_step < _burn_in ) {
      return false;
    }

    if ( _period_size == 1 ) {
      return true;
    }

    return ( ( _current_step - _burn_in ) % _period_size == 0 );
  }

  // update the scheme w.r.t the new error and incr steps
  INLINE void ApproximationScheme::updateApproximationScheme( unsigned int incr ) {
    _current_step += incr;
  }

  INLINE Size ApproximationScheme::remainingBurnIn() {
    if ( _burn_in > _current_step ) {
      return _burn_in - _current_step;
    } else {
      return 0;
    }
  }

  // stop approximation scheme by user request.
  INLINE void ApproximationScheme::stopApproximationScheme() {
    if ( _current_state == ApproximationSchemeSTATE::Continue ) {
      _stopScheme( ApproximationSchemeSTATE::Stopped );
    }
  }

  // update the scheme w.r.t the new error. Test the stopping criterions that
  // are enabled
  INLINE bool ApproximationScheme::continueApproximationScheme( double error ) {
    // For coherence, we fix the time used in the method

    double timer_step = _timer.step();

    if ( _enabled_max_time ) {
      if ( timer_step > _max_time ) {
        _stopScheme( ApproximationSchemeSTATE::TimeLimit );
        return false;
      }
    }

    if ( !startOfPeriod() ) {
      return true;
    }

    if ( _current_state != ApproximationSchemeSTATE::Continue ) {
      GUM_ERROR( OperationNotAllowed,
                 "state of the approximation scheme is not correct : " +
                     messageApproximationScheme() );
    }

    if ( verbosity() ) {
      _history.push_back( error );
    }

    if ( _enabled_max_iter ) {
      if ( _current_step > _max_iter ) {
        _stopScheme( ApproximationSchemeSTATE::Limit );
        return false;
      }
    }

    _last_epsilon = _current_epsilon;
    _current_epsilon = error;  // eps rate isEnabled needs it so affectation was
    // moved from eps isEnabled below

    if ( _enabled_eps ) {
      if ( _current_epsilon <= _eps ) {
        _stopScheme( ApproximationSchemeSTATE::Epsilon );
        return false;
      }
    }

    if ( _last_epsilon >= 0. ) {
      if ( _current_epsilon > .0 ) {
        // ! _current_epsilon can be 0. AND epsilon
        // isEnabled can be disabled !
        _current_rate =
            std::fabs( ( _current_epsilon - _last_epsilon ) / _current_epsilon );
      }
      // limit with current eps ---> 0 is | 1 - ( last_eps / 0 ) | --->
      // infinity the else means a return false if we isEnabled the rate below,
      // as we would have returned false if epsilon isEnabled was enabled
      else {
        _current_rate = _min_rate_eps;
      }

      if ( _enabled_min_rate_eps ) {
        if ( _current_rate <= _min_rate_eps ) {
          _stopScheme( ApproximationSchemeSTATE::Rate );
          return false;
        }
      }
    }

    if ( stateApproximationScheme() == ApproximationSchemeSTATE::Continue ) {
      if ( onProgress.hasListener() ) {
        GUM_EMIT3( onProgress,
                   ( _current_step * 100 ) / _max_iter,
                   _current_epsilon,
                   timer_step );
      }

      return true;
    } else {
      return false;
    }
  }

  INLINE void
  ApproximationScheme::_stopScheme( ApproximationSchemeSTATE new_state ) {
    if ( new_state == ApproximationSchemeSTATE::Continue ) {
      return;
    }

    if ( new_state == ApproximationSchemeSTATE::Undefined ) {
      return;
    }

    _current_state = new_state;
    _timer.pause();

    if ( onStop.hasListener() ) {
      GUM_EMIT1( onStop, messageApproximationScheme() );
    }
  }

}  // namespace gum
