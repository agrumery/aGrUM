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
 * @brief Implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
// ============================================================================
#include <agrum/multidim/rangeVariable.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/multidim/rangeVariable.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


  // ==========================================================================
  // Default constructor
  // ==========================================================================
  RangeVariable::RangeVariable( const std::string& aName, 
                                const std::string& aDesc, 
                                Idx minVal, Idx maxVal ):
    DiscreteVariable(aName, aDesc), __min(minVal), __max(maxVal) {
    GUM_CONSTRUCTOR( RangeVariable );
  }
  
  // ==========================================================================
  // Copy Constructor.
  // If aDRV haves any listener, it will not be copied.
  // @param aDRV the variable we copy
  // ==========================================================================
  RangeVariable::RangeVariable( const RangeVariable& aDRV ):
    DiscreteVariable(aDRV), __min(aDRV.__min), __max(aDRV.__max) {
    GUM_CONS_CPY( RangeVariable );
  }
  
  // ==========================================================================
  // destructor
  // ==========================================================================
  RangeVariable::~RangeVariable() {
    GUM_DESTRUCTOR( RangeVariable );
  }


  const std::string RangeVariable::toString() const {
  
    std::stringstream s;
    s<<name() << " [" << min() << ", " << max() << "]";
    std::string res;
    s >> res;
    return res;
  }
} /* namespace gum */

