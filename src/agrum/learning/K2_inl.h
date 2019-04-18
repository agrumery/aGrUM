
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE K2::K2() { GUM_CONSTRUCTOR(K2); }

    /// copy constructor
    INLINE K2::K2(const K2& from) :
        GreedyHillClimbing(from), __order(from.__order) {
      GUM_CONS_CPY(K2);
    }

    /// move constructor
    INLINE K2::K2(K2&& from) :
        GreedyHillClimbing(std::move(from)), __order(std::move(from.__order)) {
      GUM_CONS_MOV(K2);
    }

    /// destructor
    INLINE K2::~K2() { GUM_DESTRUCTOR(K2); }

    /// copy operator
    INLINE K2& K2::operator=(const K2& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(from);
        __order = from.__order;
      }
      return *this;
    }

    /// move operator
    INLINE K2& K2::operator=(K2&& from) {
      if (this != &from) {
        GreedyHillClimbing::operator=(std::move(from));
        __order = std::move(from.__order);
      }
      return *this;
    }

    /// sets the order on the variables
    INLINE void K2::setOrder(const Sequence< NodeId >& order) { __order = order; }

    /// sets the order on the variables
    INLINE void K2::setOrder(const std::vector< NodeId >& order) {
      __order.clear();
      for (const auto node : order) {
        __order.insert(node);
      }
    }

    /// returns the current order
    INLINE const Sequence< NodeId >& K2::order() const noexcept { return __order; }

    /** @brief checks that the order passed to K2 is coherent with the variables
     * as specified by their modalities */
    INLINE void K2::__checkOrder(const std::vector< Size >& modal) {
      if (modal.size() != __order.size()) {
        GUM_ERROR(InvalidArgument,
                  "the number of elements in the order given "
                  "to K2 is not the same as the number of nodes");
      }
      bool order_ok = true;
      for (const auto node : __order) {
        if (node >= __order.size()) {
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
