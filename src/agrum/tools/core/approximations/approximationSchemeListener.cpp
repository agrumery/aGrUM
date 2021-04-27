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


/**
 * @file
 * @brief gum::ApproximationSchemeListener header file.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/approximations/approximationSchemeListener.h>

namespace gum {

  ApproximationSchemeListener::ApproximationSchemeListener(
     IApproximationSchemeConfiguration& sch) :
      sch__(sch) {
    GUM_CONSTRUCTOR(ApproximationSchemeListener);

    GUM_CONNECT((sch__),
                onProgress,
                (*this),
                ApproximationSchemeListener::whenProgress);
    GUM_CONNECT((sch__), onStop, (*this), ApproximationSchemeListener::whenStop);
  }

  ApproximationSchemeListener::ApproximationSchemeListener(
     const ApproximationSchemeListener& other) :
      sch__(other.sch__) {
    GUM_CONS_CPY(ApproximationSchemeListener);
    GUM_ERROR(OperationNotAllowed,
              "No copy constructor for ApproximationSchemeListener");
  }

  ApproximationSchemeListener::~ApproximationSchemeListener() {
    GUM_DESTRUCTOR(ApproximationSchemeListener);
  }

  ApproximationSchemeListener& ApproximationSchemeListener::operator=(
     const ApproximationSchemeListener& other) {
    GUM_CONS_CPY(ApproximationSchemeListener);
    GUM_ERROR(OperationNotAllowed,
              "No copy constructor for ApproximationSchemeListener");
  }

}   // namespace gum
