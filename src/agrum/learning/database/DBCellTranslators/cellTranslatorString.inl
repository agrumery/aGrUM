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
 * @brief A cell translator translating DBCells that contain string
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE
    CellTranslatorString::CellTranslatorString(const CellTranslatorString &from)
        : DBCellTranslator<1, 1>(from), __max_value(from.__max_value),
          __strings(from.__strings), __check_database(from.__check_database) {
      if (from.__user_values != nullptr) {
        __user_values = new Sequence<std::string>(*from.__user_values);
      }
    }

    /// move constructor
    INLINE CellTranslatorString::CellTranslatorString(CellTranslatorString &&from)
        : DBCellTranslator<1, 1>(std::move(from)),
          __max_value(std::move(from.__max_value)),
          __strings(std::move(from.__strings)),
          __check_database(std::move(from.__check_database)) {
      if (from.__user_values != nullptr) {
        __user_values = new Sequence<std::string>(std::move(*from.__user_values));
      }
    }

    /// virtual copy constructor
    INLINE CellTranslatorString *CellTranslatorString::copyFactory() {
      return new CellTranslatorString(*this);
    }

    /// destructor
    INLINE CellTranslatorString::~CellTranslatorString() {
      if (__user_values != nullptr) {
        delete __user_values;
      }
    }

    /// copy operator
    INLINE CellTranslatorString &CellTranslatorString::
    operator=(const CellTranslatorString &from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(from);
        __max_value = from.__max_value;
        __strings = from.__strings;
        __check_database = from.__check_database;
        if (__user_values) {
          delete __user_values;
          __user_values = nullptr;
        }
        if (from.__user_values != nullptr) {
          __user_values = new Sequence<std::string>(*from.__user_values);
        }
      }
      return *this;
    }

    /// move operator
    INLINE CellTranslatorString &CellTranslatorString::
    operator=(CellTranslatorString &&from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(std::move(from));
        __max_value = std::move(from.__max_value);
        __strings = std::move(from.__strings);
        __check_database = from.__check_database;
        if (__user_values) {
          delete __user_values;
          __user_values = nullptr;
        }
        if (from.__user_values != nullptr) {
          __user_values = new Sequence<std::string>(std::move(*from.__user_values));
        }
      }
      return *this;
    }

    /// perform the translation
    ALWAYS_INLINE void CellTranslatorString::translate() {
      out(0) = __strings.second(in(0).getStringIndex());
    }

    /// initialize the cell translator by a first database parsing
    ALWAYS_INLINE void CellTranslatorString::initialize() {
      const int nb = in(0).getStringIndex();
      if (!__strings.existsFirst(nb)) {
        __strings.insert(nb, __max_value);
        ++__max_value;
      }
    }

    /// add the number of modalities discovered in the database into a vector
    INLINE void
    CellTranslatorString::modalities(std::vector<unsigned int> &modal) const
        noexcept {
      modal.push_back(__max_value);
    }

    /// returns whether the translator needs a DB parsing to initialize itself
    INLINE bool CellTranslatorString::requiresInitialization() const noexcept {
      return __check_database;
    }

    /// returns a given value as stored within the database
    INLINE std::string
    CellTranslatorString::translateBack(unsigned int col,
                                        unsigned int translated_val) const {
      std::stringstream str;
      str << DBCell::getString(__strings.first(translated_val));
      return str.str();
    }

    /// returns the name of the variable(s) the translator has processed
    INLINE void CellTranslatorString::variableNames(
        const std::vector<std::string> &db_var,
        std::vector<std::string> &output_vars) const {
      output_vars.push_back(db_var[_input_cols[0]]);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
