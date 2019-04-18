
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
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/agrum.h>
#include <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/graphs/algorithms/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>
#include <agrum/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// basic constructor. initialize the triangulation
  DefaultTriangulation::DefaultTriangulation(const UndiGraph*            theGraph,
                                             const NodeProperty< Size >* domsizes,
                                             bool   minimality,
                                             double theRatio,
                                             double theThreshold) :
      UnconstrainedTriangulation(theGraph,
                                 domsizes,
                                 DefaultEliminationSequenceStrategy(),
                                 DefaultJunctionTreeStrategy(),
                                 minimality),
      __quasi_ratio(theRatio), __threshold(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /// default constructor: initialize the triangulation for an empty graph
  DefaultTriangulation::DefaultTriangulation(bool   minimality,
                                             double theRatio,
                                             double theThreshold) :
      UnconstrainedTriangulation(DefaultEliminationSequenceStrategy(),
                                 DefaultJunctionTreeStrategy(),
                                 minimality),
      __quasi_ratio(theRatio), __threshold(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /// copy constructor
  DefaultTriangulation::DefaultTriangulation(const DefaultTriangulation& from) :
      UnconstrainedTriangulation(from), __quasi_ratio(from.__quasi_ratio),
      __threshold(from.__threshold) {
    // for debugging purposes
    GUM_CONS_CPY(DefaultTriangulation);
  }

  /// move constructor
  DefaultTriangulation::DefaultTriangulation(DefaultTriangulation&& from) :
      UnconstrainedTriangulation(std::move(from)),
      __quasi_ratio(from.__quasi_ratio), __threshold(from.__threshold) {
    // for debugging purposes
    GUM_CONS_MOV(DefaultTriangulation);
  }

  /// destructor
  DefaultTriangulation::~DefaultTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultTriangulation);
  }

  /// virtual clone constructor
  DefaultTriangulation* DefaultTriangulation::newFactory() const {
    return new DefaultTriangulation(
       isMinimalityRequired(), __quasi_ratio, __threshold);
  }

  /// virtual copy constructor
  DefaultTriangulation* DefaultTriangulation::copyFactory() const {
    return new DefaultTriangulation(*this);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
