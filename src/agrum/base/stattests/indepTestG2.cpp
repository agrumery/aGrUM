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


/**
 * @file
 * @brief Implementation of gum::learning::IndepTestG2.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/indepTestG2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/indepTestG2_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    IndepTestG2& IndepTestG2::operator=(const IndepTestG2& from) {
      if (this != &from) {
        IndependenceTest::operator=(from);
        // __chi2 = from. _chi2_;
      }
      return *this;
    }

    /// move operator
    IndepTestG2& IndepTestG2::operator=(IndepTestG2&& from) {
      if (this != &from) {
        IndependenceTest::operator=(std::move(from));
        // __chi2 = std::move(from. _chi2_);
      }
      return *this;
    }

    /// returns the score corresponding to a given nodeset
    std::pair< double, double > IndepTestG2::statistics_(const IdCondSet& idset) {
      // get the counts
      std::vector< double > N_xyz(this->counter_.counts(idset, true));
      const bool            informative_external_prior = this->prior_->isInformative();
      if (informative_external_prior) this->prior_->addJointPseudoCount(idset, N_xyz);
      const std::size_t all_size = (N_xyz.size());

      // compute the domain sizes of X and Y
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

      double cumulStat = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        const std::size_t Z_size = all_size / (X_size * Y_size);

        // get the counts for the conditioning nodes
        std::vector< double > N_xz
            = this->marginalize_(std::size_t(1), X_size, Y_size, Z_size, N_xyz);
        std::vector< double > N_yz
            = this->marginalize_(std::size_t(0), X_size, Y_size, Z_size, N_xyz);
        std::vector< double > N_z
            = this->marginalize_(std::size_t(2), X_size, Y_size, Z_size, N_xyz);

        // indicate to the chi2 distribution the set of conditioning nodes
        std::vector< Idx > cond_nodes;
        cond_nodes.reserve(idset.nbRHSIds());
        {
          const auto cond_idset = idset.conditionalIdCondSet().ids();
          if (nodeId2cols.empty()) {
            for (const auto node: cond_idset)
              cond_nodes.push_back(node);
          } else {
            for (const auto node: cond_idset)
              cond_nodes.push_back(nodeId2cols.second(node));
          }
        }
        _chi2_.setConditioningNodes(cond_nodes);


        // now, perform :
        // sum_X sum_Y sum_Z #XYZ * log ( ( #XYZ * #Z ) / ( #XZ * #YZ ) )
        for (std::size_t z      = std::size_t(0),
                         beg_xz = std::size_t(0),
                         beg_yz = std::size_t(0),
                         xyz    = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size, beg_yz += Y_size) {
          if (N_z[z] > 0) {
            for (std::size_t y = std::size_t(0), yz = beg_yz; y < Y_size; ++yz, ++y) {
              for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++xz, ++x, ++xyz) {
                const double tmp1 = N_xyz[xyz] * N_z[z];
                const double tmp2 = N_yz[yz] * N_xz[xz];
                if ((tmp1 != 0.0) && (tmp2 != 0.0)) {
                  cumulStat += N_xyz[xyz] * std::log(tmp1 / tmp2);
                }
              }
            }
          } else {   // moving xyz out of the loops x,y when if N_z[z]==0
            xyz += X_size * Y_size;
          }
        }
      } else {
        // here, there is no conditioning set

        // indicate to the chi2 distribution the set of conditioning nodes
        _chi2_.setConditioningNodes(_empty_set_);

        // now, perform sum_X sum_Y #XY * log ( ( #XY * N ) / ( #X * #Y ) )

        // get the counts for all the targets and for the conditioning nodes
        std::vector< double > N_x
            = this->marginalize_(std::size_t(1), X_size, Y_size, std::size_t(1), N_xyz);
        std::vector< double > N_y
            = this->marginalize_(std::size_t(0), X_size, Y_size, std::size_t(1), N_xyz);

        // count N
        double N = 0.0;
        for (auto n_x: N_x)
          N += n_x;

        for (std::size_t y = std::size_t(0), xy = 0; y < Y_size; ++y) {
          const double tmp_Ny = N_y[y];
          for (std::size_t x = 0; x < X_size; ++x, ++xy) {
            const double tmp = (tmp_Ny * N_x[x]);
            if ((tmp != 0.0) && (N_xyz[xy] != 0.0)) {
              cumulStat += N_xyz[xy] * std::log((N_xyz[xy] * N) / tmp);
            }
          }
        }
      }

      // used to make the G test formula asymptotically equivalent
      // to the Pearson's chi-squared test formula
      cumulStat *= 2;

      Size   df     = _chi2_.degreesOfFreedom(var_x, var_y);
      double pValue = _chi2_.probaChi2(cumulStat, df);
      return std::pair< double, double >(cumulStat, pValue);
    }

    /// returns the score corresponding to a given nodeset
    double IndepTestG2::score_(const IdCondSet& idset) {
      // compute the domain sizes of X and Y
      const auto& nodeId2cols = this->counter_.nodeId2Columns();
      Idx         var_x, var_y;
      if (nodeId2cols.empty()) {
        var_x = idset[0];
        var_y = idset[1];
      } else {
        var_x = nodeId2cols.second(idset[0]);
        var_y = nodeId2cols.second(idset[1]);
      }

      auto   stat  = statistics_(idset);
      double score = stat.first;

      // ok, here, score contains the value of the chi2 formula.
      // To get a meaningful score, we shall compute the critical values
      // for the Chi2 distribution and assign as the score of
      // (score - alpha ) / alpha, where alpha is the critical value
      const double alpha = _chi2_.criticalValue(var_x, var_y);
      score              = (score - alpha) / alpha;

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
