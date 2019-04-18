
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


/**
 * @file
 * @brief Inline implementation for the abstract base class for all multi
 * dimensionnal containers.
 *
 * MultiDimAdressable is the abstract base class for all multi dimensionnal
 * adressable. Its purpose is to deal with slave Instantiation and notification
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

// to ease IDE parser
#include <agrum/multidim/implementations/multiDimAdressable.h>

namespace gum {

  INLINE
  MultiDimAdressable::MultiDimAdressable(MultiDimAdressable&& from) noexcept :
      MultiDimInterface(std::forward< MultiDimInterface >(from)) {
    GUM_CONS_MOV(MultiDimAdressable);
  }

  INLINE MultiDimAdressable::MultiDimAdressable() {
    GUM_CONSTRUCTOR(MultiDimAdressable);
  }

  INLINE
  MultiDimAdressable::MultiDimAdressable(const MultiDimAdressable& from) {
    GUM_CONS_CPY(MultiDimAdressable);
  }

  INLINE MultiDimAdressable::~MultiDimAdressable() {
    GUM_DESTRUCTOR(MultiDimAdressable);
  }

  INLINE MultiDimAdressable& MultiDimAdressable::
                             operator=(const MultiDimAdressable& from) {
    GUM_OP_CPY(MultiDimAdressable);
    MultiDimInterface::operator=(from);
    return *this;
  }
  INLINE MultiDimAdressable& MultiDimAdressable::
                             operator=(MultiDimAdressable&& from) noexcept {
    GUM_OP_MOV(MultiDimAdressable);
    MultiDimInterface::operator=(std::forward< MultiDimInterface >(from));
    return *this;
  }

} /* namespace gum */
