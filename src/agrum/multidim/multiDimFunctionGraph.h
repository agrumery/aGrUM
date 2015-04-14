
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
 * @brief Headers of MultiDimFunctionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_H

// ============================================================================
//#include <agrum/core/inline.h>
#include <agrum/core/types.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
#include <agrum/core/list.h>
#include <agrum/core/bijection.h>
// ============================================================================
#include <agrum/graphs/graphElements.h>
// ============================================================================
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimFunctionGraphManager.h>
#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>
#include <agrum/multidim/FunctionGraphUtilities/link.h>
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
// ============================================================================

namespace gum {

  template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  class MultiDimFunctionGraphManager;
  /**
   * @class MultiDimFunctionGraph multiDimFunctionGraph.h <agrum/multidim/multiDimFunctionGraph.h>
   *
   * @brief Class implementingting a decision graph
   *
   * @ingroup multidim_group
   */
  template<typename GUM_SCALAR, template <class> class TerminalNodePolicy = ExactTerminalNodePolicy >
  class MultiDimFunctionGraph : public MultiDimImplementation< GUM_SCALAR >, public TerminalNodePolicy<GUM_SCALAR> {

    public:

      // ############################################################################
      /// Only for proper initialization of a certain returned value
      // ############################################################################
      const static GUM_SCALAR defaultValue;

      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
        // ============================================================================
        /// Default constructor.
        // ============================================================================
        MultiDimFunctionGraph();

        // ============================================================================
        /// Copy constructor.
        // ============================================================================
        MultiDimFunctionGraph( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& from );

        // ============================================================================
        /// Copy Operator.
        // ============================================================================
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& operator=(const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& from);

        // ============================================================================
        /// Destructor.
        // ============================================================================
        ~MultiDimFunctionGraph();

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
      /// @name Accessors / Modifiers herited from MultiDimImplementation
      // ############################################################################
      /// @{

        // ============================================================================
        /// see MultiDimImplementation
        // ============================================================================
        const std::string& name() const;

        // ============================================================================
        /**
        * see MultiDimImplementation::set ( const Instantiation &i, const GUM_SCALAR &value )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimFunctionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void set ( const Instantiation &i, const GUM_SCALAR &value ) const;

        // ============================================================================
        /**
        * see MultiDimImplementation::fill( const GUM_SCALAR &d )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimFunctionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void fill( const GUM_SCALAR &d ) const;

        // ==============================================================================
        /**
        * see MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimFunctionGraphManager provides the framework to editate a Decision Graph.
        */
        // ==============================================================================
        void fillWith ( const std::vector<GUM_SCALAR>& v ) const;

      /// @}


      // ############################################################################
      /// @name Implementation of MultiDimInterface
      // ############################################################################
      /// @{

        // ============================================================================
        /// See MultiDimInterface::add(const DiscreteVariable& v)
        // ============================================================================
        virtual void add (const DiscreteVariable &v);

        // ============================================================================
        /// See MultiDimInterface::erase(const DiscreteVariable& v)
        // ============================================================================
        void erase( const DiscreteVariable& v );

        // ============================================================================
        /// see MultiDimImplementation::realSize()
        // ============================================================================
        Size realSize() const;

      /// @}

      // ############################################################################
      /// @name Slave management and extension due to slave management
      // ############################################################################
      /// @{
        // ============================================================================
        /** see MultiDimImplementation::changeNotification( Instantiation& i,
        *                                                       const DiscreteVariable* const var,
        *                                                       const Idx& oldval,const Idx& newval )
        */
        // ============================================================================
        void changeNotification( Instantiation& i,
                                 const DiscreteVariable* const var,
                                 const Idx& oldval,const Idx& newval );

        // ============================================================================
        /// see MultiDimImplementation::setFirstNotification( Instantiation& i )
        // ============================================================================
        void setFirstNotification( Instantiation& i );

        // ============================================================================
        /// see MultiDimImplementation::setLastNotification( Instantiation& i )
        // ============================================================================
        void setLastNotification( Instantiation& i );

        // ============================================================================
        /// see MultiDimImplementation::setIncNotification( Instantiation& i )
        // ============================================================================
        void setIncNotification( Instantiation& i );

        // ============================================================================
        /// see MultiDimImplementation::setDecNotification( Instantiation& i )
        // ============================================================================
        void setDecNotification( Instantiation& i );

        // ============================================================================
        /// see MultiDimImplementation::setChangeNotification( Instantiation& i )
        // ===========================================================================
        void setChangeNotification( Instantiation& i );

        // ============================================================================
        /// see MultiDimImplementation::toString( const Instantiation *i )
        // ============================================================================
        const std::string toString( const Instantiation *i ) const;

      /// @}


      // ############################################################################
      /// @name Copy methods.
      // ############################################################################
      /// @{
        // ============================================================================
        /**
         * @brief Basic copy src a MultiDimContainer.
         * This method is virtual because it should be optimized in certain
         * MultiDimContainer.
         *
         * @todo specific versions for decorator and for MultiDimArray
         *
         * @param src The MultiDimContainer src which values are copied.
         * @param p_i Give the order to iterate in this MultiDimContainer during
         *            the copy (natural order if null).
         * @throw OperationNotAllowed MultiDimFunctionGraph can't copy other multiDim.
         */
        // ============================================================================
        void copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                        Instantiation *p_i = ( Instantiation * ) 0 ) const;

        // ============================================================================
        /**
         * Removes all variables in this MultiDimContainer and copy the content
         * of src, variables included.
         * @throw OperationNotAllowed MultiDimFunctionGraph can't copy other multiDim.
         */
        // ============================================================================
        void copy ( const MultiDimContainer<GUM_SCALAR>& src );

        // ============================================================================
        /**
         * Removes all variables in this MultiDimFunctionGraph and copy the content
         * of src, variables included.
         */
        // ============================================================================
        void copy ( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& src );

        // ============================================================================
        /**
         * Copies src diagrams structure into this diagrams.
         * However it also changes the variables.
         *
         * @warning This has two implications.
         * First, this is not just a renaming. Pointers are trully changed.
         * Second, for each pair of variable, the new variable MUST macth the number
         * of variables of the old variable.
         */
        // ============================================================================
        void copyAndReassign ( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy> &src,
                               const Bijection<const DiscreteVariable*, const DiscreteVariable*>& reassign );

        // ============================================================================
        /**
         * Copies src diagrams and multiply every value by the given scalar.
         */
        // ============================================================================
        void copyAndMultiplyByScalar ( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& src,
                                       GUM_SCALAR gamma );

        // ============================================================================
        /**
         * Clears the decision graph
         */
        // ============================================================================
        void clear( );

      /// @}


      // ############################################################################
      /// @name Graph Handlers.
      // ############################################################################
      /// @{
        // ============================================================================
        /// Returns a const reference to the manager of this diagram
        // ============================================================================
        std::string toDot( bool withBackArcs = false ) const;

        // ============================================================================
        /// Returns a const reference to the manager of this diagram
        // ============================================================================
        const NodeGraphPart& model() const { return __model; }

        // ============================================================================
        /// Returns a const reference to the manager of this diagram
        // ============================================================================
        MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>* manager();

        // ============================================================================
        /// Returns the id of the root node from the diagram
        // ============================================================================
        const NodeId& root() const { return __root; }

        // ============================================================================
        /// Indicates if given node is terminal or not
        // ============================================================================
        bool isTerminalNode(const NodeId & node) const { return !__internalNodeMap.exists(node); }

        // ============================================================================
        /// Returns value associated to given node
        /// @throw InvalidNode if node isn't terminal
        // ============================================================================
        const GUM_SCALAR& nodeValue( NodeId n ) const { return this->terminalNodeValue(n); }

        // ============================================================================
        /// Returns internalNode structure associated to that nodeId
        /// @throw InvalidNode if node is terminal
        // ============================================================================
        const InternalNode* node( NodeId n ) const {return this->__internalNodeMap[ n ];}

        // ============================================================================
        /// Returns the list of node associated to given variable
        // ============================================================================
        const LinkedList<NodeId>* varNodeListe( const DiscreteVariable* var ) const { return __var2NodeIdMap[var]; }

      /// @}

      // ============================================================================
      /// Returns the name of the table represented by this structure
      // ============================================================================
      const std::string& tableName() const { return __tableName;}

      // ============================================================================
      /// Sets the name of the table represented by this structure
      // ============================================================================
      void setTableName(const std::string& name) { __tableName = name;}

      virtual GUM_SCALAR get ( const Instantiation& i ) const ;


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
        * @warning If i variables set is disjoint with this MultiDimContainer
        * then 0 is assumed for dimensions (i.e. variables) not prensent in the
        * instantiation.
        *
        * @param i The instantiation used to find the data.
        */
        // ============================================================================
        GUM_SCALAR& _get(const Instantiation &inst ) const {
          GUM_ERROR(OperationNotAllowed, "You can't edit a function by other mean than the manager");
        }


    private:
      // ============================================================================
      /// The name of the data structure
      // ============================================================================
      std::string __name;

      // ============================================================================
      /// The name of the data structure
      // ============================================================================
      std::string __tableName;

      // ============================================================================
      /// Indicates available nodeIds
      // ============================================================================
      NodeGraphPart __model;

      // ============================================================================
      /// A reference to the manager that edits this decision graph
      // ============================================================================
      MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>* __manager;

      // ============================================================================
      /// The root node of the decision graph
      // ============================================================================
      NodeId __root;

      // ============================================================================
      /// Associates each non-terminal node to a variable
      // ============================================================================
      HashTable< NodeId, InternalNode* > __internalNodeMap;

      // ============================================================================
      /// Mapping between var and node
      // ============================================================================
      HashTable< const DiscreteVariable*, LinkedList<NodeId>* > __var2NodeIdMap;


      /// @}

      friend class MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>;
  };
}

#include <agrum/multidim/multiDimFunctionGraph.tcc>

#endif // GUM_MULTI_DIM_FUNCTION_GRAPH_H
