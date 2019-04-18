
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
 * @brief The class for computing Log2 (Gamma(x)).
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_GAMMA_LOG2_H
#define GUM_GAMMA_LOG2_H

#define _USE_MATH_DEFINES   // for Visual C++

#include <limits>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/core/math/math.h>

namespace gum {

  /**
   * @class GammaLog2
   * @headerfile gammaLog2.h <agrum/core/math/gammaLog2.h>
   * @brief The class for computing Log2 (Gamma(x))
   * @ingroup math_group
   */
  class GammaLog2 {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    ///@{

    /**
     * @brief Default constructor.
     * @param requires_precision Set if precision is required or not.
     */
    GammaLog2(bool requires_precision = false);

    /**
     * @brief Copy constructor.
     * @param from The gum::GammaLog2 to copy.
     */
    GammaLog2(const GammaLog2& from);

    /**
     * @brief Move constructor.
     * @param from The gum::GammaLog2 to move.
     */
    GammaLog2(GammaLog2&& from);

    /**
     * @brief Class destructor.
     */
    ~GammaLog2();

    ///@}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Returns log2 ( gamma (x) ) for x > 0.
     * @throws OutOfBounds Raised if raised if x <= 0.
     */
    double operator()(double x) const;

    /**
     * @brief Sets whether we need more precision for small values.
     * @param p If true, precision is enable.
     */
    void setPrecision(bool p);

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Returns log2 ( gamma (x) ) for x >= 0.
     * @param x A positive double.
     * @return Returns log2 ( gamma (x) ) for x >= 0.
     * @throws OutOfBounds Raised if x <= 0.
     */
    double gammaLog2(double x) const;

    /// @}

    private:
    /// Indicates whether we need more precision for small values.
    bool __requires_precision{false};

    /// The value of 1 / std::log(2).
    static constexpr double __1log2{M_LOG2E};

    /// The value of std::log ( std::sqrt(2pi) ).
    static constexpr double __log_sqrt_2pi{GUM_LOG_SQRT_2PI};

    /// The 5000 values from 0 to 50 by step of 1/100.
    static const std::vector< double > __small_values;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/core/math/gammaLog2_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_GAMMA_LOG2_H */
