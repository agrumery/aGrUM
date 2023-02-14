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
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/DBRowGenerator4CompleteRows.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// generates new lines from those the generator gets in input
    INLINE const DBRow< DBTranslatedValue >& DBRowGenerator4CompleteRows::generate() {
      this->decreaseRemainingRows();
      return *_input_row_;
    }


    /// computes the rows it will provide in output
    INLINE std::size_t
           DBRowGenerator4CompleteRows::computeRows_(const DBRow< DBTranslatedValue >& row) {
      // check that all the values are observed
      const auto& xrow = row.row();
      for (const auto col: this->columns_of_interest_) {
        switch (this->column_types_[col]) {
          case DBTranslatedValueType::DISCRETE:
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              _input_row_ = nullptr;
              return std::size_t(0);
            }
            break;

          case DBTranslatedValueType::CONTINUOUS:
            if (xrow[col].cont_val == std::numeric_limits< float >::max()) {
              _input_row_ = nullptr;
              return std::size_t(0);
            }
            break;

          default:
            GUM_ERROR(NotImplementedYet,
                      "DBTranslatedValueType " << int(this->column_types_[col])
                                               << " is not supported yet");
        }
      }
      _input_row_ = &row;
      return std::size_t(1);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
