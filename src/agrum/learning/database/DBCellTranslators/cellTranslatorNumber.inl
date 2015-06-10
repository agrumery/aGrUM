/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief A cell translator translating numbers (possibly real or integers)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE
    CellTranslatorNumber::CellTranslatorNumber(const CellTranslatorNumber &from)
        : DBCellTranslator<1, 1>(from), __max_value(from.__max_value),
          __values(from.__values), __check_database(from.__check_database) {
      if (from.__user_values != nullptr) {
        __user_values = new Sequence<float>(*from.__user_values);
      }
    }

    /// move constructor
    INLINE CellTranslatorNumber::CellTranslatorNumber(CellTranslatorNumber &&from)
        : DBCellTranslator<1, 1>(std::move(from)),
          __max_value(std::move(from.__max_value)),
          __values(std::move(from.__values)),
          __check_database(std::move(from.__check_database)) {
      if (from.__user_values != nullptr) {
        __user_values = new Sequence<float>(std::move(*from.__user_values));
      }
    }

    /// virtual copy constructor
    INLINE CellTranslatorNumber *CellTranslatorNumber::copyFactory() {
      return new CellTranslatorNumber(*this);
    }

    /// destructor
    INLINE CellTranslatorNumber::~CellTranslatorNumber() {
      if (__user_values != nullptr) {
        delete __user_values;
      }
    }

    /// copy operator
    INLINE CellTranslatorNumber &CellTranslatorNumber::
    operator=(const CellTranslatorNumber &from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(from);
        __max_value = from.__max_value;
        __values = from.__values;
        __check_database = from.__check_database;
        if (__user_values) {
          delete __user_values;
          __user_values = nullptr;
        }
        if (from.__user_values != nullptr) {
          __user_values = new Sequence<float>(*from.__user_values);
        }
      }
      return *this;
    }

    /// move operator
    INLINE CellTranslatorNumber &CellTranslatorNumber::
    operator=(CellTranslatorNumber &&from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(std::move(from));
        __max_value = std::move(from.__max_value);
        __values = std::move(from.__values);
        __check_database = from.__check_database;
        if (__user_values) {
          delete __user_values;
          __user_values = nullptr;
        }
        if (from.__user_values != nullptr) {
          __user_values = new Sequence<float>(std::move(*from.__user_values));
        }
      }
      return *this;
    }

    /// perform the translation
    ALWAYS_INLINE void CellTranslatorNumber::translate() {
      out(0) = __values.second(in(0).getFloat());
    }

    /// initialize the cell translator by a first database parsing
    ALWAYS_INLINE void CellTranslatorNumber::initialize() {
      const float nb = in(0).getFloat();
      if (!__values.existsFirst(nb)) {
        __values.insert(nb, __max_value);
        ++__max_value;
      }
    }

    /// add the number of modalities discovered in the database into a vector
    INLINE void
    CellTranslatorNumber::modalities(std::vector<unsigned int> &modal) const
        noexcept {
      modal.push_back(__values.size());
    }

    /// returns whether the translator needs a DB parsing to initialize itself
    INLINE bool CellTranslatorNumber::requiresInitialization() const noexcept {
      return __check_database;
    }

    /// returns a given value as stored within the database
    INLINE std::string
    CellTranslatorNumber::translateBack(unsigned int col,
                                        unsigned int translated_val) const {
      std::stringstream str;
      str << __values.first(translated_val);
      return str.str();
    }

    /// returns the name of the variable(s) the translator has processed
    INLINE void CellTranslatorNumber::variableNames(
        const std::vector<std::string> &db_var,
        std::vector<std::string> &output_vars) const {
      output_vars.push_back(db_var[_input_cols[0]]);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
