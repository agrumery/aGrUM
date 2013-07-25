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
 * @brief Headers of ClassBayesNet.
 *
 * @author Lionel TORTI
 */

#include <list>

#include <agrum/BN/iBaseBayesNet.h>

#include <agrum/prm/PRM.h>

#ifndef GUM_CLASS_BAYESNET_H
#define GUM_CLASS_BAYESNET_H
namespace gum {
  namespace prm {

    /**
     * @class ClassBayesNet classBayesNet.h <agrum/prm/classBayesNet.h>
     * @brief This class decorates a gum::prm::Class has an IBaseBayesNet.
     *
     * This class filters Attribute and Aggregate in a way it can be interpreted as
     * a BayesNet. SlotChains and ReferenceSlot are not represented.
     *
     * Remember that a ClassBayesNet does not contain input nodes parents and
     * output nodes children. Thus you should be careful when using one of the
     * BayesNetInference over a ClassBayesNet since some variables are missing in
     * the DAG but not in the nodes CPT.
     */
    class ClassBayesNet: public IBaseBayesNet<prm_float> {
      public:
        // ========================================================================
        /// @name Constructors & destructor.
        // ========================================================================
        /// @{

        /// Default constructor.
        /// @param c The Class decorated by this ClassBayesNet.
        ClassBayesNet( const Class& c );

        /// Copy constructor.
        ClassBayesNet( const ClassBayesNet& from );

        /// Copy operator.
        ClassBayesNet& operator= ( const ClassBayesNet& from );

        /// Destructor.
        virtual ~ClassBayesNet();

        /// @}
        // ===========================================================================
        /// @name Variable manipulation methods.
        // ===========================================================================
        /// @{

        /**
         * @brief Returns the CPT of a node in this ClassBayesNet.
         *
         * If the node is an Aggregate, this will raise an OperationNotAllowed
         * since Aggregate's CPT are define at instantiation only.
         *
         * @param varId a variable id.
         * @return the Potential of varId.
         *
         * @throw NotFound raised if varId does not match any variable in this
         *                 BayesNet.
         * @throw OperationNotAllowed raised if varId is an Aggregate.
         */
        virtual const Potential<prm_float>& cpt( NodeId varId ) const;

        /// See gum::IBaseBayesNet::dag().
        virtual const DAG& dag() const;

        /// See gum::IBaseBayesNet::variableNodeMap().
        virtual const VariableNodeMap& variableNodeMap() const;

        /// See gum::IBaseBayesNet::size().
        virtual Idx size() const;

        /// See gum::IBaseBayesNet::empty().
        virtual bool empty() const;

        /// See gum::IBaseBayesNet::variable().
        virtual const DiscreteVariable& variable( NodeId id ) const;

        /// See gum::IBaseBayesNet::nodeId().
        virtual NodeId nodeId( const DiscreteVariable& var ) const;

        /// See gum::IBaseBayesNet::idFromName().
        virtual NodeId idFromName( const std::string& name ) const;

        /// See gum::IBaseBayesNet::variableFromName().
        virtual const DiscreteVariable& variableFromName( const std::string& name ) const;

        /// See gum::IBaseBayesNet::modalities().
        const Property<unsigned int>::onNodes& modalities() const;

        /// @}
        // ===========================================================================
        /// @name Graphical methods
        // ===========================================================================
        /// @{

        /// See gum::IBaseBayesNet::moralGraph().
        virtual const UndiGraph& moralGraph( bool clear = true ) const;

        /// See gum::IBaseBayesNet::topologicalOrder().
        virtual const Sequence<NodeId>& topologicalOrder( bool clear = true ) const;

        /// @return Returns a dot representation of this BayesNet.
        virtual std::string toDot() const;

        /// @}
      private:
        /// Mapping between DiscreteVariable and their NodeId
        HashTable<const DiscreteVariable*, const ClassElement*> __varNodeMap;

        /// Private getter with type checking in case the id is not a formal Attribute.
        /// @throw NotFound Raised if id is not a formal attribute.
        const ClassElement& __get( NodeId id ) const;

        /// Private getter with type checking in case the id is not a formal Attribute.
        /// @throw NotFound Raised if id is not a formal attribute.
        const ClassElement& __get( const std::string& name ) const;

        /// The ClassElementContainer decorated by this.
        const Class* __class;

        /// The Dag of a ClassBayesNet is a subgraph of __class->dag() containing only
        /// Attribute and Aggregate.
        DAG __dag;

        /// ClassBayesNet are a read-only structure, no need to update the moral graph.
        mutable UndiGraph* __moralGraph;

        mutable Property<unsigned int>::onNodes __modalities;

        /// ClassBayesNet are a read-only structure, no need to update the
        /// topological order.
        mutable Sequence<NodeId>* __topo;

        void __init( const Class& c );
    };

  } /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/classBayesNet.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_CLASS_BAYESNET_H */


