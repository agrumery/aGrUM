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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsers in IDE
#include <agrum/variables/discreteVariable.h>

namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                         GUM_DISCRETE_VARIABLE                        === */
  /* ============================================================================ */
  /* ============================================================================ */

  /// Default constructor

  INLINE DiscreteVariable::DiscreteVariable(const std::string &aName,
                                            const std::string &aDesc)
      : Variable(aName, aDesc) {
    GUM_CONSTRUCTOR(DiscreteVariable);
  }

  /// Copy constructor

  INLINE DiscreteVariable::DiscreteVariable(const DiscreteVariable &aDRV)
      : Variable(aDRV) {
    GUM_CONSTRUCTOR(DiscreteVariable);
  }

  /// destructor

  INLINE DiscreteVariable::~DiscreteVariable() { GUM_DESTRUCTOR(DiscreteVariable); }

  /// Copy operator

  INLINE DiscreteVariable &DiscreteVariable::operator=(const DiscreteVariable &aRV) {
    if (&aRV != this) {
      Variable::operator=(aRV);
    }

    return *this;
  }

  /// equality operator

  INLINE bool DiscreteVariable::operator==(const DiscreteVariable &aRV) const {
    return (Variable::operator==(aRV) && (domainSize() == aRV.domainSize()));
  }

  /// inequality operator

  INLINE bool DiscreteVariable::operator!=(const DiscreteVariable &aRV) const {
    return (!operator==(aRV));
  }

  /// empty() <==> domainSize()<2

  INLINE bool DiscreteVariable::empty() const { return domainSize() < 2; }
} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
