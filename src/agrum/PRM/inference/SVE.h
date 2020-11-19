/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Headers of SVE (Structured Variable Elimination).
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SVE_H
#define GUM_SVE_H

#include <vector>

#include <agrum/tools/core/set.h>

#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>
#include <agrum/tools/multidim/potential.h>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/inference/PRMInference.h>

#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/instanceBayesNet.h>

namespace gum {
  namespace prm {

    /**
     * @class SVE
     * @headerfile SVE.h <agrum/PRM/SVE.h>
     * @brief This class is an implementation of the Structured Variable
     *Elimination
     *        algorithm on PRM<GUM_SCALAR>.
     *
     */
    template < typename GUM_SCALAR >
    class SVE: public PRMInference< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      SVE(const PRM< GUM_SCALAR >& prm, const PRMSystem< GUM_SCALAR >& system);

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
      typedef typename PRMInference< GUM_SCALAR >::Chain Chain;

      /// See PRMInference<GUM_SCALAR>::evidenceAdded_().
      virtual void evidenceAdded_(const Chain& chain);

      /// See PRMInference<GUM_SCALAR>::evidenceRemoved_().
      virtual void evidenceRemoved_(const Chain& chain);

      /// See PRMInference<GUM_SCALAR>::posterior_().
      virtual void posterior_(const Chain& chain, Potential< GUM_SCALAR >& m);

      /// See PRMInference<GUM_SCALAR>::joint_().
      virtual void joint_(const std::vector< Chain >& queries,
                          Potential< GUM_SCALAR >&    j);

      /// @}
      private:
      /// Code alias
      typedef Set< Potential< GUM_SCALAR >* > BucketSet;
      /// Code alias
      typedef
         typename Set< Potential< GUM_SCALAR >* >::iterator_safe BucketSetIterator;

      /// Code alias
      typedef typename Set< MultiDimArray< GUM_SCALAR >* >::iterator_safe
         ArraySetIterator;

      HashTable< const PRMClass< GUM_SCALAR >*, std::vector< NodeId >* >
         elim_orders__;

      HashTable< const PRMClass< GUM_SCALAR >*, BucketSet* > lifted_pools__;

      Sequence< std::string >* class_elim_order__;

      HashTable< const PRMInstance< GUM_SCALAR >*,
                 Set< const DiscreteVariable* >* >
         delayedVariables__;

      /// Some variable must be delayed for more than one
      /// PRMInstance<GUM_SCALAR>,
      /// when
      /// the delayed
      /// variable counter reach 0 it can be eliminated.
      HashTable< std::string, Size > delayedVariablesCounters__;

      BucketSet lifted_trash__;

      /// @name Inference sub methods.

      /// @{

      void eliminateNodes__(const PRMInstance< GUM_SCALAR >* query,
                            NodeId                           id,
                            BucketSet&                       pool,
                            BucketSet&                       trash);

      void eliminateNodesDownward__(
         const PRMInstance< GUM_SCALAR >*          from,
         const PRMInstance< GUM_SCALAR >*          i,
         BucketSet&                                pool,
         BucketSet&                                trash,
         List< const PRMInstance< GUM_SCALAR >* >& elim_list,
         Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
         Set< const PRMInstance< GUM_SCALAR >* >&  eliminated);

      void eliminateNodesUpward__(
         const PRMInstance< GUM_SCALAR >*          i,
         BucketSet&                                pool,
         BucketSet&                                trash,
         List< const PRMInstance< GUM_SCALAR >* >& elim_list,
         Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
         Set< const PRMInstance< GUM_SCALAR >* >&  eliminated);

      void eliminateNodesWithEvidence__(const PRMInstance< GUM_SCALAR >* i,
                                        BucketSet&                       pool,
                                        BucketSet&                       trash,
                                        Set< NodeId >* delayedVars = 0);

      void eliminateDelayedVariables__(const PRMInstance< GUM_SCALAR >* i,
                                       BucketSet&                       pool,
                                       BucketSet&                       trash);

      void insertLiftedNodes__(const PRMInstance< GUM_SCALAR >* i,
                               BucketSet&                       pool,
                               BucketSet&                       trash);

      void variableElimination__(const PRMInstance< GUM_SCALAR >* i,
                                 BucketSet&                       pool,
                                 BucketSet&                       trash,
                                 Set< NodeId >*                   delayedVars = 0);

      /// Returns true if second can be eliminated before first.
      bool checkElimOrder__(const PRMInstance< GUM_SCALAR >* first,
                            const PRMInstance< GUM_SCALAR >* second);

      void initElimOrder__();

      void insertEvidence__(const PRMInstance< GUM_SCALAR >* i, BucketSet& pool);

      /// When there is a loop in the references some variable elimination
      /// must be delayed, this methods add such variable to delayedVariables__
      /// to keep track of them.
      /// @param i An PRMInstance<GUM_SCALAR> with a child of j->get(id).
      /// @param j The PRMInstance<GUM_SCALAR> with the delayed variable.
      /// @param id The NodeId of the delayed variable.
      void addDelayedVariable__(const PRMInstance< GUM_SCALAR >* i,
                                const PRMInstance< GUM_SCALAR >* j,
                                NodeId                           id);

      std::vector< NodeId >& getElimOrder__(const PRMClass< GUM_SCALAR >& c);

      Potential< GUM_SCALAR >*
         getAggPotential__(const PRMInstance< GUM_SCALAR >*  i,
                           const PRMAggregate< GUM_SCALAR >* agg);

      void        initLiftedNodes__(const PRMClass< GUM_SCALAR >& c);
      std::string trim__(const std::string& s);

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class SVE< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/SVE_tpl.h>

#endif /* GUM_SVE_H */
