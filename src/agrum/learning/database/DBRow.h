
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
 * @brief The class representing a record stored in a tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_H
#define GUM_LEARNING_DB_ROW_H

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <vector>

#include <agrum/agrum.h>

namespace gum {

  namespace learning {

    /** @class DBRow
     * @headerfile DBRow.h <agrum/learning/database/DBRow.h>
     * @brief The class for storing a record in a database
     *
     * Learning algorithms use DatabaseTable instances as their input data.
     * Those are represented essentially as vectors of DBRow instances. Each
     * DBRow is encoded as a vector of elements plus a weight (which is a
     * double). To be quite general, the types of the elements stored into the
     * DBRow are specified as template parameters of the DBRow.
     *
     * @ingroup learning_database */
    template < typename T_DATA,
               template < typename > class ALLOC = std::allocator >
    class DBRow {
      public:
      using allocator_type = ALLOC< T_DATA >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRow();

      /// default constructor with specific allocator
      DBRow(const ALLOC< T_DATA >& alloc);

      /// constructs a row of a given size with missing values
      DBRow(const std::size_t      size,
            const double           weight = 1.0,
            const ALLOC< T_DATA >& alloc = ALLOC< T_DATA >());

      /// constructor with a given size for the row
      DBRow(const std::size_t      size,
            const T_DATA           default_value,
            const double           weight,
            const ALLOC< T_DATA >& alloc = ALLOC< T_DATA >());

      /// initializer list constructor
      DBRow(std::initializer_list< T_DATA > list,
            const double                    weight = 1.0,
            const ALLOC< T_DATA >&          alloc = ALLOC< T_DATA >());

      /// initializer from a vector of cells
      template < template < typename > class OTHER_ALLOC >
      DBRow(const std::vector< T_DATA, OTHER_ALLOC< T_DATA > >& cells,
            const double                                        weight = 1.0,
            const ALLOC< T_DATA >& alloc = ALLOC< T_DATA >());

      /// initializer with a move from a vector of cells
      DBRow(std::vector< T_DATA, ALLOC< T_DATA > >&& cells,
            const double                             weight = 1.0,
            const ALLOC< T_DATA >&                   alloc = ALLOC< T_DATA >());

      /// copy constructor
      DBRow(const DBRow< T_DATA, ALLOC >& from);

      /// copy constructor with a given allocator
      DBRow(const DBRow< T_DATA, ALLOC >& from, const ALLOC< T_DATA >& alloc);

      /// move constructor
      DBRow(DBRow< T_DATA, ALLOC >&& from);

      /// move constructor with a given allocator
      DBRow(DBRow< T_DATA, ALLOC >&& from, const ALLOC< T_DATA >& alloc);

      /// virtual copy constructor
      DBRow< T_DATA, ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      DBRow< T_DATA, ALLOC >* clone(const ALLOC< T_DATA >& alloc) const;

      /// destructor
      ~DBRow();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRow< T_DATA, ALLOC >& operator=(const DBRow< T_DATA, ALLOC >& from);

      /// move operator
      DBRow< T_DATA, ALLOC >& operator=(DBRow< T_DATA, ALLOC >&& from);

      /// returns the ith content of the row
      /** @warning This method does not check that there are at least i+1
       * elements in the row. If this is not the case, you will probably get
       * a segmentation fault. */
      T_DATA& operator[](const std::size_t i);

      /// returns the ith content of the row
      /** @warning This method does not check that there are at least i+1
       * elements in the row. If this is not the case, you will probably get
       * a segmentation fault. */
      const T_DATA& operator[](const std::size_t i) const;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the current row (without the weight)
      const std::vector< T_DATA, ALLOC< T_DATA > >& row() const noexcept;

      /// returns the current row (without the weight)
      std::vector< T_DATA, ALLOC< T_DATA > >& row() noexcept;

      /// returns the weight assigned to the DBRow
      const double& weight() const noexcept;

      /// returns the weight assigned to the DBRow
      double& weight() noexcept;

      /// sets a new row (without changing the weight)
      template < template < typename > class OTHER_ALLOC >
      void setRow(const std::vector< T_DATA, OTHER_ALLOC< T_DATA > >& new_row);

      /// sets a new row (without changing the weight)
      void setRow(std::vector< T_DATA, ALLOC< T_DATA > >&& new_row);

      /// sets a new weight
      void setWeight(const double new_weight);

      /// returns the number of elements in the row
      std::size_t size() const noexcept;

      /// resize a given row, i.e., its number of elements
      void resize(const std::size_t new_size);

      /// reserve a size for the elements of a given row
      void reserve(const std::size_t new_size);

      /// adds a new element at the end of the row
      void pushBack(const T_DATA& elt);

      /// adds a new element at the end of the row
      void pushBack(T_DATA&& elt);

      /// returns the allocator used by the DBRow
      ALLOC< T_DATA > getAllocator() const;

      /// @}


      protected:
      /// the row itself
      std::vector< T_DATA, ALLOC< T_DATA > > _row;

      /// the weight of the row
      double _weight{1.0f};

      // used for constructors and operators
      template < typename TX_DATA, template < typename > class OTHER_ALLOC >
      friend class DBRow;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/learning/database/DBRow_tpl.h>

#endif /* GUM_LEARNING_DB_ROW_H */
