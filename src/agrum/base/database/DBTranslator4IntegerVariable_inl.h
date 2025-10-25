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
 * @brief The databases' cell translators for integer variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/database/DBTranslator4IntegerVariable.h>

namespace gum {

  namespace learning {

    /// destructor
    INLINE DBTranslator4IntegerVariable::~DBTranslator4IntegerVariable() {
      // for debugging purposes
      GUM_DESTRUCTOR(DBTranslator4IntegerVariable);
    }

    /// returns the translation of a string, as found in the current dictionary
    INLINE DBTranslatedValue DBTranslator4IntegerVariable::translate(const std::string& str) {
      const bool b = isInteger(str);
      if (b)
        if (this->back_dico_.existsSecond(str)) return {this->back_dico_.first(str)};

      if (this->isMissingSymbol(str))
        return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};

      if (!b) {
        GUM_ERROR(TypeError,
                  "String \"" << str << "\" cannot be translated because it is not value for "
                              << _variable_);
      } else {
        GUM_ERROR(UnknownLabelInDatabase,
                  "The translation of \"" << str << "\" could not be found for " << _variable_)
      }
    }

    /// returns the original value for a given translation
    INLINE std::string
        DBTranslator4IntegerVariable::translateBack(const DBTranslatedValue translated_val) const {
      try {
        return this->back_dico_.second(translated_val.discr_val);
      } catch (Exception const&) {
        // check if this is a missing value
        if ((translated_val.discr_val == std::numeric_limits< std::size_t >::max())
            && !this->missing_symbols_.empty())
          return *(this->missing_symbols_.begin());
        else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The back translation of \"" << translated_val.discr_val
                                                 << "\" could not be found");
      }
    }

    /// indicates whether the translator has an editable dictionary or not
    INLINE bool DBTranslator4IntegerVariable::hasEditableDictionary() const { return false; }

    /// sets/unset the editable dictionary mode
    INLINE void DBTranslator4IntegerVariable::setEditableDictionaryMode(bool) {}

    /// indicates whether the translations should be reordered
    INLINE bool DBTranslator4IntegerVariable::needsReordering() const { return false; }

    /// returns a mapping to reorder the current dictionary and updates it
    INLINE HashTable< std::size_t, std::size_t > DBTranslator4IntegerVariable::reorder() {
      return HashTable< std::size_t, std::size_t >();
    }

    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4IntegerVariable::domainSize() const {
      return _variable_.domainSize();
    }

    /// returns the variable stored into the translator
    INLINE const IntegerVariable* DBTranslator4IntegerVariable::variable() const {
      return &_variable_;
    }

    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4IntegerVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
