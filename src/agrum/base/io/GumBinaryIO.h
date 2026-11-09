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
 * @brief Shared binary I/O helpers for GUM (jgum/bgum) serialization.
 *
 * Provides inline helpers to read/write a length-prefixed byte vector used by
 * the msgpack binary format (.bgum) and the default indentation of the text format
 * (.jgum).  Defined here once to avoid duplication
 * across GumBNReader/Writer, GumIDReader/Writer and GumMRFReader/Writer.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BINARY_IO_H
#define GUM_BINARY_IO_H

#include <cstdint>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <vector>

#include <agrum/config.h>

#include <string_view>

namespace gum {

  /**
   * Reads a length-prefixed byte vector from a binary stream (bgum format).
   *
   * The first 8 bytes encode the payload length as a little-endian uint64_t,
   * followed by that many payload bytes.
   *
   * @param is The input binary stream.
   * @return   The payload bytes.
   */
  GUM_SHARED_PUBLIC std::vector< uint8_t > _readVector_(std::istream& is);

  /**
   * Writes a length-prefixed byte vector to a binary stream (bgum format).
   *
   * @param os  The output binary stream.
   * @param vec The payload bytes to write.
   */
  GUM_SHARED_PUBLIC void _writeVector_(std::ostream& os, const std::vector< uint8_t >& vec);

  /**
   * The indentation used by the GUM writers (jgum text format) when none is given:
   * the value of the key `jgumIndent` of section `core` in gum::Configuration.
   *
   * @return The indentation (-1 is the most compact output).
   */
  GUM_SHARED_PUBLIC int defaultJgumIndent();

  /**
   * The format given by the extension of a GUM file (case insensitive).
   *
   * @param path The name of the file.
   * @return     true for ".bgum" (binary), false for ".jgum" (text), std::nullopt otherwise.
   */
  GUM_SHARED_PUBLIC std::optional< bool > binaryOfGUMExtension(std::string_view path);

  /**
   * The format of a GUM file: the explicit one if given, else the one of its extension (binary for
   * an extension other than ".jgum" and ".bgum").
   *
   * @param path   The name of the file.
   * @param binary The explicit format (true: binary .bgum, false: text .jgum), if any.
   * @return       true if the file is in the binary format.
   * @throw InvalidArgument if binary contradicts the extension (".jgum" or ".bgum") of path.
   */
  GUM_SHARED_PUBLIC bool isBinaryGUMFile(std::string_view      path,
                                         std::optional< bool > binary = std::nullopt);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/base/io/GumBinaryIO_inl.h>
#endif

#endif   // GUM_BINARY_IO_H
