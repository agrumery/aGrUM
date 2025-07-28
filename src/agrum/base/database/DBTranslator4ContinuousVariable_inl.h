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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/database/DBCell.h>

namespace gum {

  namespace learning {

    /// destructor
    INLINE DBTranslator4ContinuousVariable::~DBTranslator4ContinuousVariable() {
      if (_real_variable_ != nullptr) delete _real_variable_;

      GUM_DESTRUCTOR(DBTranslator4ContinuousVariable);
    }

    /// returns the original value for a given translation
    INLINE std::string DBTranslator4ContinuousVariable::translateBack(
        const DBTranslatedValue translated_val) const {
      if (translated_val.cont_val == std::numeric_limits< float >::max()) {
        if (!_nonfloat_missing_symbol_.empty()) return _nonfloat_missing_symbol_;
        if (this->missing_symbols_.empty()) return *(this->missing_symbols_.begin());
      }

      if ((translated_val.cont_val < _variable_.lowerBound())
          || (translated_val.cont_val > _variable_.upperBound())) {
        GUM_ERROR(UnknownLabelInDatabase,
                  "The back translation of "
                      << translated_val.cont_val
                      << " could not be found because the value is outside the "
                      << "domain of the continuous variable");
      }

      char buffer[100];
      snprintf(buffer, 100, "%g", translated_val.cont_val);
      return std::string(buffer);
    }

    /// indicates whether the translations should be reordered
    INLINE bool DBTranslator4ContinuousVariable::needsReordering() const { return false; }

    /// returns a mapping to reorder the current dictionary and updates it
    INLINE HashTable< std::size_t, std::size_t > DBTranslator4ContinuousVariable::reorder() {
      return HashTable< std::size_t, std::size_t >();
    }

    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4ContinuousVariable::domainSize() const {
      return std::numeric_limits< std::size_t >::max();
    }

    /// returns the variable stored into the translator
    INLINE const IContinuousVariable* DBTranslator4ContinuousVariable::variable() const {
      _real_variable_->setLowerBoundFromDouble(_variable_.lowerBound());
      _real_variable_->setUpperBoundFromDouble(_variable_.upperBound());
      return _real_variable_;
    }

    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4ContinuousVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< float >::max()};
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
