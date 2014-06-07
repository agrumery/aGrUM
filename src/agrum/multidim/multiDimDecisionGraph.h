
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
 * @brief Headers of MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_H
#define GUM_MULTI_DIM_DECISION_GRAPH_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
#include <agrum/core/list.h>
#include <agrum/core/bijection.h>
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
   *
   * @ingroup multidim_group
   */
  template<typename GUM_SCALAR>
  class MultiDimDecisionGraph : public MultiDimImplementation< GUM_SCALAR > {

    public:
      // ############################################################################
      /// The small allocator object used by every decisionGraph to allocate table
      // ############################################################################
      static SmallObjectAllocator soa;

    public:
      // ############################################################################
      /// NodeId chained list element
      // ############################################################################
      /// @{
        struct NICLElem {

          /// NodeId
          NodeId elemId;

          /// Pointer to next NICLElem
          NICLElem* nextElem;
        };

    protected :
        // ============================================================================
        /// Since elem chain are handled with soa, here is the operation of adding a node
        /// to the list.
        // ============================================================================
        static void _addElemToNICL( NICLElem** nodeChain, const NodeId& elemId);

        // ============================================================================
        /// And here the one to remove the elem
        // ============================================================================
        static void _removeElemFromNICL( NICLElem** nodeChain, const NodeId& elemId);

        // ============================================================================
        /// Delete completely the chain
        // ============================================================================
        static void _deleteNICL( NICLElem* nodeChain );
      /// @}

    public:
      // ############################################################################
      /// Parent Chained list element
      // ############################################################################
      /// @{
        struct PICLElem {

          /// NodeId
          NodeId parentId;

          /// Modality on which this parent is pointing to current node
          Idx modality;

          // Next elem
          PICLElem* nextElem;
        };

    protected :
        // ============================================================================
        /// Since elem chain are handled with soa, here is the operation of adding a node
        /// to the list.
        // ============================================================================
        static void _addElemToPICL( PICLElem** parentChain, const NodeId& elemId, const Idx& modality);

        // ============================================================================
        /// And here the one to remove the elem
        // ============================================================================
        static void _removeElemFromPICL( PICLElem** parentChain, const NodeId& elemId, const Idx& modality);

        // ============================================================================
        /// Delete completely the chain
        // ============================================================================
        static void _deletePICL( PICLElem* parentChain );
      /// @}


    public :
      // ############################################################################
      /// Structure used to represent a node internal structure
      // ############################################################################
      /// @{
        class InternalNode {
          // ============================================================================
          /// Variable associated to such node
          // ============================================================================
          const DiscreteVariable* __nodeVar;

          // ============================================================================
          /**
           * Table of sons of the node.
           * Each son is bound to a modality of the variable.
           * So those modalities are used has indexes for that table.
           * _____________________________
           * |      |      |      |      |
           * | son1 | son2 | son3 | son4 |
           * |______|______|______|______|
           *    x1     x2     x3     x4
           */
          // ============================================================================
          NodeId* __nodeSons;

          PICLElem* __nodeParents;

          public :
            // ============================================================================
            /// Constructors
            // ============================================================================
            InternalNode();
            InternalNode(const DiscreteVariable* v);

            /// You'd better known what you're doing if you're using this one.
            /// sons must have the size of domainSize of v or the program will fail!
            InternalNode(const DiscreteVariable* v, NodeId* sons);

            // ============================================================================
            /// Destructors
            // ============================================================================
            ~InternalNode();

            // ============================================================================
            /// Allocators and Deallocators redefinition
            // ============================================================================
            void* operator new(size_t);
            void operator delete(void*);

            // ============================================================================
            /// Node handlers
            // ============================================================================
            /// You'd better known what you're doing if you're using this one.
            /// sons must have the size of domainSize of v or the program will fail!
            void setNode(const DiscreteVariable* v, NodeId* sons);

            // ============================================================================
            /// Var handlers
            // ============================================================================
            void setNodeVar(const DiscreteVariable* v);
            void _setNodeVar(const DiscreteVariable* v);
            const DiscreteVariable* nodeVar() const { return __nodeVar;}

            // ============================================================================
            /// Sons handlers
            // ============================================================================
            void setSon(Idx modality, NodeId son){__nodeSons[modality] = son;}
            NodeId son(Idx modality) const {return __nodeSons[modality];}


//            const NodeId* sons() const {return __nodeSons;}


            Idx nbSons() const {return __nodeVar->domainSize();}
//            void setSons(NodeId* sons);

            // ============================================================================
            /// Parent handlers
            // ============================================================================
            void addParent( NodeId parent, Idx modality);
            void removeParent( NodeId parent, Idx modality);
            PICLElem*& parents() {return __nodeParents;}
            const PICLElem* parentsIter() const {return __nodeParents;}

            // ============================================================================
            /// Allocates a table of nodeid of the size given in parameter
            // ============================================================================
            static NodeId* allocateNodeSons(const DiscreteVariable* v);

            // ============================================================================
            /// Deallocates a NodeSons table
            // ============================================================================
            static void deallocateNodeSons(const DiscreteVariable* v, NodeId* s);

        };

      /// @}

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
        * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void set ( const Instantiation &i, const GUM_SCALAR &value ) const;

        // ============================================================================
        /**
        * see MultiDimImplementation::fill( const GUM_SCALAR &d )
        *
        * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
        * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
        */
        // ============================================================================
        void fill( const GUM_SCALAR &d ) const;

        // ==============================================================================
        /**
        * see MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
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
         * @throw OperationNotAllowed MultiDimDecisionGraph can't copy other multiDim.
         */
        // ============================================================================
        void copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                        Instantiation *p_i = ( Instantiation * ) 0 ) const;

        // ============================================================================
        /**
         * Removes all variables in this MultiDimContainer and copy the content
         * of src, variables included.
         * @throw OperationNotAllowed MultiDimDecisionGraph can't copy other multiDim.
         */
        // ============================================================================
        void copy ( const MultiDimContainer<GUM_SCALAR>& src );

        // ============================================================================
        /**
         * Removes all variables in this MultiDimDecisionGraph and copy the content
         * of src, variables included.
         */
        // ============================================================================
        void copy ( const MultiDimDecisionGraph<GUM_SCALAR>& src );

        // ============================================================================
        /**
         * Copies src diagrams structure into this diagrams.
         * However it also changes the variables.
         *
         * This has two implications.
         * First, this is not just a renaming. Pointers are trully changed.
         * Second, for each pair of variable, the new variable MUST macth the number
         * of variables of the old variable.
         */
        // ============================================================================
        void copyAndReassign ( const MultiDimDecisionGraph<GUM_SCALAR> &src,
                               const Bijection<const DiscreteVariable*, const DiscreteVariable*> reassign );

        // ============================================================================
        /**
         * Copies src diagrams and multiply every value by the given scalar.
         */
        // ============================================================================
        void copyAndMultiplyByScalar ( const MultiDimDecisionGraph<GUM_SCALAR>& src, GUM_SCALAR gamma );

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
        MultiDimDecisionGraphManager<GUM_SCALAR>* manager();

        // ============================================================================
        /// Returns the id of the root node from the diagram
        // ============================================================================
        const NodeId& root() const { return __root; }

        // ============================================================================
        /// Indicates if given node is terminal or not
        // ============================================================================
        bool isTerminalNode(const NodeId & node) const { return __valueMap.existsFirst(node); }

        // ============================================================================
        /// Returns value associated to given node
        /// @throw InvalidNode if node isn't terminal
        // ============================================================================
        const GUM_SCALAR& nodeValue( NodeId n ) const { return this->__valueMap.second( n ); }

        // ============================================================================
        /// Returns internalNode structure associated to that nodeId
        /// @throw InvalidNode if node is terminal
        // ============================================================================
        const InternalNode* node( NodeId n ) const {return this->__internalNodeMap[ n ];}

        const NICLElem* varNodeListe( const DiscreteVariable* var ) const { return __var2NodeIdMap[var]; }

        const Bijection<NodeId, GUM_SCALAR>& values(  ) const{ return this->__valueMap; }

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
        * @warning If i variables set is disjoint with this MultiDimContainer
        * then 0 is assumed for dimensions (i.e. variables) not prensent in the
        * instantiation.
        *
        * @param i The instantiation used to find the data.
        */
        // ============================================================================
        GUM_SCALAR& _get(const Instantiation &inst ) const;


    private:
      // ============================================================================
      /// The name of the data structure
      // ============================================================================
      std::string __name;

      // ============================================================================
      /// Indicates available nodeIds
      // ============================================================================
      NodeGraphPart __model;

      // ============================================================================
      /// A reference to the manager that edits this decision graph
      // ============================================================================
      MultiDimDecisionGraphManager<GUM_SCALAR>* __manager;

      // ============================================================================
      /// The root node of the decision graph
      // ============================================================================
      NodeId __root;

      // ============================================================================
      /// Associates each terminal node to a value
      // ============================================================================
      Bijection< NodeId, GUM_SCALAR > __valueMap;
      HashTable< NodeId, PICLElem* > __valueParents;

      // ============================================================================
      /// Associates each non-terminal node to a variable
      // ============================================================================
      HashTable< NodeId, InternalNode* > __internalNodeMap;

      // ============================================================================
      /// Mapping between var and node
      // ============================================================================
      HashTable< const DiscreteVariable*, NICLElem* > __var2NodeIdMap;

      // ============================================================================
      /// So to avoid any difficulites with get
      // ============================================================================
      mutable GUM_SCALAR __getRet;


      static Idx aNICLE;
      static Idx dNICLE;
      static Idx aPICLE;
      static Idx dPICLE;
      static Idx aIN;
      static Idx dIN;


      /// @}

      friend class MultiDimDecisionGraphManager<GUM_SCALAR>;
  };
}

#include <agrum/multidim/multiDimDecisionGraph.tcc>
#include <agrum/multidim/multiDimDecisionGraph.Elem.tcc>
#include <agrum/multidim/multiDimDecisionGraph.InternalNode.tcc>

#endif // GUM_MULTI_DIM_DECISION_GRAPH_H
