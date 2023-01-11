/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/DBCell.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBCell::DBCell() : _val_index_(0) {
      GUM_CONSTRUCTOR(DBCell);
      ;
    }


    /// constructor for a real number
    INLINE DBCell::DBCell(const float nb) : _type_(DBCell::EltType::REAL), _val_real_(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for an integer number
    INLINE DBCell::DBCell(const int nb) : _type_(DBCell::EltType::INTEGER), _val_integer_(nb) {
      GUM_CONSTRUCTOR(DBCell);
    }


    /// constructor for a string
    INLINE DBCell::DBCell(const std::string& str) : _type_(DBCell::EltType::STRING) {
      // store the string into the static list of strings
      if (!_strings_().existsFirst(str)) {
        _strings_().insert(str, _string_max_index_);
        _val_index_ = _string_max_index_;
        ++_string_max_index_;
      } else {
        _val_index_ = _strings_().second(str);
      }

      GUM_CONSTRUCTOR(DBCell);
    }


    /// copy constructor
    INLINE DBCell::DBCell(const DBCell& from) : _type_(from._type_) {
      std::memcpy(&_val_index_, &(from._val_index_), sizeof(UnionType));

      // for debugging
      GUM_CONS_CPY(DBCell);
    }


    /// move constructor
    INLINE DBCell::DBCell(DBCell&& from) : _type_(from._type_) {
      std::memcpy(&_val_index_, &(from._val_index_), sizeof(UnionType));

      // for debugging
      GUM_CONS_MOV(DBCell);
    }


    /// destructor
    INLINE DBCell::~DBCell() {
      GUM_DESTRUCTOR(DBCell);
      ;
    }


    /// copy operator
    INLINE DBCell& DBCell::operator=(const DBCell& from) {
      if (this != &from) {
        _type_ = from._type_;
        std::memcpy(&_val_index_, &(from._val_index_), sizeof(UnionType));
      }

      return *this;
    }


    /// move operator
    INLINE DBCell& DBCell::operator=(DBCell&& from) {
      if (this != &from) {
        _type_ = from._type_;
        std::memcpy(&_val_index_, &(from._val_index_), sizeof(UnionType));
      }

      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const float x) {
      _type_     = EltType::REAL;
      _val_real_ = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const int x) {
      _type_        = EltType::INTEGER;
      _val_integer_ = x;
      return *this;
    }


    /// assignment operator
    INLINE DBCell& DBCell::operator=(const std::string& str) {
      if (!_strings_().existsFirst(str)) {
        _strings_().insert(str, _string_max_index_);
        _val_index_ = _string_max_index_;
        ++_string_max_index_;
      } else {
        _val_index_ = _strings_().second(str);
      }
      _type_ = EltType::STRING;

      return *this;
    }


    /// test of equality
    INLINE bool DBCell::operator==(const DBCell& from) const {
      return (_type_ == from._type_)
          && ((_type_ == EltType::MISSING)
              || ((_type_ == EltType::REAL) && (_val_real_ == from._val_real_))
              || (_val_integer_ == from._val_integer_));
    }


    /// test of inequality
    INLINE bool DBCell::operator!=(const DBCell& from) const { return !operator==(from); }


    /// returns the current type of the DBCell
    INLINE DBCell::EltType DBCell::type() const noexcept { return _type_; }


    /// returns the DBcell as a float
    INLINE float DBCell::real() const {
      if (_type_ == EltType::REAL) return _val_real_;
      else GUM_ERROR(TypeError, _typeErrorMsg_("a real number"))
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setReal(const float x) {
      _type_     = EltType::REAL;
      _val_real_ = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setReal(const std::string& elt) {
      if (!isReal(elt)) GUM_ERROR(TypeError, "the string does not contain a real number")
      _val_real_ = std::stof(elt);
      _type_     = EltType::REAL;
    }


    /// returns the DBcell as an integer
    INLINE int DBCell::integer() const {
      if (_type_ == EltType::INTEGER) return _val_integer_;
      else GUM_ERROR(TypeError, _typeErrorMsg_("an integer"))
    }


    /// sets the content of the DBCell
    INLINE void DBCell::setInteger(const int x) {
      _type_        = EltType::INTEGER;
      _val_integer_ = x;
    }


    /// sets the content of the DBCell from a string
    INLINE void DBCell::setInteger(const std::string& elt) {
      if (!isInteger(elt)) GUM_ERROR(TypeError, "the string does not contain an integer")
      _val_integer_ = std::stoi(elt);
      _type_        = EltType::INTEGER;
    }


    /// returns the DBcell as a string
    INLINE const std::string& DBCell::string() const {
      if (_type_ == EltType::STRING) return _strings_().first(_val_index_);
      else GUM_ERROR(TypeError, _typeErrorMsg_("a string"))
    }


    /// returns the DBcell as a string index (if it contains a string)
    INLINE int DBCell::stringIndex() const {
      if (_type_ == EltType::STRING) return _val_index_;
      else GUM_ERROR(TypeError, _typeErrorMsg_("a string"))
    }


    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::string(const int index) { return _strings_().first(index); }


    /// set the content of the DBCell from a string
    INLINE void DBCell::setString(const std::string& str) {
      if (!_strings_().existsFirst(str)) {
        _strings_().insert(str, _string_max_index_);
        _val_index_ = _string_max_index_;
        ++_string_max_index_;
      } else {
        _val_index_ = _strings_().second(str);
      }
      _type_ = EltType::STRING;
    }


    /// sets the DBCell as a missing element
    INLINE void DBCell::setMissingState() { _type_ = EltType::MISSING; }


    /// indicates whether the cell contains a missing value
    INLINE bool DBCell::isMissing() const { return _type_ == EltType::MISSING; }

    // checks whether a string correspond to a missing value
    INLINE bool DBCell::isMissing(const std::string&                str,
                                  const std::vector< std::string >& missingVals) {
      for (auto missing: missingVals) {
        if (str == missing) return true;
      }
      return false;
    }

    // returns the best type to store a given element encoded as a string
    INLINE DBCell::EltType DBCell::bestType(const std::string&                str,
                                            const std::vector< std::string >& missingVals) {
      if (isMissing(str, missingVals)) return EltType::MISSING;
      if (isInteger(str)) return EltType::INTEGER;
      if (isReal(str)) return EltType::REAL;
      return EltType::STRING;
    }

    // returns the DBCell with the best type for an element encoded as a string
    INLINE DBCell DBCell::bestDBCell(const std::string&                str,
                                     const std::vector< std::string >& missingVals) {
      if (isMissing(str, missingVals)) return DBCell();
      if (isInteger(str)) return DBCell(std::stoi(str));
      if (isReal(str)) return DBCell(std::stof(str));

      return DBCell(str);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
