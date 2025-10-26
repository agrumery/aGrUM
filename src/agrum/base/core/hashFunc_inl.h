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
#pragma once


/** @file
 * @brief Inlined implementation of the basic hash functions
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <string>
#include <utility>

#include <agrum/agrum.h>

// to ease parsing in IDE
#include <agrum/base/core/hashFunc.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* in aGrUM, the sizes of hash tables (number of slots) are powers of 2. This
   * is
   * not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose
   * this restriction. Function  _hashTableLog2_ thus returns the size in
   * bits - 1 necessary to store the smallest power of 2 greater than or
   * equal nb. */
  INLINE unsigned int _hashTableLog2_(const Size nb) {
    unsigned int i = 0;

    for (Size nbb = nb; nbb > Size(1); ++i, nbb >>= 1) {};

    return ((Size(1) << i) < nb ? i + Size(1) : i);
  }

  // ===========================================================================

  /// returns an unnormalized hashed key for hash tables whose keys are strings
  INLINE Size HashFunc< std::string >::castToSize(const std::string& key) {
    Size        h        = 0;
    Size        size     = Size(key.size());
    const char* char_ptr = key.c_str();
    const Size* int_ptr  = (const Size*)char_ptr;

    for (; size >= sizeof(Size); size -= sizeof(Size), ++int_ptr) {
      h = h * HashFuncConst::gold + *int_ptr;
    }

    for (char_ptr = (const char*)int_ptr; size != Size(0); --size, ++char_ptr) {
      h = Size(19) * h + Size(*char_ptr);
    }

    return h;
  }

  // Returns the hashed value of a key.
  INLINE Size HashFunc< std::string >::operator()(const std::string& key) const {
    return castToSize(key) & this->hash_mask_;
  }

  // ===========================================================================

  /// returns a hashed key for hash tables whose keys are vectors of Idx
  INLINE Size HashFunc< std::vector< Idx > >::castToSize(const std::vector< Idx >& key) {
    Size h    = Size(0);
    Size size = Size(key.size());
    for (Size i = Size(0); i < size; ++i)
      h += i * Size(key[i]);

    return h;
  }

  // Returns the hashed value of a key.
  INLINE Size HashFunc< std::vector< Idx > >::operator()(const std::vector< Idx >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }

  // ===========================================================================

  /// returns a hashed key for hash tables whose keys are Debugs
  INLINE Size HashFunc< Debug >::castToSize(const Debug& key) {
    Size h = Size(0);

    for (Size i = Size(0), j = Size(key.size()); i < j; ++i)
      h = Size(19) * h + Size(key[i]);

    return h;
  }

  // Returns the hashed value of a key.
  INLINE Size HashFunc< Debug >::operator()(const Debug& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
