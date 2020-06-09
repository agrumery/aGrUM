
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

namespace gum {


  // default constructor
  INLINE dSeparation::dSeparation() { GUM_CONSTRUCTOR(dSeparation); }


  // copy constructor
  INLINE dSeparation::dSeparation(const dSeparation& from) {
    GUM_CONS_CPY(dSeparation);
  }


  // move constructor
  INLINE dSeparation::dSeparation(dSeparation&& from) {
    GUM_CONS_MOV(dSeparation);
  }


  // destructor
  INLINE dSeparation::~dSeparation() { GUM_DESTRUCTOR(dSeparation); }


  // copy operator
  INLINE dSeparation& dSeparation::operator=(const dSeparation& from) {
    return *this;
  }


  // move operator
  INLINE dSeparation& dSeparation::operator=(dSeparation&& from) { return *this; }


} /* namespace gum */
