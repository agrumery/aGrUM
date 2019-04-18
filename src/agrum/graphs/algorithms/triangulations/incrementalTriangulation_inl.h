
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
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/graphs/algorithms/triangulations/incrementalTriangulation.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===            CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS           ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */

  /// returns a junction tree corresponding to the current graph

  INLINE
  const CliqueGraph& IncrementalTriangulation::junctionTree() {
    updateTriangulation();
    return __junction_tree;
  }

  /// returns the junction tree of the maximal prime subgraphs

  INLINE
  const CliqueGraph& IncrementalTriangulation::maxPrimeSubgraphTree() {
    updateTriangulation();
    return __T_mpd;
  }

  /// returns the triangulation algorithm (useful for fine tuning it)

  INLINE const UnconstrainedTriangulation&
               IncrementalTriangulation::triangulationAlgo() const {
    return *__triangulation;
  }

  /// returns the current graph (that which is incrementally triangulated)

  INLINE const UndiGraph& IncrementalTriangulation::graph() const {
    return __graph;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
