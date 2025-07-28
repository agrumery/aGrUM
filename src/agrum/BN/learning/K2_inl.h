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
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE K2::K2() {
      GUM_CONSTRUCTOR(K2);
      ;
    }

    /// copy constructor
    INLINE K2::K2(const K2& from) : GreedyHillClimbing(from), _order_(from._order_) {
      GUM_CONS_CPY(K2);
    }

    /// move constructor
    INLINE K2::K2(K2&& from) :
        GreedyHillClimbing(std::move(from)), _order_(std::move(from._order_)) {
      GUM_CONS_MOV(K2);
    }

    /// destructor
    INLINE K2::~K2() {
      GUM_DESTRUCTOR(K2);
      ;
    }

    /// copy operator
    INLINE K2& K2::operator=(const K2& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(from);
        _order_ = from._order_;
      }
      return *this;
    }

    /// move operator
    INLINE K2& K2::operator=(K2&& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(std::move(from));
        _order_ = std::move(from._order_);
      }
      return *this;
    }

    /// sets the order on the variables
    INLINE void K2::setOrder(const Sequence< NodeId >& order) { _order_ = order; }

    /// sets the order on the variables
    INLINE void K2::setOrder(const std::vector< NodeId >& order) {
      _order_.clear();
      for (const auto node: order) {
        _order_.insert(node);
      }
    }

    /// returns the current order
    INLINE const Sequence< NodeId >& K2::order() const noexcept { return _order_; }

    /** @brief checks that the order passed to K2 is coherent with the variables
     * as specified by their modalities */
    INLINE void K2::_checkOrder_(const std::vector< Size >& modal) {
      if (modal.size() != _order_.size()) {
        GUM_ERROR(InvalidArgument,
                  "the number of elements in the order given "
                  "to K2 is not the same as the number of nodes");
      }
      bool order_ok = true;
      for (const auto node: _order_) {
        if (node >= _order_.size()) {
          order_ok = false;
          break;
        }
      }
      if (!order_ok) {
        GUM_ERROR(InvalidArgument,
                  "there exist at least one node in the order "
                  "given to K2 that has no domain size");
      }
    }

    /// returns the approximation policy of the learning algorithm
    INLINE ApproximationScheme& K2::approximationScheme() {
      return GreedyHillClimbing::approximationScheme();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
