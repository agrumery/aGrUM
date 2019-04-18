
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/database/DBCell.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBCell::DBCell() { GUM_CONSTRUCTOR(DBCell); }


    /// constructor for a real number
    INLINE DBCell::DBCell(const float nb) :
        __type(DBCell::EltType::REAL), __val_real(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for an integer number
    INLINE DBCell::DBCell(const int nb) :
        __type(DBCell::EltType::INTEGER), __val_integer(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for a string
    INLINE DBCell::DBCell(const std::string& str) :
        __type(DBCell::EltType::STRING) {
      // store the string into the static list of strings
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __val_index = __string_max_index;
        ++__string_max_index;
      } else {
        __val_index = __strings().second(str);
      }

      GUM_CONSTRUCTOR(DBCell);
    }


    /// copy constructor
    INLINE DBCell::DBCell(const DBCell& from) : __type(from.__type) {
      std::memcpy(&__val_index, &(from.__val_index), sizeof(UnionType));

      // for debugging
      GUM_CONS_CPY(DBCell);
    }


    /// move constructor
    INLINE DBCell::DBCell(DBCell&& from) : __type(from.__type) {
      std::memcpy(&__val_index, &(from.__val_index), sizeof(UnionType));

      // for debugging
      GUM_CONS_MOV(DBCell);
    }


    /// destructor
    INLINE DBCell::~DBCell() { GUM_DESTRUCTOR(DBCell); }


    /// copy operator
    INLINE DBCell& DBCell::operator=(const DBCell& from) {
      if (this != &from) {
        __type = from.__type;
        std::memcpy(&__val_index, &(from.__val_index), sizeof(UnionType));
      }

      return *this;
    }


    /// move operator
    INLINE DBCell& DBCell::operator=(DBCell&& from) {
      if (this != &from) {
        __type = from.__type;
        std::memcpy(&__val_index, &(from.__val_index), sizeof(UnionType));
      }

      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const float x) {
      __type = EltType::REAL;
      __val_real = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const int x) {
      __type = EltType::INTEGER;
      __val_integer = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const std::string& str) {
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __val_index = __string_max_index;
        ++__string_max_index;
      } else {
        __val_index = __strings().second(str);
      }
      __type = EltType::STRING;

      return *this;
    }


    /// test of equality
    INLINE bool DBCell::operator==(const DBCell& from) const {
      return (__type == from.__type)
             && ((__type == EltType::MISSING)
                 || ((__type == EltType::REAL) && (__val_real == from.__val_real))
                 || (__val_integer == from.__val_integer));
    }


    /// test of inequality
    INLINE bool DBCell::operator!=(const DBCell& from) const {
      return !operator==(from);
    }


    /// returns the current type of the DBCell
    INLINE DBCell::EltType DBCell::type() const noexcept { return __type; }


    /// returns the DBcell as a float
    INLINE float DBCell::real() const {
      if (__type == EltType::REAL)
        return __val_real;
      else
        GUM_ERROR(TypeError, __typeErrorMsg("a real number"));
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setReal(const float x) {
      __type = EltType::REAL;
      __val_real = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setReal(const std::string& elt) {
      if (!isReal(elt))
        GUM_ERROR(TypeError, "the string does not contain a real number");
      __val_real = std::stof(elt);
      __type = EltType::REAL;
    }


    /// returns the DBcell as an integer
    INLINE int DBCell::integer() const {
      if (__type == EltType::INTEGER)
        return __val_integer;
      else
        GUM_ERROR(TypeError, __typeErrorMsg("an integer"));
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setInteger(const int x) {
      __type = EltType::INTEGER;
      __val_integer = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setInteger(const std::string& elt) {
      if (!isInteger(elt))
        GUM_ERROR(TypeError, "the string does not contain an integer");
      __val_integer = std::stoi(elt);
      __type = EltType::INTEGER;
    }


    /// returns the DBcell as a string
    INLINE const std::string& DBCell::string() const {
      if (__type == EltType::STRING)
        return __strings().first(__val_index);
      else
        GUM_ERROR(TypeError, __typeErrorMsg("a string"));
    }


    /// returns the DBcell as a string index (if it contains a string)
    INLINE int DBCell::stringIndex() const {
      if (__type == EltType::STRING)
        return __val_index;
      else
        GUM_ERROR(TypeError, __typeErrorMsg("a string"));
    }


    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::string(const int index) {
      return __strings().first(index);
    }


    /// set the content of the DBCell from a string
    INLINE void DBCell::setString(const std::string& str) {
      if (!__strings().existsFirst(str)) {
        __strings().insert(str, __string_max_index);
        __val_index = __string_max_index;
        ++__string_max_index;
      } else {
        __val_index = __strings().second(str);
      }
      __type = EltType::STRING;
    }


    /// sets the DBCell as a missing element
    INLINE void DBCell::setMissingState() { __type = EltType::MISSING; }


    /// indicates whether the cell contains a missing value
    INLINE bool DBCell::isMissing() const { return __type == EltType::MISSING; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
