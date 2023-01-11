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

/** @file
 * @brief The databases' cell translators for integer variables
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/DBTranslator4NumericalDiscreteVariable.h>

namespace gum {

  namespace learning {

    /// destructor
    INLINE DBTranslator4NumericalDiscreteVariable::~DBTranslator4NumericalDiscreteVariable() {
      // for debugging purposes
      GUM_DESTRUCTOR(DBTranslator4NumericalDiscreteVariable);
    }


    /// returns the translation of a string, as found in the current dictionary
    INLINE DBTranslatedValue
       DBTranslator4NumericalDiscreteVariable::translate(const std::string& str) {
      // try to get the index of str within the NumericalDiscrete variable.
      double     d;
      const bool b = isNumericalWithResult(str, &d);
      if (b) {
        const std::string cstr = compact_tostr(d);
        if (this->back_dico_.existsSecond(cstr)) return {this->back_dico_.first(cstr)};
      }

      if (this->isMissingSymbol(str))
        return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};

      if (!b) {
        GUM_ERROR(TypeError,
                  "String \"" << str << "\" cannot be translated because it is not a number");
      } else {
        GUM_ERROR(UnknownLabelInDatabase,
                  "The translation of \"" << str << "\" could not be found for " << _variable_)
      }
    }


    /// returns the original value for a given translation
    INLINE std::string DBTranslator4NumericalDiscreteVariable::translateBack(
       const DBTranslatedValue translated_val) const {
      if (this->back_dico_.existsFirst(translated_val.discr_val))
        return this->back_dico_.second(translated_val.discr_val);

      // check if this is a missing value
      if ((translated_val.discr_val == std::numeric_limits< std::size_t >::max())
          && !this->missing_symbols_.empty()) {
        return *(this->missing_symbols_.begin());
      }

      GUM_ERROR(UnknownLabelInDatabase,
                "The back translation of \"" << translated_val.discr_val
                                             << "\" could not be found");
    }


    /// indicates whether the translator has an editable dictionary or not
    INLINE bool DBTranslator4NumericalDiscreteVariable::hasEditableDictionary() const {
      return false;
    }


    /// sets/unset the editable dictionary mode
    INLINE void DBTranslator4NumericalDiscreteVariable::setEditableDictionaryMode(bool) {}


    /// indicates whether the translations should be reordered
    INLINE bool DBTranslator4NumericalDiscreteVariable::needsReordering() const { return false; }


    /// returns a mapping to reorder the current dictionary and updates it
    INLINE HashTable< std::size_t, std::size_t > DBTranslator4NumericalDiscreteVariable::reorder() {
      return HashTable< std::size_t, std::size_t >();
    }


    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4NumericalDiscreteVariable::domainSize() const {
      return _variable_.domainSize();
    }


    /// returns the variable stored into the translator
    INLINE const NumericalDiscreteVariable*
       DBTranslator4NumericalDiscreteVariable::variable() const {
      return &_variable_;
    }


    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4NumericalDiscreteVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
