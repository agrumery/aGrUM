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
 * @brief Headers of the ChiSquare class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


#ifndef GUM_CHI_SQUARE_H
#  define GUM_CHI_SQUARE_H

#  include <agrum/base/core/hashTable.h>

#  include <agrum/base/core/math/math_utils.h>

namespace gum {

  /**
   * @class ChiSquare chiSquare.h
   * <agrum/FMDP/learning/core/testPolicy/chiSquare.h>
   * @brief
   * @ingroup fmdp_group
   *
   */

  class ChiSquare {
    public:
    static double probaChi2(double x, Size df);

    private:
    /// Required constant to compute the cdf
    /// @{

    /// maximum meaningful z value
    static constexpr double _Z_MAX_ = 6.0;

    /// accuracy of critchi approximation
    static constexpr double _CHI_EPSILON_ = 0.000001;

    /// maximum chi square value
    static constexpr double _CHI_MAX_ = 99999.0;

    /// log (sqrt (pi))
    static constexpr double _LOG_SQRT_PI_ = 0.5723649429247000870717135;

    /// 1 / sqrt (pi)
    static constexpr double _I_SQRT_PI_ = 0.5641895835477562869480795;

    /// max value to represent exp (x)
    static constexpr double _BIGX_ = 20.0;

    /// @}


    /// Required constant to compute the cdf
    /// @{

    static double _exp_(double x) { return (((x) < -_BIGX_) ? 0.0 : exp(x)); }

    static double _probaZValue_(double z);
    /// @}
  };

} /* namespace gum */

#endif   // GUM_CHI_SQUARE_H


// static HashTable<std::pair<double, unsigned long>, double>  _chi2Cache_;
// static HashTable<double, double>  _ZCache_;
// static Idx nbZ;
// static Idx nbChi;
// static Idx nbZt;
// static Idx nbChit;
