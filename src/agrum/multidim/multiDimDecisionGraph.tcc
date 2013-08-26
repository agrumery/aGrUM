
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                              //
 *                                                                        //
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or    //
 *   (at your option) any later version.                                  //
 *                                                                        //
 *   This program is distributed in the hope that it will be useful,      //
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       //
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
 *   GNU General Public License for more details.                         //
 *                                                                        //
 *   You should have received a copy of the GNU General Public License    //
 *   along with this program; if not, write to the                        //
 *   Free Software Foundation, Inc.,                                      //
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            //
 ***************************************************************************/
/**
 * @file
 * @brief Template methods of gum::MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {

  // ============================================================================
  // Since elem chain are handled with soa, here is the operation of adding a node
  // to the list.
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_addElemToNICL( NICLElem* nodeChain, const NodeId& elemId){

    NICLElem* newElem = static_cast< NICLElem* >(soa.allocate(sizeof( NICLElem )));
    newElem->elemId = elemId;
    newElem->nextElem = nodeChain;
    nodeChain = newElem;
  }

  // ============================================================================
  // And here the one to remove the elem
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(
          NICLElem* nodeChain, const NodeId& elemId){

    NICLElem* current = nodeChain;
    NICLElem** prev = &nodeChain;
    while( current != nullptr ){
      if( currrent->elemId == elemId ){
        *prev = current->nextElem;
        soa.deallocate(current, sizeof( NICLElem ));
        break;
      }
      prev = &(current->nextElem);
      current = current->nextElem;
    }
  }

  // ============================================================================
  //  Delete completely the chain
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_deleteNICL( NICLElem* nodeChain ){

    // Déallocation des chainons parents dans la liste des parents
    NICLElem* currentNode = nodeChain;
    while( currentNode ){
      NICLElem* nextNode = currentNode->nextElem;
      soa.deallocate(currentNode, sizeof( NICLElem ));
      currentNode = nextNode;
    }
    nodeChain = nullptr;
  }

  // ============================================================================
  // Create an internal node structure
  // @param x :  the associated variable
  // size of son vector is the domain size of x.
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* MultiDimDecisionGraph<GUM_SCALAR>::_createInternalNode( const DiscreteVariable* x ){

    InternalNode* newNode = static_cast<InternalNode*>(soa.allocate(sizeof(x->domainSize())));
    newNode->nodeVar = x;
    newNode->nodeSons = static_cast<NodeId*>(soa.allocate(x->domainSize()*sizeof(NodeId)));
    return newNode;

  }

  // ============================================================================
  // Remove the given internal node structure
  // @param deleteParent : if true, parentList will be erase too.
  // Typically not wanted in a swap for more efficiency
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_removeInternalNode( MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* node ){

    // Déallocation du vecteur fils
    soa.deallocate( node->nodeSons, node->nodeVar->domainSize()*sizeof(NodeId) );

    // Déallocation de la structure
    soa.deallocate( node, sizeof(InternalNode));

  }

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::MultiDimDecisionGraph():__internalNodeMap(500,true,false),__var2NodeIdMap(500,true,false),__valueMap(125, true){
      GUM_CPY( MultiDimDecisionGraph );
      __manager = nullptr;
    }

    // ============================================================================
    // Copy constructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::MultiDimDecisionGraph( const MultiDimDecisionGraph<GUM_SCALAR>& from ){
      GUM_CONS_CPY( MultiDimDecisionGraph );
      copy(from);
    }

    // ============================================================================
    // Copy Operator.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>& MultiDimDecisionGraph<GUM_SCALAR>::operator=(const MultiDimDecisionGraph<GUM_SCALAR>& from){
     copy(from);
     return *this;
    }

    // ============================================================================
    // Destructor.
    // @warning Won't destroy the variables. You have to do it on your own.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::~MultiDimDecisionGraph(){
      // Destruction du manager
      GUM_DESTRUCTOR( MultiDimDecisionGraph );
      delete __manager;
      this->clear();
    }

  // ============================================================================
  //
  // This method creates a clone of this object, withouth its content
  // (including variable), you must use this method if you want to ensure
  // that the generated object has the same type than the object containing
  // the called newFactory()
  // For example :
  //   MultiDimArray<double> y;
  //   MultiDimContainer<double>* x = y.newFactory();
  // Then x is a MultiDimArray<double>*
  //
  // @warning you must desallocate by yourself the memory
  // @return an empty clone of this object with the same type
  //
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  MultiDimContainer<GUM_SCALAR>* MultiDimDecisionGraph<GUM_SCALAR>::newFactory() const{
  }


  // ############################################################################
  // Accessors / Modifiers herited from gum::MultiDimImplementation
  // ############################################################################

    // ============================================================================
    // see gum::MultiDimImplementation
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string& MultiDimDecisionGraph<GUM_SCALAR>::name() const{
        return "multidimdecisiongraph";
    }

    // ============================================================================
    //
    // see gum::MultiDimImplementation::set ( const Instantiation &i, const GUM_SCALAR &value )
    //
    // @throw OperationNotAllowed. Decision Graph can't be edited so easily.
    // MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::set ( const Instantiation &i, const GUM_SCALAR &value ) const{
      GUM_ERROR( OperationNotAllowed, "Decision Graph can't be edited so easily.\nMultiDimDecisionGraphManager provides the framework to editate a Decision Graph.")
    }

    // ============================================================================
    //
    // see gum::MultiDimImplementation::fill( const GUM_SCALAR &d )
    //
    // @throw OperationNotAllowed. Decision Graph can't be edited so easily.
    // MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::fill( const GUM_SCALAR &d ) const{
      GUM_ERROR( OperationNotAllowed, "Decision Graph can't be edited so easily.\nMultiDimDecisionGraphManager provides the framework to editate a Decision Graph.")
    }

    // ==============================================================================
    //
    // see gum::MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
    //
    // @throw OperationNotAllowed. Decision Graph can't be edited so easily.
    // MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
    ///
    // ==============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::fillWith ( const std::vector<GUM_SCALAR>& v ) const{
      GUM_ERROR( OperationNotAllowed, "Decision Graph can't be edited so easily.\nMultiDimDecisionGraphManager provides the framework to editate a Decision Graph.")
    }


  // ############################################################################
  // Implementation of MultiDimInterface
  // ############################################################################

    // ============================================================================
    // See gum::MultiDimInterface::erase(const DiscreteVariable& v)
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::erase( const DiscreteVariable& v ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::realSize()
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    Size MultiDimDecisionGraph<GUM_SCALAR>::realSize() const{
      return __internalNodeMap.size() + __valueMap.size();
    }


  // ############################################################################
  // Slave management and extension due to slave management
  // ############################################################################
    // ============================================================================
    // see gum::MultiDimImplementation::changeNotification( Instantiation& i,
    //                                                       const DiscreteVariable* const var,
    //                                                       const Idx& oldval,const Idx& newval )
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::changeNotification( Instantiation& i,
                                 const DiscreteVariable* const var,
                                 const Idx& oldval,const Idx& newval ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::setFirstNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setFirstNotification( Instantiation& i ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::setLastNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setLastNotification( Instantiation& i ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::setIncNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setIncNotification( Instantiation& i ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::setDecNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setDecNotification( Instantiation& i ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::setChangeNotification( Instantiation& i )
    // ===========================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setChangeNotification( Instantiation& i ){

    }

    // ============================================================================
    // see gum::MultiDimImplementation::toString( const Instantiation *i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string MultiDimDecisionGraph<GUM_SCALAR>::toString( const Instantiation *i ) const{
        std::stringstream sBuff;
      //  sBuff << ( *i ) << " = " << get( *i );
        return sBuff.str();
    }


  // ############################################################################
  // Copy methods.
  // ############################################################################

    // ============================================================================
    //
    // @brief Basic copy src a gum::MultiDimContainer.
    // This method is virtual because it should be optimized in certain
    // gum::MultiDimContainer.
    //
    // @todo specific versions for decorator and for MultiDimArray
    //
    // @param src The gum::MultiDimContainer src which values are copied.
    // @param p_i Give the order to iterate in this gum::MultiDimContainer during
    //            the copy (natural order if null).
    // @throw OperationNotAllowed Raised if src does not have the same domain size
    //                            than this gum::MultiDimContainer.
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                                                       Instantiation *p_i ) const {
      GUM_ERROR( OperationNotAllowed, "You cannot copy another type of multiDim into a MultiDimDecisionGraph.");
    }

    // ============================================================================
    //
    // Removes all variables in this gum::MultiDimContainer and copy the content
    // of src, variables included.
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copy ( const MultiDimContainer<GUM_SCALAR>& src ){
      GUM_ERROR( OperationNotAllowed, "You cannot copy another type of multiDim into a MultiDimDecisionGraph.");
    }

    // ============================================================================
    //
    // Removes all variables in this gum::MultiDimContainer and copy the content
    // of src, variables included.
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copy ( const MultiDimDecisionGraph<gum::GUM_SCALAR> &src ){
      clear();
      std::vector<NodeId> lifo;
      lifo.push_back(src.root());
      HashTable< NodeId, bool > prunningTable;

      // Parcours en profondeur du diagramme source
      while( !lifo.empty() ){
        NodeId currentNodeId = lifo.pop_back();

        if( src.isTerminalNode(currentNodeId ) )
          // Si le noeud courant est terminal, sa valeur associée est ajoutée à la table des valeurs
          __valueMap.insert( currentNodeId , src.nodeValue( currentNodeId ) );
        else {
          // Si le noeud courant n'est pas terminal,
          // il nous faut dupliquer la structure interne dans une nouvelle structure
          const InternalNode* currentNode = src.node( currentNodeId );
          InternalNode* newNode = _createInternalNode( currentNode->var );

          // Recopie des fils
          for( Idx index = 0; index < newNode->nodeVar->domainSize(); ++index ){
            newNode->nodeSons[index] = currentNode->nodeSons[index];
            // Accesoirement on profite du parcours des fils pour mettre la lifo à jour
            if( !prunningTable.exists(newNode->nodeSons[index]) ){
              lifo.push_back(newNode->nodeSons[index]);
              prunningTable.insert( newNode->nodeSons[index], true );
            }
          }

          __internalNodeMap.insert( currentNodeId, newNode );
          _addElemToNICL( __var2NodeIdMap[ currentNode->nodeVar ], currentNode );
        }
      }
    }

    // ============================================================================
    //
    // Clears the decision graph
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::clear( ){

      // Nettoyage des noeuds
      for( HashTableIterator<NodeId, InternalNode*> nodeIter = __internalNodeMap.begin();
           nodeIter != __internalNodeMap.end(); ++nodeIter )
        _removeInternalNode(*nodeIter);

      // Nettoyage des liste de noeuds par variables
      for( HashTableIterator<const DiscreteVariable*, NICLElem*> varIter = __var2NodeIdMap.begin();
           varIter != __var2NodeIdMap.end(); ++varIter ){
        _deleteNICL(*varIter);
      }

      __internalNodeMap.clear();
      __var2NodeIdMap.clear();
    }


  // ############################################################################
  // @name Graph Handlers.
  // ############################################################################

    // ============================================================================
    // Indicates if given node is terminal or not
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    bool MultiDimDecisionGraph<GUM_SCALAR>::isTerminalNode(const NodeId& node) const{
      return __valueMap.existsFirst(node);
    }

    // ============================================================================
    // Returns value associated to given node
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const GUM_SCALAR& MultiDimDecisionGraph<GUM_SCALAR>::nodeValue( NodeId n ) const {
      return this->__valueMap.second( n );
    }

    // ============================================================================
    // Returns associated variable of given node
    // @throw InvalidNode if Node is terminal
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* MultiDimDecisionGraph<GUM_SCALAR>::nodeVariable( NodeId n ) const{
      return this->__internalNodeMap[ n ];
    }

    // ============================================================================
    // @brief Replace variable x by y.
    // Technically this should be call by any subclass overloading this method
    // to proceed with the changes in this class containers.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::_swap( const DiscreteVariable* x, const DiscreteVariable* y ){

    }

    // ============================================================================
    // Swap two adjacent variable.
    // Order is important here.
    // X must precede Y before the swap (at the end Y will then precede X).
    // Not respecting this constraint leads to hasardous behaviour.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::_adjacentSwap( const DiscreteVariable* x, const DiscreteVariable* y ){

      NICLElem* oldxNodes = __var2NodeIdMap[ x ];
      NICLElem* oldyNodes = __var2NodeIdMap[ y ];

      __var2NodeIdMap[ x ] = nullptr;
      __var2NodeIdMap[ y ] = nullptr;

      InternalNode* xNode = nullptr;
      InternalNode* sonNode = nullptr;
      NodeId* ysons = nullptr;
      NodeId* xsons = nullptr;
      NICLElem* nextn = nullptr;

      while( oldxNodes ){

        // Recuperation de la structure associée au noeud courant
        xNode = __internalNodeMap[oldxNodes->elemId];

        // Creation de la structure amenee à remplacer la structure courante
        // et associée par conséquence à y
        ysons = static_cast<NodeId*>( soa.allocate(y->domainSize()*sizeof(NodeId)) );

        // Maintenant il faut remapper le graphe en insérant un noeud associé à x
        // Pour chaque valeur de y
        for( Idx indy = 0; indy < y->domainSize(); ++indy ){

          // Creation du vecteur fils du noeud associe à x qui sera sont descendant pour cette valeur-ci
          xsons = static_cast<NodeId*>( soa.allocate(x->domainSize()*sizeof(NodeId)) );

          // Iteration sur les différente valeurs de x pour faire le mapping
          for( Idx indx; indx < x->domainSize(); ++indx ){
            sonNode = __internalNodeMap[ xNode->nodeSons[indx] ];
            if( xsonNode->nodeVar == y )
              xsons[indx] = sonNode->nodeSons[indy];
            else
              xsons[indx] = xNode->nodeSons[indx];
          }

          // Verification que le nouveau noeud x à insérer n'aura pas de doublon
          ysons[indy] = manager()->checkRedundancy( x, xsons );
          // Si non, alors on l'insère
          if( ysons[indy] == 0 )
            ysons[indy] = manager()->addNonTerminalNode( x, xsons );
        }

        // Même opération pour y une fois la creation de ses fils achevée
        NodeId newyNodeId = manager()->checkRedundancy( y, ysons );
        if( newyNodeId == 0 )
          newyNodeId = manager()->addNonTerminalNode( y, ysons, oldxNodes->elemId );

        nextn = oldxNodes->nextElem;
        _removeInternalNode(xNode);
        _removeElemFromNICL(oldxNodes, oldxNode->elemId);
        oldxNodes = nextn;
      }

      while(oldyNodes){

        nextn = oldyNodes->nextElem;
        _removeInternalNode(__internalNodeMap[oldyNodes->elemId]);
        __internalNodeMap.erase(oldyNodes->elemId);
        manager()->releaseId(oldyNodes->elemId);
        _removeElemFromNICL(oldyNodes, oldyNode->elemId);
        oldyNodes = nextx;
      }
    }

    // ============================================================================
    //
    // @brief Return a data, given a Instantiation.
    //
    // Note that get allows to change a value in the container.
    // The method is still tagged as const.
    //
    // @warning If i variables set is disjoint with this gum::MultiDimContainer
    // then 0 is assumed for dimensions (i.e. variables) not prensent in the
    // instantiation.
    //
    // @param i The instantiation used to find the data.
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    GUM_SCALAR & MultiDimDecisionGraph<GUM_SCALAR>::_get ( const Instantiation &inst ) const{
        NodeId currentNodeId = __root;
        InternalNode* currentNode = nullptr;
        while ( ! isTerminalNode( currentNodeId ) ){
          currentNode = __internalNodeMap[currentNodeId];
          currentNodeId = currentNode->nodeSons[ inst.val( currentNode->nodeVar ) ];
        }
        return this->__valueMap.second( currentNodeId );
    }

} //gum
