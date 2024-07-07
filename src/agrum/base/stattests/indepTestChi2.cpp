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


/**
 * @file
 * @brief Implementation of gum::learning::IndepTestChi2.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/indepTestChi2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/indepTestChi2_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    IndepTestChi2& IndepTestChi2::operator=(const IndepTestChi2& from) {
      if (this != &from) {
        IndependenceTest::operator=(from);
        // __chi2 = from. _chi2_;
      }
      return *this;
    }

    /// move operator
    IndepTestChi2& IndepTestChi2::operator=(IndepTestChi2&& from) {
      if (this != &from) {
        IndependenceTest::operator=(std::move(from));
        // __chi2 = std::move(from. _chi2_);
      }
      return *this;
    }

    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    std::pair< double, double > IndepTestChi2::statistics_(const IdCondSet& idset) {
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

      double cumulStat = 0;

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


        // now, perform sum_X sum_Y sum_Z ( #ZYX - #ZX * #ZY / #Z )^2 /
        // (#ZX * #ZY / #Z )
        for (std::size_t z      = std::size_t(0),
                         beg_xz = std::size_t(0),
                         beg_yz = std::size_t(0),
                         xyz    = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size, beg_yz += Y_size) {
          if (N_z[z] > 0) {
            for (std::size_t y = std::size_t(0), yz = beg_yz; y < Y_size; ++yz, ++y) {
              for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++xz, ++x, ++xyz) {
                const double tmp1 = (N_yz[yz] * N_xz[xz]) / N_z[z];
                if (tmp1 != 0.0) {
                  const double tmp2 = N_xyz[xyz] - tmp1;
                  cumulStat += (tmp2 * tmp2) / tmp1;
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

        // now, perform sum_X sum_Y ( #XY - (#X * #Y) / N )^2 / (#X * #Y )/N

        // get the counts for all the targets and for the conditioning nodes
        std::vector< double > N_x
            = this->marginalize_(std::size_t(1), X_size, Y_size, std::size_t(1), N_xyz);
        std::vector< double > N_y
            = this->marginalize_(std::size_t(0), X_size, Y_size, std::size_t(1), N_xyz);

        // count N
        double N = 0.0;
        for (const auto n_x: N_x)
          N += n_x;

        for (std::size_t y = std::size_t(0), xy = 0; y < Y_size; ++y) {
          const double tmp_Ny = N_y[y];
          for (std::size_t x = 0; x < X_size; ++x, ++xy) {
            const double tmp1 = (tmp_Ny * N_x[x]) / N;
            if (tmp1 != 0.0) {
              const double tmp2 = N_xyz[xy] - tmp1;
              cumulStat += (tmp2 * tmp2) / tmp1;
            }
          }
        }
      }

      Size   df     = _chi2_.degreesOfFreedom(var_x, var_y);
      double pValue = _chi2_.probaChi2(cumulStat, df);
      return std::pair< double, double >(cumulStat, pValue);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
