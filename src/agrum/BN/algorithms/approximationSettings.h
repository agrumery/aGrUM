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
 * @author Pierre-Henri Wuillemin
 */
#ifndef GUM_APPROXIMATION_SETTINGS_H
#define GUM_APPROXIMATION_SETTINGS_H

#include <agrum/core/debug.h>

namespace gum {

  class ApproximationSettings {
    public:
      enum ApproximationSchemeSTATE {
        APPROX_UNDEFINED,
        APPROX_CONTINUE,
        APPROX_EPSILON,
        APPROX_RATE,
        APPROX_LIMIT
      };

      /// Constructors and Destructors
      /// @{
      ApproximationSettings( double eps=5e-2,double rate=1e-2,Size max= ( Size ) 1000,bool v=false ) :
          __current_state( APPROX_UNDEFINED ),
          __eps( eps ),
          __min_rate_eps( rate ),
          __max_iter( max ),
          __verbosity( v ) {
        GUM_CONSTRUCTOR( ApproximationSettings );
      };

      ~ApproximationSettings() {
        GUM_DESTRUCTOR( ApproximationSettings );
      };
      /// @}


      /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
      /// @{
      /// @throw OutOfLowerBound if eps<=0
      void setEpsilon( double eps ) {
        if ( eps<=0 ) GUM_ERROR( OutOfLowerBound,"eps should be >0" );
        __eps=eps;
      };

      double epsilon( void ) const {
        return __eps;
      }
      /// @}

      /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
      /// @{
      /// @throw OutOfLowerBound if rate<=0
      void setMinEpsilonRate( double rate ) {
        if ( rate<=0 ) GUM_ERROR( OutOfLowerBound,"rate should be >0" );
        __min_rate_eps=rate;
      };

      double minEpsilonRate( void ) const {
        return __min_rate_eps;
      };
      /// @}

      /// stopping criterion on number of iteration
      /// @{
      /// @throw OutOfLowerBound if max<=1
      void setMaxIter( Size max ) {
        if ( max<1 ) GUM_ERROR( OutOfLowerBound,"max should be >=1" );
        __max_iter=max;
      };

      Size maxIter( void ) const {
        return __max_iter;
      };
      /// @}

      /// verbosity
      /// @{
      void setVerbosity( bool v ) {
        __verbosity=v;
      };

      bool verbosity( void ) const {
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
        if ( stateApproximationScheme()==APPROX_UNDEFINED ) {
          GUM_ERROR( OperationNotAllowed,
                     "state of the approximation scheme is udefined" );
        }
        return __current_step;
      };
      
      /// @throw OperationNotAllowed if scheme not performed or verbosity=false
      const std::vector<double>& history() const {
        if ( stateApproximationScheme()==APPROX_UNDEFINED ) {
          GUM_ERROR( OperationNotAllowed,
                     "state of the approximation scheme is udefined" );
        }
        if ( verbosity()==false) {
          GUM_ERROR( OperationNotAllowed,
                     "No history when verbosity=false" );
        }
        
        return __history;
      }
      /// @}

     /// Approximation Scheme
      /** The approximation scheme is assumed to be used like this
      @code
      initApproximationScheme();

      // this loop can stop with APPROX_EPSILON, APPROX_RATE, APPROX_LIMIT
      do {
      // compute new values and a T_DATA error representing the progress in this step.
        updateApproximationScheme();
      } while ( testApproximationScheme( epsilon )==APPROX_CONTINUE );
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
      }
      }
      // equivalent to
      if (verbosity()) GUM_TRACE(messageApproximationScheme());
      @endcode
      */

      /// @{

      /// initialise the scheme
      void initApproximationScheme() {
        __current_state=APPROX_CONTINUE;
        __current_step=0;
        __current_epsilon=__current_rate=__min_rate_eps=-1.0;
        __history.clear();
      };

      /// update the scheme w.r.t the new error
      void updateApproximationScheme() {
        __current_step++ ;
      }

      /// update the scheme w.r.t the new error. Test the stopping criterion
      /// @throw OperationNotAllowed if stat!=APPROX_CONTINUE
      ApproximationSchemeSTATE testApproximationScheme( double error,bool check_rate=true ) {
        if ( __current_state!=APPROX_CONTINUE ) {
          GUM_ERROR( OperationNotAllowed,
                     "state of the approximation scheme is not correct : "+messageApproximationScheme() );
        }
        
        if (verbosity()) __history.push_back(error);
        
        if ( __current_step>maxIter() )
          return ( __current_state=APPROX_LIMIT );

        __last_epsilon=__current_epsilon;
        if (( __current_epsilon=error )<epsilon() )
          return ( __current_state=APPROX_EPSILON );

        if ( check_rate && __last_epsilon>0 ) {
          if (( __current_rate=fabs(( __current_epsilon-__last_epsilon )/__current_epsilon ) )<minEpsilonRate() )
            return ( __current_state=APPROX_RATE );
        }

        return stateApproximationScheme(); // APPROX_CONTINUE
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
          case APPROX_UNDEFINED: s<<"undefined state";
            break;
        };
        return s.str();
      }

      ///  @}
    private:
      /// current state of approximationScheme
      double __current_epsilon,__last_epsilon,__current_rate;
      Size __current_step;
      ApproximationSchemeSTATE __current_state;

      /// history for trace (if verbosity=true)
      std::vector<double> __history;
      
      /// Threshold for convergence
      double __eps;

      /// Threshold for rate of epsilon
      double __min_rate_eps;

      Size __max_iter;
      bool __verbosity;
  };
} //namespace gum
#endif // GUM_APPROXIMATION_SETTINGS_H
// kate: indent-mode cstyle; space-indent on; indent-width 2;
