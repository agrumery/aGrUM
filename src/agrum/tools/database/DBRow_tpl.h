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
 * @brief The class representing a record stored in a tabular database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <algorithm>
#  include <agrum/tools/database/DBRow.h>

namespace gum {

  namespace learning {

    /// default constructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow() {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// constructor with a given size for the row
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(const std::size_t size,
                                  const T_DATA      default_cell,
                                  const double      weight) :
        row_(size, default_cell),
        weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// constructor with a given size for the row
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(const std::size_t size, const double weight) :
        DBRow(size, T_DATA(), weight) {}

    /// initializer list constructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(std::initializer_list< T_DATA > list, const double weight) :
        row_(list), weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// sets a new row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::setRow(const std::vector< T_DATA >& new_row) {
      const std::size_t size = new_row.size();
      if (size) {
        row_.resize(size);
        std::copy(new_row.begin(), new_row.end(), row_.begin());
      } else {
        row_.clear();
      }
    }


    /// sets a new row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::setRow(std::vector< T_DATA >&& new_row) {
      row_ = std::move(new_row);
    }


    /// initializer from a vector of cells
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(const std::vector< T_DATA >& cells, const double weight) :
        weight_(weight) {
      setRow(cells);
      GUM_CONSTRUCTOR(DBRow);
    }


    /// initializer from a vector of cells
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(std::vector< T_DATA >&& cells, const double weight) :
        row_(std::move(cells)), weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// copy constructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(const DBRow< T_DATA >& from) :
        row_(from.row_), weight_(from.weight_) {
      GUM_CONS_CPY(DBRow);
    }


    /// move constructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::DBRow(DBRow< T_DATA >&& from) :
        row_(std::move(from.row_)), weight_(from.weight_) {
      GUM_CONS_MOV(DBRow);
    }


    /// virtual copy constructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >* DBRow< T_DATA >::clone() const {
      return new DBRow< T_DATA >(*this);
    }


    /// destructor
    template < typename T_DATA >
    INLINE DBRow< T_DATA >::~DBRow() {
      GUM_DESTRUCTOR(DBRow);
    }


    /// copy operator
    template < typename T_DATA >
    INLINE DBRow< T_DATA >& DBRow< T_DATA >::operator=(const DBRow< T_DATA >& from) {
      if (this != &from) {
        row_    = from.row_;
        weight_ = from.weight_;
      }
      return *this;
    }


    /// move operator
    template < typename T_DATA >
    INLINE DBRow< T_DATA >& DBRow< T_DATA >::operator=(DBRow< T_DATA >&& from) {
      if (this != &from) {
        row_    = std::move(from.row_);
        weight_ = from.weight_;
      }
      return *this;
    }

    /// returns the ith T_DATA of the row
    template < typename T_DATA >
    INLINE T_DATA& DBRow< T_DATA >::operator[](const std::size_t i) {
      return row_[i];
    }

    /// returns the ith T_DATA of the row
    template < typename T_DATA >
    INLINE const T_DATA& DBRow< T_DATA >::operator[](const std::size_t i) const {
      return row_[i];
    }

    /// returns the current row
    template < typename T_DATA >
    INLINE const std::vector< T_DATA >& DBRow< T_DATA >::row() const noexcept {
      return row_;
    }

    /// returns the current row
    template < typename T_DATA >
    INLINE std::vector< T_DATA >& DBRow< T_DATA >::row() noexcept {
      return row_;
    }

    /// returns the weight
    template < typename T_DATA >
    INLINE const double& DBRow< T_DATA >::weight() const noexcept {
      return weight_;
    }

    /// returns the weight
    template < typename T_DATA >
    INLINE double& DBRow< T_DATA >::weight() noexcept {
      return weight_;
    }

    /// sets a new weight
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::setWeight(const double new_weight) {
      weight_ = new_weight;
    }

    /// returns the size of the row
    template < typename T_DATA >
    INLINE std::size_t DBRow< T_DATA >::size() const noexcept {
      return row_.size();
    }

    /// resize a given row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::resize(const std::size_t new_size) {
      row_.resize(new_size);
    }

    /// reserve a size for the elements of a given row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::reserve(const std::size_t new_size) {
      row_.reserve(new_size);
    }

    /// adds a new element at the end of the row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::pushBack(const T_DATA& elt) {
      row_.push_back(elt);
    }

    /// adds a new element at the end of the row
    template < typename T_DATA >
    INLINE void DBRow< T_DATA >::pushBack(T_DATA&& elt) {
      row_.push_back(std::move(elt));
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
