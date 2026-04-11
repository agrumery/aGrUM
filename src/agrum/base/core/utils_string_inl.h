/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Contains useful methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <algorithm>
#include <ranges>

// to ease IDE parser
#include <agrum/base/core/utils_string.h>

namespace gum {
  INLINE

  std::string toLower(std::string_view str) {
    std::string result{str};
    std::ranges::transform(result, result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
  }

  INLINE
  bool contains(std::string_view s, std::string_view needle) {
    return s.find(needle) != std::string::npos;
  }

  INLINE
  void ltrim(std::string& s) {
    s.erase(s.begin(),
            std::ranges::find_if_not(s, [](unsigned char ch) { return std::isspace(ch); }));
  }

  INLINE
  void rtrim(std::string& s) {
    auto it = std::ranges::find_if_not(s | std::views::reverse,
                                       [](unsigned char ch) { return std::isspace(ch); });
    s.erase(it.base(), s.end());
  }

  INLINE
  void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
  }

  INLINE
  std::string remove_newline(std::string_view s) {
    std::string res{s};
    std::erase(res, '\n');
    std::erase(res, '\r');
    return res;
  }

  INLINE
  bool isInteger(std::string_view val) { return isIntegerWithResult(val, nullptr); }

  INLINE
  bool isNumerical(std::string_view val) { return isNumericalWithResult(val, nullptr); }

  INLINE
  std::string trim_copy(std::string_view s) {
    std::string res{s};
    trim(res);
    return res;
  }
} /* namespace gum */
