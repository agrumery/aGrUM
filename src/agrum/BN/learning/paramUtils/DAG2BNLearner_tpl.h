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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  namespace learning {

    /// copy a tensor into another whose variables' sequence differs
    template < typename GUM_SCALAR >
    void DAG2BNLearner::_probaVarReordering_(gum::Tensor< GUM_SCALAR >&       pot,
                                             const gum::Tensor< GUM_SCALAR >& other_pot) {
      // check that the variables are identical
      if (!pot.variablesSequence().diffSet(other_pot.variablesSequence()).empty()) {
        GUM_ERROR(gum::CPTError, "the tensors do not have the same variables")
      }

      // perform the copy
      Instantiation i(other_pot);
      Instantiation j(pot);
      for (i.setFirst(); !i.end(); ++i) {
        j.setVals(i);
        pot.set(j, other_pot[i]);
      }
    }

    /// create a BN
    template < typename GUM_SCALAR >
    INLINE BayesNet< GUM_SCALAR > DAG2BNLearner::createBN(ParamEstimator& estimator,
                                                          const DAG&      dag) {
      return DAG2BNLearner()._createBN_(estimator, dag, false);
    }

    /// create a BN and compute the log-likelihood if needed
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > DAG2BNLearner::_createBN_(ParamEstimator& estimator,
                                                     const DAG&      dag,
                                                     const bool      compute_log_likelihood) {
      BayesNet< GUM_SCALAR > bn;
      log_likelihood_EM_ = 0.0;

      // create a bn with dummy parameters corresponding to the dag
      const auto& node2cols = estimator.nodeId2Columns();
      const auto& database  = estimator.database();
      if (node2cols.empty()) {
        for (const auto id: dag) {
          bn.add(dynamic_cast< const DiscreteVariable& >(database.variable(id)), id);
        }
      } else {
        for (const auto id: dag) {
          const std::size_t col = node2cols.second(id);
          bn.add(dynamic_cast< const DiscreteVariable& >(database.variable(col)), id);
        }
      }

      // add the arcs
      bn.beginTopologyTransformation();
      for (const auto& arc: dag.arcs()) {
        bn.addArc(arc.tail(), arc.head());
      }
      bn.endTopologyTransformation();

      // estimate the parameters
      const VariableNodeMap& varmap = bn.variableNodeMap();
      for (const auto id: dag) {
        // get the CPT of node id and its variables in the correct order
        auto&       pot  = const_cast< Tensor< GUM_SCALAR >& >(bn.cpt(id));
        const auto& vars = pot.variablesSequence();

        // get the conditioning variables: they are all the variables except
        // the last one in pot
        std::vector< NodeId > conditioning_ids(vars.size() - 1);
        for (auto i = std::size_t(1); i < vars.size(); ++i) {
          conditioning_ids[i - 1] = varmap.get(*(vars[i]));
        }

        log_likelihood_EM_
            += estimator.setParameters(id, conditioning_ids, pot, compute_log_likelihood);
      }

      return bn;
    }

    /// create a BN with EM: initialized by an estimator
    template < typename GUM_SCALAR >
    INLINE BayesNet< GUM_SCALAR > DAG2BNLearner::createBNwithEM(ParamEstimator& bootstrap_estimator,
                                                                ParamEstimator& EM_estimator,
                                                                const DAG&      dag) {
      // for EM estimations, we need to disable caches
      bootstrap_estimator.clear();
      EM_estimator.clear();

      // bootstrap EM by learning an initial model
      BayesNet< GUM_SCALAR > bn = createBN< GUM_SCALAR >(bootstrap_estimator, dag);

      return _performEM_(bootstrap_estimator, EM_estimator, std::move(bn));
    }

    /// create a BN with EM: initialized by the parameters of a BN
    template < typename GUM_SCALAR >
    INLINE BayesNet< GUM_SCALAR > DAG2BNLearner::createBNwithEM(ParamEstimator& bootstrap_estimator,
                                                                ParamEstimator& EM_estimator,
                                                                const BayesNet< GUM_SCALAR >& bn) {
      // for EM estimations, we need to disable caches
      bootstrap_estimator.clear();
      EM_estimator.clear();

      auto bn_copy(bn);
      return createBNwithEM(bootstrap_estimator, EM_estimator, std::move(bn_copy));
    }

    /// create a BN with EM: initialized by the parameters of a BN
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > DAG2BNLearner::createBNwithEM(ParamEstimator& bootstrap_estimator,
                                                         ParamEstimator& EM_estimator,
                                                         BayesNet< GUM_SCALAR >&& bn) {
      // estimate the parameters of the fully zeroed CPTs using the bootstrap estimator
      const VariableNodeMap& varmap = bn.variableNodeMap();
      for (const auto id: bn.dag()) {
        // get the CPT of node id and its variables in the correct order
        auto& pot = const_cast< Tensor< GUM_SCALAR >& >(bn.cpt(id));

        // check if the CPT contains only zeroes
        bool all_zeroed = true;
        for (gum::Instantiation inst(pot); !inst.end(); inst.inc()) {
          if (pot[inst] != 0.0) {
            all_zeroed = false;
            break;
          }
        }

        // estimate the initial parameters of pot if all_zeroed
        if (all_zeroed) {
          // get the conditioning variables: they are all the variables except
          // the first one in pot
          const auto&           vars = pot.variablesSequence();
          std::vector< NodeId > conditioning_ids(vars.size() - 1);
          for (auto i = std::size_t(1); i < vars.size(); ++i) {
            conditioning_ids[i - 1] = varmap.get(*(vars[i]));
          }

          // estimate the initial parameters of pot
          bootstrap_estimator.setParameters(id, conditioning_ids, pot, false);
        }
      }

      return _performEM_(bootstrap_estimator, EM_estimator, std::move(bn));
    }

    /// create a BN with EM: initialized by the parameters of a BN
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > DAG2BNLearner::_performEM_(ParamEstimator&          bootstrap_estimator,
                                                      ParamEstimator&          EM_estimator,
                                                      BayesNet< GUM_SCALAR >&& bn) {
      // if there exist no missing value, there is no need to apply EM
      if (!EM_estimator.database().hasMissingValues()) {
        // here we start/stop the approx scheme to be able to display the number
        // of EM iterations
        initApproximationScheme();
        stopApproximationScheme();

        auto bn_copy(bn);
        return bn_copy;
      }

      if (!this->isEnabledMinEpsilonRate() && !this->isEnabledEpsilon() && !this->isEnabledMaxIter()
          && !this->isEnabledMaxTime()) {
        GUM_ERROR(OperationNotAllowed,
                  "EM cannot be executed because no stopping criterion among "
                      << "{min rate, min diff, max iter, max time} has been selected")
      }

      // as bn will be modified, be sure that the DAG is kept unchanged
      const DAG dag = bn.dag();

      // perturb the CPTs to initialize EM
      if (noiseEM_ != 0.0) {
        for (const auto& node: bn.nodes()) {
          bn.cpt(node).noising(noiseEM_).normalizeAsCPT();
        }
      }

      // perform EM
      EM_estimator.setBayesNet(bn);
      initApproximationScheme();

      // compute the initial value of the log-likelihood
      log_likelihood_EM_            = 0.0;
      const VariableNodeMap& varmap = bn.variableNodeMap();
      EM_estimator.counter_.clear();   // for EM estimations, we need to disable caches
      for (const auto& node: bn.nodes()) {
        // get node's CPT and its conditioning variables: they are all the
        // variables except the first one in pot
        const auto&           pot  = const_cast< Tensor< GUM_SCALAR >& >(bn.cpt(node));
        const auto&           vars = pot.variablesSequence();
        std::vector< NodeId > conditioning_ids(vars.size() - 1);
        for (auto i = std::size_t(1); i < vars.size(); ++i) {
          conditioning_ids[i - 1] = varmap.get(*(vars[i]));
        }

        // compute the log-likelihood
        IdCondSet     idset(node, conditioning_ids, true);
        const auto&   N_ijk = EM_estimator.counter_.counts(idset, true);
        Instantiation inst(pot);
        for (std::size_t k = 0, end = pot.domainSize(); k < end; ++k, inst.inc()) {
          if (N_ijk[k]) { log_likelihood_EM_ += N_ijk[k] * std::log(pot[inst]); }
        }
      }
      double current_log_likelihood = log_likelihood_EM_;

      // it may happen (luckily very seldom) that EM will decrease the
      // log-likelihood instead of increasing it (see Table 5 on p28 of
      // https://faculty.washington.edu/fxia/courses/LING572/EM_collins97.pdf
      // for an example of such a behavior). In this case, instead of iterating
      // EM and producing worst and worst Bayes nets, we stop the iterations
      // early and we return the best Bayes net found so far.
      BayesNet< GUM_SCALAR > best_bn;
      bool                   must_return_best_bn    = false;
      unsigned int           nb_dec_likelihood_iter = 0;
      double                 delta                  = 0;

      do {
        // bugfix for parallel execution of VariableElimination
        const auto& xdag = bn.dag();
        for (const auto node: xdag) {
          xdag.parents(node);
          xdag.children(node);
        }

        EM_estimator.counter_.clear();   // for EM estimations, we need to disable caches
        BayesNet< GUM_SCALAR > new_bn = _createBN_< GUM_SCALAR >(EM_estimator, dag, true);
        updateApproximationScheme();

        if (log_likelihood_EM_ >= current_log_likelihood) {
          // here, we increased the log-likelihood, it is fine
          nb_dec_likelihood_iter = 0;
          must_return_best_bn    = false;
        } else {
          // here, we decreased the log-likelihood, so we should keep track of the
          // best Bayes net found so far. If we decreased too many times the
          // log-likelihood, we should even stop EM
          ++nb_dec_likelihood_iter;
          if (nb_dec_likelihood_iter == 1) {
            best_bn             = bn;   // bn is the Bayes net computed at the previous step
            must_return_best_bn = true;
          }
          if (nb_dec_likelihood_iter > max_nb_dec_likelihood_iter_) {
            stopApproximationScheme();
            return best_bn;
          }
        }

        // compute the difference in log-likelihood
        delta                  = log_likelihood_EM_ - current_log_likelihood;
        current_log_likelihood = log_likelihood_EM_;

        bn = std::move(new_bn);
      } while (continueApproximationScheme(this->isEnabledMinEpsilonRate() ? -log_likelihood_EM_
                                                                           : delta));

      stopApproximationScheme();   // just to be sure of the approximationScheme
                                   // has been notified of the end of loop

      return must_return_best_bn ? best_bn : bn;
    }

  }   // namespace learning

} /* namespace gum */
