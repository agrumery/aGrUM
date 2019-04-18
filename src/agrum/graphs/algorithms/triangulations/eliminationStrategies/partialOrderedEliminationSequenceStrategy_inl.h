
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
 * @brief Base class for all elimination sequence algorithm that impose a given
 * partial ordering on the nodes elimination sequence, that is, the set of all
 * the nodes is divided into several subsets. Within each subset, any ordering
 * can be chosen. But all the nodes of the first subset must be eliminated
 * before the nodes of the second, which must be eliminated before those of the
 * third subset, and so on.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {


  // returns the current partial ordering
  INLINE const List< NodeSet >*
               PartialOrderedEliminationSequenceStrategy::partialOrder() const noexcept {
    return _subsets;
  }


  // indicates if a new partial ordering is needed
  INLINE bool
     PartialOrderedEliminationSequenceStrategy::isPartialOrderNeeded() const
     noexcept {
    return _partial_order_needed;
  }


} /* namespace gum */
