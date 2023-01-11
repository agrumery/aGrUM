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
 * @brief The class for storing several translators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/DBTranslatorSet.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBTranslatorSet_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// remove all the translators from the vector
    void DBTranslatorSet::clear() {
      for (auto translator: _translators_) {
        delete translator;
      }

      _translators_.clear();
      _columns_.clear();
      _highest_column_ = std::size_t(0);
    }


    /// copy the content of another translator set
    void DBTranslatorSet::_copy_(const DBTranslatorSet& from) {
      if (_translators_.size() != 0) clear();

      // resize the vectors used in the set. First, we reserve new memory. This
      // will keep the DBTranslatorSet in a correct state, even if the memory
      // allocation fails
      const std::size_t size = from._translators_.size();
      _translators_.reserve(size);
      _columns_.reserve(size);
      _translators_.resize(size);
      _columns_.resize(size);

      std::size_t i;
      try {
        for (i = std::size_t(0); i < size; ++i) {
          _translators_[i] = from._translators_[i]->clone();
          _columns_[i]     = from._columns_[i];
        }
      } catch (...) {
        _translators_.resize(i);
        clear();
        throw;
      }

      _highest_column_ = from._highest_column_;
    }


    /// copy operator
    DBTranslatorSet& DBTranslatorSet::operator=(const DBTranslatorSet& from) {
      if (this != &from) {
        clear();
        _copy_(from);
      }

      return *this;
    }


    /// returns a new translator corresponding to a variable and some missing symbols


    /// inserts a new translator for a given variable in the translator set
    std::size_t DBTranslatorSet::insertTranslator(const Variable&                   var,
                                                  const std::size_t                 column,
                                                  const std::vector< std::string >& missing_symbols,
                                                  const bool                        unique_column) {
      // create the translator, depending on the type of the variable
      DBTranslator* translator = DBTranslators::create(var, missing_symbols);

      const std::size_t index = insertTranslator(*translator, column, unique_column);

      // deallocate the translator
      delete translator;

      return index;
    }


    /// inserts a new translator at the end of the translator set
    std::size_t DBTranslatorSet::insertTranslator(const DBTranslator& translator,
                                                  const std::size_t   column,
                                                  const bool          unique_column) {
      // if the unique_column parameter is set to true and there exists already
      // another translator that parses the column, raise a DuplicateElement
      // exception
      const std::size_t size = _translators_.size();
      if (unique_column) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (_columns_[i] == column)
            GUM_ERROR(DuplicateElement,
                      "There already exists a DBTranslator that parses Column" << column)
        }
      }

      // reserve some place for the new translator
      _translators_.reserve(size + 1);
      _columns_.reserve(size + 1);

      // create and add the new translator
      DBTranslator* new_translator = translator.clone();

      _translators_.resize(size + 1);
      _columns_.resize(size + 1);
      _translators_[size] = new_translator;
      _columns_[size]     = column;

      // update the highest column
      if (column > _highest_column_) _highest_column_ = column;

      return size;
    }


    /// erase the kth translator
    void DBTranslatorSet::eraseTranslator(const std::size_t k, const bool k_is_input_col) {
      const std::size_t nb_trans = _translators_.size();

      if (!k_is_input_col) {
        if (nb_trans < k) return;

        // remove the translator and its corresponding column
        delete _translators_[k];

        const std::size_t colk = _columns_[k];
        _translators_.erase(_translators_.begin() + k);
        _columns_.erase(_columns_.begin() + k);

        // if the highest column index corresponded to the kth translator,
        // we must recomput it
        if (_highest_column_ == colk) {
          _highest_column_ = std::size_t(0);
          for (const auto col: _columns_)
            if (_highest_column_ < col) _highest_column_ = col;
        }
      } else {
        // remove all the translators parsing the kth column
        auto iter_trans       = _translators_.rbegin();
        bool translator_found = false;
        for (auto iter_col = _columns_.rbegin(); iter_col != _columns_.rend();
             ++iter_col, ++iter_trans) {
          if (*iter_col == k) {
            // remove the translator and its corresponding column
            delete *iter_trans;

            _translators_.erase((iter_trans + 1).base());
            _columns_.erase((iter_col + 1).base());
            translator_found = true;
          }
        }

        // if the highest column index corresponded to one of the translators
        // removed, we must recompute it
        if (translator_found && (k == _highest_column_)) {
          _highest_column_ = std::size_t(0);
          for (const auto col: _columns_)
            if (_highest_column_ < col) _highest_column_ = col;
        }
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
