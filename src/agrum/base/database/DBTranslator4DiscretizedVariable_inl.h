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

#  include <agrum/base/database/DBTranslator4DiscretizedVariable.h>

namespace gum {

  namespace learning {

    /// destructor
    INLINE DBTranslator4DiscretizedVariable::~DBTranslator4DiscretizedVariable() {
      if (_real_variable_ != nullptr) delete _real_variable_;

      GUM_DESTRUCTOR(DBTranslator4DiscretizedVariable);
    }

    /// returns the translation of a string, as found in the current dictionary
    INLINE DBTranslatedValue DBTranslator4DiscretizedVariable::translate(const std::string& str) {
      // try to get the index of str within the discretized variable.
      try {
        return DBTranslatedValue{std::size_t(_variable_[str])};
      } catch (gum::Exception& e) {
        // check for a missing symbol
        if (this->isMissingSymbol(str))
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};

        // check if the back_dictionary does not contain str. This enables
        // to execute translate ( translateBack ( translate ( str ) ) )
        // without raising an exception
        try {
          return DBTranslatedValue{this->back_dico_.first(str)};
        } catch (const gum::Exception&) {
          if (!DBCell::isReal(str)) {
            GUM_ERROR(TypeError,
                      "String \"" << str << "\" cannot be translated because it is not a value for "
                                  << _variable_);
          } else {
            GUM_ERROR(UnknownLabelInDatabase,
                      "The translation of \"" << str << "\" could not be found : " << e.what()
                                              << " for variable " << _variable_);
          }
        }
      }
    }

    /// returns the original value for a given translation
    INLINE std::string DBTranslator4DiscretizedVariable::translateBack(
        const DBTranslatedValue translated_val) const {
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
    INLINE bool DBTranslator4DiscretizedVariable::hasEditableDictionary() const { return false; }

    /// sets/unset the editable dictionary mode
    INLINE void DBTranslator4DiscretizedVariable::setEditableDictionaryMode(bool) {}

    /// indicates whether the translations should be reordered
    INLINE bool DBTranslator4DiscretizedVariable::needsReordering() const { return false; }

    /// returns a mapping to reorder the current dictionary and updates it
    INLINE HashTable< std::size_t, std::size_t > DBTranslator4DiscretizedVariable::reorder() {
      return HashTable< std::size_t, std::size_t >();
    }

    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4DiscretizedVariable::domainSize() const {
      return _variable_.domainSize();
    }

    /// returns the variable stored into the translator
    INLINE const IDiscretizedVariable* DBTranslator4DiscretizedVariable::variable() const {
      return _real_variable_;
    }

    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4DiscretizedVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
