/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief gum::ApproximationSchemeListener header file.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/core/approximations/approximationSchemeListener.h>

namespace gum {

  ApproximationSchemeListener::ApproximationSchemeListener(
      IApproximationSchemeConfiguration& sch )
      : __sch( sch ) {
    GUM_CONSTRUCTOR( ApproximationSchemeListener );

    GUM_CONNECT( ( __sch ),
                 onProgress,
                 ( *this ),
                 ApproximationSchemeListener::whenProgress );
    GUM_CONNECT(
        ( __sch ), onStop, ( *this ), ApproximationSchemeListener::whenStop );
  }

  ApproximationSchemeListener::ApproximationSchemeListener(
      const ApproximationSchemeListener& other )
      : __sch( other.__sch ) {
    GUM_CONS_CPY( ApproximationSchemeListener );
    GUM_ERROR( OperationNotAllowed,
               "No copy constructor for ApproximationSchemeListener" );
  }

  ApproximationSchemeListener::~ApproximationSchemeListener() {
    GUM_DESTRUCTOR( ApproximationSchemeListener );
  }

  ApproximationSchemeListener& ApproximationSchemeListener::
  operator=( const ApproximationSchemeListener& other ) {
    GUM_CONS_CPY( ApproximationSchemeListener );
    GUM_ERROR( OperationNotAllowed,
               "No copy constructor for ApproximationSchemeListener" );
  }

}  // namespace
