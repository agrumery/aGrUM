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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/database/DBRowGenerator4CompleteRows.h>

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
          case DBTranslatedValueType::DISCRETE :
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              _input_row_ = nullptr;
              return std::size_t(0);
            }
            break;

          case DBTranslatedValueType::CONTINUOUS :
            if (xrow[col].cont_val == std::numeric_limits< float >::max()) {
              _input_row_ = nullptr;
              return std::size_t(0);
            }
            break;

          default :
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
