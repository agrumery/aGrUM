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


/**
 * @file
 * @brief Parsing of the GUM formats (jgum text, bgum msgpack) into a JSON value.
 *
 * nlohmann::json keeps the last value of a key written twice in an object, silently (RFC 8259
 * leaves it to the implementation). The GUM readers (graphs and graphical models) parse with these
 * functions, which refuse it, in the text format as in the binary one.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_GUM_JSON_H
#define GUM_GUM_JSON_H

#include <cstdint>
#include <istream>
#include <string_view>
#include <vector>

#include <agrum/config.h>

#include <agrum/base/external/json/json.hpp>

namespace gum::detail {
  /**
   * Parses a GUM text (JSON).
   *
   * Implementation detail of the GUM readers: declared here because the readers of the graphical
   * models are templates.
   *
   * @param text The JSON text.
   * @return     The JSON value.
   * @throw IOError if @p text is not a JSON text or if an object has the same key twice.
   */
  GUM_SHARED_PUBLIC nlohmann::json parseGumText(std::string_view text);

  /**
   * Parses a GUM text (JSON) read from a stream, up to its end.
   *
   * @param input The stream.
   * @return      The JSON value.
   * @throw IOError if the stream is not a JSON text or if an object has the same key twice.
   */
  GUM_SHARED_PUBLIC nlohmann::json parseGumText(std::istream& input);

  /**
   * Parses the payload of a binary GUM file (msgpack), as given by gum::detail::readVector.
   *
   * @param payload The msgpack bytes.
   * @return        The JSON value.
   * @throw IOError if @p payload is not a msgpack content or if a map has the same key twice.
   */
  GUM_SHARED_PUBLIC nlohmann::json parseGumBinary(const std::vector< uint8_t >& payload);
}   // namespace gum::detail

#endif   // GUM_GUM_JSON_H
