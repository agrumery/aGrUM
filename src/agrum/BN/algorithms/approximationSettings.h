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
      /// Constructors and Destructors
      /// @{
      ApproximationSettings ( double eps=5e-2,double rate=1e-2,Size max= ( Size ) 1000,bool v=false ) :
          __eps ( eps ),
          __min_rate_eps ( rate ),
          __max_iter ( max ),
          __verbosity ( v ) {
        GUM_CONSTRUCTOR ( ApproximationSettings );
      };
      
      ~ApproximationSettings() {
        GUM_DESTRUCTOR ( ApproximationSettings );
      };

      /// @}


      /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
      void setEpsilon ( double eps ) {
        __eps=eps;
      };

      double epsilon ( void ) const {
        return __eps;
      }

      /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
      void setMinEpsilonRate ( double rate ) {
        __min_rate_eps=rate;
      };

      double minEpsilonRate ( void ) const {
        return __min_rate_eps;
      };

      /// stopping criterion on number of iteration
      void setMaxIter ( Size max ) {
        __max_iter=max;
      };

      Size maxIter ( void ) const {
        return __max_iter;
      };

      /// verbosity
      void setVerbosity ( bool v ) {
        __verbosity=v;
      };

      bool verbosity ( void ) const {
        return __verbosity;
      };

    private:
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
