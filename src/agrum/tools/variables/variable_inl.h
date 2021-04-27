/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// protected copy

  INLINE void Variable::copy_(const Variable& aRV) {
    name__        = aRV.name__;
    description__ = aRV.description__;
  }

  /// constructor

  INLINE Variable::Variable(const std::string& aName, const std::string& aDesc) :
      name__(aName), description__(aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Variable);
  }

  /// copy constructor

  INLINE Variable::Variable(const Variable& aRV) :
      name__(aRV.name__), description__(aRV.description__) {
    // for debugging purposes
    GUM_CONS_CPY(Variable);
  }

  /// destructor

  INLINE Variable::~Variable() {
    // for debugging purposes
    GUM_DESTRUCTOR(Variable);
  }

  /// Copy operator

  INLINE Variable& Variable::operator=(const Variable& aRV) {
    // avoid self assignment
    if (this != &aRV) copy_(aRV);

    return *this;
  }

  /// sets the name__ of the variable

  INLINE void Variable::setName(const std::string& theValue) { name__ = theValue; }

  /// returns the name__ of the variable

  INLINE const std::string& Variable::name() const { return name__; }

  /// sets the description__ of the variable

  INLINE void Variable::setDescription(const std::string& theValue) const {
    description__ = theValue;
  }

  /// returns the description__ of the variable

  INLINE const std::string& Variable::description() const { return description__; }

  /// equality operator

  INLINE bool Variable::operator==(const Variable& aRV) const {
    return (name__ == aRV.name__);
  }

  /// inequality operator

  INLINE bool Variable::operator!=(const Variable& aRV) const {
    return (!operator==(aRV));
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
