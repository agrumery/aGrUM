/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>
#include <agrum/base/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// basic constructor. initialize the triangulation
  DefaultTriangulation::DefaultTriangulation(const UndiGraph*            theGraph,
                                             const NodeProperty< Size >* domsizes,
                                             bool                        minimality,
                                             double                      theRatio,
                                             double                      theThreshold) :
      UnconstrainedTriangulation(theGraph,
                                 domsizes,
                                 DefaultEliminationSequenceStrategy(),
                                 DefaultJunctionTreeStrategy(),
                                 minimality),
      _quasi_ratio_(theRatio), _threshold_(theThreshold) {   // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /// default constructor: initialize the triangulation for an empty graph
  DefaultTriangulation::DefaultTriangulation(bool   minimality,
                                             double theRatio,
                                             double theThreshold) :
      UnconstrainedTriangulation(DefaultEliminationSequenceStrategy(),
                                 DefaultJunctionTreeStrategy(),
                                 minimality),
      _quasi_ratio_(theRatio), _threshold_(theThreshold) {   // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /// copy constructor
  DefaultTriangulation::DefaultTriangulation(const DefaultTriangulation& from) :
      UnconstrainedTriangulation(from), _quasi_ratio_(from._quasi_ratio_),
      _threshold_(from._threshold_) {   // for debugging purposes
    GUM_CONS_CPY(DefaultTriangulation);
  }

  /// move constructor
  DefaultTriangulation::DefaultTriangulation(DefaultTriangulation&& from) :
      UnconstrainedTriangulation(std::move(from)), _quasi_ratio_(from._quasi_ratio_),
      _threshold_(from._threshold_) {
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
    return new DefaultTriangulation(isMinimalityRequired(), _quasi_ratio_, _threshold_);
  }

  /// virtual copy constructor
  DefaultTriangulation* DefaultTriangulation::copyFactory() const {
    return new DefaultTriangulation(*this);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
