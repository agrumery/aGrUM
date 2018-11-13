/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
    INLINE typename DAG2BNLearner2< ALLOC >::allocator_type
      DAG2BNLearner2< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::DAG2BNLearner2(
      const typename DAG2BNLearner2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc) {
      GUM_CONSTRUCTOR(DAG2BNLearner2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::DAG2BNLearner2(
      const DAG2BNLearner2< ALLOC >&                          from,
      const typename DAG2BNLearner2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc) {
      GUM_CONS_CPY(DAG2BNLearner2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::DAG2BNLearner2(const DAG2BNLearner2< ALLOC >& from) :
        DAG2BNLearner2(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::DAG2BNLearner2(
      DAG2BNLearner2< ALLOC >&&                               from,
      const typename DAG2BNLearner2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc) {
      GUM_CONS_MOV(DAG2BNLearner2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::DAG2BNLearner2(DAG2BNLearner2< ALLOC >&& from) :
        DAG2BNLearner2(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >* DAG2BNLearner2< ALLOC >::clone(
      const typename DAG2BNLearner2< ALLOC >::allocator_type& alloc) const {
      ALLOC< DAG2BNLearner2< ALLOC > > allocator(alloc);
      DAG2BNLearner2< ALLOC >*         new_learner = allocator.allocate(1);
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
    DAG2BNLearner2< ALLOC >* DAG2BNLearner2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >::~DAG2BNLearner2() {
      GUM_DESTRUCTOR(DAG2BNLearner2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >& DAG2BNLearner2< ALLOC >::
                             operator=(const DAG2BNLearner2< ALLOC >& from) {
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DAG2BNLearner2< ALLOC >& DAG2BNLearner2< ALLOC >::
                             operator=(DAG2BNLearner2< ALLOC >&& from) {
      return *this;
    }


    /// copy a potential into another whose variables' sequence differs
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    void DAG2BNLearner2< ALLOC >::__probaVarReordering(
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
      DAG2BNLearner2< ALLOC >::createBN(ParamEstimator2< ALLOC >& estimator,
                                        const DAG&                dag) {
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
        Potential< GUM_SCALAR >& pot =
          const_cast< Potential< GUM_SCALAR >& >(bn.cpt(id));

        // get the variables of the CPT of id in the correct order
        const Sequence< const DiscreteVariable* >& vars = pot.variablesSequence();

        // setup the estimation
        std::vector< NodeId > conditioning_ids(vars.size() - 1);
        for (std::size_t i = std::size_t(1); i < vars.size(); ++i) {
          conditioning_ids[i - 1] = varmap.get(*(vars[i]));
        }
        estimator.setParameters(id, conditioning_ids, pot);

        /*
        // create a potential with the appropriate size
        Potential< GUM_SCALAR > ordered_pot;
        ordered_pot.beginMultipleChanges();
        for (const auto var : vars) {
          ordered_pot.add(*var);
        }
        ordered_pot.endMultipleChanges();


        // assign the potential to the BN
        __probaVarReordering(pot, ordered_pot);
        */
      }

      return bn;
    }

  } /* namespace learning */

} /* namespace gum */
