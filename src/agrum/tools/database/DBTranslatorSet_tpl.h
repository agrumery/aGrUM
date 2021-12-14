/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The set of translators stored into a row filter
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// inserts a new translator at the end of the translator set
    template < class Translator >
    std::size_t DBTranslatorSet::insertTranslator(const Translator& translator,
                                                  const std::size_t column,
                                                  const bool        unique_column) {
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


    /// substitute a translator by another one
    template < class Translator >
    void DBTranslatorSet::changeTranslator(const Translator& new_translator,
                                           const std::size_t pos) {
      // check that the translator to be substituted exsts
      if (_translators_.size() < pos) {
        GUM_ERROR(OutOfBounds,
                  "The translatorSet contains only "
                     << _translators_.size()
                     << " translators. It is therefore impossible to substitute "
                     << "the translator at index " << pos);
      }

      // copy the new translator
      DBTranslator* translator = new_translator.clone();

      // remove the old translator
      delete _translators_[pos];

      // insert the copy
      _translators_[pos] = translator;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
