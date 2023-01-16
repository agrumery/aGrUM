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


/**
 * @file
 * @brief Headers of SVED (Structured Value Elimination with d-seperation).
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_SVED_H
#define GUM_SVED_H

#include <vector>


#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/BN/inference/variableElimination.h>


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
      using Chain = typename PRMInference< GUM_SCALAR >::Chain;

      /// See PRMInference::evidenceAdded_().
      virtual void evidenceAdded_(const Chain& chain);

      /// See PRMInference::evidenceRemoved_().
      virtual void evidenceRemoved_(const Chain& chain);

      /// See PRMInference::posterior_().
      virtual void posterior_(const Chain& chain, Potential< GUM_SCALAR >& m);

      /// See PRMInference::joint_().
      virtual void joint_(const std::vector< Chain >& queries, Potential< GUM_SCALAR >& j);

      /// @}
      private:
      /// Code alias
      using BucketSet         = Set< Potential< GUM_SCALAR >* >;
      using BucketSetIterator = typename Set< Potential< GUM_SCALAR >* >::iterator_safe;
      using ArraySetIterator  = typename Set< MultiDimArray< GUM_SCALAR >* >::iterator_safe;

      HashTable< const PRMClass< GUM_SCALAR >*, std::vector< NodeId >* > _elim_orders_;

      /// The Set<NodeId> returned by StructuredBayesBall<GUM_SCALAR> is unique
      /// for
      /// each
      /// family of instances with the same requisite set (thus the same lifted
      /// potentials).
      HashTable< const Set< NodeId >*, BucketSet* > _lifted_pools_;

      Sequence< std::string >* _class_elim_order_;

      StructuredBayesBall< GUM_SCALAR > _bb_;

      /// First pair  -> requisite Attributes
      /// Second pair -> requisite SlotChains
      HashTable< const Set< NodeId >*, std::pair< Set< NodeId >*, Set< NodeId >* > > _req_set_;

      /// @name Inference sub methods.

      /// @{

      void _eliminateNodes_(const PRMInstance< GUM_SCALAR >* query,
                            NodeId                           id,
                            BucketSet&                       pool,
                            BucketSet&                       trash);

      void _eliminateNodesDownward_(const PRMInstance< GUM_SCALAR >*          from,
                                    const PRMInstance< GUM_SCALAR >*          i,
                                    BucketSet&                                pool,
                                    BucketSet&                                trash,
                                    List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                                    Set< const PRMInstance< GUM_SCALAR >* >&  ignore);

      void _eliminateNodesUpward_(const PRMInstance< GUM_SCALAR >*          i,
                                  BucketSet&                                pool,
                                  BucketSet&                                trash,
                                  List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                                  Set< const PRMInstance< GUM_SCALAR >* >&  ignore);

      void _eliminateNodesWithEvidence_(const PRMInstance< GUM_SCALAR >* i,
                                        BucketSet&                       pool,
                                        BucketSet&                       trash);

      void
         _insertLiftedNodes_(const PRMInstance< GUM_SCALAR >* i, BucketSet& pool, BucketSet& trash);

      /// Returns true if second can be eliminated before first.
      bool _checkElimOrder_(const PRMInstance< GUM_SCALAR >* first,
                            const PRMInstance< GUM_SCALAR >* second);

      void _initElimOrder_();

      void _insertEvidence_(const PRMInstance< GUM_SCALAR >* i, BucketSet& pool);

      std::vector< NodeId >& _getElimOrder_(const PRMClass< GUM_SCALAR >& c);

      Potential< GUM_SCALAR >* _getAggPotential_(const PRMInstance< GUM_SCALAR >*  i,
                                                 const PRMAggregate< GUM_SCALAR >* agg);

      void _initLiftedNodes_(const PRMInstance< GUM_SCALAR >* i, BucketSet& trash);

      void _initReqSets_(const PRMInstance< GUM_SCALAR >* i);

      Set< NodeId >& _getAttrSet_(const PRMInstance< GUM_SCALAR >* i);

      Set< NodeId >& _getSCSet_(const PRMInstance< GUM_SCALAR >* i);

      void _reduceElimList_(const PRMInstance< GUM_SCALAR >*          i,
                            List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                            List< const PRMInstance< GUM_SCALAR >* >& reduced_list,
                            Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
                            BucketSet&                                pool,
                            BucketSet&                                trash);

      std::string _trim_(const std::string& s);
      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class SVED< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/SVED_tpl.h>

#endif /* GUM_SVED_H */
