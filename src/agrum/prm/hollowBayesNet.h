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
//  * @brief Headers of HollowBayesNet.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// #include <agrum/BN/BayesNet.h>
// // ============================================================================
// #include <agrum/prm/PRM.h>
// // ============================================================================
// #ifndef GUM_HOLLOW_BAYESNET_H
// #define GUM_HOLLOW_BAYESNET_H
// namespace gum {
// namespace prm {
// 
// /**
//  * @class HollowBayesNet hollowBayesNet.h <agrum/prm/hollowBayesNet.h>
//  * @brief This class decorates a ClassElementContainer has an AbstractBayesNet.
//  *
//  * This class filters Attribute and Aggregate in a way it can be interpreted as
//  * a BayesNet. SlotChains are not represented since their implementations is error
//  * prone when accessing without a global scope: the DiscreteVariable pointer of a
//  * SlotChains is always the Class-level Attribute, thus they differ (as a
//  * pointer) from their equivalents in instantiated input nodes.
//  *
//  * ReferenceSlot are not represented since there exists no such elements in the
//  * BayesNet formalism.
//  *
//  * Remeber that a HollowBayesNet does not contain the parents of input nodes and
//  * the output nodes children. Thus you should be careful when using one of the
//  * BayesNetInference over a HollowBayesNet since some variables are missing in
//  * the DAG but not in the input nodes Potential.
//  */
// class HollowBayesNet: public AbstractBayesNet<prm_float> {
//   public:
//   // ========================================================================
//   /// @name Constructors & destructor.
//   // ========================================================================
//     /// @{
// 
//     /// Default constructor.
//     /// @param cec The ClassElementContainer decorated by this HollowBayesNet.
//     HollowBayesNet(const ClassElementContainer& cec);
// 
//     /// Copy constructor.
//     HollowBayesNet(const HollowBayesNet& from);
// 
//     /// Copy operator.
//     HollowBayesNet& operator=(const HollowBayesNet& from);
// 
//     /// Destructor.
//     virtual ~HollowBayesNet();
// 
//     /// @}
//   // ===========================================================================
//   /// @name Variable manipulation methods.
//   // ===========================================================================
//     /// @{
// 
//     /// See gum::AbstractBayesNet::cpt().
//     /// @warning In some case when varId is an Aggregate and __cec a Class this
//     ///          will raise an OperationNotAllowed since Aggregate's Potential are
//     ///          defined at instance-level.
//     virtual const Potential<prm_float>& cpt( NodeId varId ) const;
// 
//     /// See gum::AbstractBayesNet::dag().
//     virtual const DAG& dag() const;
// 
//     /// See gum::AbstractBayesNet::variableNodeMap().
//     virtual const VariableNodeMap& variableNodeMap() const;
// 
//     /// See gum::AbstractBayesNet::size().
//     virtual Idx size() const;
// 
//     /// See gum::AbstractBayesNet::empty().
//     virtual bool empty() const;
// 
//     /// See gum::AbstractBayesNet::variable().
//     virtual const DiscreteVariable& variable( NodeId id ) const;
// 
//     /// See gum::AbstractBayesNet::nodeId().
//     virtual NodeId nodeId( const DiscreteVariable &var ) const;
// 
//     /// See gum::AbstractBayesNet::idFromName().
//     virtual NodeId idFromName( const std::string& name ) const;
// 
//     /// See gum::AbstractBayesNet::variableFromName().
//     virtual const DiscreteVariable& variableFromName( const std::string& name ) const;
// 
//     const Property<unsigned int>::onNodes& modalities() const;
// 
//     /// @}
//   // ===========================================================================
//   /// @name Graphical methods
//   // ===========================================================================
//     /// @{
// 
//     /// See gum::AbstractBayesNet::moralGraph().
//     virtual const UndiGraph& moralGraph( bool clear = true ) const;
// 
//     /// See gum::AbstractBayesNet::topologicalOrder().
//     virtual const Sequence<NodeId>& topologicalOrder( bool clear = true ) const;
// 
//     /// @return Returns a dot representation of this BayesNet.
//     virtual std::string toDot( void ) const;
// 
//     /// @}
//   private:
//     /// Mapping between DiscreteVariable and their NodeId
//     HashTable<const DiscreteVariable*, NodeId> __varNodeMap;
// 
//     /// Private getter with type checking in case the id is not a formal Attribute.
//     /// @throw NotFound Raised if id is not a formal attribute.
//     const ClassElement& __get(NodeId id) const;
// 
//     /// The ClassElementContainer decorated by this.
//     const ClassElementContainer* __cec;
// 
//     /// The Dag of a HollowBayesNet is a subgraph of __cec->dag() containing only
//     /// Attribute and Aggregate
//     DAG __dag;
// 
//     /// HollowBayesNet are a read-only structure, no need to update the moral graph.
//     mutable UndiGraph* __moralGraph;
// 
//     Property<unsigned int>::onNodes __modalities;
// 
//     /// HollowBayesNet are a read-only structure, no need to update the
//     /// topological order.
//     mutable Sequence<NodeId>* __topo;
// 
//     void __init(const ClassElementContainer& cec);
// };
// 
// } /* namespace prm */
// } /* namespace gum */
// #ifndef GUM_NO_INLINE
// #include <agrum/prm/hollowBayesNet.inl>
// #endif // GUM_NO_INLINE
// // ============================================================================
// #endif /* GUM_HOLLOW_BAYESNET_H */
// // ============================================================================
