
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
 * @brief Provides basic types used in aGrUM.
 *
 * @warning Do not include this file directlty : instead include
 * <agrum/config.h>.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_TYPES_H
#define GUM_TYPES_H

#include <climits>
#include <cstddef>

namespace gum {

  /// @ingroup configuration_group
  /// @{

  /**
   * In aGrUM, hashed values are unsigned long int. This is sufficient to deal
   * with Bayes nets and other graphs already big enough to be unmanageable
   * from a computational point of view.
   */
  typedef std::size_t Size;   // std::size_t Size;

  /**
   * Type for indexes.
   */
  typedef Size Idx;   // std::size_t Idx;


  /// @}

} /* namespace gum */

#endif /* GUM_TYPES_H */
