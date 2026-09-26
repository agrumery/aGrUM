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


/** @file
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/K2.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/K2_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // Out-of-line on purpose (not INLINE, see K2_inl.h): under MSVC,
    // dllexport on a class forces eager, non-weak emission of every
    // inline-defined special member in *every* TU that includes the header --
    // colliding (LNK2005) with a leaf .pyd's own local reinstantiation of a
    // template that uses this class (GUM_NO_EXTERN_TEMPLATE_CLASS).

    /// default constructor
    K2::K2() {
      GUM_CONSTRUCTOR(K2);
    }

    /// copy constructor
    K2::K2(const K2& from) : GreedyHillClimbing(from), _order_(from._order_) {
      GUM_CONS_CPY(K2);
    }

    /// move constructor
    K2::K2(K2&& from) : GreedyHillClimbing(std::move(from)), _order_(std::move(from._order_)) {
      GUM_CONS_MOV(K2);
    }

    /// destructor
    K2::~K2() {
      GUM_DESTRUCTOR(K2);
    }

    /// copy operator
    K2& K2::operator=(const K2& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(from);
        _order_ = from._order_;
      }
      return *this;
    }

    /// move operator
    K2& K2::operator=(K2&& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(std::move(from));
        _order_ = std::move(from._order_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
