/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBRowGenerator4CompleteRows.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBRowGenerator4CompleteRows_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBRowGenerator4CompleteRows::DBRowGenerator4CompleteRows(
        const std::vector< DBTranslatedValueType >& column_types) :
        DBRowGenerator(column_types, DBRowGeneratorGoal::ONLY_REMOVE_MISSING_VALUES) {
      GUM_CONSTRUCTOR(DBRowGenerator4CompleteRows);
    }

    /// copy constructor
    DBRowGenerator4CompleteRows::DBRowGenerator4CompleteRows(
        const DBRowGenerator4CompleteRows& from) :
        DBRowGenerator(from), _input_row_(from._input_row_) {
      GUM_CONS_CPY(DBRowGenerator4CompleteRows);
    }

    /// move constructor
    DBRowGenerator4CompleteRows::DBRowGenerator4CompleteRows(DBRowGenerator4CompleteRows&& from) :
        DBRowGenerator(std::move(from)), _input_row_(from._input_row_) {
      GUM_CONS_MOV(DBRowGenerator4CompleteRows);
    }

    /// virtual copy constructor
    DBRowGenerator4CompleteRows* DBRowGenerator4CompleteRows::clone() const {
      return new DBRowGenerator4CompleteRows(*this);
    }

    /// destructor
    DBRowGenerator4CompleteRows::~DBRowGenerator4CompleteRows() {
      GUM_DESTRUCTOR(DBRowGenerator4CompleteRows);
    }

    /// copy operator
    DBRowGenerator4CompleteRows&
        DBRowGenerator4CompleteRows::operator=(const DBRowGenerator4CompleteRows& from) {
      DBRowGenerator::operator=(from);
      _input_row_ = from._input_row_;
      return *this;
    }

    /// move operator
    DBRowGenerator4CompleteRows&
        DBRowGenerator4CompleteRows::operator=(DBRowGenerator4CompleteRows&& from) {
      DBRowGenerator::operator=(std::move(from));
      _input_row_ = from._input_row_;
      return *this;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
