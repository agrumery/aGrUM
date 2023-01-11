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
 * @brief The base class for all the tabular databases' cell translators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/DBTranslator.h>

namespace gum {

  namespace learning {

    /// returns the type of values handled by the translator
    INLINE DBTranslatedValueType DBTranslator::getValType() const { return val_type_; }


    /// returns a Boolean indicating whether the translation is lossless or not
    INLINE bool DBTranslator::isLossless() const { return is_lossless_; }


    /// default constructor
    INLINE DBTranslator::DBTranslator(DBTranslatedValueType             val_type,
                                      const bool                        is_lossless,
                                      const std::vector< std::string >& missing_symbols,
                                      const bool                        dynamic_dictionary,
                                      std::size_t                       max_dico_entries) :
        is_lossless_(is_lossless),
        is_dictionary_dynamic_(dynamic_dictionary), max_dico_entries_(max_dico_entries),
        val_type_(val_type) {
      const std::size_t size = missing_symbols.size();

      if (size) {
        // save the set of symbols representing the missing values
        missing_symbols_.resize((Size)missing_symbols.size());
        for (const auto& symbol: missing_symbols) {
          missing_symbols_.insert(symbol);
        }
      }

      GUM_CONSTRUCTOR(DBTranslator);
    }


    /// default constructor
    INLINE DBTranslator::DBTranslator(DBTranslatedValueType val_type,
                                      const bool            is_lossless,
                                      const bool            dynamic_dictionary,
                                      std::size_t           max_dico_entries) :
        is_lossless_(is_lossless),
        is_dictionary_dynamic_(dynamic_dictionary), max_dico_entries_(max_dico_entries),
        val_type_(val_type) {
      GUM_CONSTRUCTOR(DBTranslator);
    }


    /// copy constructor
    INLINE DBTranslator::DBTranslator(const DBTranslator& from) :
        is_lossless_(from.is_lossless_), is_dictionary_dynamic_(from.is_dictionary_dynamic_),
        max_dico_entries_(from.max_dico_entries_), missing_symbols_(from.missing_symbols_),
        back_dico_(from.back_dico_), val_type_(from.val_type_) {
      GUM_CONS_CPY(DBTranslator);
    }


    /// move constructor
    INLINE DBTranslator::DBTranslator(DBTranslator&& from) :
        is_lossless_(from.is_lossless_), is_dictionary_dynamic_(from.is_dictionary_dynamic_),
        max_dico_entries_(from.max_dico_entries_),
        missing_symbols_(std::move(from.missing_symbols_)), back_dico_(std::move(from.back_dico_)),
        val_type_(from.val_type_) {
      GUM_CONS_MOV(DBTranslator);
    }


    /// destructor
    INLINE DBTranslator::~DBTranslator() { GUM_DESTRUCTOR(DBTranslator); }


    /// copy operator
    INLINE DBTranslator& DBTranslator::operator=(const DBTranslator& from) {
      if (this != &from) {
        is_lossless_           = from.is_lossless_;
        is_dictionary_dynamic_ = from.is_dictionary_dynamic_;
        max_dico_entries_      = from.max_dico_entries_;
        missing_symbols_       = from.missing_symbols_;
        back_dico_             = from.back_dico_;
        val_type_              = from.val_type_;
      }
      return *this;
    }


    /// move operator
    INLINE DBTranslator& DBTranslator::operator=(DBTranslator&& from) {
      is_lossless_           = from.is_lossless_;
      is_dictionary_dynamic_ = from.is_dictionary_dynamic_;
      max_dico_entries_      = from.max_dico_entries_;
      missing_symbols_       = std::move(from.missing_symbols_);
      back_dico_             = std::move(from.back_dico_);
      val_type_              = from.val_type_;

      return *this;
    }


    /// alias for method translate
    INLINE DBTranslatedValue DBTranslator::operator<<(const std::string& str) {
      return translate(str);
    }


    /// alias for method translateBack
    INLINE std::string DBTranslator::operator>>(const DBTranslatedValue translated_val) {
      return translateBack(translated_val);
    }


    /// indicates whether the translator has an editable dictionary or not
    INLINE bool DBTranslator::hasEditableDictionary() const { return is_dictionary_dynamic_; }


    /// sets/unset the editable dictionary mode
    INLINE void DBTranslator::setEditableDictionaryMode(bool new_mode) {
      is_dictionary_dynamic_ = new_mode;
    }


    /// returns the translation from database indices to input strings
    INLINE const Bijection< std::size_t, std::string >& DBTranslator::getDictionary() const {
      return back_dico_;
    }


    /// returns the set of missing symbols taken into account by the translator
    INLINE const Set< std::string >& DBTranslator::missingSymbols() const {
      return missing_symbols_;
    }


    /// indicates whether a string corresponds to a missing symbol
    INLINE bool DBTranslator::isMissingSymbol(const std::string& str) const {
      return missing_symbols_.exists(str);
    }


    /// sets the name of the variable stored into the translator
    INLINE void DBTranslator::setVariableName(const std::string& str) const {
      const_cast< Variable* >(this->variable())->setName(str);
    }


    /// sets the name of the variable stored into the translator
    INLINE void DBTranslator::setVariableDescription(const std::string& str) const {
      const_cast< Variable* >(this->variable())->setDescription(str);
    }


    /// indicates whether a translated value corresponds to a missing value
    INLINE bool DBTranslator::isMissingValue(const DBTranslatedValue& value) const {
      switch (val_type_) {
        case DBTranslatedValueType::DISCRETE:
          return value.discr_val == std::numeric_limits< std::size_t >::max();

        case DBTranslatedValueType::CONTINUOUS:
          return value.cont_val == std::numeric_limits< float >::max();

        default:
          GUM_ERROR(NotImplementedYet,
                    "No missing value interpretation for this "
                    "translated value type");
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
