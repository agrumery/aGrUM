/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/** @file
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <cctype>
#  include <cstdlib>

#  include <agrum/tools/variables/continuousVariable.h>

namespace gum {


  /// returns the GUM_SCALAR corresponding to a string, specialized for floats
  template <>
  INLINE float ContinuousVariable< float >::operator[](const std::string& str) const {
    float       value;
    std::size_t pos;
    try {
      value = std::stof(str, &pos);
    } catch (std::invalid_argument&) {
      GUM_ERROR(TypeError, "the value is not a number")
    } catch (std::out_of_range&) { GUM_ERROR(OutOfBounds, "the value is too huge") }

    // check whether there remains non-space unprocessed characters
    for (auto iter = str.begin() + pos, end = str.end(); iter != end; ++iter) {
      if (!std::isspace(static_cast< unsigned char >(*iter)))
        GUM_ERROR(TypeError, "the value is not a number")
    }

    if (belongs(value)) return value;
    else GUM_ERROR(OutOfBounds, "the value does not belong to the domain of the variable")
  }


  /// returns the GUM_SCALAR corresponding to a string, specialized for doubles
  template <>
  INLINE double ContinuousVariable< double >::operator[](const std::string& str) const {
    double      value;
    std::size_t pos;
    try {
      value = std::stod(str, &pos);
    } catch (std::invalid_argument&) {
      GUM_ERROR(TypeError, "the value is not a number")
    } catch (std::out_of_range&) { GUM_ERROR(OutOfBounds, "the value is too huge") }

    // check whether there remains non-space unprocessed characters
    for (auto iter = str.begin() + pos, end = str.end(); iter != end; ++iter) {
      if (!std::isspace(static_cast< unsigned char >(*iter)))
        GUM_ERROR(TypeError, "the value is not a number")
    }

    if (belongs(value)) return value;
    else GUM_ERROR(OutOfBounds, "the value does not belong to the domain of the variable")
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
