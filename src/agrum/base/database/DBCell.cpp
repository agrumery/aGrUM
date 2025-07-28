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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief The implementation of DBCells
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <cstdio>

#include <agrum/base/database/DBCell.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBCell_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // create the static members
    int DBCell::_string_max_index_ = 0;

    Bijection< std::string, int >& DBCell::_strings_() {
#  ifdef GUM_DEBUG_MODE
      static bool first_time = true;
      if (first_time) {
        first_time = false;
        __debug__::_dec_creation_("Bijection", " __strings", 0, "BCell string bijection", 0);
        __debug__::_dec_creation_("BijectionImplementation",
                                  " __strings",
                                  0,
                                  "BCell string bijection",
                                  0);
        __debug__::_dec_creation_("HashTable", " __strings", 0, "BCell string bijection", 0);
        __debug__::_dec_creation_("HashTable", " __strings", 0, "BCell string bijection", 0);
      }
#  endif
      static Bijection< std::string, int > strings;
      return strings;
    }

    // determines whether a string corresponds to an integer
    bool DBCell::isInteger(const std::string& str) {
      if (str.empty()) return false;

      // trim the string
      auto start_iter = str.begin() + str.find_first_not_of(" \t");
      auto end_iter   = str.begin() + str.find_last_not_of(" \t\r\n") + 1;

      if (start_iter == end_iter) return false;

      // if the number is negative, pass the '-' sign
      if (*start_iter == '-') ++start_iter;

      // check wether we have a number
      for (; start_iter != end_iter; ++start_iter) {
        if ((*start_iter < '0') || (*start_iter > '9')) return false;
      }

      return true;
    }

    // determines whether a string corresponds to an integer
    bool DBCell::isReal(const std::string& str) {
      if (str.empty()) return false;

      // trim the string
      auto start_iter = str.begin() + str.find_first_not_of(" \t");
      auto end_iter   = str.begin() + str.find_last_not_of(" \t\r\n") + 1;

      if (start_iter == end_iter) return false;

      // check wether we have a number
      bool has_dot      = false;
      bool has_exponent = false;
      bool has_digit    = false;
      bool has_negation = false;
      for (; start_iter != end_iter; ++start_iter) {
        if (*start_iter == '-') {
          if (has_negation) return false;
        } else if (*start_iter == '.') {
          if (has_dot || has_exponent) return false;
          has_dot = true;
        } else if ((*start_iter == 'e') || (*start_iter == 'E')) {
          if (has_exponent || !has_digit) return false;
          has_exponent = true;
          has_negation = false;
        } else if ((*start_iter < '0') || (*start_iter > '9')) return false;
        else has_digit = true;
      }

      return true;
    }

    // try to convert the content of the DBCell into another type
    bool DBCell::convertType(const EltType new_type) {
      if (new_type == _type_) return true;
      switch (new_type) {
          // ===================================
        case EltType::REAL :
          switch (_type_) {
            case EltType::INTEGER :
              _val_real_ = float(_val_integer_);
              _type_     = EltType::REAL;
              return true;

            case EltType::STRING :
              try {
                const std::string& str = _strings_().first(_val_index_);
                if (!isReal(str)) return false;
                _val_real_ = std::stof(str);
                _type_     = EltType::REAL;
                return true;
              } catch (std::invalid_argument&) { return false; }

            case EltType::MISSING : return false;

            default : GUM_ERROR(NotImplementedYet, "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::INTEGER :
          switch (_type_) {
            case EltType::REAL : {
              const int nb = int(_val_real_);
              if (nb == _val_real_) {
                _val_integer_ = nb;
                _type_        = EltType::INTEGER;
                return true;
              } else return false;
            }

            case EltType::STRING :
              try {
                const std::string& str = _strings_().first(_val_index_);
                if (!isInteger(str)) return false;
                _val_integer_ = std::stoi(str);
                _type_        = EltType::INTEGER;
                return true;
              } catch (std::invalid_argument&) { return false; }

            case EltType::MISSING : return false;

            default : GUM_ERROR(NotImplementedYet, "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::STRING :
          switch (_type_) {
            case EltType::REAL : {
              char buffer[100];
              snprintf(buffer, 100, "%g", _val_real_);
              const std::string str(buffer);
              if (!_strings_().existsFirst(str)) {
                _strings_().insert(str, _string_max_index_);
                _val_index_ = _string_max_index_;
                ++_string_max_index_;
              } else {
                _val_index_ = _strings_().second(str);
              }
            }
              _type_ = EltType::STRING;
              return true;

            case EltType::INTEGER : {
              const std::string str = std::to_string(_val_integer_);
              if (!_strings_().existsFirst(str)) {
                _strings_().insert(str, _string_max_index_);
                _val_index_ = _string_max_index_;
                ++_string_max_index_;
              } else {
                _val_index_ = _strings_().second(str);
              }
            }
              _type_ = EltType::STRING;
              return true;

            case EltType::MISSING : return false;

            default : GUM_ERROR(NotImplementedYet, "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::MISSING : _type_ = EltType::MISSING; return true;

        default : GUM_ERROR(NotImplementedYet, "type not supported by DBCell convertType")
      }

      return false;
    }

    // raises an appropriate exception when encountering a type error
    std::string DBCell::_typeErrorMsg_(const std::string& true_type) const {
      std::stringstream str;
      switch (_type_) {
        case EltType::REAL :
          str << "The DBCell contains a real number instead of " << true_type;
          break;

        case EltType::INTEGER :
          str << "The DBCell contains an integer instead of " << true_type;
          break;

        case EltType::STRING :
          str << "The DBCell contains a string instead of " << true_type;
          break;

        case EltType::MISSING :
          str << "The DBCell contains a missing value instead of " << true_type;
          break;

        default : GUM_ERROR(NotImplementedYet, "DBCell type not implemented yet")
      }

      return str.str();
    }

    // returns the content of the DBCell as a string, whatever its type
    std::string DBCell::toString(const std::vector< std::string >& missingVals) const {
      switch (_type_) {
        case EltType::STRING : return _strings_().first(_val_index_);

        case EltType::REAL : {
          char buffer[100];
          snprintf(buffer, 100, "%g", _val_real_);
          return std::string(buffer);
        }

        case EltType::INTEGER : return std::to_string(_val_integer_);

        case EltType::MISSING :
          if (missingVals.size()) return missingVals[0];
          else GUM_ERROR(UndefinedElement, "no missing value symbol found")
      }

      GUM_ERROR(NotImplementedYet, "type not supported by DBCell toString")
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
