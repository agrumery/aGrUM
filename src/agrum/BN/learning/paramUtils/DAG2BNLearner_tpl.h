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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  namespace learning {

    /// copy a potential into another whose variables' sequence differs
    template < typename GUM_SCALAR >
    void DAG2BNLearner::_probaVarReordering_(gum::Potential< GUM_SCALAR >&       pot,
                                             const gum::Potential< GUM_SCALAR >& other_pot) {
      // check that the variables are identical
      if (!pot.variablesSequence().diffSet(other_pot.variablesSequence()).empty()) {
        GUM_ERROR(gum::CPTError, "the potentials do not have the same variables")
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
    BayesNet< GUM_SCALAR > DAG2BNLearner::createBN(ParamEstimator& estimator, const DAG& dag) {
      BayesNet< GUM_SCALAR > bn;

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
        // get the sequence of variables and make the targets be the last
        auto& pot = const_cast< Potential< GUM_SCALAR >& >(bn.cpt(id));

        // get the variables of the CPT of id in the correct order
        const Sequence< const DiscreteVariable* >& vars = pot.variablesSequence();

        // setup the estimation
        std::vector< NodeId > conditioning_ids(vars.size() - 1);
        for (auto i = std::size_t(1); i < vars.size(); ++i) {
          conditioning_ids[i - 1] = varmap.get(*(vars[i]));
        }
        estimator.setParameters(id, conditioning_ids, pot);
      }

      return bn;
    }

    /// create a BN
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > DAG2BNLearner::createBN(ParamEstimator& bootstrap_estimator,
                                                   ParamEstimator& general_estimator,
                                                   const DAG&      dag) {
      // bootstrap EM by learning an initial model
      BayesNet< GUM_SCALAR > bn = createBN< GUM_SCALAR >(bootstrap_estimator, dag);
      for (const auto& nod: bn.nodes()) {
        bn.cpt(nod).noising(0.1);
      }
      general_estimator.setBayesNet(bn);

      // perform EM
      initApproximationScheme();

      GUM_SCALAR delta;
      do {
        // bugfix for parallel execution of VariableElimination
        const auto& xdag = bn.dag();
        for (const auto node: xdag) {
          xdag.parents(node);
          xdag.children(node);
        }

        BayesNet< GUM_SCALAR > new_bn = createBN< GUM_SCALAR >(general_estimator, dag);
        updateApproximationScheme();

        delta = GUM_SCALAR(0.0);
        for (const auto node: dag) {
          const auto& old_cpt = bn.cpt(node);
          const auto& new_cpt = new_bn.cpt(node);

          Instantiation old_inst(old_cpt);
          Instantiation new_inst(new_cpt);

          for (; !old_inst.end(); ++old_inst, ++new_inst) {
            const GUM_SCALAR old_val = old_cpt.get(old_inst);
            if (old_val > 0.0) {
              const GUM_SCALAR new_val = new_cpt.get(new_inst);
              const GUM_SCALAR diff    = new_val - old_val;
              const auto       diffrel = (diff < 0.0) ? (-diff / old_val) : (diff / old_val);
              if (delta < diffrel) delta = diffrel;
            }
          }
        }

        bn = std::move(new_bn);
      } while (continueApproximationScheme(double(delta)));

      stopApproximationScheme();   // just to be sure of the approximationScheme
                                   // has been notified of the end of loop

      return bn;
    }   // namespace learning


  }   // namespace learning

} /* namespace gum */
