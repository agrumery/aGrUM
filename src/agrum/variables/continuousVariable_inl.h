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
/** @file
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/variables/continuousVariable.h>

namespace gum {


  /// returns the GUM_SCALAR corresponding to a string, specialized for floats
  template <>
  INLINE float
  ContinuousVariable<float>::operator[] ( const std::string& str ) const {
    float value;
    try {
      value = std::stof ( str );
    }
    catch ( ... ) {
      GUM_ERROR ( TypeError, "cannot convert the string into a float" );
    }

    if ( belongs ( value ) )
      return value;
    else
      GUM_ERROR ( OutOfBounds,
                  "the value does not delong to the domain of the variable" );
  }


  /// returns the GUM_SCALAR corresponding to a string, specialized for doubles
  template <>
  ALWAYS_INLINE double
  ContinuousVariable<double>::operator[] ( const std::string& str ) const {
    double value;
    try {
      value = std::stod ( str );
    }
    catch ( ... ) {
      GUM_ERROR ( TypeError, "cannot convert the string into a float" );
    }

    if ( belongs ( value ) )
      return value;
    else
      GUM_ERROR ( OutOfBounds,
                  "the value does not delong to the domain of the variable" );
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
