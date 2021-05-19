/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief The implementation of DBCells
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <cstdio>
#include <agrum/tools/database/DBCell.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBCell_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // create the static members
    int DBCell::string_max_index__ = 0;


    Bijection< std::string, int >& DBCell::strings__() {
#  ifdef GUM_DEBUG_MODE
      static bool first_time = true;
      if (first_time) {
        first_time = false;
        __debug__::dec_creation__("Bijection",
                                  "__strings",
                                  0,
                                  "BCell string bijection",
                                  0);
        __debug__::dec_creation__("BijectionImplementation",
                                  "__strings",
                                  0,
                                  "BCell string bijection",
                                  0);
        __debug__::dec_creation__("HashTable",
                                  "__strings",
                                  0,
                                  "BCell string bijection",
                                  0);
        __debug__::dec_creation__("HashTable",
                                  "__strings",
                                  0,
                                  "BCell string bijection",
                                  0);
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
        } else if ((*start_iter < '0') || (*start_iter > '9'))
          return false;
        else
          has_digit = true;
      }

      return true;
    }


    // try to convert the content of the DBCell into another type
    bool DBCell::convertType(const EltType new_type) {
      if (new_type == type__) return true;
      switch (new_type) {
          // ===================================
        case EltType::REAL:
          switch (type__) {
            case EltType::INTEGER:
              val_real__ = float(val_integer__);
              type__     = EltType::REAL;
              return true;

            case EltType::STRING:
              try {
                const std::string& str = strings__().first(val_index__);
                if (!isReal(str)) return false;
                val_real__ = std::stof(str);
                type__     = EltType::REAL;
                return true;
              } catch (std::invalid_argument&) { return false; }

            case EltType::MISSING:
              return false;

            default:
              GUM_ERROR(NotImplementedYet,
                        "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::INTEGER:
          switch (type__) {
            case EltType::REAL: {
              const int nb = int(val_real__);
              if (nb == val_real__) {
                val_integer__ = nb;
                type__        = EltType::INTEGER;
                return true;
              } else
                return false;
            }

            case EltType::STRING:
              try {
                const std::string& str = strings__().first(val_index__);
                if (!isInteger(str)) return false;
                val_integer__ = std::stoi(str);
                type__        = EltType::INTEGER;
                return true;
              } catch (std::invalid_argument&) { return false; }

            case EltType::MISSING:
              return false;

            default:
              GUM_ERROR(NotImplementedYet,
                        "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::STRING:
          switch (type__) {
            case EltType::REAL: {
              char buffer[100];
              sprintf(buffer, "%g", val_real__);
              const std::string str(buffer);
              if (!strings__().existsFirst(str)) {
                strings__().insert(str, string_max_index__);
                val_index__ = string_max_index__;
                ++string_max_index__;
              } else {
                val_index__ = strings__().second(str);
              }
            }
              type__ = EltType::STRING;
              return true;

            case EltType::INTEGER: {
              const std::string str = std::to_string(val_integer__);
              if (!strings__().existsFirst(str)) {
                strings__().insert(str, string_max_index__);
                val_index__ = string_max_index__;
                ++string_max_index__;
              } else {
                val_index__ = strings__().second(str);
              }
            }
              type__ = EltType::STRING;
              return true;

            case EltType::MISSING:
              return false;

            default:
              GUM_ERROR(NotImplementedYet,
                        "type not supported by DBCell convertType")
          }

          // ===================================
        case EltType::MISSING:
          type__ = EltType::MISSING;
          return true;

        default:
          GUM_ERROR(NotImplementedYet, "type not supported by DBCell convertType")
      }

      return false;
    }


    // raises an appropriate exception when encountering a type error
    std::string DBCell::typeErrorMsg__(const std::string& true_type) const {
      std::stringstream str;
      switch (type__) {
        case EltType::REAL:
          str << "The DBCell contains a real number instead of " << true_type;
          break;

        case EltType::INTEGER:
          str << "The DBCell contains an integer instead of " << true_type;
          break;

        case EltType::STRING:
          str << "The DBCell contains a string instead of " << true_type;
          break;

        case EltType::MISSING:
          str << "The DBCell contains a missing value instead of " << true_type;
          break;

        default:
          GUM_ERROR(NotImplementedYet, "DBCell type not implemented yet")
      }

      return str.str();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
