/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief d-separation analysis (as described in Koller & Friedman 2009)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {


  // default constructor
  INLINE dSeparationAlgorithm::dSeparationAlgorithm() {
    GUM_CONSTRUCTOR(dSeparationAlgorithm);
    ;
  }


  // copy constructor
  INLINE dSeparationAlgorithm::dSeparationAlgorithm(const dSeparationAlgorithm& from) {
    GUM_CONS_CPY(dSeparationAlgorithm);
  }


  // move constructor
  INLINE dSeparationAlgorithm::dSeparationAlgorithm(dSeparationAlgorithm&& from) {
    GUM_CONS_MOV(dSeparationAlgorithm);
  }


  // destructor
  INLINE dSeparationAlgorithm::~dSeparationAlgorithm() {
    GUM_DESTRUCTOR(dSeparationAlgorithm);
    ;
  }


  // copy operator
  INLINE dSeparationAlgorithm& dSeparationAlgorithm::operator=(const dSeparationAlgorithm& from) {
    return *this;
  }


  // move operator
  INLINE dSeparationAlgorithm& dSeparationAlgorithm::operator=(dSeparationAlgorithm&& from) {
    return *this;
  }


} /* namespace gum */
