
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <algorithm>
#include <string>
#include <vector>

namespace gum {

  namespace learning {

    /// returns the allocator used by the score
    template < template < typename > class ALLOC >
    INLINE typename DAG2BNLearner< ALLOC >::allocator_type
       DAG2BNLearner< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::DAG2BNLearner(
       const typename DAG2BNLearner< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc) {
      GUM_CONSTRUCTOR(DAG2BNLearner);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::DAG2BNLearner(
       const DAG2BNLearner< ALLOC >&                          from,
       const typename DAG2BNLearner< ALLOC >::allocator_type& alloc) :
        ApproximationScheme(from),
        ALLOC< NodeId >(alloc) {
      GUM_CONS_CPY(DAG2BNLearner);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::DAG2BNLearner(const DAG2BNLearner< ALLOC >& from) :
        DAG2BNLearner(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::DAG2BNLearner(
       DAG2BNLearner< ALLOC >&&                               from,
       const typename DAG2BNLearner< ALLOC >::allocator_type& alloc) :
        ApproximationScheme(std::move(from)),
        ALLOC< NodeId >(alloc) {
      GUM_CONS_MOV(DAG2BNLearner);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::DAG2BNLearner(DAG2BNLearner< ALLOC >&& from) :
        DAG2BNLearner(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >* DAG2BNLearner< ALLOC >::clone(
       const typename DAG2BNLearner< ALLOC >::allocator_type& alloc) const {
      ALLOC< DAG2BNLearner< ALLOC > > allocator(alloc);
      DAG2BNLearner< ALLOC >*         new_learner = allocator.allocate(1);
      try {
        allocator.construct(new_learner, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_learner, 1);
        throw;
      }

      return new_learner;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >* DAG2BNLearner< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >::~DAG2BNLearner() {
      GUM_DESTRUCTOR(DAG2BNLearner);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >& DAG2BNLearner< ALLOC >::
                            operator=(const DAG2BNLearner< ALLOC >& from) {
      ApproximationScheme::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DAG2BNLearner< ALLOC >& DAG2BNLearner< ALLOC >::
                            operator=(DAG2BNLearner< ALLOC >&& from) {
      ApproximationScheme::operator=(std::move(from));
      return *this;
    }


    /// copy a potential into another whose variables' sequence differs
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    void DAG2BNLearner< ALLOC >::__probaVarReordering(
       gum::Potential< GUM_SCALAR >&       pot,
       const gum::Potential< GUM_SCALAR >& other_pot) {
      // check that the variables are identical
      if (!pot.variablesSequence()
              .diffSet(other_pot.variablesSequence())
              .empty()) {
        GUM_ERROR(gum::CPTError, "the potentials do not have the same variables");
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
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR >
       DAG2BNLearner< ALLOC >::createBN(ParamEstimator< ALLOC >& estimator,
                                        const DAG&               dag) {
      BayesNet< GUM_SCALAR > bn;

      // create a bn with dummy parameters corresponding to the dag
      const auto& node2cols = estimator.nodeId2Columns();
      const auto& database = estimator.database();
      if (node2cols.empty()) {
        for (const auto id : dag) {
          bn.add(dynamic_cast< const DiscreteVariable& >(database.variable(id)),
                 id);
        }
      } else {
        for (const auto id : dag) {
          const std::size_t col = node2cols.second(id);
          bn.add(dynamic_cast< const DiscreteVariable& >(database.variable(col)),
                 id);
        }
      }

      // add the arcs
      bn.beginTopologyTransformation();
      for (const auto& arc : dag.arcs()) {
        bn.addArc(arc.tail(), arc.head());
      }
      bn.endTopologyTransformation();

      // estimate the parameters
      const VariableNodeMap& varmap = bn.variableNodeMap();
      for (const auto id : dag) {
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
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > DAG2BNLearner< ALLOC >::createBN(
       ParamEstimator< ALLOC >& bootstrap_estimator,
       ParamEstimator< ALLOC >& general_estimator,
       const DAG&               dag) {
      // bootstrap EM by learning an initial model
      BayesNet< GUM_SCALAR > bn = createBN< GUM_SCALAR >(bootstrap_estimator, dag);
      for (const auto& nod : bn.nodes()) {
        bn.cpt(nod).noising(0.1);
      }
      general_estimator.setBayesNet(bn);

      // perform EM
      initApproximationScheme();

      GUM_SCALAR delta;
      do {
        // bugfix for parallel execution of VariableElimination
        const auto& xdag = bn.dag();
        for (const auto node : xdag) {
          xdag.parents(node);
          xdag.children(node);
        }

        BayesNet< GUM_SCALAR > new_bn =
           createBN< GUM_SCALAR >(general_estimator, dag);
        updateApproximationScheme();

        delta = GUM_SCALAR(0.0);
        for (const auto node : dag) {
          const auto& old_cpt = bn.cpt(node);
          const auto& new_cpt = new_bn.cpt(node);

          Instantiation old_inst(old_cpt);
          Instantiation new_inst(new_cpt);

          for (; !old_inst.end(); ++old_inst, ++new_inst) {
            const GUM_SCALAR old_val = old_cpt.get(old_inst);
            if (old_val > 0.0) {
              const GUM_SCALAR new_val = new_cpt.get(new_inst);
              const GUM_SCALAR diff = new_val - old_val;
              const auto       diffrel =
                 (diff < 0.0) ? (-diff / old_val) : (diff / old_val);
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


    /// returns the approximation policy of the learning algorithm
    template < template < typename > class ALLOC >
    INLINE ApproximationScheme& DAG2BNLearner< ALLOC >::approximationScheme() {
      return *this;
    }


  }   // namespace learning

} /* namespace gum */
