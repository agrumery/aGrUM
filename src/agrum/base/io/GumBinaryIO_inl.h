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

#include <agrum/config.h>

#include <agrum/base/core/inline.h>
#include <agrum/base/io/GumBinaryIO.h>

namespace gum {

  INLINE std::vector< uint8_t > _readVector_(std::istream& is) {
    uint8_t bytes[8];
    is.read(reinterpret_cast< char* >(bytes), sizeof(bytes));
    if (!is) throw std::runtime_error("GumBinaryIO: failed to read vector size");
    uint64_t size = 0;
    for (std::size_t i = 0; i < sizeof(bytes); ++i)
      size |= static_cast< uint64_t >(bytes[i]) << (8 * i);

    // checked before the allocation: a corrupted size can not ask for more than the stream has
    const auto start = is.tellg();
    is.seekg(0, std::ios::end);
    const auto end = is.tellg();
    is.seekg(start);
    if (!is) throw std::runtime_error("GumBinaryIO: failed to find the size of the stream");
    if (size != static_cast< uint64_t >(end - start))
      throw std::runtime_error(
          "GumBinaryIO: the vector size does not match the size of the stream");

    std::vector< uint8_t > vec(size);
    is.read(reinterpret_cast< char* >(vec.data()), static_cast< std::streamsize >(size));
    if (!is) throw std::runtime_error("GumBinaryIO: failed to read vector data");
    return vec;
  }

  INLINE void _writeVector_(std::ostream& os, const std::vector< uint8_t >& vec) {
    const uint64_t size = vec.size();
    char           bytes[8];
    for (std::size_t i = 0; i < sizeof(bytes); ++i)
      bytes[i] = static_cast< char >((size >> (8 * i)) & 0xFF);
    os.write(bytes, sizeof(bytes));
    os.write(reinterpret_cast< const char* >(vec.data()), static_cast< std::streamsize >(size));
  }

}   // namespace gum
