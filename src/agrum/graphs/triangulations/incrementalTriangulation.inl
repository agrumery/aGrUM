/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/** @file
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <agrum/graphs/triangulations/incrementalTriangulation.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===            CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS           === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// returns a junction tree corresponding to the current graph

  INLINE
  const CliqueGraph &IncrementalTriangulation::junctionTree() {
    updateTriangulation();
    return __junction_tree;
  }

  /// returns the junction tree of the maximal prime subgraphs

  INLINE
  const CliqueGraph &IncrementalTriangulation::maxPrimeSubgraphTree() {
    updateTriangulation();
    return __T_mpd;
  }

  /// returns the triangulation algorithm (useful for fine tuning it)

  INLINE const UnconstrainedTriangulation &
  IncrementalTriangulation::triangulationAlgo() const {
    return *__triangulation;
  }

  /// returns the current graph (that which is incrementally triangulated)

  INLINE const UndiGraph &IncrementalTriangulation::graph() const { return __graph; }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
