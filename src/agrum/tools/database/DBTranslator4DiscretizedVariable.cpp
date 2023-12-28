/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The databases' cell translators for discretized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/DBTranslator4DiscretizedVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBTranslator4DiscretizedVariable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor with a IDiscretized variable as translator
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
       const IDiscretizedVariable&       var,
       const std::vector< std::string >& missing_symbols,
       std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     false,
                     missing_symbols,
                     false,
                     max_dico_entries),
        _variable_(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // copy the ticks of var into our internal variable
      const auto ticks = var.ticksAsDoubles();
      for (const auto tick: ticks) {
        _variable_.addTick((float)tick);
      }

      // the bounds of the discretized variable
      const auto lower_bound = float(ticks[0]);
      const auto upper_bound = float(ticks.back());

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->missing_symbols_.erase(iter);
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols. If, in addition, it has already
        // been entered into the back_dictionary, then, this has been done
        // because the label corresponded to a missing value, so we should
        // remove the label as well from the back_dictionary.
        if (this->missing_symbols_.exists(label)) { this->missing_symbols_.erase(label); }

        this->back_dico_.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }

    /// default constructor with a IDiscretized variable as translator
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
       const IDiscretizedVariable& var,
       std::size_t                 max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, false, false, max_dico_entries),
        _variable_(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // copy the ticks of var into our internal variable
      const auto ticks = var.ticksAsDoubles();
      for (const auto tick: ticks) {
        _variable_.addTick((float)tick);
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        this->back_dico_.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }

    /// copy constructor
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
       const DBTranslator4DiscretizedVariable& from) :
        DBTranslator(from),
        _variable_(from._variable_) {
      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = from._real_variable_->clone();

      GUM_CONS_CPY(DBTranslator4DiscretizedVariable);
    }

    /// move constructor
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
       DBTranslator4DiscretizedVariable&& from) :
        DBTranslator(std::move(from)),
        _variable_(std::move(from._variable_)) {
      // moves the copy of the variable, that should be used by method variable ()
      _real_variable_      = from._real_variable_;
      from._real_variable_ = nullptr;

      GUM_CONS_MOV(DBTranslator4DiscretizedVariable);
    }

    /// virtual copy constructor
    DBTranslator4DiscretizedVariable* DBTranslator4DiscretizedVariable::clone() const {
      return new DBTranslator4DiscretizedVariable(*this);
    }

    /// copy operator
    DBTranslator4DiscretizedVariable&
       DBTranslator4DiscretizedVariable::operator=(const DBTranslator4DiscretizedVariable& from) {
      if (this != &from) {
        DBTranslator::operator=(from);
        _variable_ = from._variable_;

        if (_real_variable_ != nullptr) delete _real_variable_;
        _real_variable_ = from._real_variable_->clone();
      }

      return *this;
    }

    /// move operator
    DBTranslator4DiscretizedVariable&
       DBTranslator4DiscretizedVariable::operator=(DBTranslator4DiscretizedVariable&& from) {
      if (this != &from) {
        DBTranslator::operator=(std::move(from));
        _variable_ = std::move(from._variable_);

        if (_real_variable_ != nullptr) delete _real_variable_;
        _real_variable_      = from._real_variable_;
        from._real_variable_ = nullptr;
      }

      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
