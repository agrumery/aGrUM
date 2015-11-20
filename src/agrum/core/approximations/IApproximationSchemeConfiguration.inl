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
 * @brief This file contains getters and setters definition for
 * ApproximationSchem settings.
 *
 * @see ApproximationScheme
 *
 * @author Pierre-Henri WUILLEMIN
 */

// To help IDE parsers
#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>

namespace gum {

  INLINE
  void IApproximationSchemeConfiguration::copyConfiguration(
      const IApproximationSchemeConfiguration& cfg ) {
    GUM_TRACE( "COPYING CONFIGURATION" );
  }

  INLINE
  std::string
  IApproximationSchemeConfiguration::messageApproximationScheme() const {
    std::stringstream s;

    switch ( stateApproximationScheme() ) {
      case ApproximationSchemeSTATE::Continue:
        s << "in progress";
        break;

      case ApproximationSchemeSTATE::Epsilon:
        s << "stopped with epsilon=" << epsilon();
        break;

      case ApproximationSchemeSTATE::Rate:
        s << "stopped with rate=" << minEpsilonRate();
        break;

      case ApproximationSchemeSTATE::Limit:
        s << "stopped with max iteration=" << maxIter();
        break;

      case ApproximationSchemeSTATE::TimeLimit:
        s << "stopped with timeout=" << maxTime();
        break;

      case ApproximationSchemeSTATE::Stopped:
        s << "stopped on request";
        break;

      case ApproximationSchemeSTATE::Undefined:
        s << "undefined state";
        break;
    };

    return s.str();
  }

}  // namespace gum
