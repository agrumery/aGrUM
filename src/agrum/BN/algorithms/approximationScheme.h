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
 * @brief This file contains getters and setters for Approximation settings
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
  * @see ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri Wuillemin
 */
#ifndef GUM_APPROXIMATION_SCHEME_H
#define GUM_APPROXIMATION_SCHEME_H

#include <math.h>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/timer.h>

namespace gum {


  /// Approximation Scheme
  /** The approximation scheme is assumed to be used like this
   *      @code
   *      initApproximationScheme();
   *
   *      // this loop can stop with APPROX_EPSILON, APPROX_RATE, APPROX_LIMIT
   *      do {
   *      // compute new values and a GUM_SCALAR error representing the progress in this step.
   *        updateApproximationScheme();
   *        compute state of the approximation
   *        if (startOfPeriod()) w.r.t to the state of approximation
   *      compute epsilon
   } while ( continueApproximationScheme( epsilon ));
   // end of loop

   if ( verbosity() ) {
     switch ( stateApproximationScheme() ) {
       case APPROX_CONTINUE: // should not be possible
      break;
       case APPROX_EPSILON: GUM_TRACE( "stop with epsilon="<<epsilon() );
       break;
       case APPROX_RATE: GUM_TRACE( "stop with rate="<<minEpsilonRate() );
       break;
       case APPROX_LIMIT: GUM_TRACE( "stop with max iteration="<<maxIter() );
       break;
       case APPROX_TIME_LIMIT: GUM_TRACE( "stop with timemout="<<currentTime() );
       break;
     }
   }
   // equivalent to
   if (verbosity()) GUM_TRACE(messageApproximationScheme());
   @endcode
   */
  class ApproximationScheme {
    public:
      Signaler3<int, double, double> onProgress; // progression,error
      Signaler1<std::string> onStop; // criteria messageApproximationScheme

      enum ApproximationSchemeSTATE {
        APPROX_UNDEFINED,
        APPROX_CONTINUE,
        APPROX_EPSILON,
        APPROX_RATE,
        APPROX_LIMIT,
        APPROX_TIME_LIMIT
      };

      /// Constructors and Destructors
      /// @{
      ApproximationScheme ( bool v = false ) :
        __current_state ( APPROX_UNDEFINED ),
        __eps ( 5e-2 ),
        __min_rate_eps ( 1e-2 ),
        __max_time ( 0.0 ),
        __max_iter ( ( Size ) 10000 ) ,
        __burn_in ( ( Size ) 0 ),
        __period_size ( ( Size ) 1 ),
        __verbosity ( v ) {
        GUM_CONSTRUCTOR ( ApproximationScheme );
      };


      ~ApproximationScheme() {
        GUM_DESTRUCTOR ( ApproximationScheme );
      };
      /// @}

      /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
      /// @{
      /// @throw OutOfLowerBound if eps<=0
      void setEpsilon ( double eps ) {
        if ( eps <= 0 ) {
          GUM_ERROR ( OutOfLowerBound, "eps should be >0" );
        }

        __eps = eps;
      };

      double epsilon ( void ) const {
        return __eps;
      }
      /// @}

      /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
      /// @{
      /// @throw OutOfLowerBound if rate<=0
      void setMinEpsilonRate ( double rate ) {
        if ( rate <= 0 ) {
          GUM_ERROR ( OutOfLowerBound, "rate should be >0" );
        }

        __min_rate_eps = rate;
      };

      double minEpsilonRate ( void ) const {
        return __min_rate_eps;
      };
      /// @}

      /// stopping criterion on number of iteration
      /// @{
      /// @throw OutOfLowerBound if max<=1
      void setMaxIter ( Size max ) {
        if ( max < 1 ) {
          GUM_ERROR ( OutOfLowerBound, "max should be >=1" );
        }

        __max_iter = max;
      };

      Size maxIter ( void ) const {
        return __max_iter;
      };

      /// @}

      
      /// stopping criterion ontimeout
      /// @{
      /// @throw OutOfLowerBound if timeout<=0.0
      /** timeout is time in second (double).
       * if timeout==0, no timeout stopping criterion.
      */
      void setMaxTime ( double timeout ) {
        if ( timeout < 0.0 ) {
          GUM_ERROR ( OutOfLowerBound, "timeout should be >=0.0" );
        }

        __max_time = timeout;
      };

      /// returns the timeout (in seconds)
      double maxTime ( void ) const {
        return __max_time;
      };

      /// get the current running time in second (double)
      double currentTime ( void ) const {
        return __timer.step();
      }
      /// @}

      /// how many samples between 2 stopping tests
      /// @{

      /// @throw OutOfLowerBound if p<1
      void setPeriodSize ( Size p ) {
        if ( p < 1 ) {
          GUM_ERROR ( OutOfLowerBound, "p should be >=1" );
        }

        __period_size = p;

      };

      Size periodSize ( void ) const {return __period_size;};
      /// @}

      /// size of burn in on number of iteration
      /// @{

      /// @throw OutOfLowerBound if b<1
      void setBurnIn ( Size b ) {
        if ( b < 1 ) {
          GUM_ERROR ( OutOfLowerBound, "b should be >=1" );
        }

        __burn_in = b;
      };

      Size burnIn ( void ) const {return __burn_in;};
      /// @}

      /// verbosity
      /// @{
      void setVerbosity ( bool v ) {
        __verbosity = v;
      };

      bool verbosity ( void ) const {
        return __verbosity;
      };
      /// @}

      /// history
      /// @{

      ApproximationSchemeSTATE stateApproximationScheme() const {
        return __current_state;
      }

      /// @throw OperationNotAllowed if scheme not performed
      Size nbrIterations() const {
        if ( stateApproximationScheme() == APPROX_UNDEFINED ) {
          GUM_ERROR ( OperationNotAllowed,
                      "state of the approximation scheme is undefined" );
        }

        return __current_step;
      };

      /// @throw OperationNotAllowed if scheme not performed or verbosity=false
      const std::vector<double>& history() const {
        if ( stateApproximationScheme() == APPROX_UNDEFINED ) {
          GUM_ERROR ( OperationNotAllowed,
                      "state of the approximation scheme is udefined" );
        }

        if ( verbosity() == false ) {
          GUM_ERROR ( OperationNotAllowed,
                      "No history when verbosity=false" );
        }

        return __history;
      }
      /// @}
      /// @{

      /// initialise the scheme
      void initApproximationScheme() {
        __current_state = APPROX_CONTINUE;
        __current_step = 0;
        __current_epsilon = __current_rate = -1.0;
        __history.clear();
        __timer.reset();
      };

      /// @return true if we are at the beginning of a period (compute error is mandatory)
      bool startOfPeriod() {
        if ( __current_step < __burn_in ) return false;

        return ( ( __current_step - __burn_in ) % __period_size == 0 );
      }

      /// update the scheme w.r.t the new error
      void updateApproximationScheme() {
        __current_step++ ;
      }

      /// update the scheme w.r.t the new error. Test the stopping criterion
      /// @throw OperationNotAllowed if stat!=APPROX_CONTINUE
      /// @return true if stat become != APPROX_CONTINUE
      bool continueApproximationScheme ( double error, bool check_rate = true ) {
        // For coherence, we fix the time used in the method.
        double timer_step=__timer.step();

        if ( ! startOfPeriod() ) return true;

        if ( __current_state != APPROX_CONTINUE ) {
          GUM_ERROR ( OperationNotAllowed,
                      "state of the approximation scheme is not correct : " + messageApproximationScheme() );
        }

        if ( verbosity() ) __history.push_back ( error );

        if ( __max_time > 0 ) {
          if ( timer_step > __max_time ) {
            __stopScheme ( APPROX_TIME_LIMIT );
            return false;
          }
        }

        if ( __current_step > maxIter() ) {
          __stopScheme ( APPROX_LIMIT );
          return false;
        }

        __last_epsilon = __current_epsilon;

        if ( ( __current_epsilon = error ) < epsilon() ) {
          __stopScheme ( APPROX_EPSILON );
          return false;
        }

        if ( check_rate && __last_epsilon > 0 ) {
          if ( ( __current_rate = fabs ( ( __current_epsilon - __last_epsilon ) / __current_epsilon ) ) < minEpsilonRate() ) {
            __stopScheme ( APPROX_RATE );
            return false;
          }

        }

        if (stateApproximationScheme() == APPROX_CONTINUE) {
          if (onProgress.hasListener()) GUM_EMIT3(onProgress,(__current_step*100)/__max_iter,__current_epsilon,timer_step);
          return true;
        } else
          return false;
      };

      std::string messageApproximationScheme() const {
        std::stringstream s;

        switch ( stateApproximationScheme() ) {
          case APPROX_CONTINUE: s<<"in progress";
            break;
          case APPROX_EPSILON: s<<"stopped with epsilon="<<epsilon();
            break;
          case APPROX_RATE: s<<"stopped with rate="<<minEpsilonRate();
            break;
          case APPROX_LIMIT: s<<"stopped with max iteration="<<maxIter();
            break;
          case APPROX_TIME_LIMIT: s<<"stopped with timeout="<<maxTime();
            break;
          case APPROX_UNDEFINED: s<<"undefined state";
            break;
        };

        return s.str();
      }

      ///  @}
    private:
      void __stopScheme ( ApproximationSchemeSTATE new_state ) {
        if ( new_state == APPROX_CONTINUE )  return;

        if ( new_state == APPROX_UNDEFINED )  return;

        __current_state = new_state;
        __timer.pause();

        if (onStop.hasListener()) GUM_EMIT1(onStop,messageApproximationScheme());
      }
      /// current state of approximationScheme
      double __current_epsilon, __last_epsilon, __current_rate;
      Size __current_step;
      Timer __timer;
      ApproximationSchemeSTATE __current_state;

      /// history for trace (if verbosity=true)
      std::vector<double> __history;

      /// Threshold for convergence
      double __eps;

      /// Threshold for rate of epsilon
      double __min_rate_eps;

      // timeout (not used if 0)
      double __max_time;

      /// max iterations as a stopping criterion
      Size __max_iter;

      /// nbr of iterations before checking stopping criteria
      Size __burn_in;

      /// checking criteria every __period_size iterations
      Size __period_size;

      bool __verbosity;
  };
} //namespace gum
#endif // GUM_APPROXIMATION_SCHEME_H

