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
 * @brief The class representing rows in tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_H
#define GUM_LEARNING_DB_ROW_H

#include <vector>
#include <initializer_list>

#include <agrum/learning/database/DBCell.h>

namespace gum {

  namespace learning {

    /** @class DBRow
     * @ingroup learning_group
     * @brief The class for storing a row of DBCells into the database */
    class DBRow {
      public:
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// default constructor
      DBRow();

      /// constructor with a given size for the row
      DBRow(unsigned int size, const DBCell &default_cell = DBCell(),
            float weight = 1.0f);

      /// initializer list constructor
      DBRow(std::initializer_list<DBCell> &list);

      /// initializer from a vector of cells
      DBRow(const std::vector<DBCell> &cells, float weight = 1.0f);

      /// initializer from a vector of cells
      DBRow(std::vector<DBCell> &&cells, float weight = 1.0f);

      /// copy operator
      DBRow(const DBRow &from);

      /// move operator
      DBRow(DBRow &&from);

      /// destructor
      ~DBRow();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRow &operator=(const DBRow &from);

      /// move operator
      DBRow &operator=(DBRow &&from);

      /// returns the ith DBCell of the row
      DBCell &operator[](unsigned int i);

      /// returns the ith DBCell of the row
      const DBCell &operator[](unsigned int i) const;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the current row
      const std::vector<DBCell> &row() const noexcept;

      /// returns the current row
      std::vector<DBCell> &row() noexcept;

      /// returns the weight
      const float &weight() const noexcept;

      /// returns the weight
      float &weight() noexcept;

      /// sets a new row
      void setRow(const std::vector<DBCell> &new_row);

      /// sets a new row
      void setRow(std::vector<DBCell> &&new_row);

      /// sets a new weight
      void setWeight(float new_weight);

      /// returns the size of the row
      unsigned int size() const noexcept;

      /// resize a given row
      void resize(unsigned int new_size);

      /// @}

      protected:
      // the row itself
      std::vector<DBCell> _row;

      // the weight of the row
      float _weight{1.0f};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/DBRow.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_ROW_H */
