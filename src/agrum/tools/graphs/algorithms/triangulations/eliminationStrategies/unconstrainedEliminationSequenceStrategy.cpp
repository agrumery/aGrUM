
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
 * @brief Base Class for all elimination sequence algorithms that require only
 *the
 * graph to be triangulated and the nodes domain sizes to produce the node
 * elimination ordering.
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/unconstrainedEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor
  UnconstrainedEliminationSequenceStrategy::
     UnconstrainedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR(UnconstrainedEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  UnconstrainedEliminationSequenceStrategy::
     UnconstrainedEliminationSequenceStrategy(
        UndiGraph* graph, const NodeProperty< Size >* dom_sizes) :
      EliminationSequenceStrategy(graph, dom_sizes) {
    // for debugging purposes
    GUM_CONSTRUCTOR(UnconstrainedEliminationSequenceStrategy);
  }

  /// copy constructor
  UnconstrainedEliminationSequenceStrategy::
     UnconstrainedEliminationSequenceStrategy(
        const UnconstrainedEliminationSequenceStrategy& from) :
      EliminationSequenceStrategy(from) {
    // for debugging purposes
    GUM_CONS_CPY(UnconstrainedEliminationSequenceStrategy);
  }

  /// move constructor
  UnconstrainedEliminationSequenceStrategy::
     UnconstrainedEliminationSequenceStrategy(
        UnconstrainedEliminationSequenceStrategy&& from) :
      EliminationSequenceStrategy(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(UnconstrainedEliminationSequenceStrategy);
  }

  /// destructor
  UnconstrainedEliminationSequenceStrategy::
     ~UnconstrainedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(UnconstrainedEliminationSequenceStrategy);
  }

} /* namespace gum */
