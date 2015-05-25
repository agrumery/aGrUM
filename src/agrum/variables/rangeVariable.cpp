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

#include <agrum/variables/rangeVariable.h>

#ifdef GUM_NO_INLINE
#include <agrum/variables/rangeVariable.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  // ==========================================================================
  // Default constructor
  // ==========================================================================
  RangeVariable::RangeVariable(const std::string &aName, const std::string &aDesc,
                               Idx minVal, Idx maxVal)
      : DiscreteVariable(aName, aDesc), __minBound(minVal), __maxBound(maxVal) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  // ==========================================================================
  // Copy Constructor.
  // If aDRV haves any listener, it will not be copied.
  // @param aDRV the variable we copy
  // ==========================================================================
  RangeVariable::RangeVariable(const RangeVariable &aDRV)
      : DiscreteVariable(aDRV), __minBound(aDRV.__minBound),
        __maxBound(aDRV.__maxBound) {
    GUM_CONS_CPY(RangeVariable);
  }

  // ==========================================================================
  // destructor
  // ==========================================================================
  RangeVariable::~RangeVariable() { GUM_DESTRUCTOR(RangeVariable); }

  const std::string RangeVariable::toString() const {
    std::stringstream s;
    s << name() << "[" << minVal() << "-" << maxVal() << "]";
    return s.str();
  }
} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
