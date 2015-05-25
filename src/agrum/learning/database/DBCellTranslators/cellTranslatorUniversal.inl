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
 * @brief A cell translator translating any DBCell
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE CellTranslatorUniversal::CellTranslatorUniversal(
        const CellTranslatorUniversal &from)
        : DBCellTranslator<1, 1>(from), __max_value(from.__max_value),
          __numbers(from.__numbers), __strings(from.__strings),
          __check_database(from.__check_database) {
      if (from.__str_user_values != nullptr) {
        __str_user_values = new Sequence<std::string>(*from.__str_user_values);
      }

      if (from.__num_user_values != nullptr) {
        __num_user_values = new Sequence<float>(*from.__num_user_values);
      }
    }

    /// move constructor
    INLINE
    CellTranslatorUniversal::CellTranslatorUniversal(CellTranslatorUniversal &&from)
        : DBCellTranslator<1, 1>(std::move(from)),
          __max_value(std::move(from.__max_value)),
          __numbers(std::move(from.__numbers)), __strings(std::move(from.__strings)),
          __check_database(std::move(from.__check_database)) {
      if (from.__str_user_values != nullptr) {
        __str_user_values =
            new Sequence<std::string>(std::move(*from.__str_user_values));
      }

      if (from.__num_user_values != nullptr) {
        __num_user_values = new Sequence<float>(std::move(*from.__num_user_values));
      }
    }

    /// virtual copy constructor
    INLINE CellTranslatorUniversal *CellTranslatorUniversal::copyFactory() {
      return new CellTranslatorUniversal(*this);
    }

    /// destructor
    INLINE CellTranslatorUniversal::~CellTranslatorUniversal() {
      if (__str_user_values != nullptr) {
        delete __str_user_values;
      }

      if (__num_user_values != nullptr) {
        delete __num_user_values;
      }
    }

    /// copy operator
    INLINE CellTranslatorUniversal &CellTranslatorUniversal::
    operator=(const CellTranslatorUniversal &from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(from);
        __max_value = from.__max_value;
        __numbers = from.__numbers;
        __strings = from.__strings;
        __check_database = from.__check_database;

        if (__str_user_values) {
          delete __str_user_values;
          __str_user_values = nullptr;
        }

        if (__num_user_values) {
          delete __num_user_values;
          __num_user_values = nullptr;
        }

        if (from.__str_user_values != nullptr) {
          __str_user_values = new Sequence<std::string>(*from.__str_user_values);
        }

        if (from.__num_user_values != nullptr) {
          __num_user_values = new Sequence<float>(*from.__num_user_values);
        }
      }

      return *this;
    }

    /// move operator
    INLINE CellTranslatorUniversal &CellTranslatorUniversal::
    operator=(CellTranslatorUniversal &&from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(std::move(from));
        __max_value = std::move(from.__max_value);
        __numbers = std::move(from.__numbers);
        __strings = std::move(from.__strings);
        __check_database = from.__check_database;

        if (__str_user_values) {
          delete __str_user_values;
          __str_user_values = nullptr;
        }

        if (__num_user_values) {
          delete __num_user_values;
          __num_user_values = nullptr;
        }

        if (from.__str_user_values != nullptr) {
          __str_user_values =
              new Sequence<std::string>(std::move(*from.__str_user_values));
        }

        if (from.__num_user_values != nullptr) {
          __num_user_values =
              new Sequence<float>(std::move(*from.__num_user_values));
        }
      }

      return *this;
    }

    /// perform the translation
    ALWAYS_INLINE void CellTranslatorUniversal::translate() {
      /*auto res=in ( 0 ).type() == DBCell::EltType::FLOAT
               ? __numbers.second ( in ( 0 ).getFloat() )
               : __strings.second ( in ( 0 ).getStringIndex() );
      std::cout<< ( in ( 0 ).type() == DBCell::EltType::FLOAT
                    ? "FLOAT "
                    : "STRING " )
               << ( in ( 0 ).type() == DBCell::EltType::FLOAT
                    ? in ( 0 ).getFloat()
                    : in ( 0 ).getStringIndex() ) << "  ** "<<__numbers << "  **
      "<<__strings<<" ==> "<<res<<std::endl;*/
      out(0) = in(0).type() == DBCell::EltType::FLOAT
                   ? __numbers.second(in(0).getFloat())
                   : __strings.second(in(0).getStringIndex());
    }

    /// initialize the cell translator by a first database parsing
    ALWAYS_INLINE void CellTranslatorUniversal::initialize() {
      if (__check_database) {
        if (in(0).type() == DBCell::EltType::FLOAT) {
          const float nb = in(0).getFloat();

          if (!__numbers.existsFirst(nb)) {
            __numbers.insert(nb, __max_value);
            ++__max_value;
          }
        } else {
          const int nb = in(0).getStringIndex();

          if (!__strings.existsFirst(nb)) {
            __strings.insert(nb, __max_value);
            ++__max_value;
          }
        }
      }
    }

    /// add the number of modalities discovered in the database into a vector
    INLINE void
    CellTranslatorUniversal::modalities(std::vector<unsigned int> &modal) const
        noexcept {
      modal.push_back(__max_value);
    }

    /// returns whether the translator needs a DB parsing to initialize itself
    INLINE bool CellTranslatorUniversal::requiresInitialization() const noexcept {
      return __check_database;
    }

    /// returns a given value as stored within the database
    INLINE std::string
    CellTranslatorUniversal::translateBack(unsigned int col,
                                           unsigned int translated_val) const {
      std::stringstream str;

      if (__numbers.existsSecond(translated_val))
        str << __numbers.first(translated_val);
      else
        str << DBCell::getString(__strings.first(translated_val));

      return str.str();
    }

    /// returns the name of the variable(s) the translator has processed
    INLINE void CellTranslatorUniversal::variableNames(
        const std::vector<std::string> &db_var,
        std::vector<std::string> &output_vars) const {
      output_vars.push_back(db_var[_input_cols[0]]);
    }

    /// returns the set of translations for string values in the database
    INLINE const Bijection<int, unsigned int> &
    CellTranslatorUniversal::stringTranslations() const noexcept {
      return __strings;
    }

    /// returns the set of translations for number values in the database
    INLINE const Bijection<float, unsigned int> &
    CellTranslatorUniversal::numberTranslations() const noexcept {
      return __numbers;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
