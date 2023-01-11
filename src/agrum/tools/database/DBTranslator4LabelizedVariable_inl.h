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
 * @brief The databases' cell translators for labelized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <utility>
#include <vector>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(
       const DBTranslator4LabelizedVariable& from) :
        DBTranslator(from),
        _variable_(from._variable_) {
      GUM_CONS_CPY(DBTranslator4LabelizedVariable);
    }


    /// move constructor
    INLINE DBTranslator4LabelizedVariable::DBTranslator4LabelizedVariable(
       DBTranslator4LabelizedVariable&& from) :
        DBTranslator(std::move(from)),
        _variable_(std::move(from._variable_)) {
      GUM_CONS_MOV(DBTranslator4LabelizedVariable);
    }


    /// destructor
    INLINE DBTranslator4LabelizedVariable::~DBTranslator4LabelizedVariable() {
      GUM_DESTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// returns the original value for a given translation
    INLINE std::string
       DBTranslator4LabelizedVariable::translateBack(const DBTranslatedValue translated_val) const {
      try {
        return this->back_dico_.second(translated_val.discr_val);
      } catch (Exception const&) {
        // check if this is a missing value
        if ((translated_val.discr_val == std::numeric_limits< std::size_t >::max())
            && !this->missing_symbols_.empty())
          return *(this->missing_symbols_.begin());
        else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The back translation of \"" << translated_val.discr_val
                                                 << "\" could not be found");
      }
    }


    /// returns the domain size of a variable corresponding to the translations
    INLINE std::size_t DBTranslator4LabelizedVariable::domainSize() const {
      return _variable_.domainSize();
    }


    /// returns the variable stored into the translator
    INLINE const LabelizedVariable* DBTranslator4LabelizedVariable::variable() const {
      return &_variable_;
    }


    /// returns the translation of a missing value
    INLINE DBTranslatedValue DBTranslator4LabelizedVariable::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
