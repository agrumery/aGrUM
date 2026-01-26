/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief The databases' cell translators for labelized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBTranslator4LabelizedVariable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(
        const std::vector< std::string >& missing_symbols,
        std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     true,
                     missing_symbols,
                     true,
                     max_dico_entries),
        _variable_("var", "", 0) {
      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }

    /// default constructor without missing symbols
    DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(std::size_t max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, true, true, max_dico_entries),
        _variable_("var", "", 0) {
      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }

    /// default constructor with a labelized variable as translator
    DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(
        const LabelizedVariable&          var,
        const std::vector< std::string >& missing_symbols,
        const bool                        editable_dictionary,
        std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     true,
                     missing_symbols,
                     editable_dictionary,
                     max_dico_entries),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols.
        if (this->missing_symbols_.exists(label)) { this->missing_symbols_.erase(label); }

        // insert the label into the back_dictionary
        this->back_dico_.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }

    /// default constructor with a labelized variable as translator
    DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(const LabelizedVariable& var,
                                                                   const bool  editable_dictionary,
                                                                   std::size_t max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, true, editable_dictionary, max_dico_entries),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // insert the label into the back_dictionary
        this->back_dico_.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }

    /// virtual copy constructor
    DBTranslator4LabelizedVariable* DBTranslator4LabelizedVariable::clone() const {
      return new DBTranslator4LabelizedVariable(*this);
    }

    /// copy operator
    DBTranslator4LabelizedVariable&
        DBTranslator4LabelizedVariable::operator=(const DBTranslator4LabelizedVariable& from) {
      if (this != &from) {
        DBTranslator::operator=(from);
        _variable_ = from._variable_;
      }

      return *this;
    }

    /// move operator
    DBTranslator4LabelizedVariable&
        DBTranslator4LabelizedVariable::operator=(DBTranslator4LabelizedVariable&& from) {
      if (this != &from) {
        DBTranslator::operator=(std::move(from));
        _variable_ = std::move(from._variable_);
      }

      return *this;
    }

    /// returns the translation of a string, as found in the current dictionary
    DBTranslatedValue DBTranslator4LabelizedVariable::translate(const std::string& str) {
      // try to get the index of str within the labelized variable. If this
      // cannot be found, try to find if this corresponds to a missing value.
      // Finally, if this is still not a missing value and, if enabled, try
      // to add str as a new label
      try {
        return DBTranslatedValue{std::size_t(_variable_[str])};
      } catch (gum::Exception&) {
        // check that this is not a missing value
        if (this->isMissingSymbol(str)) {
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
        }

        // try to add str as a new value if possible
        if (this->hasEditableDictionary()) {
          const std::size_t size = _variable_.domainSize();
          if (size >= this->max_dico_entries_)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated "
                                  << "because the dictionary is already full");
          _variable_.addLabel(str);
          this->back_dico_.insert(size, str);
          return DBTranslatedValue{size};
        } else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The translation of \"" << str << "\" could not be found for variable '"
                                            << _variable_ << "'.");
      }
    }

    /// indicates whether the translations should be reordered
    bool DBTranslator4LabelizedVariable::needsReordering() const {
      // if the variable contains only numbers, they should be increasing
      const auto& labels      = _variable_.labels();
      float       last_number = std::numeric_limits< float >::lowest();
      float       number;
      bool        only_numbers = true;
      for (const auto& label: labels) {
        if (!DBCell::isReal(label)) {
          only_numbers = false;
          break;
        }
        number = std::stof(label);
        if (number < last_number) return true;
        last_number = number;
      }

      if (!only_numbers) {
        // here we shall examine whether the strings are sorted by
        // lexicographical order
        const std::size_t size = labels.size();
        for (std::size_t i = 1; i < size; ++i) {
          if (labels[i] < labels[i - 1]) return true;
        }
      }

      return false;
    }

    /// returns a mapping to reorder the current dictionary and updates it
    HashTable< std::size_t, std::size_t > DBTranslator4LabelizedVariable::reorder() {
      // check whether the variable contains only numeric values. In this
      // case, we have to sort the values by increasing number
      const auto&       labels = _variable_.labels();
      const std::size_t size   = labels.size();

      bool only_numbers = true;
      for (const auto& label: labels) {
        if (!DBCell::isReal(label)) {
          only_numbers = false;
          break;
        }
      }

      // assign to each label its current index
      std::vector< std::pair< std::size_t, std::string > > xlabels;
      xlabels.reserve(size);
      for (std::size_t i = std::size_t(0); i < size; ++i)
        xlabels.push_back(std::make_pair(i, labels[i]));

      // reorder by increasing order
      if (only_numbers)
        std::sort(xlabels.begin(),
                  xlabels.end(),
                  [](const std::pair< std::size_t, std::string >& a,
                     const std::pair< std::size_t, std::string >& b) -> bool {
                    return std::stof(a.second) < std::stof(b.second);
                  });
      else
        std::sort(xlabels.begin(),
                  xlabels.end(),
                  [](const std::pair< std::size_t, std::string >& a,
                     const std::pair< std::size_t, std::string >& b) -> bool {
                    return a.second < b.second;
                  });

      // check whether there were any modification
      bool modifications = false;
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        if (xlabels[i].first != i) {
          modifications = true;
          break;
        }
      }

      // if there were no modification, return an empty update hashtable
      if (!modifications) { return HashTable< std::size_t, std::size_t >(); }

      // recreate the variable so that the labels correspond to the
      // new ordering
      _variable_.eraseLabels();
      for (auto& label: xlabels)
        _variable_.addLabel(label.second);

      // create the hashTable corresponding to the mapping from the old
      // indices to the new one
      this->back_dico_.clear();
      HashTable< std::size_t, std::size_t > mapping((Size)size);
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        mapping.insert(xlabels[i].first, i);
        this->back_dico_.insert(i, xlabels[i].second);
      }

      return mapping;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
