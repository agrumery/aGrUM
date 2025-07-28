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
 * @brief The class representing a record stored in a tabular database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_ROW_H
#define GUM_LEARNING_DB_ROW_H

#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

#include <agrum/agrum.h>

#include <initializer_list>

namespace gum {

  namespace learning {

    /** @class DBRow
     * @headerfile DBRow.h <agrum/base/database/DBRow.h>
     * @brief The class for storing a record in a database
     *
     * Learning algorithms use DatabaseTable instances as their input data.
     * Those are represented essentially as vectors of DBRow instances. Each
     * DBRow is encoded as a vector of elements plus a weight (which is a
     * double). To be quite general, the types of the elements stored into the
     * DBRow are specified as template parameters of the DBRow.
     *
     * @ingroup learning_database */
    template < typename T_DATA >
    class DBRow {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRow();

      /// constructs a row of a given size with missing values
      DBRow(const std::size_t size, const double weight = 1.0);

      /// constructor with a given size for the row
      DBRow(const std::size_t size, const T_DATA default_value, const double weight);

      /// initializer list constructor
      DBRow(std::initializer_list< T_DATA > list, const double weight = 1.0);

      /// initializer from a vector of cells
      DBRow(const std::vector< T_DATA >& cells, const double weight = 1.0);

      /// initializer with a move from a vector of cells
      DBRow(std::vector< T_DATA >&& cells, const double weight = 1.0);

      /// copy constructor
      DBRow(const DBRow< T_DATA >& from);

      /// move constructor
      DBRow(DBRow< T_DATA >&& from);

      /// virtual copy constructor
      DBRow< T_DATA >* clone() const;

      /// destructor
      ~DBRow();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRow< T_DATA >& operator=(const DBRow< T_DATA >& from);

      /// move operator
      DBRow< T_DATA >& operator=(DBRow< T_DATA >&& from);

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
      const std::vector< T_DATA >& row() const noexcept;

      /// returns the current row (without the weight)
      std::vector< T_DATA >& row() noexcept;

      /// returns the weight assigned to the DBRow
      const double& weight() const noexcept;

      /// returns the weight assigned to the DBRow
      double& weight() noexcept;

      /// sets a new row (without changing the weight)
      void setRow(const std::vector< T_DATA >& new_row);

      /// sets a new row (without changing the weight)
      void setRow(std::vector< T_DATA >&& new_row);

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

      /// @}


      protected:
      /// the row itself
      std::vector< T_DATA > row_;

      /// the weight of the row
      double weight_{1.0f};
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/base/database/DBRow_tpl.h>

#endif /* GUM_LEARNING_DB_ROW_H */
