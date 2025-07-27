/****************************************************************************
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Classes used to practice approximation on value
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */
#ifndef GUM_APPROXIMATION_POLICY_H
#define GUM_APPROXIMATION_POLICY_H
#include <agrum/agrum.h>

namespace gum {

  /**
   * @class ApproximationPolicy
   * @brief Mother class for all approximation policy classes.
   * @ingroup approximationpolicy_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/base/core/approximations/approximationPolicy.h>
   * @endcode
   */
  template < typename GUM_SCALAR >
  class ApproximationPolicy {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    ApproximationPolicy() {};

    /**
     *  @brief Copy constructor.
     */
    ApproximationPolicy(const ApproximationPolicy< GUM_SCALAR >* md) {};

    /// @}
    // ===========================================================================
    /// @name Accessors / Modifiers
    // ===========================================================================
    /// @{

    /**
     * Convert value to his approximation. This method, at least in release
     * mode, should not verify the limits
     * @param value The converted value.
     * @return The value approximation representation.
     */
    virtual GUM_SCALAR fromExact(const GUM_SCALAR& value) const = 0;

    /**
     * @brief Combine using addition with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineAdd(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /**
     * @brief Combine using substraction with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineSub(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /**
     * @brief Combine using multiplication with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMult(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /**
     * @brief Combine using division with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineDiv(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /**
     * @brief Combine using max with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMax(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /**
     * @brief Combine using min with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMin(const ApproximationPolicy< GUM_SCALAR >* ap) = 0;

    /// @}
  };
}   // namespace gum

#endif /* GUM_APPROXIMATION_POLICY_H */
