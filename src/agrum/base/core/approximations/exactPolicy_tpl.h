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


/**
 * @file
 * @brief Inlined implementation of gum::ExactPolicy.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */

// To help IDE Parsers
#include <agrum/base/core/approximations/exactPolicy.h>

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
