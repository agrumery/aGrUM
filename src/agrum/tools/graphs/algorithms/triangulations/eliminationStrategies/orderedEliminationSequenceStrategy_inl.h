
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
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
 * @brief An Elimination sequence algorithm that imposes a given complete
 * ordering on the nodes elimination sequence
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

namespace gum {


  // returns the current complete ordering
  INLINE const std::vector< NodeId >*
               OrderedEliminationSequenceStrategy::order() const noexcept {
    return __order;
  }


  // indicates whether a new complete ordering is needed
  INLINE bool OrderedEliminationSequenceStrategy::isOrderNeeded() const noexcept {
    return __order_needed;
  }


} /* namespace gum */