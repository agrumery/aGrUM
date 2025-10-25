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
#pragma once


/** @file
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>
#  include <utility>
#  include <vector>

#  include <agrum/base/database/DBCell.h>
#  include <agrum/base/database/DBTranslator4ContinuousVariable.h>

namespace gum {

  namespace learning {

    /// default constructor with a continuous variable as translator
    template < typename GUM_SCALAR >
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        const ContinuousVariable< GUM_SCALAR >& var,
        const std::vector< std::string >&       missing_symbols,
        const bool                              fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, missing_symbols, fit_range, 1),
        _variable_(var.name(), var.description()), _fit_range_(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = float(var.lowerBound());
      const float upper_bound = float(var.upperBound());
      _variable_.setLowerBound(lower_bound);
      _variable_.setUpperBound(upper_bound);

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      bool non_float_symbol_found = false;
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->missing_symbols_.erase(iter);
          } else _status_float_missing_symbols_.insert(*iter, false);
        } else if (!non_float_symbol_found) {
          non_float_symbol_found    = true;
          _nonfloat_missing_symbol_ = *iter;
        }
      }

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }

    /// default constructor with a continuous variable as translator
    template < typename GUM_SCALAR >
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        const ContinuousVariable< GUM_SCALAR >& var,
        const bool                              fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, fit_range, 1),
        _variable_(var.name(), var.description()), _fit_range_(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = float(var.lowerBound());
      const float upper_bound = float(var.upperBound());
      _variable_.setLowerBound(lower_bound);
      _variable_.setUpperBound(upper_bound);

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }

  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
