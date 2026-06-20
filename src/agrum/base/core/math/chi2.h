/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LEARNING_CHI2_H
#define GUM_LEARNING_CHI2_H

#include <agrum/agrum.h>

#include <agrum/base/core/math/math_utils.h>

#define GUM_LEARNING_CONFIDENCE_PROBA 0.05

namespace gum {

  // =========================================================================
  // ===                             CHI2 CLASS                            ===
  // =========================================================================

  /**
   * @class Chi2
   * @headerfile chi2.h <agrum/base/core/math/chi2.h>
   * @brief Static math utilities for the chi2 distribution.
   * @ingroup math_group
   */
  class Chi2 {
    public:
    // ==========================================================================
    /// @name Static math utilities
    // ==========================================================================
    /// @{

    /**
     * @brief Computes the probability of chi2 value.
     *
     * This code has been written by Gary Perlman.
     *
     * ALGORITHM Compute probability of chi square value.
     * Adapted from:
     * Hill, I. D. and Pike, M. C.  Algorithm 299
     * Collected Algorithms for the CACM 1967 p. 243
     * Updated for rounding errors based on remark in
     * ACM TOMS June 1985, page 185
     *
     * @param x The chi2 value.
     * @param df The number of degrees of freedom.
     * @return The probability of x given df degrees of freedom.
     */
    static double probaChi2(double x, Size df);

    /**
     * @brief Computes the critical chi2 value for a given confidence
     * probability and number of degrees of freedom.
     *
     * This code has been written by Gary Perlman.
     *
     * @param proba The confidence probability.
     * @param df The number of degrees of freedom.
     * @return The critical chi2 value.
     */
    static double criticalValue(double proba, Size df);

    /// @}

    private:
    Chi2() = delete;

    /**
     * @brief Computes the probability of normal z value.
     *
     * This code has been written by Gary Perlman.
     *
     * ALGORITHM Adapted from a polynomial approximation in:
     * Ibbetson D, Algorithm 209
     * Collected Algorithms of the CACM 1963 p. 616
     *
     * This routine has six digit accuracy, so it is only useful for absolute z
     * values < 6.  For z values >= to 6.0,  _probaZValue_() returns 0.0.
     *
     * @param z A value.
     * @return The probability of z.
     */
    static double _probaZValue_(double z);
  };

} /* namespace gum */

#endif /* GUM_LEARNING_CHI2_H */
