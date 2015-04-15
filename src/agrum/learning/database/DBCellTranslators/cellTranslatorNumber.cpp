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

#include <agrum/learning/database/DBCellTranslators/cellTranslatorNumber.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorNumber.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    CellTranslatorNumber::CellTranslatorNumber(Sequence<float> values,
                                               bool check_database)
        : __check_database(check_database) {

      if (!check_database) {
        if (values.empty()) {
          __check_database = true;
        } else {
          // if we do not want to parse the database, store all the values directly
          for (const auto &val : values) {
            __values.insert(val, __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __user_values = new Sequence<float>(std::move(values));
        }
      }
    }

    /// perform a post initialization after the database parsing
    void CellTranslatorNumber::postInitialize() {
      if (__user_values != nullptr) {
        // we probably need to reorder the sequence to be compatible with the
        // order of the values specified by the user
        std::vector<float> no_user_vals;
        std::vector<std::pair<float, unsigned int>> user_vals;
        for (unsigned int i = 0; i < __max_value; ++i) {
          const float val = __values.first(i);
          if (__user_values->exists(val)) {
            user_vals.push_back(
                std::pair<float, unsigned int>(val, __user_values->pos(val)));
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
        __values.clear();
        __max_value = 0;
        for (const auto &val : user_vals) {
          __values.insert(val.first, __max_value);
          ++__max_value;
        }
        for (const auto val : no_user_vals) {
          __values.insert(val, __max_value);
          ++__max_value;
        }

        // the user values are not needed any more, so, remove them
        delete __user_values;
        __user_values = nullptr;
      }
    }

    /// specify the set of possible values (to do before creating the row filter)
    void CellTranslatorNumber::setUserValues(const Sequence<float> &values,
                                             bool check_database) {
      // clear all current data
      __values.clear();
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
          for (const auto &val : values) {
            __values.insert(val, __max_value);
            ++__max_value;
          }
        }
      } else {
        // if we specified values, store them
        if (!values.empty()) {
          __user_values = new Sequence<float>(values);
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */
