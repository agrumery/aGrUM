
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
 * @brief Base Class for all elimination sequence algorithms used by
 * triangulations
 *
 * This class is the interface that should be implemented by all elimination
 * sequence algorithms used by triangulation algorithms.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {


  /// returns the current graph
  INLINE UndiGraph* EliminationSequenceStrategy::graph() const noexcept {
    return _graph;
  }


  /// returns the current domain sizes
  INLINE const NodeProperty< Size >*
               EliminationSequenceStrategy::domainSizes() const noexcept {
    return _domain_sizes;
  }


} /* namespace gum */
