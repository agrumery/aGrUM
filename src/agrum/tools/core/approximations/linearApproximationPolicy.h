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
 * @brief Classes used to practice approximation on value.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */
#ifndef GUM_LINEAR_APPROXIMATION_POLICY_H
#define GUM_LINEAR_APPROXIMATION_POLICY_H

#include <agrum/agrum.h>
#include <agrum/tools/core/approximations/approximationPolicy.h>
#include <typeinfo>

namespace gum {

  /**
   * @class LinearApproximationPolicy
   * @brief Class implementing linear approximation policy (meaning possible
   * value are split out in interval).
   * @ingroup approximationpolicy_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/core/approximations/linearApproximationPolicy.h>
   * @endcode
   *
   * @tparam GUM_SCALAR The type used for computations.
   */
  template < typename GUM_SCALAR >
  class LinearApproximationPolicy: public virtual ApproximationPolicy< GUM_SCALAR > {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * @param low The lower limit.
     * @param high The higher limit.
     * @param eps The epsilon.
     * @throw OutOfBounds if out of bounds (low<high, eps>0)
     */
    LinearApproximationPolicy(GUM_SCALAR low  = (GUM_SCALAR)0.0,
                              GUM_SCALAR high = (GUM_SCALAR)1.0,
                              GUM_SCALAR eps  = (GUM_SCALAR)0.1);

    /**
     * @brief Copy constructor.
     * @param md The gum::LinearApproximationPolicy to copy.
     */
    LinearApproximationPolicy(const LinearApproximationPolicy< GUM_SCALAR >* md);

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{

    /**
     * @brief Convert value to his approximation.
     * @param value The converted value.
     * @return The value approximation representation.
     */
    GUM_SCALAR fromExact(const GUM_SCALAR& value) const;

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

    /**
     * @brief Convert value to his approximation.
     *
     * This method is slower than @ref fromExact since it verifies the
     * bounds.
     *
     * @throw OutOfBounds Raised if value is out of bounds.
     * @throw OutOfBounds Raised if value is out of bounds.
     */
    GUM_SCALAR safeFromExact(const GUM_SCALAR& value);

    /**
     * @brief Encode a given value into its approximation representation.
     * @param value The to encode.
     * @return Returns the encoded value.
     * @throw OutOfBounds Raised if value is out of bounds.
     * @throw OutOfBounds Raised if value is out of bounds.
     */
    Idx encode(const GUM_SCALAR& value) const;

    /**
     * @brief Convert approximation representation to value.
     * @param representation The approximation representation to decode.
     * @return Returns the value decoded from its approximation
     * reprensentation.
     */
    GUM_SCALAR decode(Idx representation) const;

    /**
     * @brief Sets approximation factor.
     * @param e The new epsilon value.
     */
    virtual void setEpsilon(const GUM_SCALAR& e);

    /**
     * @brief Set bounds in a whole.
     * @param newLowLimit New lower bound.
     * @param newHighLimit New higher bound.
     * @throw OutOfBounds Raised if new bounds are not legit.
     */
    virtual void setLimits(const GUM_SCALAR& newLowLimit, const GUM_SCALAR& newHighLimit);

    /**
     * @brief Sets lowest possible value.
     * @param newLowLimit New lower bound.
     * @throw OutOfBounds Raised if out of bound.
     */
    virtual void setLowLimit(const GUM_SCALAR& newLowLimit);

    /**
     * @brief Gets lowest possible value.
     * @return Returns the lowest possible value.
     */
    const GUM_SCALAR& lowLimit() const;

    /**
     * @brief Sets Highest possible value.
     * @param newHighLimit New higher bound.
     * @throw OutOfBounds Raised if out of bound.
     */
    virtual void setHighLimit(const GUM_SCALAR& newHighLimit);

    /**
     * @brief Gets Highest possible value.
     * @return Returns the highest possible value.
     */
    const GUM_SCALAR& highLimit() const;
    /// @}

    protected:
    /// Lowest value possible.
    GUM_SCALAR lowLimit_;

    /// Highest value possible.
    GUM_SCALAR highLimit_;

    /// Approximation factor.
    GUM_SCALAR epsilon_;

    /**
     * @brief Concretely computes the approximate representation.
     *
     * @warning We accept value smaller or higher than limits : please @see
     * gum::ApproximationPolicy::safeFromExact(const GUM_SCALAR&).
     *
     * @param value The value to encode.
     * @return The encoded value.
     */
    Idx _encode_(const GUM_SCALAR& value) const;

    /**
     * @brief Concretely computes the approximate value from representation.
     * @param representation The approximate value to decode.
     * @return The decoded value.
     */
    GUM_SCALAR _decode_(const GUM_SCALAR& representation) const;

    /**
     * @brief Get the number of interval.
     */
    void computeNbInterval_();

    /// The number of interval.
    Idx nbInterval_;
  };
}   // namespace gum


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::LinearApproximationPolicy< double >;
#endif


// Always inline template classes implementation.
#include <agrum/tools/core/approximations/linearApproximationPolicy_tpl.h>

#endif /* GUM_LINEAR_APPROXIMATION_POLICY_H */
