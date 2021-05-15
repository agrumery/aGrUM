/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief base class for graph triangulations without constraints on nodes
 * elimination ordering.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/agrum.h>
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/unconstrainedEliminationSequenceStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/unconstrainedTriangulation.h>

namespace gum {

  // default constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
     const UnconstrainedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&                     JTStrategy,
     bool                                            minimality) :
      StaticTriangulation(elimSeq, JTStrategy, minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(UnconstrainedTriangulation);
  }

  // constructor with a given graph
  UnconstrainedTriangulation::UnconstrainedTriangulation(
     const UndiGraph*                                theGraph,
     const NodeProperty< Size >*                     domsizes,
     const UnconstrainedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&                     JTStrategy,
     bool                                            minimality) :
      StaticTriangulation(theGraph, domsizes, elimSeq, JTStrategy, minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(UnconstrainedTriangulation);
  }

  /// copy constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
     const UnconstrainedTriangulation& from) :
      StaticTriangulation(from) {   // for debugging purposes
    GUM_CONS_CPY(UnconstrainedTriangulation);
  }

  /// move constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
     UnconstrainedTriangulation&& from) :
      StaticTriangulation(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(UnconstrainedTriangulation);
  }

  /// destructor
  UnconstrainedTriangulation::~UnconstrainedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(UnconstrainedTriangulation);
  }

} /* namespace gum */
