
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Headers of SVED (Structured Value Elimination with d-seperation).
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SVED_H
#define GUM_SVED_H

#include <vector>

#include <agrum/tools/core/set.h>

#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/potential.h>

#include <agrum/PRM/inference/PRMInference.h>
#include <agrum/PRM/inference/structuredBayesBall.h>

#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/classDependencyGraph.h>
#include <agrum/PRM/instanceBayesNet.h>
namespace gum {
  namespace prm {

    /**
     * @class SVED
     * @headerfile SVED.h <agrum/PRM/SVED.h>
     * @brief This class is an implementation of the Structured Value
     *Elimination
     *        algorithm on PRM<GUM_SCALAR>.
     *
     */
    template < typename GUM_SCALAR >
    class SVED: public PRMInference< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      SVED(const PRM< GUM_SCALAR >& prm, const PRMSystem< GUM_SCALAR >& model);

      /// Destructor.
      ~SVED();

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

      /// See PRMInference::evidenceAdded_().
      virtual void evidenceAdded_(const Chain& chain);

      /// See PRMInference::evidenceRemoved_().
      virtual void evidenceRemoved_(const Chain& chain);

      /// See PRMInference::posterior_().
      virtual void posterior_(const Chain& chain, Potential< GUM_SCALAR >& m);

      /// See PRMInference::joint_().
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

      /// The Set<NodeId> returned by StructuredBayesBall<GUM_SCALAR> is unique
      /// for
      /// each
      /// family of instances with the same requisite set (thus the same lifted
      /// potentials).
      HashTable< const Set< NodeId >*, BucketSet* > lifted_pools__;

      Sequence< std::string >* class_elim_order__;

      StructuredBayesBall< GUM_SCALAR > bb__;

      /// First pair  -> requisite Attributes
      /// Second pair -> requisite SlotChains
      HashTable< const Set< NodeId >*,
                 std::pair< Set< NodeId >*, Set< NodeId >* > >
         req_set__;

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
         Set< const PRMInstance< GUM_SCALAR >* >&  ignore);

      void eliminateNodesUpward__(
         const PRMInstance< GUM_SCALAR >*          i,
         BucketSet&                                pool,
         BucketSet&                                trash,
         List< const PRMInstance< GUM_SCALAR >* >& elim_list,
         Set< const PRMInstance< GUM_SCALAR >* >&  ignore);

      void eliminateNodesWithEvidence__(const PRMInstance< GUM_SCALAR >* i,
                                        BucketSet&                       pool,
                                        BucketSet&                       trash);

      void insertLiftedNodes__(const PRMInstance< GUM_SCALAR >* i,
                               BucketSet&                       pool,
                               BucketSet&                       trash);

      /// Returns true if second can be eliminated before first.
      bool checkElimOrder__(const PRMInstance< GUM_SCALAR >* first,
                            const PRMInstance< GUM_SCALAR >* second);

      void initElimOrder__();

      void insertEvidence__(const PRMInstance< GUM_SCALAR >* i, BucketSet& pool);

      std::vector< NodeId >& getElimOrder__(const PRMClass< GUM_SCALAR >& c);

      Potential< GUM_SCALAR >*
         getAggPotential__(const PRMInstance< GUM_SCALAR >*  i,
                           const PRMAggregate< GUM_SCALAR >* agg);

      void initLiftedNodes__(const PRMInstance< GUM_SCALAR >* i, BucketSet& trash);

      void initReqSets__(const PRMInstance< GUM_SCALAR >* i);

      Set< NodeId >& getAttrSet__(const PRMInstance< GUM_SCALAR >* i);

      Set< NodeId >& getSCSet__(const PRMInstance< GUM_SCALAR >* i);

      void reduceElimList__(const PRMInstance< GUM_SCALAR >*          i,
                            List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                            List< const PRMInstance< GUM_SCALAR >* >& reduced_list,
                            Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
                            BucketSet&                                pool,
                            BucketSet&                                trash);

      std::string trim__(const std::string& s);
      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class SVED< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/SVED_tpl.h>

#endif /* GUM_SVED_H */
