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

#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    CellTranslatorUniversal::CellTranslatorUniversal(Sequence<std::string> values,
                                                     bool check_database)
        : __check_database(check_database) {

      if (!check_database) {
        if (values.empty()) {
          __check_database = true;
        } else {
          // if we do not want to parse the database, store all the values directly
          DBCell cell;
          for (const auto &str : values) {
            cell.setStringSafe(str);
            __strings.insert(cell.getStringIndex(), __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __str_user_values = new Sequence<std::string>(std::move(values));
        }
      }
    }

    /// default constructor
    CellTranslatorUniversal::CellTranslatorUniversal(Sequence<float> values,
                                                     bool check_database)
        : __check_database(check_database) {

      if (!check_database) {
        if (values.empty()) {
          __check_database = true;
        } else {
          // if we do not want to parse the database, store all the values directly
          for (const auto &val : values) {
            __numbers.insert(val, __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __num_user_values = new Sequence<float>(std::move(values));
        }
      }
    }

    /// perform a post initialization after the database parsing
    void CellTranslatorUniversal::postInitialize() {
      if (__str_user_values != nullptr) {
        // we probably need to reorder the sequence to be compatible with the
        // order of the values specified by the user
        std::vector<int> no_user_vals;
        std::vector<std::pair<unsigned int, unsigned int>> user_vals;
        for (auto iter = __strings.begin(); iter != __strings.end(); ++iter) {
          const int str_index = iter.first();
          const std::string &str = DBCell::getString(str_index);
          if (__str_user_values->exists(str)) {
            user_vals.push_back(std::pair<unsigned int, unsigned int>(
                str_index, __str_user_values->pos(str)));
          } else {
            no_user_vals.push_back(str_index);
          }
        }

        // reorder user_vals in increasing order of the second argument
        std::sort(user_vals.begin(), user_vals.end(),
                  [](const std::pair<unsigned int, unsigned int> &elt1,
                     const std::pair<unsigned int, unsigned int> &elt2)
                      -> bool { return elt1.second < elt2.second; });

        // restore in the appropriate order the element of __strings
        __strings.clear();
        __max_value = 0;
        for (const auto &str : user_vals) {
          __strings.insert(str.first, __max_value);
          ++__max_value;
        }
        for (const auto str : no_user_vals) {
          __strings.insert(str, __max_value);
          ++__max_value;
        }

        // if there existed numbers, add them (in any order)
        std::vector<float> numbers(__numbers.size());
        unsigned int i = 0;
        for (auto iter = __numbers.begin(); iter != __numbers.end(); ++iter, ++i) {
          numbers[i] = iter.first();
        }
        __numbers.clear();
        for (const auto num : numbers) {
          __numbers.insert(num, __max_value);
          ++__max_value;
        }

        // the user values are not needed any more, so, remove them
        delete __str_user_values;
        __str_user_values = nullptr;
      } else if (__num_user_values != nullptr) {
        // we probably need to reorder the sequence to be compatible with the
        // order of the values specified by the user
        std::vector<float> no_user_vals;
        std::vector<std::pair<float, unsigned int>> user_vals;
        for (auto iter = __numbers.begin(); iter != __numbers.end(); ++iter) {
          const float val = iter.first();
          if (__num_user_values->exists(val)) {
            user_vals.push_back(
                std::pair<float, unsigned int>(val, __num_user_values->pos(val)));
          } else {
            no_user_vals.push_back(val);
          }
        }

        // reorder user_vals in increasing order of the second argument
        std::sort(user_vals.begin(), user_vals.end(),
                  [](const std::pair<float, unsigned int> &elt1,
                     const std::pair<float, unsigned int> &elt2)
                      -> bool { return elt1.second < elt2.second; });

        // restore in the appropriate order the element of __strings
        __numbers.clear();
        __max_value = 0;
        for (const auto &val : user_vals) {
          __numbers.insert(val.first, __max_value);
          ++__max_value;
        }
        for (const auto val : no_user_vals) {
          __numbers.insert(val, __max_value);
          ++__max_value;
        }

        // if there existed strings, add them (in any order)
        std::vector<int> strings(__strings.size());
        unsigned int i = 0;
        for (auto iter = __strings.begin(); iter != __strings.end(); ++iter, ++i) {
          strings[i] = iter.first();
        }
        __strings.clear();
        for (const auto str : strings) {
          __strings.insert(str, __max_value);
          ++__max_value;
        }

        // the user values are not needed any more, so, remove them
        delete __num_user_values;
        __num_user_values = nullptr;
      }
    }

    /// specify the set of possible values (to do before creating the row filter)
    void CellTranslatorUniversal::setUserValues(const Sequence<float> &values,
                                                bool check_database) {
      // clear all current data
      __numbers.clear();
      __strings.clear();
      __max_value = 0;
      if (__str_user_values != nullptr) {
        delete __str_user_values;
        __str_user_values = nullptr;
      }
      if (__num_user_values != nullptr) {
        delete __num_user_values;
        __num_user_values = nullptr;
      }

      // set the internal structures according to the method's parameters
      __check_database = check_database;

      if (!check_database) {
        if (values.empty()) {
          __check_database = true;
        } else {
          // if we do not want to parse the database, store all the values directly
          for (const auto &val : values) {
            __numbers.insert(val, __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __num_user_values = new Sequence<float>(values);
        }
      }
    }

    /// specify the set of possible values (to do before creating the row filter)
    void CellTranslatorUniversal::setUserValues(const Sequence<std::string> &values,
                                                bool check_database) {
      // clear all current data
      __numbers.clear();
      __strings.clear();
      __max_value = 0;
      if (__str_user_values != nullptr) {
        delete __str_user_values;
        __str_user_values = nullptr;
      }
      if (__num_user_values != nullptr) {
        delete __num_user_values;
        __num_user_values = nullptr;
      }

      // set the internal structures according to the method's parameters
      __check_database = check_database;

      if (!check_database) {
        if (values.empty()) {
          __check_database = true;
        } else {
          // if we do not want to parse the database, store all the values directly
          DBCell cell;
          for (const auto &str : values) {
            cell.setStringSafe(str);
            __strings.insert(cell.getStringIndex(), __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __str_user_values = new Sequence<std::string>(values);
        }
      }
    }

    std::string CellTranslatorUniversal::toString() const noexcept {
      std::stringstream s;

      s << "numbers: " << __numbers << " ### strings: ";
      for (auto iter = __strings.begin(); iter != __strings.end(); ++iter) {
        s << '(' << DBCell::getString(iter.first()) << " -> " << iter.second() << ')'
          << "  ";
      }

      return s.str();
    }

  } /* namespace learning */

} /* namespace gum */
