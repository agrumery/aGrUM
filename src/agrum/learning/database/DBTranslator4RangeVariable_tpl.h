
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The databases' cell translators for range variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <utility>
#include <vector>
#include <limits>
#include <cstdio>

#include <agrum/learning/database/DBTranslator4RangeVariable.h>
#include <agrum/learning/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       std::size_t                                              max_dico_entries,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              true,
                              max_dico_entries,
                              alloc),
        __variable("var", "", 1, 0) {
      // assign to each integer missing symbol a Boolean indicating that
      // we did not translate it yet. If we encounter a non integer missing
      // symbol, we record it because it cannot be compomised by updating the
      // domain of the range variable
      bool non_int_symbol_found = false;
      for (const auto& symbol : this->_missing_symbols) {
        if (DBCell::isInteger(symbol)) {
          __status_int_missing_symbols.insert(symbol, false);
        } else if (!non_int_symbol_found) {
          non_int_symbol_found = true;
          __nonint_missing_symbol = symbol;
        }
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       std::size_t max_dico_entries,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::DISCRETE, true, max_dico_entries, alloc),
        __variable("var", "", 1, 0) {
      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor with a range variable as translator
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       const RangeVariable&                                     var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool  editable_dictionary,
       std::size_t max_dico_entries,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              editable_dictionary,
                              max_dico_entries,
                              alloc),
        __variable(var) {
      // get the bounds of the range variable
      const long lower_bound = var.minVal();
      const long upper_bound = var.maxVal();

      // check that the variable has not too many entries for the dictionary
      if ((upper_bound >= lower_bound)
          && (std::size_t(upper_bound - lower_bound + 1)
              > this->_max_dico_entries)) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // if the range variable is not empty, i.e., its upper bound is greater
      // than or equal to its lower bound, remove all the missing symbols
      // corresponding to a number between lower_bound and upper_bound
      if (lower_bound <= upper_bound) {
        for (auto iter = this->_missing_symbols.beginSafe();
             iter != this->_missing_symbols.endSafe();
             ++iter) {
          if (DBCell::isInteger(*iter)) {
            const long missing_val = std::stol(*iter);
            if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
              this->_missing_symbols.erase(iter);
            }
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // insert the label into the back_dictionary
        this->_back_dico.insert(size, label);
        ++size;
      }

      // assign to each integer missing symbol a Boolean indicating that
      // we did not translate it yet. If we encounter a non integer symbol,
      // we record it because it cannot be compomised by updating the domain
      // of the range variable. This will be useful for back translations
      bool non_int_symbol_found = false;
      for (const auto& symbol : this->_missing_symbols) {
        if (DBCell::isInteger(symbol)) {
          __status_int_missing_symbols.insert(symbol, false);
        } else if (!non_int_symbol_found) {
          non_int_symbol_found = true;
          __nonint_missing_symbol = symbol;
        }
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor with a range variable as translator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       const RangeVariable& var,
       const bool           editable_dictionary,
       std::size_t          max_dico_entries,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              editable_dictionary,
                              max_dico_entries,
                              alloc),
        __variable(var) {
      // get the bounds of the range variable
      const long lower_bound = var.minVal();
      const long upper_bound = var.maxVal();

      // check that the variable has not too many entries for the dictionary
      if ((upper_bound >= lower_bound)
          && (std::size_t(upper_bound - lower_bound + 1)
              > this->_max_dico_entries)) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // insert the label into the back_dictionary
        this->_back_dico.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       const DBTranslator4RangeVariable< ALLOC >&                          from,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(from, alloc),
        __variable(from.__variable),
        __status_int_missing_symbols(from.__status_int_missing_symbols),
        __translated_int_missing_symbols(from.__translated_int_missing_symbols),
        __nonint_missing_symbol(from.__nonint_missing_symbol) {
      GUM_CONS_CPY(DBTranslator4RangeVariable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       const DBTranslator4RangeVariable< ALLOC >& from) :
        DBTranslator4RangeVariable< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       DBTranslator4RangeVariable< ALLOC >&&                               from,
       const typename DBTranslator4RangeVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(std::move(from), alloc),
        __variable(std::move(from.__variable)),
        __status_int_missing_symbols(std::move(from.__status_int_missing_symbols)),
        __translated_int_missing_symbols(
           std::move(from.__translated_int_missing_symbols)),
        __nonint_missing_symbol(std::move(from.__nonint_missing_symbol)) {
      GUM_CONS_MOV(DBTranslator4RangeVariable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >::DBTranslator4RangeVariable(
       DBTranslator4RangeVariable< ALLOC >&& from) :
        DBTranslator4RangeVariable< ALLOC >(std::move(from), from.getAllocator()) {
    }


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >*
       DBTranslator4RangeVariable< ALLOC >::clone(
          const typename DBTranslator4RangeVariable< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBTranslator4RangeVariable< ALLOC > > allocator(alloc);
      DBTranslator4RangeVariable< ALLOC >* translator = allocator.allocate(1);
      try {
        allocator.construct(translator, *this, alloc);
      } catch (...) {
        allocator.deallocate(translator, 1);
        throw;
      }
      return translator;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4RangeVariable< ALLOC >*
           DBTranslator4RangeVariable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4RangeVariable< ALLOC >::~DBTranslator4RangeVariable() {
      GUM_DESTRUCTOR(DBTranslator4RangeVariable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >& DBTranslator4RangeVariable< ALLOC >::
                                         operator=(const DBTranslator4RangeVariable< ALLOC >& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(from);
        __variable = from.__variable;
        __status_int_missing_symbols = from.__status_int_missing_symbols;
        __translated_int_missing_symbols = from.__translated_int_missing_symbols;
        __nonint_missing_symbol = from.__nonint_missing_symbol;
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBTranslator4RangeVariable< ALLOC >& DBTranslator4RangeVariable< ALLOC >::
                                         operator=(DBTranslator4RangeVariable< ALLOC >&& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(std::move(from));
        __variable = std::move(from.__variable);
        __status_int_missing_symbols =
           std::move(from.__status_int_missing_symbols);
        __translated_int_missing_symbols =
           std::move(from.__translated_int_missing_symbols);
        __nonint_missing_symbol = std::move(from.__nonint_missing_symbol);
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    template < template < typename > class ALLOC >
    DBTranslatedValue
       DBTranslator4RangeVariable< ALLOC >::translate(const std::string& str) {
      // try to get the index of str within the labelized variable. If this
      // cannot be found, try to find if this corresponds to a missing value.
      // Finally, if this is still not a missing value and, if enabled, try
      // to add str as a new label
      try {
        return DBTranslatedValue{this->_back_dico.first(str)};
      } catch (gum::Exception&) {
        // check that this is not a missing value
        if (this->isMissingSymbol(str)) {
          try {
            const bool is_str_translated = __status_int_missing_symbols[str];
            if (!is_str_translated) {
              __status_int_missing_symbols[str] = true;
              __translated_int_missing_symbols.insert(std::stol(str));
            }
          } catch (gum::NotFound&) {}
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
        }

        // check if we are allowed to update the range variable
        if (!this->hasEditableDictionary()) {
          GUM_ERROR(UnknownLabelInDatabase,
                    "The translation of String \"" << str
                                                   << "\" could not be found");
        }

        // check if str could correspond to a bound of the range variable
        if (!DBCell::isInteger(str)) {
          GUM_ERROR(TypeError,
                    "String \"" << str << "\" cannot be translated because "
                                << "it cannot be converted into an integer");
        }
        const long new_value = std::stol(str);

        // if str corresponds to a missing symbol that we already
        // translated, raise an exception
        if (__translated_int_missing_symbols.exists(new_value)) {
          GUM_ERROR(
             OperationNotAllowed,
             "String \""
                << str << "\" cannot be translated because "
                << "it corresponds to an already translated missing symbol");
        }

        // now, we can try to add str as a new bound of the range variable
        // if possible

        // if the range variable is empty, set the min and max ranges. Here,
        // there is no need to check whether the new range would contain an
        // already translated missing symbol because this was already tested
        // in the above test.
        if (__variable.minVal() > __variable.maxVal()) {
          if (this->_max_dico_entries == 0) {
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");
          }
          __variable.setMinVal(new_value);
          __variable.setMaxVal(new_value);
          this->_back_dico.insert(std::size_t(0), str);
          return DBTranslatedValue{std::size_t(0)};
        }

        // here, the domain is not empty. So we should update either the
        // lower bound or the upper bound of the range variable, unless
        // a missing symbol lies within the new bounds and we have already
        // translated it.
        const long lower_bound = __variable.minVal();
        const long upper_bound = __variable.maxVal();

        std::size_t size = upper_bound - lower_bound + 1;

        if (new_value < __variable.minVal()) {
          if (std::size_t(upper_bound - new_value + 1) > this->_max_dico_entries)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");

          // check that there does not already exist a translated missing
          // value within the new bounds of the range variable
          for (const auto& missing : __translated_int_missing_symbols) {
            if ((missing >= new_value) && (missing <= upper_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \""
                           << str << "\" cannot be translated "
                           << "because it would induce a new range containing "
                           << "an already translated missing symbol");
            }
          }

          // remove all the missing symbols that were not translated yet and
          // that lie within the new bounds of the range variable
          for (auto iter = __status_int_missing_symbols.beginSafe();
               iter != __status_int_missing_symbols.endSafe();
               ++iter) {
            if (iter.val() == false) {
              const long missing = std::stol(iter.key());
              if ((missing >= new_value) && (missing <= upper_bound)) {
                this->_missing_symbols.erase(iter.key());
                __status_int_missing_symbols.erase(iter);
              }
            }
          }

          // update the range and the back dictionary
          const std::size_t index = size;
          for (long i = new_value; i < __variable.minVal(); ++i) {
            this->_back_dico.insert(size, std::to_string(i));
            ++size;
          }
          __variable.setMinVal(new_value);

          return DBTranslatedValue{index};
        } else {
          if (std::size_t(new_value - lower_bound + 1) > this->_max_dico_entries)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");

          // check that there does not already exist a translated missing
          // value within the new bounds of the range variable
          for (const auto& missing : __translated_int_missing_symbols) {
            if ((missing <= new_value) && (missing >= lower_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \""
                           << str << "\" cannot be translated "
                           << "because it would induce a new range containing "
                           << "an already translated missing symbol");
            }
          }

          // remove all the missing symbols that were not translated yet and
          // that lie within the new bounds of the range variable
          for (auto iter = __status_int_missing_symbols.beginSafe();
               iter != __status_int_missing_symbols.endSafe();
               ++iter) {
            if (iter.val() == false) {
              const long missing = std::stol(iter.key());
              if ((missing <= new_value) && (missing >= lower_bound)) {
                this->_missing_symbols.erase(iter.key());
                __status_int_missing_symbols.erase(iter);
              }
            }
          }

          // update the range and the back dictionary
          for (long i = __variable.maxVal() + 1; i <= new_value; ++i) {
            this->_back_dico.insert(size, std::to_string(i));
            ++size;
          }
          __variable.setMaxVal(new_value);

          return DBTranslatedValue{size - std::size_t(1)};
        }
      }
    }


    /// returns the original value for a given translation
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator4RangeVariable< ALLOC >::translateBack(
       const DBTranslatedValue translated_val) const {
      try {
        return this->_back_dico.second(translated_val.discr_val);
      } catch (Exception&) {
        // check if this is a missing value
        if (translated_val.discr_val
            == std::numeric_limits< std::size_t >::max()) {
          if (!__nonint_missing_symbol.empty()) return __nonint_missing_symbol;
          if (this->_missing_symbols.empty())
            return *(this->_missing_symbols.begin());
        }

        GUM_ERROR(UnknownLabelInDatabase,
                  "The back translation of \"" << translated_val.discr_val
                                               << "\" could not be found");
      }
    }


    /// indicates whether the translations should be reordered
    template < template < typename > class ALLOC >
    bool DBTranslator4RangeVariable< ALLOC >::needsReordering() const {
      // if the variable contains only numbers, they should be increasing
      const auto& labels = __variable.labels();
      std::size_t last_number = std::numeric_limits< std::size_t >::lowest();
      std::size_t number;
      for (const auto& label : labels) {
        number = this->_back_dico.first(label);
        if (number < last_number) return true;
        last_number = number;
      }

      return false;
    }


    /// returns a mapping to reorder the current dictionary and updates it
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslator4RangeVariable< ALLOC >::reorder() {
      // assign to each label the index it had before reordering
      const auto&       labels = __variable.labels();
      const std::size_t size = labels.size();
      std::vector< std::pair< std::size_t, std::string >,
                   ALLOC< std::pair< std::size_t, std::string > > >
         xlabels;
      xlabels.reserve(size);
      bool modifications = false;
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        const std::size_t old_val = this->_back_dico.first(labels[i]);
        xlabels.push_back(std::make_pair(old_val, labels[i]));
        if (old_val != i) modifications = true;
      }


      // if there were no modification, return an empty update hashtable
      if (!modifications) {
        return HashTable< std::size_t,
                          std::size_t,
                          ALLOC< std::pair< std::size_t, std::size_t > > >();
      }

      // create the hashTable corresponding to the mapping from the old
      // indices to the new one
      this->_back_dico.clear();
      HashTable< std::size_t,
                 std::size_t,
                 ALLOC< std::pair< std::size_t, std::size_t > > >
         mapping((Size)size);
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        mapping.insert(xlabels[i].first, i);
        this->_back_dico.insert(i, xlabels[i].second);
      }

      return mapping;
    }


    /// returns the domain size of a variable corresponding to the translations
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslator4RangeVariable< ALLOC >::domainSize() const {
      return __variable.domainSize();
    }


    /// returns the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE const RangeVariable*
                 DBTranslator4RangeVariable< ALLOC >::variable() const {
      return &__variable;
    }


    /// returns the translation of a missing value
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue
           DBTranslator4RangeVariable< ALLOC >::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
