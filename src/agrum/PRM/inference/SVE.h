/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of SVE (Structured Variable Elimination).
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SVE_H
#define GUM_SVE_H

#include <vector>

#include <agrum/core/set.h>

#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimSparse.h>

#include <agrum/PRM/inference/PRMInference.h>

#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/instanceBayesNet.h>

namespace gum {
  namespace prm {

    /**
     * @class SVE SVE.h <agrum/PRM/SVE.h>
     * @brief This class is an implementation of the Structured Variable Elimination
     *        algorithm on PRM<GUM_SCALAR>.
     *
     */
    template <typename GUM_SCALAR> class SVE : public PRMInference<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      SVE(const PRM<GUM_SCALAR> &prm, const System<GUM_SCALAR> &system);

      /// Destructor.
      ~SVE();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /// Returns the name of the current inference algorithm
      virtual std::string name() const;

      /// @}
      protected:
      // ========================================================================
      /// @name Query methods.
      // ========================================================================
      /// @{

      /// Code alias.
      typedef typename PRMInference<GUM_SCALAR>::Chain Chain;

      /// See PRMInference<GUM_SCALAR>::_evidenceAdded().
      virtual void _evidenceAdded(const Chain &chain);

      /// See PRMInference<GUM_SCALAR>::_evidenceRemoved().
      virtual void _evidenceRemoved(const Chain &chain);

      /// See PRMInference<GUM_SCALAR>::_marginal().
      virtual void _marginal(const Chain &chain, Potential<GUM_SCALAR> &m);

      /// See PRMInference<GUM_SCALAR>::_joint().
      virtual void _joint(const std::vector<Chain> &queries,
                          Potential<GUM_SCALAR> &j);

      /// @}
      private:
      /// Code alias
      typedef Set<Potential<GUM_SCALAR> *> BucketSet;
      /// Code alias
      typedef typename Set<Potential<GUM_SCALAR> *>::iterator_safe BucketSetIterator;

      /// Code alias
      typedef typename Set<MultiDimArray<GUM_SCALAR> *>::iterator_safe
          ArraySetIterator;

      HashTable<const Class<GUM_SCALAR> *, std::vector<NodeId> *> __elim_orders;

      HashTable<const Class<GUM_SCALAR> *, BucketSet *> __lifted_pools;

      Sequence<const ClassElementContainer<GUM_SCALAR> *> *__class_elim_order;

      HashTable<const Instance<GUM_SCALAR> *, Set<const DiscreteVariable *> *>
          __delayedVariables;

      /// Some variable must be delayed for more than one Instance<GUM_SCALAR>, when
      /// the delayed
      /// variable counter reach 0 it can be eliminated.
      HashTable<std::string, Size> __delayedVariablesCounters;

      BucketSet __lifted_trash;

      /// @name Inference sub methods.

      /// @{

      void __eliminateNodes(const Instance<GUM_SCALAR> *query, NodeId id,
                            BucketSet &pool, BucketSet &trash);

      void __eliminateNodesDownward(const Instance<GUM_SCALAR> *from,
                                    const Instance<GUM_SCALAR> *i, BucketSet &pool,
                                    BucketSet &trash,
                                    List<const Instance<GUM_SCALAR> *> &elim_list,
                                    Set<const Instance<GUM_SCALAR> *> &ignore,
                                    Set<const Instance<GUM_SCALAR> *> &eliminated);

      void __eliminateNodesUpward(const Instance<GUM_SCALAR> *i, BucketSet &pool,
                                  BucketSet &trash,
                                  List<const Instance<GUM_SCALAR> *> &elim_list,
                                  Set<const Instance<GUM_SCALAR> *> &ignore,
                                  Set<const Instance<GUM_SCALAR> *> &eliminated);

      void __eliminateNodesWithEvidence(const Instance<GUM_SCALAR> *i,
                                        BucketSet &pool, BucketSet &trash,
                                        Set<NodeId> *delayedVars = 0);

      void __eliminateDelayedVariables(const Instance<GUM_SCALAR> *i,
                                       BucketSet &pool, BucketSet &trash);

      void __insertLiftedNodes(const Instance<GUM_SCALAR> *i, BucketSet &pool,
                               BucketSet &trash);

      void __variableElimination(const Instance<GUM_SCALAR> *i, BucketSet &pool,
                                 BucketSet &trash, Set<NodeId> *delayedVars = 0);

      /// Returns true if second can be eliminated before first.
      bool __checkElimOrder(const Instance<GUM_SCALAR> *first,
                            const Instance<GUM_SCALAR> *second);

      void __initElimOrder();

      void __insertEvidence(const Instance<GUM_SCALAR> *i, BucketSet &pool);

      /// When there is a loop in the references some variable elimination
      /// must be delayed, this methods add such variable to __delayedVariables
      /// to keep track of them.
      /// @param i An Instance<GUM_SCALAR> with a child of j->get(id).
      /// @param j The Instance<GUM_SCALAR> with the delayed variable.
      /// @param id The NodeId of the delayed variable.
      void __addDelayedVariable(const Instance<GUM_SCALAR> *i,
                                const Instance<GUM_SCALAR> *j, NodeId id);

      std::vector<NodeId> &__getElimOrder(const Class<GUM_SCALAR> &c);

      Potential<GUM_SCALAR> *__getAggPotential(const Instance<GUM_SCALAR> *i,
                                               const Aggregate<GUM_SCALAR> *agg);

      void __initLiftedNodes(const Class<GUM_SCALAR> &c);

      /// @}
    };

    extern template class SVE<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/SVE.tcc>

#endif /* GUM_SVE_H */
