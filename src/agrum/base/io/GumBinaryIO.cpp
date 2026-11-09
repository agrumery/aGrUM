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


#include "agrum/base/io/GumBinaryIO.h"

#include <algorithm>
#include <cctype>
#include <climits>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>

namespace gum {

  int defaultJgumIndent() {
    const auto indent = Configuration::instance().typed< std::int64_t >("core", "jgumIndent");
    return static_cast< int >(std::clamp< std::int64_t >(indent, -1, INT_MAX));
  }

  std::optional< bool > binaryOfGUMExtension(std::string_view path) {
    const auto hasExtension = [path](std::string_view ext) {
      return path.size() >= ext.size()
          && std::equal(ext.begin(), ext.end(), path.end() - ext.size(), [](char a, char b) {
               return a == std::tolower(static_cast< unsigned char >(b));
             });
    };
    if (hasExtension(".bgum")) return true;
    if (hasExtension(".jgum")) return false;
    return std::nullopt;
  }

  bool isBinaryGUMFile(std::string_view path, std::optional< bool > binary) {
    const auto fromExtension = binaryOfGUMExtension(path);
    if (!binary.has_value()) return fromExtension.value_or(true);
    if (fromExtension.has_value() && *fromExtension != *binary)
      GUM_ERROR(InvalidArgument,
                "The file '" << path << "' can not be written or read in the "
                             << (*binary ? "binary" : "text") << " GUM format: its extension is "
                             << (*binary ? ".jgum" : ".bgum"))
    return *binary;
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/base/io/GumBinaryIO_inl.h>
#endif   // GUM_NO_INLINE
