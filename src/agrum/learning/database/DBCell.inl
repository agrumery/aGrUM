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
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBCell::DBCell() { GUM_CONSTRUCTOR(DBCell); }

    /// constructor for a number
    INLINE DBCell::DBCell(float nb) : __float(nb) { GUM_CONSTRUCTOR(DBCell); }

    /// constructor for a string
    INLINE DBCell::DBCell(const std::string &str) : __type(DBCell::EltType::STRING) {
      // store the string into the static list of strings
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __int = __string_max_index;
        ++__string_max_index;
      } else {
        __int = __strings().second(str);
      }

      GUM_CONSTRUCTOR(DBCell);
    }

    /// copy constructor
    INLINE DBCell::DBCell(const DBCell &from) : __type(from.__type) {
      std::memcpy(&__float, &(from.__float), sizeof(Float));

      // for debugging
      GUM_CONS_CPY(DBCell);
    }

    /// move constructor
    INLINE DBCell::DBCell(DBCell &&from) : __type(from.__type) {
      std::memcpy(&__float, &(from.__float), sizeof(Float));

      // for debugging
      GUM_CONS_MOV(DBCell);
    }

    /// destructor
    INLINE DBCell::~DBCell() { GUM_DESTRUCTOR(DBCell); }

    /// copy operator
    INLINE DBCell &DBCell::operator=(const DBCell &from) {
      if (this != &from) {
        __type = from.__type;
        std::memcpy(&__float, &(from.__float), sizeof(Float));
      }

      return *this;
    }

    /// move operator
    INLINE DBCell &DBCell::operator=(DBCell &&from) {
      if (this != &from) {
        __type = from.__type;
        std::memcpy(&__float, &(from.__float), sizeof(Float));
      }

      return *this;
    }

    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell &DBCell::operator=(float x) noexcept {
      __type = EltType::FLOAT;
      __float = x;
      return *this;
    }

    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell &DBCell::operator=(const std::string &str) noexcept {
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __int = __string_max_index;
        ++__string_max_index;
      } else {
        __int = __strings().second(str);
      }

      return *this;
    }

    /// returns the DBcell as a float (without checking its type)
    INLINE float DBCell::getFloat() const noexcept { return __float; }

    /// returns the DBcell as a float (safe with type checking)
    INLINE float DBCell::getFloatSafe() const {
      if (__type == EltType::FLOAT)
        return __float;
      else
        GUM_ERROR(TypeError, "the DBCell does not contain a float");
    }

    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setFloat(float x) { __float = x; }

    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setFloatSafe(float elt) {
      __type = EltType::FLOAT;
      __float = elt;
    }

    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setFloatFromStringSafe(const std::string &elt) {
      __float = stof(elt);
      __type = EltType::FLOAT;
    }

    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string &DBCell::getString() const noexcept {
      return __strings().first(__int);
    }

    /// returns the DBcell as a string (safe with type checking)
    INLINE const std::string &DBCell::getStringSafe() const {
      if (__type == EltType::STRING)
        return __strings().first(__int);
      else
        GUM_ERROR(TypeError, "the DBCell does not contain a string");
    }

    /// returns the DBcell as a string index (without checking its type)
    INLINE int DBCell::getStringIndex() const noexcept { return __int; }

    /// returns the DBcell as a string (safe with type checking)
    INLINE int DBCell::getStringIndexSafe() const {
      if (__type == EltType::STRING)
        return __int;
      else
        GUM_ERROR(TypeError, "the DBCell does not contain a string");
    }

    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string &DBCell::getString(unsigned int index) {
      return __strings().first(index);
    }

    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setString(const std::string &str) {
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __int = __string_max_index;
        ++__string_max_index;
      } else {
        __int = __strings().second(str);
      }
    }

    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setStringSafe(const std::string &str) {
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __int = __string_max_index;
        ++__string_max_index;
      } else {
        __int = __strings().second(str);
      }
      __type = EltType::STRING;
    }

    /// sets the DBCell as a missing element
    INLINE void DBCell::setMissingSafe() { __type = EltType::MISSING; }

    /// safely sets the content of the DBCell with the best possible type
    INLINE void DBCell::setBestTypeSafe(const std::string &elt) {
      // try to convert the string into a float
      try {
        __setFloatFromStringSafe(elt);
        return;
      } catch (std::invalid_argument &) {
      }

      // here, the best type is a string
      setStringSafe(elt);
    }

    /// returns the current type of the DBCell
    INLINE DBCell::EltType DBCell::type() const noexcept { return __type; }

    /// try to convert the content of the DBCell into another type
    INLINE bool DBCell::convertType(EltType new_type) {
      if (new_type == __type)
        return true;
      switch (new_type) {
        // ===================================
        case FLOAT:
          switch (__type) {
            case STRING:
              try {
                __float = stof(__strings().first(__int));
                __type = EltType::FLOAT;
                return true;
              } catch (std::invalid_argument &) {
                return false;
              }

            case MISSING:
              __type = EltType::MISSING;
              return true;

            default:
              GUM_ERROR(TypeError, "type not supported by DBCell convertType");
          }

        // ===================================
        case STRING:
          switch (__type) {
            case FLOAT: {
              std::string str = std::to_string(__float);
              if (!__strings().existsFirst(str)) {
                __strings().insert(str, __string_max_index);
                __int = __string_max_index;
                ++__string_max_index;
              } else {
                __int = __strings().second(str);
              }
              __type = EltType::STRING;
            }
              return true;

            case MISSING:
              __type = EltType::MISSING;
              return true;

            default:
              GUM_ERROR(TypeError, "type not supported by DBCell convertType");
          }

        // ===================================
        case MISSING:
          setMissingSafe();
          return true;

        default:
          GUM_ERROR(TypeError, "type not supported by DBCell convertType");
      }

      return false;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
