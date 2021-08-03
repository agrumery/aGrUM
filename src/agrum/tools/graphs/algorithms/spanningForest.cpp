/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Interface for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(_at_AMU)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphs/algorithms/spanningForest.h>

namespace gum {

  SpanningForest::SpanningForest() {
    GUM_CONSTRUCTOR(SpanningForest);
    ;
  }

  SpanningForest::~SpanningForest() {
    GUM_DESTRUCTOR(SpanningForest);
    ;
  }

  SpanningForest::SpanningForest(const SpanningForest& toCopy) { GUM_CONS_CPY(SpanningForest); }

  SpanningForest& SpanningForest::operator=(const SpanningForest& toCopy) { return *this; }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
