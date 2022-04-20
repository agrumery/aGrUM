/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The databases' cell translators for range variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/DBTranslator4RangeVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBTranslator4RangeVariable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(
       const std::vector< std::string >& missing_symbols,
       std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     true,
                     missing_symbols,
                     true,
                     max_dico_entries),
        _variable_("var", "", 1, 0) {
      // assign to each integer missing symbol a Boolean indicating that
      // we did not translate it yet. If we encounter a non integer missing
      // symbol, we record it because it cannot be compomised by updating the
      // domain of the range variable
      bool non_int_symbol_found = false;
      for (const auto& symbol: this->missing_symbols_) {
        if (DBCell::isInteger(symbol)) {
          _status_int_missing_symbols_.insert(symbol, false);
        } else if (!non_int_symbol_found) {
          non_int_symbol_found    = true;
          _nonint_missing_symbol_ = symbol;
        }
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(std::size_t max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, true, true, max_dico_entries),
        _variable_("var", "", 1, 0) {
      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor with a range variable as translator
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(
       const RangeVariable&              var,
       const std::vector< std::string >& missing_symbols,
       const bool                        editable_dictionary,
       std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     true,
                     missing_symbols,
                     editable_dictionary,
                     max_dico_entries),
        _variable_(var) {
      // get the bounds of the range variable
      const long lower_bound = var.minVal();
      const long upper_bound = var.maxVal();

      // check that the variable has not too many entries for the dictionary
      if ((upper_bound >= lower_bound)
          && (std::size_t(upper_bound - lower_bound + 1) > this->max_dico_entries_)) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // if the range variable is not empty, i.e., its upper bound is greater
      // than or equal to its lower bound, remove all the missing symbols
      // corresponding to a number between lower_bound and upper_bound
      if (lower_bound <= upper_bound) {
        for (auto iter = this->missing_symbols_.beginSafe();
             iter != this->missing_symbols_.endSafe();
             ++iter) {
          if (DBCell::isInteger(*iter)) {
            const long missing_val = std::stol(*iter);
            if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
              this->missing_symbols_.erase(iter);
            }
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // insert the label into the back_dictionary
        this->back_dico_.insert(size, label);
        ++size;
      }

      // assign to each integer missing symbol a Boolean indicating that
      // we did not translate it yet. If we encounter a non integer symbol,
      // we record it because it cannot be compomised by updating the domain
      // of the range variable. This will be useful for back translations
      bool non_int_symbol_found = false;
      for (const auto& symbol: this->missing_symbols_) {
        if (DBCell::isInteger(symbol)) {
          _status_int_missing_symbols_.insert(symbol, false);
        } else if (!non_int_symbol_found) {
          non_int_symbol_found    = true;
          _nonint_missing_symbol_ = symbol;
        }
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// default constructor with a range variable as translator
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(const RangeVariable& var,
                                                           const bool           editable_dictionary,
                                                           std::size_t          max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, true, editable_dictionary, max_dico_entries),
        _variable_(var) {
      // get the bounds of the range variable
      const long lower_bound = var.minVal();
      const long upper_bound = var.maxVal();

      // check that the variable has not too many entries for the dictionary
      if ((upper_bound >= lower_bound)
          && (std::size_t(upper_bound - lower_bound + 1) > this->max_dico_entries_)) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // insert the label into the back_dictionary
        this->back_dico_.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4RangeVariable);
    }


    /// copy constructor
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(const DBTranslator4RangeVariable& from) :
        DBTranslator(from), _variable_(from._variable_),
        _status_int_missing_symbols_(from._status_int_missing_symbols_),
        _translated_int_missing_symbols_(from._translated_int_missing_symbols_),
        _nonint_missing_symbol_(from._nonint_missing_symbol_) {
      GUM_CONS_CPY(DBTranslator4RangeVariable);
    }


    /// move constructor
    DBTranslator4RangeVariable::DBTranslator4RangeVariable(DBTranslator4RangeVariable&& from) :
        DBTranslator(std::move(from)), _variable_(std::move(from._variable_)),
        _status_int_missing_symbols_(std::move(from._status_int_missing_symbols_)),
        _translated_int_missing_symbols_(std::move(from._translated_int_missing_symbols_)),
        _nonint_missing_symbol_(std::move(from._nonint_missing_symbol_)) {
      GUM_CONS_MOV(DBTranslator4RangeVariable);
    }


    /// virtual copy constructor
    DBTranslator4RangeVariable* DBTranslator4RangeVariable::clone() const {
      return new DBTranslator4RangeVariable(*this);
    }


    /// copy operator
    DBTranslator4RangeVariable&
       DBTranslator4RangeVariable::operator=(const DBTranslator4RangeVariable& from) {
      if (this != &from) {
        DBTranslator::operator           =(from);
        _variable_                       = from._variable_;
        _status_int_missing_symbols_     = from._status_int_missing_symbols_;
        _translated_int_missing_symbols_ = from._translated_int_missing_symbols_;
        _nonint_missing_symbol_          = from._nonint_missing_symbol_;
      }

      return *this;
    }


    /// move operator
    DBTranslator4RangeVariable&
       DBTranslator4RangeVariable::operator=(DBTranslator4RangeVariable&& from) {
      if (this != &from) {
        DBTranslator::operator           =(std::move(from));
        _variable_                       = std::move(from._variable_);
        _status_int_missing_symbols_     = std::move(from._status_int_missing_symbols_);
        _translated_int_missing_symbols_ = std::move(from._translated_int_missing_symbols_);
        _nonint_missing_symbol_          = std::move(from._nonint_missing_symbol_);
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    DBTranslatedValue DBTranslator4RangeVariable::translate(const std::string& str) {
      // try to get the index of str within the labelized variable. If this
      // cannot be found, try to find if this corresponds to a missing value.
      // Finally, if this is still not a missing value and, if enabled, try
      // to add str as a new label
      try {
        return DBTranslatedValue{this->back_dico_.first(str)};
      } catch (gum::Exception&) {
        // check that this is not a missing value
        if (this->isMissingSymbol(str)) {
          try {
            const bool is_str_translated = _status_int_missing_symbols_[str];
            if (!is_str_translated) {
              _status_int_missing_symbols_[str] = true;
              _translated_int_missing_symbols_.insert(std::stol(str));
            }
          } catch (gum::NotFound&) {}
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
        }

        // check if we are allowed to update the range variable
        if (!this->hasEditableDictionary()) {
          GUM_ERROR(UnknownLabelInDatabase,
                    "The translation of String \"" << str << "\" could not be found");
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
        if (_translated_int_missing_symbols_.exists(new_value)) {
          GUM_ERROR(OperationNotAllowed,
                    "String \"" << str << "\" cannot be translated because "
                                << "it corresponds to an already translated missing symbol");
        }

        // now, we can try to add str as a new bound of the range variable
        // if possible

        // if the range variable is empty, set the min and max ranges. Here,
        // there is no need to check whether the new range would contain an
        // already translated missing symbol because this was already tested
        // in the above test.
        if (_variable_.minVal() > _variable_.maxVal()) {
          if (this->max_dico_entries_ == 0) {
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");
          }
          _variable_.setMinVal(new_value);
          _variable_.setMaxVal(new_value);
          this->back_dico_.insert(std::size_t(0), str);
          return DBTranslatedValue{std::size_t(0)};
        }

        // here, the domain is not empty. So we should update either the
        // lower bound or the upper bound of the range variable, unless
        // a missing symbol lies within the new bounds and we have already
        // translated it.
        const long lower_bound = _variable_.minVal();
        const long upper_bound = _variable_.maxVal();

        std::size_t size = upper_bound - lower_bound + 1;

        if (new_value < _variable_.minVal()) {
          if (std::size_t(upper_bound - new_value + 1) > this->max_dico_entries_)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");

          // check that there does not already exist a translated missing
          // value within the new bounds of the range variable
          for (const auto& missing: _translated_int_missing_symbols_) {
            if ((missing >= new_value) && (missing <= upper_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \"" << str << "\" cannot be translated "
                                    << "because it would induce a new range containing "
                                    << "an already translated missing symbol");
            }
          }

          // remove all the missing symbols that were not translated yet and
          // that lie within the new bounds of the range variable
          for (auto iter = _status_int_missing_symbols_.beginSafe();
               iter != _status_int_missing_symbols_.endSafe();
               ++iter) {
            if (iter.val() == false) {
              const long missing = std::stol(iter.key());
              if ((missing >= new_value) && (missing <= upper_bound)) {
                this->missing_symbols_.erase(iter.key());
                _status_int_missing_symbols_.erase(iter);
              }
            }
          }

          // update the range and the back dictionary
          const std::size_t index = size;
          for (long i = new_value; i < _variable_.minVal(); ++i) {
            this->back_dico_.insert(size, std::to_string(i));
            ++size;
          }
          _variable_.setMinVal(new_value);

          return DBTranslatedValue{index};
        } else {
          if (std::size_t(new_value - lower_bound + 1) > this->max_dico_entries_)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated because "
                                  << "the dictionary is already full");

          // check that there does not already exist a translated missing
          // value within the new bounds of the range variable
          for (const auto& missing: _translated_int_missing_symbols_) {
            if ((missing <= new_value) && (missing >= lower_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \"" << str << "\" cannot be translated "
                                    << "because it would induce a new range containing "
                                    << "an already translated missing symbol");
            }
          }

          // remove all the missing symbols that were not translated yet and
          // that lie within the new bounds of the range variable
          for (auto iter = _status_int_missing_symbols_.beginSafe();
               iter != _status_int_missing_symbols_.endSafe();
               ++iter) {
            if (iter.val() == false) {
              const long missing = std::stol(iter.key());
              if ((missing <= new_value) && (missing >= lower_bound)) {
                this->missing_symbols_.erase(iter.key());
                _status_int_missing_symbols_.erase(iter);
              }
            }
          }

          // update the range and the back dictionary
          for (long i = _variable_.maxVal() + 1; i <= new_value; ++i) {
            this->back_dico_.insert(size, std::to_string(i));
            ++size;
          }
          _variable_.setMaxVal(new_value);

          return DBTranslatedValue{size - std::size_t(1)};
        }
      }
    }


    /// indicates whether the translations should be reordered
    bool DBTranslator4RangeVariable::needsReordering() const {
      // if the variable contains only numbers, they should be increasing
      const auto& labels      = _variable_.labels();
      std::size_t last_number = std::numeric_limits< std::size_t >::lowest();
      std::size_t number;
      for (const auto& label: labels) {
        number = this->back_dico_.first(label);
        if (number < last_number) return true;
        last_number = number;
      }

      return false;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
