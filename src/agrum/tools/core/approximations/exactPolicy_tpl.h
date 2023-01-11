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
 * @brief Inlined implementation of gum::ExactPolicy.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */

// To help IDE Parsers
#include <agrum/tools/core/approximations/exactPolicy.h>

namespace gum {

  // Default constructor.
  template < typename GUM_SCALAR >
  ExactPolicy< GUM_SCALAR >::ExactPolicy() {}

  // @brief Convert value to approximation representation.
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ExactPolicy< GUM_SCALAR >::fromExact(const GUM_SCALAR& value) const {
    return value;
  }

  // @brief Combine using addition with the given gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineAdd(const ApproximationPolicy< GUM_SCALAR >* ap) {}

  // @brief Combine using substraction with the given
  // gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineSub(const ApproximationPolicy< GUM_SCALAR >* ap) {}

  // @brief Combine using multiplication with the given
  // gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineMult(const ApproximationPolicy< GUM_SCALAR >* ap) {}

  // @brief Combine using division with the given gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineDiv(const ApproximationPolicy< GUM_SCALAR >* ap) {}

  // @brief Combine using max with the given gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineMax(const ApproximationPolicy< GUM_SCALAR >* ap) {}

  // @brief Combine using min with the given gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  void ExactPolicy< GUM_SCALAR >::combineMin(const ApproximationPolicy< GUM_SCALAR >* ap) {}

}   // namespace gum
