/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The class for computing Log2 (Gamma(x))
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_GAMMA_LOG2_H
#define GUM_GAMMA_LOG2_H

#include <vector>
#include <cmath>

#include <agrum/config.h>
#include <agrum/core/math/math.h>

namespace gum {

  /** @class GammaLog2
   * @brief The class for computing Log2 (Gamma(x))
   */
  class GammaLog2 {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    ///@{

    /// default constructor
    GammaLog2(bool requires_precision = false)
        : __requires_precision{requires_precision} {}

    /// copy constructor
    GammaLog2(const GammaLog2 &from)
        : __requires_precision{from.__requires_precision} {}

    /// move constructor
    GammaLog2(GammaLog2 &&from) : __requires_precision{from.__requires_precision} {}

    /// destructor
    ~GammaLog2() {}

    ///@}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// returns log2 ( gamma (x) ) for x > 0
    /** @hrows OutOfBounds exception is raised if x <= 0 */
    float operator()(float x) const;

    /// sets whether we need more precision for small values
    void setPrecision(bool);

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// returns log2 ( gamma (x) ) for x >= 0
    /** @hrows OutOfBounds exception is raised if x <= 0 */
    float gammaLog2(float x) const;

    /// @}

    private:
    /// indicates whether we need more precision for small values
    bool __requires_precision{false};

    /// 1 / std::log(2)
    static constexpr float __1log2{M_LOG2E};

    /// std::log ( std::sqrt(2pi) )
    static constexpr float __log_sqrt_2pi{GUM_LOG_SQRT_2PI};

    /// the 5000 values from 0 to 50 by step of 1/100
    static const std::vector<float> __small_values;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/math/gammaLog2.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_GAMMA_LOG2_H */
