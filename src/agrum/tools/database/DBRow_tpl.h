
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <algorithm>
#  include <agrum/tools/database/DBRow.h>

namespace gum {

  namespace learning {

    /// returns the allocator used by the DBRow
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE ALLOC< T_DATA > DBRow< T_DATA, ALLOC >::getAllocator() const {
      return row_.get_allocator();
    }


    /// default constructor with specific allocator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(const ALLOC< T_DATA >& alloc) :
        row_(alloc) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// default constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow() : DBRow(ALLOC< T_DATA >()) {}


    /// constructor with a given size for the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(const std::size_t      size,
                                         const T_DATA           default_cell,
                                         const double           weight,
                                         const ALLOC< T_DATA >& alloc) :
        row_(size, default_cell, alloc),
        weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// constructor with a given size for the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(const std::size_t      size,
                                         const double           weight,
                                         const ALLOC< T_DATA >& alloc) :
        DBRow(size, T_DATA(), weight, alloc) {}

    /// initializer list constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(std::initializer_list< T_DATA > list,
                                         const double                    weight,
                                         const ALLOC< T_DATA >&          alloc) :
        row_(list, alloc),
        weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// sets a new row
    template < typename T_DATA, template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::setRow(
       const std::vector< T_DATA, OTHER_ALLOC< T_DATA > >& new_row) {
      const std::size_t size = new_row.size();
      if (size) {
        row_.resize(size);
        std::copy(new_row.begin(), new_row.end(), row_.begin());
      } else {
        row_.clear();
      }
    }


    /// sets a new row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::setRow(
       std::vector< T_DATA, ALLOC< T_DATA > >&& new_row) {
      row_ = std::move(new_row);
    }


    /// initializer from a vector of cells
    template < typename T_DATA, template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(
       const std::vector< T_DATA, OTHER_ALLOC< T_DATA > >& cells,
       const double                                        weight,
       const ALLOC< T_DATA >&                              alloc) :
        row_(alloc),
        weight_(weight) {
      setRow(cells);
      GUM_CONSTRUCTOR(DBRow);
    }


    /// initializer from a vector of cells
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(
       std::vector< T_DATA, ALLOC< T_DATA > >&& cells,
       const double                             weight,
       const ALLOC< T_DATA >&                   alloc) :
        row_(std::move(cells), alloc),
        weight_(weight) {
      GUM_CONSTRUCTOR(DBRow);
    }


    /// copy constructor with a given allocator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(const DBRow< T_DATA, ALLOC >& from,
                                         const ALLOC< T_DATA >&        alloc) :
        row_(from.row_, alloc),
        weight_(from.weight_) {
      GUM_CONS_CPY(DBRow);
    }


    /// copy constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(const DBRow< T_DATA, ALLOC >& from) :
        DBRow< T_DATA, ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(DBRow< T_DATA, ALLOC >&& from,
                                         const ALLOC< T_DATA >&   alloc) :
        row_(std::move(from.row_), alloc),
        weight_(from.weight_) {
      GUM_CONS_MOV(DBRow);
    }


    /// move constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::DBRow(DBRow< T_DATA, ALLOC >&& from) :
        DBRow< T_DATA, ALLOC >(from, from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < typename T_DATA, template < typename > class ALLOC >
    DBRow< T_DATA, ALLOC >*
       DBRow< T_DATA, ALLOC >::clone(const ALLOC< T_DATA >& alloc) const {
      ALLOC< DBRow< T_DATA, ALLOC > > allocator(alloc);
      DBRow< T_DATA, ALLOC >*         row = allocator.allocate(1);
      try {
        allocator.construct(row, *this, alloc);
      } catch (...) {
        allocator.deallocate(row, 1);
        throw;
      }
      return row;
    }


    /// virtual copy constructor
    template < typename T_DATA, template < typename > class ALLOC >
    DBRow< T_DATA, ALLOC >* DBRow< T_DATA, ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >::~DBRow() {
      GUM_DESTRUCTOR(DBRow);
    }


    /// copy operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >&
       DBRow< T_DATA, ALLOC >::operator=(const DBRow< T_DATA, ALLOC >& from) {
      if (this != &from) {
        row_ = from.row_;
        weight_ = from.weight_;
      }
      return *this;
    }


    /// move operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE DBRow< T_DATA, ALLOC >&
       DBRow< T_DATA, ALLOC >::operator=(DBRow< T_DATA, ALLOC >&& from) {
      if (this != &from) {
        row_ = std::move(from.row_);
        weight_ = from.weight_;
      }
      return *this;
    }

    /// returns the ith T_DATA of the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE T_DATA& DBRow< T_DATA, ALLOC >::operator[](const std::size_t i) {
      return row_[i];
    }

    /// returns the ith T_DATA of the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const T_DATA&
       DBRow< T_DATA, ALLOC >::operator[](const std::size_t i) const {
      return row_[i];
    }

    /// returns the current row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const std::vector< T_DATA, ALLOC< T_DATA > >&
                 DBRow< T_DATA, ALLOC >::row() const noexcept {
      return row_;
    }

    /// returns the current row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::vector< T_DATA, ALLOC< T_DATA > >&
           DBRow< T_DATA, ALLOC >::row() noexcept {
      return row_;
    }

    /// returns the weight
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const double& DBRow< T_DATA, ALLOC >::weight() const noexcept {
      return weight_;
    }

    /// returns the weight
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE double& DBRow< T_DATA, ALLOC >::weight() noexcept {
      return weight_;
    }

    /// sets a new weight
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::setWeight(const double new_weight) {
      weight_ = new_weight;
    }

    /// returns the size of the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t DBRow< T_DATA, ALLOC >::size() const noexcept {
      return row_.size();
    }

    /// resize a given row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::resize(const std::size_t new_size) {
      row_.resize(new_size);
    }

    /// reserve a size for the elements of a given row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::reserve(const std::size_t new_size) {
      row_.reserve(new_size);
    }

    /// adds a new element at the end of the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::pushBack(const T_DATA& elt) {
      row_.push_back(elt);
    }

    /// adds a new element at the end of the row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void DBRow< T_DATA, ALLOC >::pushBack(T_DATA&& elt) {
      row_.push_back(std::move(elt));
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
