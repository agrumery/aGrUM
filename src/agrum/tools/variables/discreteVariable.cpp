
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/tools/variables/discreteVariable.h>
#include <sstream>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/variables/discreteVariable_inl.h>
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  const std::string DiscreteVariable::toStringWithDescription() const {
    std::stringstream s;
    s << description();
    s << domain();

    return s.str();
  }

  const std::string DiscreteVariable::toString() const {
    std::stringstream s;
    s << name();
    s << domain();

    return s.str();
  }

  /// for friendly displaying the content of the variable

  std::ostream& operator<<(std::ostream& s, const DiscreteVariable& DRV) {
    s << DRV.toString();
    return s;
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
