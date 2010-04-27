// /***************************************************************************
//  *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
//  *   {prenom.nom}_at_lip6.fr                                               *
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   This program is distributed in the hope that it will be useful,       *
//  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//  *   GNU General Public License for more details.                          *
//  *                                                                         *
//  *   You should have received a copy of the GNU General Public License     *
//  *   along with this program; if not, write to the                         *
//  *   Free Software Foundation, Inc.,                                       *
//  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
//  ***************************************************************************/
// /**
//  * @file
//  * @brief Headers of SVE (Structured Value Elimination).
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// #include <vector>
// // ============================================================================
// #include <agrum/core/set.h>
// // ============================================================================
// #include <agrum/graphs/defaultTriangulation.h>
// // ============================================================================
// #include <agrum/BN/inference/valueElimination.h>
// // ============================================================================
// #include <agrum/multidim/multiDimBucket.h>
// #include <agrum/multidim/potential.h>
// #include <agrum/multidim/multiDimArray.h>
// // ============================================================================
// #include <agrum/prm/PRMInference.h>
// #include <agrum/prm/hollowBayesNet.h>
// #include <agrum/prm/CDG.h>
// // ============================================================================
// #ifndef GUM_SVE_H
// #define GUM_SVE_H
// namespace gum {
// namespace prm {
// 
// /**
//  * @class SVE SVE.h <agrum/prm/SVE.h>
//  * @brief This class is an implementation of the Structured Value Elimination
//  *        algorithm on PRM.
//  *
//  */
// class SVE: public PRMInference{
//   public:
//   // ========================================================================
//   /// @name Constructors & destructor.
//   // ========================================================================
//     /// @{
// 
//     /// Default Constructor.
//     SVE(const PRM& prm, const Model& model);
// 
//     /// Default Constructor.
//     SVE(const PRM& prm, const std::string& model);
// 
//     /// Destructor.
//     ~SVE();
// 
//     /// @}
//   protected:
//   // ========================================================================
//   /// @name Query methods.
//   // ========================================================================
//     /// @{
// 
//     /// See PRMInference::_evidenceAdded().
//     virtual void _evidenceAdded(const Instance* i, NodeId elt_id);
// 
//     /// See PRMInference::_evidenceRemoved().
//     virtual void _evidenceRemoved(const Instance* i, NodeId elt_id);
// 
//     /// See PRMInference::_marginal().
//     virtual void _marginal(const Instance* i, const Attribute* elt,
//                            Potential<prm_float>& m);
// 
//     /// See PRMInference::_joint().
//     virtual void _joint(const Set<InstAttrPair>& queries, Potential<prm_float>& j);
// 
//     /// @}
//   private:
//     /// Code alias
//     typedef Set< Potential<prm_float>* > BucketSet;
//     /// Code alias
//     typedef Set< Potential<prm_float>* >::iterator BucketSetIterator;
// 
//     /// Code alias
//     typedef Set< MultiDimArray<prm_float>* > ArraySet;
//     /// Code alias
//     typedef Set< MultiDimArray<prm_float>* >::iterator ArraySetIterator;
// 
//     HashTable<const Class*, std::vector<NodeId>*> __elim_orders;
// 
//     HashTable<const Class*, ArraySet*> __lifted_pools;
// 
//     Sequence<const Class*>* __class_elim_order;
// 
//     BucketSet __lifted_trash;
// 
//     // ============================================================================
//     /// @name Inference sub methods.
//     // ============================================================================
//     /// @{
// 
//     void __eliminateNodes(const Instance* query, NodeId id,
//                           BucketSet& pool, BucketSet& trash);
// 
//     void __eliminateNodesDownward(const Instance* from, const Instance* i,
//                                   BucketSet& pool, BucketSet& trash,
//                                   List<const Instance*>& elim_list,
//                                   Set<const Instance*>& ignore);
// 
//     void __eliminateNodesUpward(const Instance* i,
//                                 BucketSet& pool, BucketSet& trash,
//                                 List<const Instance*>& elim_list,
//                                 Set<const Instance*>& ignore);
// 
//     void __eliminatNodesWithEvidence(const Instance* i,
//                                      BucketSet& pool, BucketSet& trash);
// 
//     void __insertLiftedNodes(const Instance* i, BucketSet& pool,
//                              BucketSet& trash);
// 
//     /// Returns true if second can be eliminated before first.
//     bool __checkElimOrder(const Instance* first, const Instance* second);
// 
//     void __initElimOrder();
// 
//     void __insertEvidence(const Instance* i, BucketSet& pool);
// 
//     std::vector<NodeId>& __getElimOrder(const Class& c);
// 
//     Potential<prm_float>* __getAggPotential(const Instance* i,
//                                         const Aggregate* agg);
// 
//     void __initLiftedNodes(const Class& c);
// 
//     /// @}
// };
// 
// } /* namespace prm */
// } /* namespace gum */
// #ifndef GUM_NO_INLINE
// #include <agrum/prm/SVE.inl>
// #endif // GUM_NO_INLINE
// // ============================================================================
// #endif /* GUM_SVE_H */
// // ============================================================================
