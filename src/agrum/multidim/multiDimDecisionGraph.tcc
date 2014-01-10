
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
 * @brief Template methods of MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {

template<typename GUM_SCALAR>
  SmallObjectAllocator MultiDimDecisionGraph<GUM_SCALAR>::soa(GUM_DEFAULT_CHUNK_SIZE, GUM_DEFAULT_MAX_OBJECT_SIZE);

  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::aNICLE(0);
  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::dNICLE(0);
  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::aIN(0);
  template<typename GUM_SCALAR>
 Idx MultiDimDecisionGraph<GUM_SCALAR>:: dIN(0);

  // ============================================================================
  // Since elem chain are handled with soa, here is the operation of adding a node
  // to the list.
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_addElemToNICL( NICLElem **nodeChain, const NodeId& elemId){

    NICLElem* newElem = static_cast< NICLElem* >(soa.allocate(sizeof( NICLElem )));
    newElem->elemId = elemId;
    newElem->nextElem = *nodeChain;
    *nodeChain = newElem;
    aNICLE++;
  }

  // ============================================================================
  // And here the one to remove the elem
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( NICLElem **nodeChain, const NodeId& elemId){

    NICLElem* current = *nodeChain;
    NICLElem** prev = nodeChain;
    while( current != nullptr ){
      if( current->elemId == elemId ){
        *prev = current->nextElem;
        soa.deallocate(current, sizeof( NICLElem ));
        break;
      }
      prev = &(current->nextElem);
      current = current->nextElem;
    }
    dNICLE++;
  }

  // ============================================================================
  //  Delete completely the chain
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_deleteNICL( NICLElem* nodeChain ){

    // Déallocation des chainons parents dans la liste des parents
    NICLElem* currentNode = nodeChain;
    NICLElem* nextNode = nullptr;
    while( currentNode ){
      nextNode = currentNode->nextElem;
      soa.deallocate(currentNode, sizeof( NICLElem ));
      currentNode = nextNode;
      dNICLE++;
    }
    nodeChain = nullptr;
  }

  // ############################################################################
  // Internal Nodes methods
  // ############################################################################
      // ============================================================================
      /// Constructors
      // ============================================================================
      template<typename GUM_SCALAR>
      INLINE
      MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::InternalNode(){
      }

      template<typename GUM_SCALAR>
      INLINE
      MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::InternalNode(const DiscreteVariable* v){
        setNodeVar(v);
      }

      // ============================================================================
      /// Destructors
      // ============================================================================
      template<typename GUM_SCALAR>
      INLINE
      MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::~InternalNode(){
        _deallocateNodeSons();
      }

      // ============================================================================
      /// Allocators and Deallocators redefinition
      // ============================================================================
      template<typename GUM_SCALAR>
      INLINE
      void* MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::operator new(size_t t){
        return soa.allocate(sizeof(InternalNode));
      }

      template<typename GUM_SCALAR>
      INLINE
      void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::operator delete(void* in){
        soa.deallocate(in, sizeof(InternalNode));
      }

      // ============================================================================
      /// Allocates a table of nodeid of the size given in parameter
      // ============================================================================
      template<typename GUM_SCALAR>
      INLINE
      void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::_allocateNodeSons(){
        __nodeSons = soa.allocate( sizeof(NodeId)*__nodeVar->domainSize() );
        for( gum::Idx i = 0; i < __nodeVar->domainSize(); ++i)
          __nodeSons[i] = 0;
      }

      // ============================================================================
      /// Deallocates a NodeSons table
      // ============================================================================
      template<typename GUM_SCALAR>
      INLINE
      void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::_deallocateNodeSons(){
        if( __nodeVar != nullptr )
          soa.deallocate(__nodeSons, sizeof(NodeId)*__nodeVar->domainSize());
      }



  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::MultiDimDecisionGraph():
        __name("MultiDimDecisionGraph"), __model( 500, true ),__valueMap(125, true), __internalNodeMap(500,true,false),__var2NodeIdMap(500,true,false),__getRet(0){
      GUM_CONSTRUCTOR( MultiDimDecisionGraph );
      __manager = nullptr;
      // Pop up a first node so that id 0 is unavailable
      __model.insertNode();
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
//      std::cout << "ANICLE = " << aNICLE << " - dNICLE = " << dNICLE << " - AIN = " << aIN << " - dIN = " << dIN << std::endl;
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
      return new MultiDimDecisionGraph<GUM_SCALAR>();
  }


  // ############################################################################
  // Accessors / Modifiers herited from MultiDimImplementation
  // ############################################################################

    // ============================================================================
    // see MultiDimImplementation
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string& MultiDimDecisionGraph<GUM_SCALAR>::name() const{
        return __name;
    }

    // ============================================================================
    //
    // see MultiDimImplementation::set ( const Instantiation &i, const GUM_SCALAR &value )
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
    // see MultiDimImplementation::fill( const GUM_SCALAR &d )
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
    // see MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
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
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::add (const DiscreteVariable &v){
      if( ! this->variablesSequence().exists(&v))
        MultiDimImplementation<GUM_SCALAR>::add(v);

      if( ! this->__var2NodeIdMap.exists(&v))
        __var2NodeIdMap.insert(&v,nullptr);
    }

    // ============================================================================
    // See MultiDimInterface::erase(const DiscreteVariable& v)
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::erase( const DiscreteVariable& v ){
      if( this->__var2NodeIdMap.exists(&v)){
        while(__var2NodeIdMap[&v] != nullptr){
          manager()->eraseNode(__var2NodeIdMap[&v]->elemId);
        }
        _deleteNICL(__var2NodeIdMap[&v]);
        __var2NodeIdMap.erase(&v);
      }

      if( this->variablesSequence().exists(&v))
        MultiDimImplementation<GUM_SCALAR>::erase(v);
    }

    // ============================================================================
    // see MultiDimImplementation::realSize()
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
    // see MultiDimImplementation::changeNotification( Instantiation& i,
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
    // see MultiDimImplementation::setFirstNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setFirstNotification( Instantiation& i ){

    }

    // ============================================================================
    // see MultiDimImplementation::setLastNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setLastNotification( Instantiation& i ){

    }

    // ============================================================================
    // see MultiDimImplementation::setIncNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setIncNotification( Instantiation& i ){

    }

    // ============================================================================
    // see MultiDimImplementation::setDecNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setDecNotification( Instantiation& i ){

    }

    // ============================================================================
    // see MultiDimImplementation::setChangeNotification( Instantiation& i )
    // ===========================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setChangeNotification( Instantiation& i ){

    }

    // ============================================================================
    // see MultiDimImplementation::toString( const Instantiation *i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string MultiDimDecisionGraph<GUM_SCALAR>::toString( const Instantiation *i ) const{
      std::stringstream sBuff;
      sBuff << ( *i ) << " = " << this->get( *i );
      return sBuff.str();
    }


  // ############################################################################
  // Copy methods.
  // ############################################################################

    // ============================================================================
    //
    // @brief Basic copy src a MultiDimContainer.
    // This method is virtual because it should be optimized in certain
    // MultiDimContainer.
    //
    // @todo specific versions for decorator and for MultiDimArray
    //
    // @param src The MultiDimContainer src which values are copied.
    // @param p_i Give the order to iterate in this MultiDimContainer during
    //            the copy (natural order if null).
    // @throw OperationNotAllowed Raised if src does not have the same domain size
    //                            than this MultiDimContainer.
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
    // Removes all variables in this MultiDimContainer and copy the content
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
    // Removes all variables in this MultiDimContainer and copy the content
    // of src, variables included.
    ///
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copy ( const MultiDimDecisionGraph<GUM_SCALAR> &src ){

      this->clear();

      // Insertion des nouvelles variables
      for( SequenceIterator<const DiscreteVariable*> varIter = src.variablesSequence().begin(); varIter != src.variablesSequence().end(); ++varIter)
        this->add(**varIter);

      // Recopie des Ids utilisées
      this->__model = src.model();

      std::vector<NodeId> lifo;
      lifo.push_back(src.root());
      HashTable< NodeId, bool > prunningTable;

      // Parcours en profondeur du diagramme source
      while( !lifo.empty() ){
        NodeId currentNodeId = lifo.back();
        lifo.pop_back();

        if( src.isTerminalNode(currentNodeId ) ){
          // Si le noeud courant est terminal, sa valeur associée est ajoutée à la table des valeurs
          __valueMap.insert( currentNodeId , src.nodeValue( currentNodeId ) );
        }else {
          // Si le noeud courant n'est pas terminal,
          // il nous faut dupliquer la structure interne dans une nouvelle structure
          const InternalNode* currentNode = src.node( currentNodeId );
          InternalNode* newNode = new InternalNode( currentNode->nodeVar() );

          // Recopie des fils
          for( Idx index = 0; index < currentNode->nbSons(); ++index ){
            newNode->setSon(index, currentNode->son(index));
            // Accesoirement on profite du parcours des fils pour mettre la lifo à jour
            if( !prunningTable.exists(newNode->son(index) ) ){
              lifo.push_back(newNode->son(index));
              prunningTable.insert( newNode->son(index), true );
            }
          }

          __internalNodeMap.insert( currentNodeId, newNode );
          _addElemToNICL( &(__var2NodeIdMap[ currentNode->nodeVar() ]), currentNodeId );
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

      __model.clear();
      __valueMap.clear();

      // Nettoyage des noeuds
      for( HashTableIterator<NodeId, InternalNode*> nodeIter = __internalNodeMap.begin();
            nodeIter != __internalNodeMap.end(); ++nodeIter )
        delete(*nodeIter);

      __internalNodeMap.clear();

      // Nettoyage des liste de noeuds par variables
      for( HashTableIterator<const DiscreteVariable*, NICLElem*> varIter = __var2NodeIdMap.begin();
            varIter != __var2NodeIdMap.end(); ++varIter ){
        _deleteNICL(*varIter);
      }
      __var2NodeIdMap.clear();

      for( SequenceIterator<const DiscreteVariable*> varIter = this->variablesSequence().rbegin(); varIter != this->variablesSequence().rend(); --varIter){
        this->erase(**varIter);
      }
    }


  // ############################################################################
  // @name Graph Handlers.
  // ############################################################################
    // =============================================================================
    template< typename GUM_SCALAR >
    INLINE
    std::string MultiDimDecisionGraph< GUM_SCALAR >::toDot() const {

      std::stringstream output;
      std::stringstream terminalStream;
      std::stringstream nonTerminalStream;
      std::stringstream arcstream;
//      std::stringstream defaultarcstream;
      output << std::endl << "digraph \"no_name\" {" << std::endl;

      terminalStream << "node [shape = box];" << std::endl;
      nonTerminalStream << "node [shape = ellipse];" << std::endl;
      std::string tab = "  ";

      for ( NodeGraphPart::NodeIterator nodeIter = __model.begin(); nodeIter != __model.end(); ++nodeIter )
        if ( *nodeIter != 0 ) {
          if ( __valueMap.existsFirst ( *nodeIter ) )
            terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\""<< *nodeIter << " - "
                           << __valueMap.second ( *nodeIter ) << "\"]"<< ";" << std::endl;
          else {
            InternalNode* currentNode = __internalNodeMap[ *nodeIter ];
            nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\""<< *nodeIter << " - "
                              << currentNode->nodeVar()->name() << "\"]"<< ";" << std::endl;

//              if ( _arcMap[*nodeIter] != NULL )
            for ( Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter )
              if ( currentNode->son(sonIter) != 0 )
                arcstream << tab <<  *nodeIter << " -> " << currentNode->son(sonIter)
                          << " [label=\"" << currentNode->nodeVar()->label ( sonIter ) << "\",color=\"#0000ff\"]"<< ";" << std::endl;

//              if ( _defaultArcMap.exists ( *nodeIter ) )
//                defaultarcstream << tab <<  *nodeIter << " -> " << _defaultArcMap[*nodeIter] << " [color=\"#ff0000\"]"<< ";" << std::endl;
          }
        }

        output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << "}" << std::endl;
//        << defaultarcstream.str()
        return output.str();
      }

    // ============================================================================
    // Returns a const reference to the manager of this diagram
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraphManager<GUM_SCALAR>* MultiDimDecisionGraph<GUM_SCALAR>::manager(){
      if( __manager == nullptr )
        __manager = new MultiDimDecisionGraphManager<GUM_SCALAR>(this);
      return __manager;
    }

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
    const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* MultiDimDecisionGraph<GUM_SCALAR>::node( NodeId n ) const{
      return this->__internalNodeMap[ n ];
    }

    // ============================================================================
    // Not implemented yet
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::_swap( const DiscreteVariable* x, const DiscreteVariable* y ){
      GUM_ERROR( OperationNotAllowed, "Not Implemented Yet")
    }

    // ============================================================================
    //
    // @brief Return a data, given a Instantiation.
    //
    // Note that get allows to change a value in the container.
    // The method is still tagged as const.
    //
    // @warning If i variables set is disjoint with this MultiDimContainer
    // then 0 is assumed for dimensions (i.e. variables) not prensent in the
    // instantiation.
    //
    // @param i The instantiation used to find the data.
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    GUM_SCALAR& MultiDimDecisionGraph<GUM_SCALAR>::_get ( const Instantiation &inst ) const{
      NodeId currentNodeId = __root;
      InternalNode* currentNode = nullptr;
      while ( ! isTerminalNode( currentNodeId ) ){
        currentNode = __internalNodeMap[currentNodeId];
        currentNodeId = currentNode->son( inst.val( *(currentNode->nodeVar()) ) );
      }
      __getRet = __valueMap.second( currentNodeId );
      return __getRet;
    }

} //gum
