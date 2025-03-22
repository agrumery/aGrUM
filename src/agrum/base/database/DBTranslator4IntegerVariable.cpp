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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The databases' cell translators for integer variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBTranslator4IntegerVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBTranslator4IntegerVariable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor with a integer variable as translator
    DBTranslator4IntegerVariable::DBTranslator4IntegerVariable(
        const IntegerVariable&            var,
        const std::vector< std::string >& missing_symbols,
        std::size_t                       max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     true,
                     missing_symbols,
                     false,
                     max_dico_entries),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // remove all the missing symbols corresponding to a number belonging to the variable
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isInteger(*iter)) {
          const int missing_val = std::stoi(*iter);
          if (_variable_.isValue(missing_val)) { this->missing_symbols_.erase(iter); }
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

      // for debugging purposes
      GUM_CONSTRUCTOR(DBTranslator4IntegerVariable);
    }

    /// default constructor with an integer variable as translator but no missing value
    DBTranslator4IntegerVariable::DBTranslator4IntegerVariable(const IntegerVariable& var,
                                                               std::size_t max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, true, false, max_dico_entries),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        this->back_dico_.insert(size, label);
        ++size;
      }

      // for debugging purposes
      GUM_CONSTRUCTOR(DBTranslator4IntegerVariable);
    }

    /// copy constructor
    DBTranslator4IntegerVariable::DBTranslator4IntegerVariable(
        const DBTranslator4IntegerVariable& from) :
        DBTranslator(from), _variable_(from._variable_) {
      // for debugging purposes
      GUM_CONS_CPY(DBTranslator4IntegerVariable);
    }

    /// move constructor
    DBTranslator4IntegerVariable::DBTranslator4IntegerVariable(
        DBTranslator4IntegerVariable&& from) :
        DBTranslator(std::move(from)), _variable_(std::move(from._variable_)) {
      // for debugging purposes
      GUM_CONS_MOV(DBTranslator4IntegerVariable);
    }

    /// virtual copy constructor
    DBTranslator4IntegerVariable* DBTranslator4IntegerVariable::clone() const {
      return new DBTranslator4IntegerVariable(*this);
    }

    /// copy operator
    DBTranslator4IntegerVariable&
        DBTranslator4IntegerVariable::operator=(const DBTranslator4IntegerVariable& from) {
      if (this != &from) {
        DBTranslator::operator=(from);
        _variable_ = from._variable_;
      }

      return *this;
    }

    /// move operator
    DBTranslator4IntegerVariable&
        DBTranslator4IntegerVariable::operator=(DBTranslator4IntegerVariable&& from) {
      if (this != &from) {
        DBTranslator::operator=(std::move(from));
        _variable_ = std::move(from._variable_);
      }

      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
