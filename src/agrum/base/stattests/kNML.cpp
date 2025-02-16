/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


/**
 * @file
 * @brief Implementation of gum::learning::kNML.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/kNML.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/kNML_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    KNML& KNML::operator=(const KNML& from) {
      if (this != &from) {
        IndependenceTest::operator=(from);
        _param_complexity_ = from._param_complexity_;
      }
      return *this;
    }

    /// move operator
    KNML& KNML::operator=(KNML&& from) {
      if (this != &from) {
        IndependenceTest::operator=(std::move(from));
        _param_complexity_ = std::move(from._param_complexity_);
      }
      return *this;
    }

    /// clears all the data structures from memory, including the cache
    void KNML::clear() {
      IndependenceTest::clear();
      _param_complexity_.clearCache();
    }

    /// clears the current cache
    void KNML::clearCache() {
      IndependenceTest::clearCache();
      _param_complexity_.clearCache();
    }

    /// turn on/off the use of a cache of the previously computed score
    void KNML::useCache(const bool on_off) {
      IndependenceTest::useCache(on_off);
      _param_complexity_.useCache(on_off);
    }

    /// returns the score corresponding to a given nodeset
    double KNML::score_(const IdCondSet& idset) {
      // perform the counts on the database for all the nodes in the idset
      // This will help optimizing the computations of the Nxui, Nyui and Nui
      // that we will be needed subsequently
      this->counter_.counts(idset, true);

      const bool informative_external_prior = this->prior_->isInformative();

      // get the domain sizes of X and Y
      const auto& db        = this->database();
      const auto& node2cols = this->nodeId2Columns();
      std::size_t r_x, r_y;
      if (!node2cols.empty()) {
        r_x = db.domainSize(node2cols.second(idset[0]));
        r_y = db.domainSize(node2cols.second(idset[1]));
      } else {
        r_x = db.domainSize(idset[0]);
        r_y = db.domainSize(idset[1]);
      }


      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // now get the Nxui, Nyui and Nui
        IdCondSet idset_xui = idset;
        idset_xui.erase(idset[1]);
        IdCondSet idset_yui = idset;
        idset_yui.erase(idset[0]);

        std::vector< double > N_ui  = this->counter_.counts(idset.conditionalIdCondSet(), false);
        std::vector< double > N_xui = this->counter_.counts(idset_xui, false);
        std::vector< double > N_yui = this->counter_.counts(idset_yui, false);

        if (informative_external_prior) {
          this->prior_->addConditioningPseudoCount(idset, N_ui);
          this->prior_->addJointPseudoCount(idset, N_xui);
          this->prior_->addJointPseudoCount(idset, N_yui);
        }


        // the value of kNML is equal to:
        // 0.5 * sum_Z ( sum_X( log( C^(r_y)_#ZX ) ) - log( C^(r_y)_#Z ) +
        // 		 sum_Y( log( C^(r_x)_#ZY ) ) - log( C^(r_x)_#Z ) )
        double score = 0.0;
        for (auto n_xui: N_xui)
          score += _param_complexity_.log2Cnr(r_y, n_xui);
        for (auto n_yui: N_yui)
          score += _param_complexity_.log2Cnr(r_x, n_yui);
        for (auto n_ui: N_ui) {
          score -= _param_complexity_.log2Cnr(r_y, n_ui);
          score -= _param_complexity_.log2Cnr(r_x, n_ui);
        }

        score *= 0.5;

        return score;
      } else {
        // here, there is no conditioning set
        // now get the Nxui, Nyui and Nui
        IdCondSet idset_xui(idset[0], this->empty_ids_, true);
        IdCondSet idset_yui(idset[1], this->empty_ids_, true);

        std::vector< double > N_xui = this->counter_.counts(idset_xui, false);
        std::vector< double > N_yui = this->counter_.counts(idset_yui, false);

        if (informative_external_prior) {
          this->prior_->addJointPseudoCount(idset, N_xui);
          this->prior_->addJointPseudoCount(idset, N_yui);
        }


        // so, the formula for kNML is:
        // 0.5 * ( sum_X( log( C^(r_y)_#X ) ) - log( C^(r_y)N_ ) +
        //         sum_Y( log( C^(r_x)_#Y ) ) - log( C^(r_x)N_ ) )
        double N     = 0.0;
        double score = 0.0;
        for (auto n_xui: N_xui) {
          score += _param_complexity_.log2Cnr(r_y, n_xui);
          N += n_xui;
        }
        for (auto n_yui: N_yui)
          score += _param_complexity_.log2Cnr(r_x, n_yui);
        score -= _param_complexity_.log2Cnr(r_y, N);
        score -= _param_complexity_.log2Cnr(r_x, N);

        score *= 0.5;

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
