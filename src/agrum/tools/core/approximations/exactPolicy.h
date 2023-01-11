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
 * @brief Classes used to practice approximation on value
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */
#ifndef GUM_EXACT_POLICY_H
#define GUM_EXACT_POLICY_H
#include <agrum/agrum.h>

#include <agrum/tools/core/approximations/approximationPolicy.h>

namespace gum {

  /**
   * @class ExactPolicy
   * @headerfile exactPolicy.h <agrum/tools/core/approximations/exactPolicy.h>
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
#include <agrum/tools/core/approximations/exactPolicy_tpl.h>

#endif /* GUM_EXACT_POLICY_H */
