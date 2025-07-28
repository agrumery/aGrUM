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
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */


namespace gum {

  // sets the conditioning nodes (useful for computing degrees of freedom)
  INLINE void Chi2::setConditioningNodes(const std::vector< Idx >& db_conditioning_ids) {
    _conditioning_size_ = 1;
    for (Idx i = 0; i < db_conditioning_ids.size(); ++i) {
      _conditioning_size_ *= _modalities_[db_conditioning_ids[i]];
    }
  }

  // returns the number of degrees of freedom
  INLINE Size Chi2::degreesOfFreedom(const std::pair< Idx, Idx >& pair) {
    return degreesOfFreedom(pair.first, pair.second);
  }

  // returns the number of degrees of freedom
  INLINE Size Chi2::degreesOfFreedom(Idx var1, Idx var2) {
    return (_conditioning_size_ * (_modalities_[var1] - 1) * (_modalities_[var2] - 1));
  }

  // computes the critical value according to the number of degrees of freedom
  ALWAYS_INLINE double Chi2::criticalValue(const std::pair< Idx, Idx >& pair) {
    return criticalValue(pair.first, pair.second);
  }

  // computes the critical value according to the number of degrees of freedom
  ALWAYS_INLINE double Chi2::criticalValue(Idx var1, Idx var2) {
    Size DF = degreesOfFreedom(var1, var2);

    // try to see if the threshold is not already in cache
    try {
      return _critical_values_[DF];
    } catch (const Exception&) {
      // here we have to compute the threshold of the chi2
      // we use Gary Perlman's algorithm
      double value = _criticalValue_(_confidence_proba_, DF);
      _critical_values_.insert(DF, value);
      return value;
    }
  }

  // modifies the confidence proba
  INLINE void Chi2::setConfidenceProba(double new_proba) {
    // if we did not change the confidence proba, do nothing
    if (_confidence_proba_ == new_proba) return;

    _confidence_proba_ = new_proba;

    // remove the currently stored critical values
    _critical_values_.clear();
  }

} /* namespace gum */
