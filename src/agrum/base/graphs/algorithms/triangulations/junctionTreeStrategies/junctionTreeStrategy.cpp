/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Base Class for all the algorithms producing a junction given a set
 * of cliques/subcliques resulting from a triangulation
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/graphs/algorithms/triangulations/junctionTreeStrategies/junctionTreeStrategy.h>

namespace gum {

  // default constructor
  JunctionTreeStrategy::JunctionTreeStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR(JunctionTreeStrategy);
  }

  // copy constructor
  JunctionTreeStrategy::JunctionTreeStrategy(const JunctionTreeStrategy& from) :
      triangulation_(from.triangulation_) {   // for debugging purposes
    GUM_CONS_CPY(JunctionTreeStrategy);
  }

  // move constructor
  JunctionTreeStrategy::JunctionTreeStrategy(JunctionTreeStrategy&& from) :
      triangulation_(from.triangulation_) {
    // for debugging purposes
    GUM_CONS_MOV(JunctionTreeStrategy);
  }

  // destructor
  JunctionTreeStrategy::~JunctionTreeStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(JunctionTreeStrategy);
  }

  /** @brief assigns a new triangulation to the junction tree strategy
   * during a move construction */
  void JunctionTreeStrategy::moveTriangulation(StaticTriangulation* triangulation) {
    triangulation_ = triangulation;
  }

} /* namespace gum */
