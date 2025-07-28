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
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/paramUtils/paramEstimatorML_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// destructor
    ParamEstimatorML::~ParamEstimatorML() { GUM_DESTRUCTOR(ParamEstimatorML); }

    /// copy operator
    ParamEstimatorML& ParamEstimatorML::operator=(const ParamEstimatorML& from) {
      ParamEstimator::operator=(from);
      return *this;
    }

    /// move operator
    ParamEstimatorML& ParamEstimatorML::operator=(ParamEstimatorML&& from) {
      ParamEstimator::operator=(std::move(from));
      return *this;
    }

    /// returns the CPT's parameters corresponding to a given set of nodes
    std::pair< std::vector< double >, double > ParamEstimatorML::_parametersAndLogLikelihood_(
        const NodeId                 target_node,
        const std::vector< NodeId >& conditioning_nodes,
        const bool                   compute_log_likelihood) {
      // create an idset that contains all the nodes in the following order:
      // first, the target node, then all the conditioning nodes
      IdCondSet idset(target_node, conditioning_nodes, true);

      // get the counts for all the nodes in the idset and add the external and
      // score internal priors
      this->counter_.clear();   // for EM estimations, we need to disable caches
      const std::vector< double >& original_N_ijk = this->counter_.counts(idset, true);
      std::vector< double >        N_ijk          = original_N_ijk;
      const bool informative_external_prior       = this->external_prior_->isInformative();
      const bool informative_score_internal_prior = this->score_internal_prior_->isInformative();

      // add the priors pseudocounts
      if (informative_external_prior) this->external_prior_->addJointPseudoCount(idset, N_ijk);
      if (informative_score_internal_prior)
        this->score_internal_prior_->addJointPseudoCount(idset, N_ijk);
      double log_likelihood = 0.0;

      // now, normalize N_ijk

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (!conditioning_nodes.empty()) {
        // get the counts for all the conditioning nodes, and add them the
        // external and score internal priors
        std::vector< double > N_ij(this->counter_.counts(idset.conditionalIdCondSet(), false));
        if (informative_external_prior)
          this->external_prior_->addConditioningPseudoCount(idset, N_ij);
        if (informative_score_internal_prior)
          this->score_internal_prior_->addConditioningPseudoCount(idset, N_ij);

        const std::size_t conditioning_domsize = N_ij.size();
        const std::size_t target_domsize       = N_ijk.size() / conditioning_domsize;

        // check that all conditioning nodes have strictly positive counts
        for (std::size_t j = std::size_t(0); j < conditioning_domsize; ++j) {
          if (N_ij[j] == 0) {
            // get the domain sizes of the conditioning nodes
            const std::size_t  cond_nb = conditioning_nodes.size();
            std::vector< Idx > cond_domsize(cond_nb);

            const auto& node2cols = this->counter_.nodeId2Columns();
            const auto& database  = this->counter_.database();
            if (node2cols.empty()) {
              for (std::size_t i = std::size_t(0); i < cond_nb; ++i) {
                cond_domsize[i] = database.domainSize(conditioning_nodes[i]);
              }
            } else {
              for (std::size_t i = std::size_t(0); i < cond_nb; ++i) {
                cond_domsize[i] = database.domainSize(node2cols.second(conditioning_nodes[i]));
              }
            }

            // determine the value of each conditioning variable in N_ij[j]
            std::vector< Idx > offsets(cond_nb);
            Idx                offset = 1;
            std::size_t        i;
            for (i = std::size_t(0); i < cond_nb; ++i) {
              offsets[i] = offset;
              offset *= cond_domsize[i];
            }
            std::vector< Idx > values(cond_nb);
            i      = 0;
            offset = j;
            for (Idx jj = cond_nb - 1; i < cond_nb; ++i, --jj) {
              values[jj] = offset / offsets[jj];
              offset %= offsets[jj];
            }

            // create the error message
            std::stringstream str;
            str << "The conditioning set <";
            bool deja = false;
            for (i = std::size_t(0); i < cond_nb; ++i) {
              if (deja) str << ", ";
              else deja = true;
              std::size_t             col = node2cols.empty() ? conditioning_nodes[i]
                                                              : node2cols.second(conditioning_nodes[i]);
              const DiscreteVariable& var
                  = dynamic_cast< const DiscreteVariable& >(database.variable(col));
              str << var.name() << "=" << var.labels()[values[i]];
            }
            auto target_col     = node2cols.empty() ? target_node : node2cols.second(target_node);
            const Variable& var = database.variable(target_col);
            str << "> for target node " << var.name()
                << " never appears in the database. Please consider using "
                << "priors such as smoothing.";

            GUM_ERROR(DatabaseError, str.str())
          }
        }

        // normalize the counts and compute, if needed, the log_likelihood
        if (compute_log_likelihood) {
          for (std::size_t j = std::size_t(0), k = std::size_t(0); j < conditioning_domsize; ++j) {
            for (std::size_t i = std::size_t(0); i < target_domsize; ++i, ++k) {
              N_ijk[k] /= N_ij[j];
              if (original_N_ijk[k]) { log_likelihood += original_N_ijk[k] * std::log(N_ijk[k]); }
            }
          }
        } else {
          for (std::size_t j = std::size_t(0), k = std::size_t(0); j < conditioning_domsize; ++j) {
            for (std::size_t i = std::size_t(0); i < target_domsize; ++i, ++k) {
              N_ijk[k] /= N_ij[j];
            }
          }
        }
      } else {
        // here, there are no conditioning nodes. Hence N_ijk is the marginal
        // probability distribution over the target node. To normalize it, it
        // is sufficient to divide each cell by the sum over all the cells
        double sum = 0;
        for (const double n_ijk: N_ijk)
          sum += n_ijk;

        if (sum != 0) {
          if (compute_log_likelihood) {
            for (std::size_t k = std::size_t(0), end = N_ijk.size(); k < end; ++k) {
              N_ijk[k] /= sum;
              if (original_N_ijk[k]) { log_likelihood += original_N_ijk[k] * std::log(N_ijk[k]); }
            }
          } else {
            for (double& n_ijk: N_ijk)
              n_ijk /= sum;
          }
        } else {
          std::stringstream str;

          const auto& node2cols  = this->counter_.nodeId2Columns();
          const auto& database   = this->counter_.database();
          auto        target_col = node2cols.empty() ? target_node : node2cols.second(target_node);
          const Variable& var    = database.variable(target_col);
          str << "No data for target node " << var.name()
              << ". It is impossible to estimate the parameters by maximum "
                 "likelihood";
          GUM_ERROR(DatabaseError, str.str())
        }
      }

      return {std::move(N_ijk), log_likelihood};
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
