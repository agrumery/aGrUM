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
 * @brief The databases' cell translators for range variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <utility>
#include <vector>
#include <limits>
#include <cstdio>

#include <agrum/tools/database/DBTranslator4RangeVariable.h>
#include <agrum/tools/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// destructor
    INLINE DBTranslator4RangeVariable::~DBTranslator4RangeVariable() {
      GUM_DESTRUCTOR(DBTranslator4RangeVariable);
    }


    /// returns the original value for a given translation
    INLINE std::string
           DBTranslator4RangeVariable::translateBack(const DBTranslatedValue translated_val) const {
      try {
        return this->back_dico_.second(translated_val.discr_val);
      } catch (Exception const&) {
        // check if this is a missing value
        if (translated_val.discr_val == std::numeric_limits< std::size_t >::max()) {
          if (!_nonint_missing_symbol_.empty()) return _nonint_missing_symbol_;
          if (this->missing_symbols_.empty()) return *(this->missing_symbols_.begin());
        }

        GUM_ERROR(UnknownLabelInDatabase,
                  "The back translation of \"" << translated_val.discr_val
                                               << "\" could not be found");
      }
    }


    /// returns a mapping to reorder the current dictionary and updates it
    INLINE HashTable< std::size_t, std::size_t > DBTranslator4RangeVariable::reorder() {
      // assign to each label the index it had before reordering
      const auto&                                          labels = _variable_.labels();
      const std::size_t                                    size   = labels.size();
      std::vector< std::pair< std::size_t, std::string > > xlabels;
      xlabels.reserve(size);
      bool modifications = false;
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        const std::size_t old_val = this->back_dico_.first(labels[i]);
        xlabels.push_back(std::make_pair(old_val, labels[i]));
        if (old_val != i) modifications = true;
      }


      // if there were no modification, return an empty update hashtable
      if (!modifications) { return HashTable< std::size_t, std::size_t >(); }

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


    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4RangeVariable::domainSize() const {
      return _variable_.domainSize();
    }


    /// returns the variable stored into the translator
    INLINE const RangeVariable* DBTranslator4RangeVariable::variable() const { return &_variable_; }


    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4RangeVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
