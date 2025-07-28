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


/** @file
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBTranslator4ContinuousVariable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {
    /// default constructor
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        const std::vector< std::string >& missing_symbols,
        const bool                        fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, missing_symbols, fit_range, 1),
        _variable_("var", ""), _fit_range_(fit_range) {
      // Here, if fit_range is set to false, and the range of the
      // random variable will remain (-inf,+inf). So all the missing symbols
      // that are numbers should be discarded since they lie in the domain
      // of the variable. On the other hand, if fit_range is true, each newly
      // observed value will update the range of the variable, so that, again,
      // all the missing symbols that are numbers should be discarded since
      // they always end up lying in the domain of the variable.
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) { this->missing_symbols_.erase(iter); }
      }

      // the remaining symbols are not numbers. Take the first one as
      // the default missing symbols for back translations.
      if (!this->missing_symbols_.empty()) {
        _nonfloat_missing_symbol_ = *(this->missing_symbols_.begin());
      }

      // if fit_range is true, we shall be able to update the ranges of
      // the continuous variable. To indicate that we did not encountered any
      // value yet in the database, we fix the lower bound of  _variable_ to +max
      if (_fit_range_) _variable_.setLowerBound(std::numeric_limits< float >::infinity());

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = _variable_.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }

    /// default constructor
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(const bool fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, fit_range, 1), _variable_("var", ""),
        _fit_range_(fit_range) {
      // if fit_range is true, we shall be able to update the ranges of
      // the continuous variable. To indicate that we did not encountered any
      // value yet in the database, we fix the lower bound of  _variable_ to +max
      if (_fit_range_) _variable_.setLowerBound(std::numeric_limits< float >::infinity());

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = _variable_.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }

    /// default constructor with a IContinuous variable as translator
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        const IContinuousVariable&        var,
        const std::vector< std::string >& missing_symbols,
        const bool                        fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, missing_symbols, fit_range, 1),
        _variable_(var.name(), var.description()), _fit_range_(fit_range) {
      // get the bounds of the range variable
      const auto lower_bound = float(var.lowerBoundAsDouble());
      const auto upper_bound = float(var.upperBoundAsDouble());
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

    /// default constructor with a IContinuous variable as translator
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(const IContinuousVariable& var,
                                                                     const bool fit_range) :
        DBTranslator(DBTranslatedValueType::CONTINUOUS, true, fit_range, 1),
        _variable_(var.name(), var.description()), _fit_range_(fit_range) {
      // get the bounds of the range variable
      const auto lower_bound = float(var.lowerBoundAsDouble());
      const auto upper_bound = float(var.upperBoundAsDouble());
      _variable_.setLowerBound(lower_bound);
      _variable_.setUpperBound(upper_bound);

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }

    /// copy constructor
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        const DBTranslator4ContinuousVariable& from) :
        DBTranslator(from), _variable_(from._variable_),
        _status_float_missing_symbols_(from._status_float_missing_symbols_),
        _nonfloat_missing_symbol_(from._nonfloat_missing_symbol_), _fit_range_(from._fit_range_) {
      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = from._real_variable_->clone();

      GUM_CONS_CPY(DBTranslator4ContinuousVariable);
    }

    /// move constructor
    DBTranslator4ContinuousVariable::DBTranslator4ContinuousVariable(
        DBTranslator4ContinuousVariable&& from) :
        DBTranslator(std::move(from)), _variable_(std::move(from._variable_)),
        _status_float_missing_symbols_(std::move(from._status_float_missing_symbols_)),
        _nonfloat_missing_symbol_(std::move(from._nonfloat_missing_symbol_)),
        _fit_range_(from._fit_range_) {
      // store a copy of the variable, that should be used by method variable ()
      _real_variable_      = from._real_variable_;
      from._real_variable_ = nullptr;

      GUM_CONS_MOV(DBTranslator4ContinuousVariable);
    }

    /// virtual copy constructor
    DBTranslator4ContinuousVariable* DBTranslator4ContinuousVariable::clone() const {
      return new DBTranslator4ContinuousVariable(*this);
    }

    /// copy operator
    DBTranslator4ContinuousVariable&
        DBTranslator4ContinuousVariable::operator=(const DBTranslator4ContinuousVariable& from) {
      if (this != &from) {
        DBTranslator::operator=(from);
        _variable_                     = from._variable_;
        _status_float_missing_symbols_ = from._status_float_missing_symbols_;
        _nonfloat_missing_symbol_      = from._nonfloat_missing_symbol_;
        _fit_range_                    = from._fit_range_;

        if (_real_variable_ != nullptr) delete _real_variable_;
        _real_variable_ = from._real_variable_->clone();
      }

      return *this;
    }

    /// move operator
    DBTranslator4ContinuousVariable&
        DBTranslator4ContinuousVariable::operator=(DBTranslator4ContinuousVariable&& from) {
      if (this != &from) {
        DBTranslator::operator=(std::move(from));
        _variable_                     = std::move(from._variable_);
        _status_float_missing_symbols_ = std::move(from._status_float_missing_symbols_);
        _nonfloat_missing_symbol_      = std::move(from._nonfloat_missing_symbol_);
        _fit_range_                    = from._fit_range_;

        if (_real_variable_ != nullptr) delete _real_variable_;
        _real_variable_      = from._real_variable_;
        from._real_variable_ = nullptr;
      }

      return *this;
    }

    /// returns the translation of a string, as found in the current dictionary
    DBTranslatedValue DBTranslator4ContinuousVariable::translate(const std::string& str) {
      // check if the string is actually a number
      if (!DBCell::isReal(str)) {
        if (this->isMissingSymbol(str)) {
          return DBTranslatedValue{std::numeric_limits< float >::max()};
        } else
          GUM_ERROR(TypeError,
                    "String \"" << str << "\" cannot be translated because it is not a value for "
                                << _variable_);
      }

      // here we know that the string is a number
      const float number = std::stof(str);

      // if we are in the range of the variable, return the number
      if (_variable_.belongs(number)) return DBTranslatedValue{number};

      // check that this is not a missing value
      if (this->isMissingSymbol(str)) {
        if (!_status_float_missing_symbols_[str]) { _status_float_missing_symbols_[str] = true; }
        return DBTranslatedValue{std::numeric_limits< float >::max()};
      }

      // check if we are allowed to update the domain of the variable
      if (!_fit_range_) {
        GUM_ERROR(UnknownLabelInDatabase,
                  "String \"" << str
                              << "\" cannot be translated because it is "
                                 "out of the domain of the continuous variable");
      }

      // now, we can try to add str as a new bound of the range variable
      // if possible

      // if the variable is empty, set the min and max ranges. Here,
      // there is no need to check whether the new range would contain an
      // already translated missing symbol because this was already tested
      // in the above test.
      if (_variable_.lowerBound() == std::numeric_limits< float >::infinity()) {
        _variable_.setLowerBound(number);
        _variable_.setUpperBound(number);
        return DBTranslatedValue{number};
      }

      // here, the domain is not empty. So we should update either the
      // lower bound or the upper bound of the variable, unless
      // a missing symbol lies within the new bounds and we have already
      // translated it.
      const float lower_bound = _variable_.lowerBound();
      const float upper_bound = _variable_.upperBound();
      if (number < lower_bound) {
        // check that there does not already exist a translated missing
        // value within the new bounds of the variable
        for (const auto& missing: _status_float_missing_symbols_) {
          if (missing.second) {
            const float miss_val = std::stof(missing.first);
            if ((miss_val >= number) && (miss_val <= upper_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \"" << str << "\" cannot be translated because "
                                    << "it would induce a new domain containing an already "
                                    << "translated missing symbol");
            }
          }
        }

        // remove all the missing symbols that were not translated yet and
        // that lie within the new bounds of the variable
        for (auto iter = _status_float_missing_symbols_.beginSafe();
             iter != _status_float_missing_symbols_.endSafe();
             ++iter) {
          if (iter.val() == false) {
            const float miss_val = std::stof(iter.key());
            if ((miss_val >= number) && (miss_val <= upper_bound)) {
              this->missing_symbols_.erase(iter.key());
              _status_float_missing_symbols_.erase(iter);
            }
          }
        }

        // update the domain of the continuous variable
        _variable_.setLowerBound(number);

        return DBTranslatedValue{number};
      } else {
        // check that there does not already exist a translated missing
        // value within the new bounds of the variable
        for (const auto& missing: _status_float_missing_symbols_) {
          if (missing.second) {
            const float miss_val = std::stof(missing.first);
            if ((miss_val >= lower_bound) && (miss_val <= number)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \"" << str << "\" cannot be translated because "
                                    << "it would induce a new domain containing an already "
                                    << "translated missing symbol");
            }
          }
        }

        // remove all the missing symbols that were not translated yet and
        // that lie within the new bounds of the variable
        for (auto iter = _status_float_missing_symbols_.beginSafe();
             iter != _status_float_missing_symbols_.endSafe();
             ++iter) {
          if (iter.val() == false) {
            const float miss_val = std::stof(iter.key());
            if ((miss_val >= lower_bound) && (miss_val <= number)) {
              this->missing_symbols_.erase(iter.key());
              _status_float_missing_symbols_.erase(iter);
            }
          }
        }

        // update the domain of the continuous variable
        _variable_.setUpperBound(number);

        return DBTranslatedValue{number};
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
