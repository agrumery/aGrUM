/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/database/DBTranslator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename DBTranslator< ALLOC >::allocator_type
       DBTranslator< ALLOC >::getAllocator() const {
      return *this;
    }


    /// returns the type of values handled by the translator
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValueType DBTranslator< ALLOC >::getValType() const {
      return val_type_;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(
       DBTranslatedValueType                                    val_type,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               dynamic_dictionary,
       std::size_t                                              max_dico_entries,
       const typename DBTranslator< ALLOC >::allocator_type&    alloc) :
        DBTranslator< ALLOC >::allocator_type(alloc),
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
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(
       DBTranslatedValueType                                 val_type,
       const bool                                            dynamic_dictionary,
       std::size_t                                           max_dico_entries,
       const typename DBTranslator< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >::allocator_type(alloc),
        is_dictionary_dynamic_(dynamic_dictionary), max_dico_entries_(max_dico_entries),
        val_type_(val_type) {
      GUM_CONSTRUCTOR(DBTranslator);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(
       const DBTranslator< ALLOC >&                          from,
       const typename DBTranslator< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >::allocator_type(alloc),
        is_dictionary_dynamic_(from.is_dictionary_dynamic_),
        max_dico_entries_(from.max_dico_entries_), missing_symbols_(from.missing_symbols_),
        back_dico_(from.back_dico_), val_type_(from.val_type_) {
      GUM_CONS_CPY(DBTranslator);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(const DBTranslator< ALLOC >& from) :
        DBTranslator< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(
       DBTranslator< ALLOC >&&                               from,
       const typename DBTranslator< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >::allocator_type(alloc),
        is_dictionary_dynamic_(from.is_dictionary_dynamic_),
        max_dico_entries_(from.max_dico_entries_),
        missing_symbols_(std::move(from.missing_symbols_)), back_dico_(std::move(from.back_dico_)),
        val_type_(from.val_type_) {
      GUM_CONS_MOV(DBTranslator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::DBTranslator(DBTranslator< ALLOC >&& from) :
        DBTranslator< ALLOC >(from, from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >::~DBTranslator() {
      GUM_DESTRUCTOR(DBTranslator);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
       DBTranslator< ALLOC >::operator=(const DBTranslator< ALLOC >& from) {
      if (this != &from) {
        is_dictionary_dynamic_ = from.is_dictionary_dynamic_;
        max_dico_entries_      = from.max_dico_entries_;
        missing_symbols_       = from.missing_symbols_;
        back_dico_             = from.back_dico_;
        val_type_              = from.val_type_;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >& DBTranslator< ALLOC >::operator=(DBTranslator< ALLOC >&& from) {
      is_dictionary_dynamic_ = from.is_dictionary_dynamic_;
      max_dico_entries_      = from.max_dico_entries_;
      missing_symbols_       = std::move(from.missing_symbols_);
      back_dico_             = std::move(from.back_dico_);
      val_type_              = from.val_type_;

      return *this;
    }


    /// alias for method translate
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue DBTranslator< ALLOC >::operator<<(const std::string& str) {
      return translate(str);
    }


    /// alias for method translateBack
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator< ALLOC >::operator>>(const DBTranslatedValue translated_val) {
      return translateBack(translated_val);
    }


    /// indicates whether the translator has an editable dictionary or not
    template < template < typename > class ALLOC >
    INLINE bool DBTranslator< ALLOC >::hasEditableDictionary() const {
      return is_dictionary_dynamic_;
    }


    /// sets/unset the editable dictionary mode
    template < template < typename > class ALLOC >
    INLINE void DBTranslator< ALLOC >::setEditableDictionaryMode(bool new_mode) {
      is_dictionary_dynamic_ = new_mode;
    }


    /// returns the set of missing symbols taken into account by the translator
    template < template < typename > class ALLOC >
    INLINE const Set< std::string, ALLOC< std::string > >&
                 DBTranslator< ALLOC >::missingSymbols() const {
      return missing_symbols_;
    }


    /// indicates whether a string corresponds to a missing symbol
    template < template < typename > class ALLOC >
    INLINE bool DBTranslator< ALLOC >::isMissingSymbol(const std::string& str) const {
      return missing_symbols_.exists(str);
    }


    /// sets the name of the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE void DBTranslator< ALLOC >::setVariableName(const std::string& str) const {
      const_cast< Variable* >(this->variable())->setName(str);
    }


    /// sets the name of the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE void DBTranslator< ALLOC >::setVariableDescription(const std::string& str) const {
      const_cast< Variable* >(this->variable())->setDescription(str);
    }


    /// indicates whether a translated value corresponds to a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslator< ALLOC >::isMissingValue(const DBTranslatedValue& value) const {
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
