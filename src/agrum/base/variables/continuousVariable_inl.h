/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <cctype>
#  include <cstdlib>

#  include <agrum/base/variables/continuousVariable.h>

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
