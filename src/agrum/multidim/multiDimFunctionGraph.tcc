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
 * @brief Template methods of MultiDimFunctionGraph.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe Magnan
 */
#include <agrum/multidim/multiDimFunctionGraph.h>

namespace gum {

  // Default constructor.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::MultiDimFunctionGraph(
      bool isReduced )
      : MultiDimImplementation<GUM_SCALAR>()
      , __name( "MultiDimFunctionGraph" )
      , __tableName( "NO NAME" )
      , __model( 500, true )
      , __manager( nullptr )
      , __root( 0 )
      , __internalNodeMap( 500, true, false )
      , __var2NodeIdMap( 500, true, false )
      , __isReduced( isReduced ) {

    GUM_CONSTRUCTOR( MultiDimFunctionGraph );
    __manager = nullptr;
    // Pop up a first node so that id 0 is unavailable
    __model.addNode();
  }

  // Copy constructor.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::MultiDimFunctionGraph(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& from )
      : MultiDimImplementation<GUM_SCALAR>()
      , __name( "MultiDimFunctionGraph" )
      , __tableName( "No NAME" )
      , __model( 500, true )
      , __manager( nullptr )
      , __root( 0 )
      , __internalNodeMap( 500, true, false )
      , __var2NodeIdMap( 500, true, false )
      , __isReduced( from.isReducedAndOrdered() ) {

    GUM_CONS_CPY( MultiDimFunctionGraph );
    copy( from );
  }

  // Copy Operator.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>&
         MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::
  operator=( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& from ) {
    copy( from );
    return *this;
  }

  // Destructor.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraph<GUM_SCALAR,
                               TerminalNodePolicy>::~MultiDimFunctionGraph() {
    // Manager deletion
    GUM_DESTRUCTOR( MultiDimFunctionGraph );
    if ( __manager != nullptr ) delete __manager;
    this->clear();
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimContainer<GUM_SCALAR>*
         MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::newFactory() const {
    if ( __isReduced )
      return MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::
          getReducedAndOrderedInstance();
    else
      return MultiDimFunctionGraph<GUM_SCALAR,
                                   TerminalNodePolicy>::getTreeInstance();
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const std::string&
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::name() const {
    return __name;
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::set(
      const Instantiation& i, const GUM_SCALAR& value ) const {
    GUM_ERROR( OperationNotAllowed,
               "Function Graph can't be edited so "
               "easily.\nMultiDimFunctionGraphManager "
               "provides the framework to edit a "
               "Function Graph." )
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::fill(
      const GUM_SCALAR& d ) const {
    GUM_ERROR( OperationNotAllowed,
               "Function Graph can't be edited so "
               "easily.\nMultiDimFunctionGraphManager "
               "provides the framework to edit a "
               "Function Graph." )
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::populate(
      const std::vector<GUM_SCALAR>& v ) const {
    GUM_ERROR( OperationNotAllowed,
               "Function Graph can't be edited so "
               "easily.\nMultiDimFunctionGraphManager "
               "provides the framework to editaa "
               "Function Graph." )
  }
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::populate(
      std::initializer_list<GUM_SCALAR> l ) const {
    GUM_ERROR( OperationNotAllowed,
               "Function Graph can't be edited so "
               "easily.\nMultiDimFunctionGraphManager "
               "provides the framework to edit a "
               "Function Graph." )
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::add(
      const DiscreteVariable& v ) {

    if ( !this->variablesSequence().exists( &v ) )
      MultiDimImplementation<GUM_SCALAR>::add( v );

    if ( !this->__var2NodeIdMap.exists( &v ) )
      __var2NodeIdMap.insert( &v, new LinkedList<NodeId>() );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::erase(
      const DiscreteVariable& v ) {
    if ( this->__var2NodeIdMap.exists( &v ) ) {
      while ( __var2NodeIdMap[&v]->list() != nullptr ) {
        manager()->eraseNode( __var2NodeIdMap[&v]->list()->element() );
      }
      delete __var2NodeIdMap[&v];
      __var2NodeIdMap.erase( &v );
    }

    if ( this->variablesSequence().exists( &v ) )
      MultiDimImplementation<GUM_SCALAR>::erase( v );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE Size
         MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::realSize() const {
    return __internalNodeMap.size();  // + __valueMap.size();
  }


  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::changeNotification(
      Instantiation&                i,
      const DiscreteVariable* const var,
      const Idx&                    oldval,
      const Idx&                    newval ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setFirstNotification(
      Instantiation& i ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setLastNotification(
      Instantiation& i ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setIncNotification(
      Instantiation& i ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setDecNotification(
      Instantiation& i ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setChangeNotification(
      Instantiation& i ) {}

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const std::string
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::toString(
      const Instantiation* i ) const {
    std::stringstream sBuff;
    sBuff << ( *i ) << " = " << this->get( *i );
    return sBuff.str();
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::copyFrom(
      const MultiDimContainer<GUM_SCALAR>& src, Instantiation* p_i ) const {
    GUM_ERROR( OperationNotAllowed,
               "You cannot copy another type of multiDim "
               "into a MultiDimFunctionGraph." );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::copy(
      const MultiDimContainer<GUM_SCALAR>& src ) {
    GUM_ERROR( OperationNotAllowed,
               "You cannot copy another type of multiDim "
               "into a MultiDimFunctionGraph." );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::copy(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& src ) {

    if ( this->__isReduced != src.isReducedAndOrdered() )
      GUM_ERROR( OperationNotAllowed,
                 "Cannot copy a Reduced and Ordered "
                 "function graph into Tree function graph "
                 "(or vice-versa)." )

    this->clear();

    // New variables insertion
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              src.variablesSequence().beginSafe();
          varIter != src.variablesSequence().endSafe();
          ++varIter )
      this->add( **varIter );

    std::vector<NodeId> lifo;
    Bijection<NodeId, NodeId> src2dest;

    if ( src.isTerminalNode( src.root() ) )
      this->manager()->setRootNode(
          this->manager()->addTerminalNode( src.nodeValue( src.root() ) ) );
    else {
      this->manager()->setRootNode(
          this->manager()->addInternalNode( src.node( src.root() )->nodeVar() ) );
      src2dest.insert( src.root(), this->root() );
      lifo.push_back( src.root() );
    }

    // Depth-first exploration and copy
    while ( !lifo.empty() ) {
      NodeId currentSrcNodeId = lifo.back();
      lifo.pop_back();

      const InternalNode* currentSrcNode = src.node( currentSrcNodeId );

      for ( Idx index = 0; index < currentSrcNode->nbSons(); ++index ) {
        if ( !src2dest.existsFirst( currentSrcNode->son( index ) ) ) {
          NodeId srcSonNodeId = currentSrcNode->son( index ), destSonNodeId = 0;
          if ( src.isTerminalNode( srcSonNodeId ) ) {
            destSonNodeId =
                this->manager()->addTerminalNode( src.nodeValue( srcSonNodeId ) );
          } else {
            destSonNodeId = this->manager()->addInternalNode(
                src.node( srcSonNodeId )->nodeVar() );
            lifo.push_back( srcSonNodeId );
          }
          src2dest.insert( srcSonNodeId, destSonNodeId );
        }
        this->manager()->setSon( src2dest.second( currentSrcNodeId ),
                                 index,
                                 src2dest.second( currentSrcNode->son( index ) ) );
      }
    }

    manager()->clean();
  }

  // Copies src diagrams structure into this diagrams.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::copyAndReassign(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& src,
      const Bijection<const DiscreteVariable*, const DiscreteVariable*>&
          reassign ) {

    if ( this->__isReduced != src.isReducedAndOrdered() )
      GUM_ERROR( OperationNotAllowed,
                 "Cannot copy a Reduced and Ordered "
                 "function graph into Tree function graph "
                 "(or vice-versa)." )

    this->clear();

    // New variables insertion
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              src.variablesSequence().beginSafe();
          varIter != src.variablesSequence().endSafe();
          ++varIter ) {
      if ( ( *varIter )->domainSize() !=
           reassign.second( *varIter )->domainSize() )
        GUM_ERROR( InvalidArgument,
                   "Var " << ( *varIter )->name() << " and var "
                          << reassign.second( *varIter )->name()
                          << " have different domain sizes ("
                          << ( *varIter )->domainSize()
                          << "!="
                          << reassign.second( *varIter )->domainSize()
                          << ")" )
      this->add( *( reassign.second( *varIter ) ) );
    }

    std::vector<NodeId> lifo;
    Bijection<NodeId, NodeId> src2dest;

    if ( src.isTerminalNode( src.root() ) ) {
      this->manager()->setRootNode(
          this->manager()->addTerminalNode( src.nodeValue( src.root() ) ) );
    } else {
      this->manager()->setRootNode( this->manager()->addInternalNode(
          reassign.second( src.node( src.root() )->nodeVar() ) ) );
      src2dest.insert( src.root(), this->root() );
      lifo.push_back( src.root() );
    }

    // Depth-first exploration and copy
    while ( !lifo.empty() ) {
      NodeId currentSrcNodeId = lifo.back();
      lifo.pop_back();

      const InternalNode* currentSrcNode = src.node( currentSrcNodeId );

      for ( Idx index = 0; index < currentSrcNode->nbSons(); ++index ) {
        if ( !src2dest.existsFirst( currentSrcNode->son( index ) ) ) {
          NodeId srcSonNodeId = currentSrcNode->son( index ), destSonNodeId = 0;
          if ( src.isTerminalNode( srcSonNodeId ) ) {
            destSonNodeId =
                this->manager()->addTerminalNode( src.nodeValue( srcSonNodeId ) );
          } else {
            destSonNodeId = this->manager()->addInternalNode(
                reassign.second( src.node( srcSonNodeId )->nodeVar() ) );
            lifo.push_back( srcSonNodeId );
          }
          src2dest.insert( srcSonNodeId, destSonNodeId );
        }
        this->manager()->setSon( src2dest.second( currentSrcNodeId ),
                                 index,
                                 src2dest.second( currentSrcNode->son( index ) ) );
      }
    }

    manager()->clean();
  }

  // Copies src diagrams and multiply every value by the given scalar.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::copyAndMultiplyByScalar(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>& src,
      GUM_SCALAR gamma ) {


    if ( this->__isReduced != src.isReducedAndOrdered() )
      GUM_ERROR( OperationNotAllowed,
                 "Cannot copy a Reduced and Ordered "
                 "function graph into Tree function graph "
                 "(or vice-versa)." )

    this->clear();

    // New variables insertion
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              src.variablesSequence().beginSafe();
          varIter != src.variablesSequence().endSafe();
          ++varIter )
      this->add( **varIter );

    std::vector<NodeId> lifo;
    HashTable<NodeId, NodeId> src2dest;

    if ( src.isTerminalNode( src.root() ) )
      this->manager()->setRootNode( this->manager()->addTerminalNode(
          gamma * src.nodeValue( src.root() ) ) );
    else {
      this->manager()->setRootNode(
          this->manager()->addInternalNode( src.node( src.root() )->nodeVar() ) );
      src2dest.insert( src.root(), this->root() );
      lifo.push_back( src.root() );
    }

    // Depth-first exploration an copy
    while ( !lifo.empty() ) {
      NodeId currentSrcNodeId = lifo.back();
      lifo.pop_back();

      const InternalNode* currentSrcNode = src.node( currentSrcNodeId );

      for ( Idx index = 0; index < currentSrcNode->nbSons(); ++index ) {
        if ( !src2dest.exists( currentSrcNode->son( index ) ) ) {
          NodeId srcSonNodeId = currentSrcNode->son( index ), destSonNodeId = 0;
          if ( src.isTerminalNode( srcSonNodeId ) ) {
            destSonNodeId = this->manager()->addTerminalNode(
                gamma * src.nodeValue( srcSonNodeId ) );
          } else {
            destSonNodeId = this->manager()->addInternalNode(
                src.node( srcSonNodeId )->nodeVar() );
            lifo.push_back( srcSonNodeId );
          }
          src2dest.insert( srcSonNodeId, destSonNodeId );
        }
        this->manager()->setSon( src2dest[currentSrcNodeId],
                                 index,
                                 src2dest[currentSrcNode->son( index )] );
      }
    }

    manager()->clean();
  }

  // Clears the function graph
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::clear() {
    __model.clear();
    // Always discard the nodeId 0
    __model.addNode();

    this->clearAllTerminalNodes();

    // Nodes cleaning
    for ( HashTableIterator<NodeId, InternalNode*> nodeIter =
              __internalNodeMap.begin();
          nodeIter != __internalNodeMap.end();
          ++nodeIter ) {

      delete nodeIter.val();
    }
    __internalNodeMap.clear();

    // Cleaning the list of nodes for each variables
    for ( HashTableIterator<const DiscreteVariable*, LinkedList<NodeId>*> varIter =
              __var2NodeIdMap.begin();
          varIter != __var2NodeIdMap.end();
          ++varIter ) {
      delete varIter.val();
    }
    __var2NodeIdMap.clear();

    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              this->variablesSequence().rbeginSafe();
          varIter != this->variablesSequence().rendSafe();
          --varIter ) {
      this->erase( **varIter );
    }
  }


  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE std::string MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::toDot(
      bool withBackArcs ) const {

    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;
    //      std::stringstream defaultarcstream;
    output << std::endl << "digraph \" " << __tableName << "\" {" << std::endl;

    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for ( NodeGraphPart::NodeIterator nodeIter = __model.begin();
          nodeIter != __model.end();
          ++nodeIter )
      if ( *nodeIter != 0 ) {
        if ( this->isTerminalNode( (NodeId)*nodeIter ) )
          terminalStream << tab << *nodeIter << ";" << tab << *nodeIter
                         << " [label=\"" << *nodeIter << " - "
                         << std::setprecision( 30 )
                         << this->terminalNodeValue( *nodeIter ) << "\"]"
                         << ";" << std::endl;
        else {
          InternalNode* currentNode = __internalNodeMap[*nodeIter];
          nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter
                            << " [label=\"" << *nodeIter << " - "
                            << currentNode->nodeVar()->name() << "\"]"
                            << ";" << std::endl;

          //              if (_arcMap[*nodeIter] != NULL)
          HashTable<NodeId, LinkedList<Idx>*> sonMap;
          for ( Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter ) {
            if ( !sonMap.exists( currentNode->son( sonIter ) ) )
              sonMap.insert( currentNode->son( sonIter ), new LinkedList<Idx>() );
            sonMap[currentNode->son( sonIter )]->addLink( sonIter );
          }

          for ( auto sonIter = sonMap.beginSafe(); sonIter != sonMap.endSafe();
                ++sonIter ) {
            arcstream << tab << *nodeIter << " -> " << sonIter.key()
                      << " [label=\" ";
            Link<Idx>* modaIter = sonIter.val()->list();
            while ( modaIter ) {
              arcstream << currentNode->nodeVar()->label( modaIter->element() )
                        << ", ";
              modaIter = modaIter->nextLink();
            }
            arcstream << "\",color=\"#0000ff\"]"
                      << ";" << std::endl;
            delete sonIter.val();
          }

          if ( withBackArcs ) {
            Link<Parent>* parentIter = currentNode->parents();
            while ( parentIter != nullptr ) {
              arcstream << tab << *nodeIter << " -> "
                        << parentIter->element().parentId << " [label=\""
                        << parentIter->element().modality
                        << "\",color=\"#ff0000\"]"
                        << ";" << std::endl;
              parentIter = parentIter->nextLink();
            }
          }
        }
      }

    output << terminalStream.str() << std::endl
           << nonTerminalStream.str() << std::endl
           << arcstream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }

  // Returns a const reference to the manager of this diagram
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const NodeGraphPart&
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::model() const {
    return __model;
  }

  // Returns a const reference to the manager of this diagram
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>*
         MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::manager() {
    if ( __manager == nullptr ) {
      if ( __isReduced )
        __manager =
            new MultiDimFunctionGraphROManager<GUM_SCALAR, TerminalNodePolicy>(
                this );
      else
        __manager =
            new MultiDimFunctionGraphTreeManager<GUM_SCALAR, TerminalNodePolicy>(
                this );
    }
    return __manager;
  }

  // Returns the id of the root node from the diagram
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const NodeId&
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::root() const {
    return __root;
  }

  // Indicates if given node is terminal or not
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE bool
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::isTerminalNode(
      const NodeId& node ) const {
    return this->existsTerminalNodeWithId( node );
  }

  // Indicates if given node is terminal or not
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE bool
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::isInternalNode(
      const NodeId& node ) const {
    return this->__internalNodeMap.exists( node );
  }

  // Returns value associated to given node.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const GUM_SCALAR&
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::nodeValue(
      NodeId n ) const {
    if ( !isTerminalNode( n ) )
      GUM_ERROR( InvalidArgument,
                 "Id " << n << " is not bound to any terminal node" )
    return this->terminalNodeValue( n );
  }

  // Returns internalNode structure associated to that nodeId
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const InternalNode*
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::node( NodeId n ) const {
    if ( !isInternalNode( n ) )
      GUM_ERROR( InvalidArgument,
                 "Id " << n << " is not bound to any terminal node" )
    return this->__internalNodeMap[n];
  }

  // Returns the list of node associated to given variable
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const LinkedList<NodeId>*
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::varNodeListe(
      const DiscreteVariable* var ) const {
    if ( !this->variablesSequence().exists( var ) )
      GUM_ERROR( InvalidArgument,
                 "Var " << var->name()
                        << " has not been inserted in the function graph" )
    return __var2NodeIdMap[var];
  }

  // Returns the name of the table represented by this structure.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE const std::string&
               MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::tableName() const {
    return __tableName;
  }

  // Sets the name of the table represented by this structure.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::setTableName(
      const std::string& name ) {
    __tableName = name;
  }

  // Returns true if this MultiDimFunctionGraph is reduced and Ordered.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE bool
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::isReducedAndOrdered()
      const {
    return __isReduced;
  }

  // Returns a reduced and ordered instance.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
         MultiDimFunctionGraph<GUM_SCALAR,
                        TerminalNodePolicy>::getReducedAndOrderedInstance() {
    return new MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>();
  }

  // Returns an arborescent instance
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
         MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::getTreeInstance() {
    return new MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>( false );
  }

  // Not implemented yet
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::_swap(
      const DiscreteVariable* x, const DiscreteVariable* y ) {
    GUM_ERROR( OperationNotAllowed, "Not Implemented Yet" )
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE GUM_SCALAR& MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::_get(
      const Instantiation& inst ) const {
    GUM_ERROR( OperationNotAllowed,
               "You can't edit a function by other mean than the manager" );
  }

  // Return a data, given a Instantiation.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE GUM_SCALAR MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::get(
      const Instantiation& inst ) const {
    NodeId        currentNodeId = __root;
    InternalNode* currentNode = nullptr;
    while ( !isTerminalNode( currentNodeId ) ) {
      currentNode = __internalNodeMap[currentNodeId];
      currentNodeId = currentNode->son( inst.val( *( currentNode->nodeVar() ) ) );
    }
    return this->terminalNodeValue( currentNodeId );
  }

}  // gum
