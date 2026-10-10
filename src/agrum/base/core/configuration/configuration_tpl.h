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
 * @brief Template definitions of gum::Configuration.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease automatic parsers
#include <cstdlib>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>

#include <type_traits>

namespace gum {

  template < typename T >
  T Configuration::getTyped(std::string_view section, std::string_view key) const {
    static_assert(std::is_same_v< T, bool > || std::is_same_v< T, int64_t >
                      || std::is_same_v< T, double > || std::is_same_v< T, std::string >,
                  "Configuration::getTyped<T>: T must be bool, int64_t, double or std::string");

    const auto [value, kind] = _typedRaw_(section, key);

    if constexpr (std::is_same_v< T, std::string >) {
      return value;
    } else if constexpr (std::is_same_v< T, bool >) {
      if (kind != Kind::BOOL) {
        GUM_ERROR(InvalidArgument, "'" << section << "." << key << "' is not a bool")
      }
      return value == "True";
    } else if constexpr (std::is_same_v< T, int64_t >) {
      if (kind != Kind::INT) {
        GUM_ERROR(InvalidArgument, "'" << section << "." << key << "' is not an int")
      }
      return static_cast< int64_t >(std::strtoll(value.c_str(), nullptr, 10));
    } else {
      if (kind != Kind::FLOAT && kind != Kind::INT) {
        GUM_ERROR(InvalidArgument, "'" << section << "." << key << "' is not a float")
      }
      return std::strtod(value.c_str(), nullptr);
    }
  }

}   // namespace gum
