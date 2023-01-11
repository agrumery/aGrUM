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
 * @brief The class for computing Log2 (Gamma(x)).
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  ALWAYS_INLINE double GammaLog2::gammaLog2(double x) const {
    if (x <= 0) GUM_ERROR(OutOfBounds, "log2(gamma()) should be called with a positive argument")

    // if x is small, use precomputed values
    if (x < 50) {
      if (x >= 0.01) {
        if (_requires_precision_) {
          const Idx index = int(x * 100);
          return _small_values_[index]
               + (_small_values_[index + 1] - _small_values_[index]) * double(x * 100 - index);
        } else {
          const Idx index = int(x * 100 + 0.5);
          return _small_values_[index];
        }
      } else {
        // for very small values of x, Gamma(x) is approximately equal to
        // 1/x. Hence gammaLog2(x) is approximately equal to log2(1/x)
        return std::log2(1.0 / x);
      }
    }

    // returns the approximation by the stirling formula
    return (_log_sqrt_2pi_ + (x - 0.5f) * log(x) - x + log(1.0 + 1.0 / (12 * x))) * _inv_log2_;
  }

  ALWAYS_INLINE double GammaLog2::operator()(double x) const { return gammaLog2(x); }

  INLINE void GammaLog2::setPrecision(bool prec) { _requires_precision_ = prec; }

} /* namespace gum */
