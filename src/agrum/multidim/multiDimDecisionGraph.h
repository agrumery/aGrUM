
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
 * @brief Headers of gum::MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_H
#define GUM_MULTI_DIM_DECISION_GRAPH_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/graphs/graphElements.h>
// ============================================================================
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecisionGraphManager.h>
// ============================================================================

namespace gum {

  template<typename GUM_SCALAR>
  class MultiDimDecisionGraphManager;
  /**
   * @class MultiDimDecisionGraph multiDimDecisionGraph.h <agrum/multidim/multiDimDecisionGraph.h>
   *
   * @brief Class implementingting a decision graph
   * For description of general methods, please refer to multidimdecisiondiagrambase
   * For specific implementation and use of multidimdecisiondiagram this is the right place :
   * The idea is that approximation policy is given as a template class to this class ( please
   * refer to agrum/core/approximationPolicy.h for more details on such classes ). MultiDimDecisionDiagram inherits from
   * this class.
   * A default classe exists : Exact Policy whixh commit no approximation on value given. As a default
   * you just have to declare MultiDimDecisionDiagram<GUM_SCALAR> and play with.
   * If you want to use a different approximation pattern, just MultiDimDecisionDiagram<GUM_SCALAR, CLASS_DESCRIBING_NEW_PATTERN>.
   * Therefor, if do operation on MultiDimDecisionDiagram the return type will likely be MultiDimDecisionDiagramBase
   *
   * @ingroup multidim_group
   */
  template<typename GUM_SCALAR>
  class MultiDimDecisionGraph : public MultiDimImplementation< GUM_SCALAR > {

    template<GUM_SCALAR> friend class MultiDimDecisionGraphManager;

    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
        // ============================================================================
        /// Default constructor.
        // ============================================================================
        MultiDimDecisionGraph();

        // ============================================================================
        /// Copy constructor.
        // ============================================================================
        MultiDimDecisionGraph( const MultiDimDecisionGraph<GUM_SCALAR>& from );

        // ============================================================================
        /// Copy Operator.
        // ============================================================================
        MultiDimDecisionGraph<GUM_SCALAR>& operator=(const MultiDimDecisionGraph<GUM_SCALAR>& from);

        // ============================================================================
        /// Destructor.
        // ============================================================================
        ~MultiDimDecisionGraph();

      /// @}

        // ============================================================================
        /**
        * This method creates a clone of this object, withouth its content
        * (including variable), you must use this method if you want to ensure
        * that the generated object has the same type than the object containing
        * the called newFactory()
        * For example :
        *   MultiDimArray<double> y;
        *   MultiDimContainer<double>* x = y.newFactory();
        * Then x is a MultiDimArray<double>*
        *
        * @warning you must desallocate by yourself the memory
        * @return an empty clone of this object with the same type
        */
        // ============================================================================
        MultiDimContainer<GUM_SCALAR>* newFactory() const;


      // ############################################################################
      /// @name Accessors / Modifiers herited from gum::MultiDimImplementation
      // ############################################################################
      /// @{

        // ============================================================================
        /// see gum::MultiDimImplementation
        // ============================================================================
        const std::string& name() const;

        // ============================================================================
        /**
        * see gum::MultiDimImplementation::set ( const Instantiation &i, const GUM_SCALAR &value )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void set ( const Instantiation &i, const GUM_SCALAR &value ) const;

        // ============================================================================
        /**
        * see gum::MultiDimImplementation::fill( const GUM_SCALAR &d )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void fill( const GUM_SCALAR &d ) const;

        // ==============================================================================
        /**
        * see gum::MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
        */
        // ==============================================================================
        void fillWith ( const std::vector<GUM_SCALAR>& v ) const;

      /// @}


      // ############################################################################
      /// @name Implementation of MultiDimInterface
      // ############################################################################
      /// @{

        // ============================================================================
        /// See gum::MultiDimInterface::erase(const DiscreteVariable& v)
        // ============================================================================
        void erase( const DiscreteVariable& v );

        // ============================================================================
        /// see gum::MultiDimImplementation::realSize()
        // ============================================================================
        Size realSize() const;

      /// @}

      // ############################################################################
      /// @name Slave management and extension due to slave management
      // ############################################################################
      /// @{
        // ============================================================================
        /** see gum::MultiDimImplementation::changeNotification( Instantiation& i,
        *                                                       const DiscreteVariable* const var,
        *                                                       const Idx& oldval,const Idx& newval )
        */
        // ============================================================================
        void changeNotification( Instantiation& i,
                                 const DiscreteVariable* const var,
                                 const Idx& oldval,const Idx& newval );

        // ============================================================================
        /// see gum::MultiDimImplementation::setFirstNotification( Instantiation& i )
        // ============================================================================
        void setFirstNotification( Instantiation& i );

        // ============================================================================
        /// see gum::MultiDimImplementation::setLastNotification( Instantiation& i )
        // ============================================================================
        void setLastNotification( Instantiation& i );

        // ============================================================================
        /// see gum::MultiDimImplementation::setIncNotification( Instantiation& i )
        // ============================================================================
        void setIncNotification( Instantiation& i );

        // ============================================================================
        /// see gum::MultiDimImplementation::setDecNotification( Instantiation& i )
        // ============================================================================
        void setDecNotification( Instantiation& i );

        // ============================================================================
        /// see gum::MultiDimImplementation::setChangeNotification( Instantiation& i )
        // ===========================================================================
        void setChangeNotification( Instantiation& i );

        // ============================================================================
        /// see gum::MultiDimImplementation::toString( const Instantiation *i )
        // ============================================================================
        const std::string toString( const Instantiation *i ) const;

      /// @}


      // ############################################################################
      /// @name Copy methods.
      // ############################################################################
      /// @{
        // ============================================================================
        /**
        * @brief Basic copy src a gum::MultiDimContainer.
        * This method is virtual because it should be optimized in certain
        * gum::MultiDimContainer.
        *
        * @todo specific versions for decorator and for MultiDimArray
        *
        * @param src The gum::MultiDimContainer src which values are copied.
        * @param p_i Give the order to iterate in this gum::MultiDimContainer during
        *            the copy (natural order if null).
        * @throw OperationNotAllowed Raised if src does not have the same domain size
        *                            than this gum::MultiDimContainer.
        */
        // ============================================================================
        void copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                        Instantiation *p_i = ( Instantiation * ) 0 ) const;

        // ============================================================================
        /**
        * Removes all variables in this gum::MultiDimContainer and copy the content
        * of src, variables included.
        */
        // ============================================================================
        void copy ( const MultiDimContainer<GUM_SCALAR>& src );

      /// @}


      // ############################################################################
      /// @name Graph Handlers.
      // ############################################################################
      /// @{
        // ============================================================================
        /// Indicates if given node is terminal or not
        // ============================================================================
        MultiDimDecisionGraphManager<GUM_SCALAR>* getManager();

        // ============================================================================
        /// Returns the id of the root node from the diagram
        // ============================================================================
        const NodeId& root() const { return __root; };

        // ============================================================================
        /// Indicates if given node is terminal or not
        // ============================================================================
        bool isTerminalNode(const NodeId & node) const;

        // ============================================================================
        /// Returns value associated to given node
        /// @throw InvalidNode if node isn't terminal
        // ============================================================================
        const GUM_SCALAR& nodeValue( NodeId n ) const;
        const GUM_SCALAR& unsafeNodeValue( NodeId n ) const;

        // ============================================================================
        /// Returns associated variable of given node
        /// @throw InvalidNode if Node is terminal
        // ============================================================================
        const DiscreteVariable* nodeVariable( NodeId n ) const;
        const DiscreteVariable* unsafeNodeVariable( NodeId n ) const;

        // ============================================================================
        /// Returns node's sons map
        /// @throw InvalidNode if node is terminal
        // ============================================================================
        const NodeId* nodeSons( NodeId n ) const;
        const NodeId* unsafeNodeSons( NodeId n ) const;

        // ============================================================================
        /// Returns true if node has a default son
        // ============================================================================
        bool hasNodeDefaultSon( NodeId n ) const;

        // ============================================================================
        /// Returns node's default son
        /// @throw InvalidNode if node is terminal
        /// @throw NotFound if node doesn't have a default son
        // ============================================================================
        const NodeId nodeDefaultSon( NodeId n ) const;
        const NodeId unsafeNodeDefaultSon( NodeId n ) const;

        // ============================================================================
        /// Returns associated nodes of the variable pointed by the given node
        // ============================================================================
        const List< NodeId >* variableNodes( const DiscreteVariable* v ) const;

        // ============================================================================
        /// Returns associated nodes of the variable pointed by the given node
        // ============================================================================
        const Idx* variableUsedModalities( const DiscreteVariable* v ) const;

        // ============================================================================
        /// Returns true if variable is in diagram
        // ============================================================================
        bool isInDiagramVariable( const DiscreteVariable* v ) const;

      /// @}

    protected:

        // ============================================================================
        /// @brief Replace variable x by y.
        /// Technically this should be call by any subclass overloading this method
        /// to proceed with the changes in this class containers.
        // ============================================================================
        void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

        // ============================================================================
        /**
        * @brief Return a data, given a Instantiation.
        *
        * Note that get allows to change a value in the container.
        * The method is still tagged as const.
        *
        * @warning If i variables set is disjoint with this gum::MultiDimContainer
        * then 0 is assumed for dimensions (i.e. variables) not prensent in the
        * instantiation.
        *
        * @param i The instantiation used to find the data.
        */
        // ============================================================================
        GUM_SCALAR &_get (const Instantiation &inst ) const;

    private:

      // ============================================================================
      /// The root node of the decision graph
      // ============================================================================
      MultiDimDecisionGraphManager* __manager;

      // ============================================================================
      /// The root node of the decision graph
      // ============================================================================
      NodeId __root;

      // ============================================================================
      /// Associate each terminal node to a value
      // ============================================================================
      Bijection< NodeId, GUM_SCALAR > __valueMap;

      // ============================================================================
      /// Mapping between var and node
      // ============================================================================
      HashTable< const DiscreteVariable*, List<NodeId>* > __var2NodeIdMap;

      // ============================================================================
      /// Mapping between var and node
      // ============================================================================
      HashTable< const DiscreteVariable*, Idx* > __varUsedModalitiesMap;


      // ############################################################################
      /// @name Non Terminal Nodes Data Structure
      // ############################################################################
      /// @{

        // ============================================================================
        /// Associates each non-terminal node to a variable
        // ============================================================================
        HashTable< NodeId, const DiscreteVariable* > __variableMap;

        // ============================================================================
        /**
         * Each non-terminal node points to a set of son node
         * (one for each value that asociate variable can have).
         * A table gathers those links.
         * Table index : one of the value assume by the variable.
         * Table value : corresponding son node.
         */
        // ============================================================================
        HashTable< NodeId, NodeId* > __sonsMap;

        // ============================================================================
        /**
         * Each non-terminal node has the possibility to designate a specific son.
         * This one will be the default son.
         * If for a given value of bound variable, no son exists, this specific son will be chosen.
         */
        // ============================================================================
        HashTable< NodeId, NodeId > __defaultSonMap;

      /// @}
  };
}

#include <agrum/multidim/multiDimDecisionGraph.tcc>

#endif // GUM_MULTI_DIM_DECISION_GRAPH_H
