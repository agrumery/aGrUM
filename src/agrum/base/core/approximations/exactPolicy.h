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
 * @brief Classes used to practice approximation on value
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */
#ifndef GUM_EXACT_POLICY_H
#define GUM_EXACT_POLICY_H
#include <agrum/agrum.h>

#include <agrum/base/core/approximations/approximationPolicy.h>

namespace gum {

  /**
   * @class ExactPolicy
   * @headerfile exactPolicy.h <agrum/base/core/approximations/exactPolicy.h>
   * @brief Class implementing exact approximation policy (meaning a value is
   * approximate to itself).
   * @ingroup approximationpolicy_group
   *
   * @tparam GUM_SCALAR The type used for computations.
   */
  template < typename GUM_SCALAR >
  class ExactPolicy: public virtual ApproximationPolicy< GUM_SCALAR > {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    ExactPolicy();

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{

    /**
     * @brief Convert value to approximation representation.
     * @param value The converted value.
     * @return The value approximation representation.
     */
    INLINE GUM_SCALAR fromExact(const GUM_SCALAR& value) const;

    /**
     * @brief Combine using addition with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineAdd(const ApproximationPolicy< GUM_SCALAR >* ap);

    /**
     * @brief Combine using substraction with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineSub(const ApproximationPolicy< GUM_SCALAR >* ap);

    /**
     * @brief Combine using multiplication with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineMult(const ApproximationPolicy< GUM_SCALAR >* ap);

    /**
     * @brief Combine using division with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineDiv(const ApproximationPolicy< GUM_SCALAR >* ap);

    /**
     * @brief Combine using max with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineMax(const ApproximationPolicy< GUM_SCALAR >* ap);

    /**
     * @brief Combine using min with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    void combineMin(const ApproximationPolicy< GUM_SCALAR >* ap);

    /// @}
  };
}   // namespace gum


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::ExactPolicy< double >;
#endif


// Always include inlined implementation of template class
#include <agrum/base/core/approximations/exactPolicy_tpl.h>

#endif /* GUM_EXACT_POLICY_H */
