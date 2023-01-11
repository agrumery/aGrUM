/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Provides basic types used in aGrUM.
 *
 * @warning Do not include this file directly : instead include
 * <agrum/config.h>.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_TYPES_H
#define GUM_TYPES_H

#include <climits>
#include <cstddef>


#ifdef SWIG
#  define GUM_NODISCARD
#else
#  define GUM_NODISCARD [[nodiscard]]
#endif   // SWIG

namespace gum {

  /// @ingroup configuration_group
  /// @{

  /**
   * In aGrUM, hashed values are unsigned long int. This is sufficient to deal
   * with Bayes nets and other graphs already big enough to be unmanageable
   * from a computational point of view.
   */
  using Size = std::size_t;

  /**
   * Type for indexes.
   */
  using Idx = Size;


  /// @}

} /* namespace gum */

#endif /* GUM_TYPES_H */
