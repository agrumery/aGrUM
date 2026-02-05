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
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/CN/polytope/LpInterface.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  INLINE Size HashFunc< credal::lp::LpCol >::castToSize(const credal::lp::LpCol& key) {
    return Size(key.id());
  }

  INLINE Size HashFunc< credal::lp::LpCol >::operator()(const credal::lp::LpCol& key) const {
    return (castToSize(key) * gum::HashFuncConst::gold) & this->hash_mask_;
  }

  namespace credal {
    namespace lp {
      /**
       * class LpCol
       */

      INLINE unsigned int LpCol::id() const { return _id_; }

      INLINE bool LpCol::operator<(const LpCol& col) const { return (_id_ < col.id()); }

      INLINE bool LpCol::operator==(const LpCol& col) const { return (_id_ == col.id()); }

      INLINE bool LpCol::operator!=(const LpCol& col) const { return (_id_ != col.id()); }

      INLINE LpCol& LpCol::operator=(const LpCol& col) {
        _id_ = col._id_;

        return *this;
      }

      INLINE std::ostream& operator<<(std::ostream& out, const LpCol& col) {
        out << col.toString();
        return out;
      }

      INLINE std::string LpCol::toString() const {
        std::string res = "V";
        return res.append(std::to_string(_id_));
      }

      INLINE void swap(HashTable< LpCol, double >*& a, HashTable< LpCol, double >*& b) {
        HashTable< LpCol, double >* tmp(a);
        a = b;
        b = tmp;
      }

      INLINE std::ostream& operator<<(std::ostream& out, const LpExpr& expr) {
        out << expr.toString();
        return out;
      }
    }   // namespace lp

  }   // namespace credal

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
