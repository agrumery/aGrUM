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
 * @brief A DBRowGenerator class that returns exactly the rows it gets in input
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBRowGeneratorIdentity_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBRowGeneratorIdentity::DBRowGeneratorIdentity(
       const std::vector< DBTranslatedValueType >& column_types) :
        DBRowGenerator(column_types, DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES) {
      GUM_CONSTRUCTOR(DBRowGeneratorIdentity);
    }

    /// copy constructor
    DBRowGeneratorIdentity::DBRowGeneratorIdentity(const DBRowGeneratorIdentity& from) :
        DBRowGenerator(from), _input_row_(from._input_row_) {
      GUM_CONS_CPY(DBRowGeneratorIdentity);
    }

    /// move constructor
    DBRowGeneratorIdentity::DBRowGeneratorIdentity(DBRowGeneratorIdentity&& from) :
        DBRowGenerator(std::move(from)), _input_row_(from._input_row_) {
      GUM_CONS_MOV(DBRowGeneratorIdentity);
    }

    /// virtual copy constructor
    DBRowGeneratorIdentity* DBRowGeneratorIdentity::clone() const {
      return new DBRowGeneratorIdentity(*this);
    }

    /// destructor
    DBRowGeneratorIdentity::~DBRowGeneratorIdentity() { GUM_DESTRUCTOR(DBRowGeneratorIdentity); }

    /// copy operator
    DBRowGeneratorIdentity& DBRowGeneratorIdentity::operator=(const DBRowGeneratorIdentity& from) {
      DBRowGenerator::operator=(from);
      _input_row_ = from._input_row_;
      return *this;
    }

    /// move operator
    DBRowGeneratorIdentity& DBRowGeneratorIdentity::operator=(DBRowGeneratorIdentity&& from) {
      DBRowGenerator::operator=(std::move(from));
      _input_row_ = from._input_row_;
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
