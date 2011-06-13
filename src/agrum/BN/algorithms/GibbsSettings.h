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
 * @brief This file contains getters and setters for specific Gibbs approximation settings
 * @author Pierre-Henri Wuillemin
 */
#ifndef GUM_GIBBS_SETTINGS_H
#define GUM_GIBBS_SETTINGS_H

#include <agrum/BN/algorithms/approximationSettings.h>

namespace gum {

  class GibbsSettings : public ApproximationSettings {
    public:
      /// Constructors and Destructors
      /// @{
      GibbsSettings ( double eps=5e-2,double rate=1e-2,Size max= ( Size ) 1000,bool v=false,Size burn=1000,Size period=10 ) :
          ApproximationSettings ( eps,rate,max,v ),
          __burn_in ( burn ),
          __periode_size ( period ) {
        GUM_CONSTRUCTOR ( GibbsSettings );
      };

      ~GibbsSettings() {
        GUM_DESTRUCTOR ( GibbsSettings );
      };

      /// @}

      /// size of burn in on number of iteration
      void setBurnIn ( Size b ) {__burn_in=b;};

      Size burnIn ( void ) const {return __burn_in;};

      /// how many samples between 2 stopping tests
      void setPeriodeSize ( Size p ) {__periode_size=p;};

      Size periodeSize ( void ) const {return __periode_size;};

    private:

      Size __burn_in;
      Size __periode_size;
    };
}//namespace gum

#endif //GUM_GIBBS_SETTINGS_H


// kate: indent-mode cstyle; space-indent on; indent-width 2; 
