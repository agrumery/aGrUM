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

#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    CellTranslatorString::CellTranslatorString(Sequence<std::string> values,
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
          __user_values = new Sequence<std::string>(std::move(values));
        }
      }
    }

    /// perform a post initialization after the database parsing
    void CellTranslatorString::postInitialize() {
      if (__user_values != nullptr) {
        // we probably need to reorder the sequence to be compatible with the
        // order of the values specified by the user
        std::vector<int> no_user_vals;
        std::vector<std::pair<unsigned int, unsigned int>> user_vals;
        for (unsigned int i = 0; i < __max_value; ++i) {
          const unsigned int str_index = __strings.first(i);
          const std::string &str = DBCell::getString(str_index);
          if (__user_values->exists(str)) {
            user_vals.push_back(std::pair<unsigned int, unsigned int>(
                str_index, __user_values->pos(str)));
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

        // the user values are not needed any more, so, remove them
        delete __user_values;
        __user_values = nullptr;
      }
    }

    /// specify the set of possible values (to do before creating the row filter)
    void CellTranslatorString::setUserValues(const Sequence<std::string> &values,
                                             bool check_database) {
      // clear all current data
      __strings.clear();
      __max_value = 0;
      if (__user_values != nullptr) {
        delete __user_values;
        __user_values = nullptr;
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
          __user_values = new Sequence<std::string>(values);
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */
