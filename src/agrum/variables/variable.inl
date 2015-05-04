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

namespace gum {

  /// protected copy

  INLINE void Variable::_copy(const Variable &aRV) {
    __name = aRV.__name;
    __description = aRV.__description;
  }

  /// constructor

  INLINE Variable::Variable(const std::string &aName, const std::string &aDesc)
      : __name(aName), __description(aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Variable);
  }

  /// copy constructor

  INLINE Variable::Variable(const Variable &aRV)
      : __name(aRV.__name), __description(aRV.__description) {
    // for debugging purposes
    GUM_CONS_CPY(Variable);
  }

  /// destructor

  INLINE Variable::~Variable() {
    // for debugging purposes
    GUM_DESTRUCTOR(Variable);
  }

  /// Copy operator

  INLINE Variable &Variable::operator=(const Variable &aRV) {
    // avoid self assignment
    if (this != &aRV)
      _copy(aRV);

    return *this;
  }

  /// sets the __name of the variable

  INLINE void Variable::setName(const std::string &theValue) { __name = theValue; }

  /// returns the __name of the variable

  INLINE const std::string &Variable::name() const { return __name; }

  /// sets the __description of the variable

  INLINE void Variable::setDescription(const std::string &theValue) const {
    __description = theValue;
  }

  /// returns the __description of the variable

  INLINE const std::string &Variable::description() const { return __description; }

  /// equality operator

  INLINE bool Variable::operator==(const Variable &aRV) const {
    return (__name == aRV.__name);
  }

  /// inequality operator

  INLINE bool Variable::operator!=(const Variable &aRV) const {
    return (!operator==(aRV));
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
