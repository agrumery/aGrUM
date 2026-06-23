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
 *   useful, but WITHOUT ANY WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,      *
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


#pragma once

/** @file
 * @brief template implementation of computeStatistics_(), shared by all
 *        chi-squared-family independence tests (Chi2, G2, …)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/independenceTest.h>   // to ease IDE parser
#include <agrum/base/core/math/chi2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// shared loop for chi-squared-family statistics
    /** Handles counts, marginalisation, structural-zero detection (reduces df),
     *  and the final chi2 p-value.  Only active cells (margX * margY != 0) are
     *  forwarded to @p cellContrib; the lambda must return the cell's contribution
     *  to the statistic (0.0 for sampling zeros).
     *
     *  Signature expected for CellContribFn:
     *    double cellContrib(double O, double margX, double margY, double total)
     *  where O = observed count, margX/margY = relevant marginals, total = N_z or N.
     */
    template < typename CellContribFn >
    std::pair< double, double >
        IndependenceTest::computeStatistics_(const IdCondSet& idset, CellContribFn cellContrib) {
      // get the counts
      std::vector< double > N_xyz(this->counter_.counts(idset, true));
      const bool            informative_external_prior = this->prior_->isInformative();
      if (informative_external_prior) { this->prior_->addJointPseudoCount(idset, N_xyz); }
      const std::size_t all_size = N_xyz.size();

      const auto& nodeId2cols = this->counter_.nodeId2Columns();
      const auto& database    = this->counter_.database();
      Idx         var_x, var_y;
      if (nodeId2cols.empty()) {
        var_x = idset[0];
        var_y = idset[1];
      } else {
        var_x = nodeId2cols.second(idset[0]);
        var_y = nodeId2cols.second(idset[1]);
      }

      const std::size_t X_size = database.domainSize(var_x);
      const std::size_t Y_size = database.domainSize(var_y);

      double            cumulStat = 0.0;
      std::size_t       n_skipped = 0;
      const std::size_t Z_size
          = idset.hasConditioningSet() ? all_size / (X_size * Y_size) : std::size_t(1);

      if (idset.hasConditioningSet()) {
        std::vector< double > N_xz
            = this->marginalize_(std::size_t(1), X_size, Y_size, Z_size, N_xyz);
        std::vector< double > N_yz
            = this->marginalize_(std::size_t(0), X_size, Y_size, Z_size, N_xyz);
        std::vector< double > N_z
            = this->marginalize_(std::size_t(2), X_size, Y_size, Z_size, N_xyz);

        for (std::size_t z = 0, beg_xz = 0, beg_yz = 0, xyz = 0; z < Z_size;
             ++z, beg_xz += X_size, beg_yz += Y_size) {
          if (N_z[z] > 0) {
            for (std::size_t y = std::size_t(0), yz = beg_yz; y < Y_size; ++yz, ++y) {
              for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++xz, ++x, ++xyz) {
                // structural zero (a marginal is 0 → E undefined): skip, reduce df.
                // Sampling zeros (O=0, E>0) are left to cellContrib (return 0).
                // Agresti, "Categorical Data Analysis", 3rd ed. (Wiley, 2013).
                if (N_yz[yz] * N_xz[xz] != 0.0) {
                  cumulStat += cellContrib(N_xyz[xyz], N_xz[xz], N_yz[yz], N_z[z]);
                } else {
                  ++n_skipped;
                }
              }
            }
          } else {
            n_skipped += X_size * Y_size;
            xyz += X_size * Y_size;
          }
        }
      } else {
        std::vector< double > N_x
            = this->marginalize_(std::size_t(1), X_size, Y_size, std::size_t(1), N_xyz);
        std::vector< double > N_y
            = this->marginalize_(std::size_t(0), X_size, Y_size, std::size_t(1), N_xyz);

        double N = 0.0;
        for (const auto n_x: N_x)
          N += n_x;

        for (std::size_t y = std::size_t(0), xy = 0; y < Y_size; ++y) {
          for (std::size_t x = 0; x < X_size; ++x, ++xy) {
            if (N_y[y] * N_x[x] != 0.0) {
              cumulStat += cellContrib(N_xyz[xy], N_x[x], N_y[y], N);
            } else {
              ++n_skipped;
            }
          }
        }
      }

      Size   df     = degreesOfFreedom_(X_size, Y_size, Z_size, n_skipped);
      double pValue = Chi2::probaChi2(cumulStat, df);
      return {cumulStat, pValue};
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
