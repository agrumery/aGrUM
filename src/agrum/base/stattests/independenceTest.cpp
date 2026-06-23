/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of gum::learning::IndependenceTest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <limits>

#include <agrum/base/stattests/independenceTest.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/independenceTest_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    IndependenceTest& IndependenceTest::operator=(const IndependenceTest& from) {
      if (this != &from) {
        CachedContingencyCounter::operator=(from);
        _domain_sizes_ = from._domain_sizes_;
      }
      return *this;
    }

    /// move operator
    IndependenceTest& IndependenceTest::operator=(IndependenceTest&& from) {
      if (this != &from) {
        CachedContingencyCounter::operator=(std::move(from));
        _domain_sizes_ = std::move(from._domain_sizes_);
      }
      return *this;
    }

    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    std::vector< double > IndependenceTest::marginalize_(const std::size_t node_2_marginalize,
                                                         const std::size_t X_size,
                                                         const std::size_t Y_size,
                                                         const std::size_t Z_size,
                                                         const std::vector< double >& N_xyz) const {
      // determine the size of the output vector
      std::size_t out_size = Z_size;
      if (node_2_marginalize == std::size_t(0)) {
        if (Z_size != 0 && Y_size > std::numeric_limits< std::size_t >::max() / Z_size)
          GUM_ERROR(OutOfBounds, "marginalize_: out_size overflow (Z_size * Y_size)")
        out_size *= Y_size;
      } else if (node_2_marginalize == std::size_t(1)) {
        if (Z_size != 0 && X_size > std::numeric_limits< std::size_t >::max() / Z_size)
          GUM_ERROR(OutOfBounds, "marginalize_: out_size overflow (Z_size * X_size)")
        out_size *= X_size;
      }

      // allocate the output vector
      std::vector< double > res(out_size, 0.0);

      // fill the vector:
      if (node_2_marginalize == std::size_t(0)) {   // marginalize X
        for (std::size_t yz = std::size_t(0), xyz = std::size_t(0); yz < out_size; ++yz) {
          for (std::size_t x = std::size_t(0); x < X_size; ++x, ++xyz) {
            res[yz] += N_xyz[xyz];
          }
        }
      } else if (node_2_marginalize == std::size_t(1)) {   // marginalize Y
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0), beg_xz = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size) {
          for (std::size_t y = std::size_t(0); y < Y_size; ++y) {
            for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++x, ++xz, ++xyz) {
              res[xz] += N_xyz[xyz];
            }
          }
        }
      } else if (node_2_marginalize == std::size_t(2)) {   // marginalize X and Y
        const std::size_t XY_size = X_size * Y_size;
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0); z < out_size; ++z) {
          for (std::size_t xy = std::size_t(0); xy < XY_size; ++xy, ++xyz) {
            res[z] += N_xyz[xyz];
          }
        }
      } else {
        GUM_ERROR(NotImplementedYet,
                  "_marginalize not implemented for nodeset " << node_2_marginalize);
      }

      return res;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
