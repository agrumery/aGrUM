/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/DBCell.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBCell::DBCell() { GUM_CONSTRUCTOR(DBCell); }


    /// constructor for a real number
    INLINE DBCell::DBCell(const float nb) :
        type__(DBCell::EltType::REAL), val_real__(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for an integer number
    INLINE DBCell::DBCell(const int nb) :
        type__(DBCell::EltType::INTEGER), val_integer__(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for a string
    INLINE DBCell::DBCell(const std::string& str) :
        type__(DBCell::EltType::STRING) {
      // store the string into the static list of strings
      if (!strings__().existsFirst(str)) {
        strings__().insert(str, string_max_index__);
        val_index__ = string_max_index__;
        ++string_max_index__;
      } else {
        val_index__ = strings__().second(str);
      }

      GUM_CONSTRUCTOR(DBCell);
    }


    /// copy constructor
    INLINE DBCell::DBCell(const DBCell& from) : type__(from.type__) {
      std::memcpy(&val_index__, &(from.val_index__), sizeof(UnionType));

      // for debugging
      GUM_CONS_CPY(DBCell);
    }


    /// move constructor
    INLINE DBCell::DBCell(DBCell&& from) : type__(from.type__) {
      std::memcpy(&val_index__, &(from.val_index__), sizeof(UnionType));

      // for debugging
      GUM_CONS_MOV(DBCell);
    }


    /// destructor
    INLINE DBCell::~DBCell() { GUM_DESTRUCTOR(DBCell); }


    /// copy operator
    INLINE DBCell& DBCell::operator=(const DBCell& from) {
      if (this != &from) {
        type__ = from.type__;
        std::memcpy(&val_index__, &(from.val_index__), sizeof(UnionType));
      }

      return *this;
    }


    /// move operator
    INLINE DBCell& DBCell::operator=(DBCell&& from) {
      if (this != &from) {
        type__ = from.type__;
        std::memcpy(&val_index__, &(from.val_index__), sizeof(UnionType));
      }

      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const float x) {
      type__     = EltType::REAL;
      val_real__ = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const int x) {
      type__        = EltType::INTEGER;
      val_integer__ = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const std::string& str) {
      if (!strings__().existsFirst(str)) {
        strings__().insert(str, string_max_index__);
        val_index__ = string_max_index__;
        ++string_max_index__;
      } else {
        val_index__ = strings__().second(str);
      }
      type__ = EltType::STRING;

      return *this;
    }


    /// test of equality
    INLINE bool DBCell::operator==(const DBCell& from) const {
      return (type__ == from.type__)
          && ((type__ == EltType::MISSING)
              || ((type__ == EltType::REAL) && (val_real__ == from.val_real__))
              || (val_integer__ == from.val_integer__));
    }


    /// test of inequality
    INLINE bool DBCell::operator!=(const DBCell& from) const {
      return !operator==(from);
    }


    /// returns the current type of the DBCell
    INLINE DBCell::EltType DBCell::type() const noexcept { return type__; }


    /// returns the DBcell as a float
    INLINE float DBCell::real() const {
      if (type__ == EltType::REAL)
        return val_real__;
      else
        GUM_ERROR(TypeError, typeErrorMsg__("a real number"))
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setReal(const float x) {
      type__     = EltType::REAL;
      val_real__ = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setReal(const std::string& elt) {
      if (!isReal(elt))
        GUM_ERROR(TypeError, "the string does not contain a real number")
      val_real__ = std::stof(elt);
      type__     = EltType::REAL;
    }


    /// returns the DBcell as an integer
    INLINE int DBCell::integer() const {
      if (type__ == EltType::INTEGER)
        return val_integer__;
      else
        GUM_ERROR(TypeError, typeErrorMsg__("an integer"))
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setInteger(const int x) {
      type__        = EltType::INTEGER;
      val_integer__ = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setInteger(const std::string& elt) {
      if (!isInteger(elt))
        GUM_ERROR(TypeError, "the string does not contain an integer")
      val_integer__ = std::stoi(elt);
      type__        = EltType::INTEGER;
    }


    /// returns the DBcell as a string
    INLINE const std::string& DBCell::string() const {
      if (type__ == EltType::STRING)
        return strings__().first(val_index__);
      else
        GUM_ERROR(TypeError, typeErrorMsg__("a string"))
    }


    /// returns the DBcell as a string index (if it contains a string)
    INLINE int DBCell::stringIndex() const {
      if (type__ == EltType::STRING)
        return val_index__;
      else
        GUM_ERROR(TypeError, typeErrorMsg__("a string"))
    }


    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::string(const int index) {
      return strings__().first(index);
    }


    /// set the content of the DBCell from a string
    INLINE void DBCell::setString(const std::string& str) {
      if (!strings__().existsFirst(str)) {
        strings__().insert(str, string_max_index__);
        val_index__ = string_max_index__;
        ++string_max_index__;
      } else {
        val_index__ = strings__().second(str);
      }
      type__ = EltType::STRING;
    }


    /// sets the DBCell as a missing element
    INLINE void DBCell::setMissingState() { type__ = EltType::MISSING; }


    /// indicates whether the cell contains a missing value
    INLINE bool DBCell::isMissing() const { return type__ == EltType::MISSING; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
