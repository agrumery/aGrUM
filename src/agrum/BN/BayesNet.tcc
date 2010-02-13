/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Template implementation of bns/bayesNet.h classes.
 *
 * @author Lionel Torti and Pierre-Henri Wuillemin
 */
// ============================================================================
#include <agrum/BN/BayesNet.h>
// ============================================================================
namespace gum {
// ============================================================================
//                                  BayesNet
// ============================================================================

template<typename T_DATA> INLINE
BayesNet<T_DATA>::BayesNet():
  AbstractBayesNet<T_DATA>(),
  __moralGraph( 0 ), __topologicalOrder( 0 )
{
  GUM_CONSTRUCTOR( BayesNet );
  __topologicalOrder = new Sequence<NodeId>();
  __moralGraph = new UndiGraph();
}

template<typename T_DATA>
BayesNet<T_DATA>::BayesNet( const BayesNet<T_DATA>& source ) :
    AbstractBayesNet<T_DATA>(source),
    __dag( source.__dag ), __varMap( source.__varMap ),
    __moralGraph( 0 ), __topologicalOrder( 0 )
{
  GUM_CONS_CPY( BayesNet );
  Potential<T_DATA> *copyPtr = 0;
  __moralGraph = new UndiGraph( *( source.__moralGraph ) );
  __topologicalOrder = new Sequence<NodeId>( *( source.__topologicalOrder ) );
  // Copying potentials
  typedef HashTableConstIterator<NodeId, Potential<T_DATA>*> PotIterator;
  for (PotIterator cptIter = source.__probaMap.begin(); cptIter != source.__probaMap.end(); ++cptIter) {
    // First we build the node's CPT
    copyPtr = new Potential<T_DATA>();
    ( *copyPtr ) << variable( cptIter.key() );
    // Add it's parents
    const ArcSet& parentArcs = __dag.parents( cptIter.key() );
    for ( ArcSetIterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter ) {
      ( *copyPtr ) << variable(( *arcIter ).tail() );
    }
    // Second, we fill the CPT
    Instantiation srcInst( **cptIter );
    Instantiation cpyInst( *copyPtr );
    // Slowest but safest
    for ( cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc() ) {
      for ( Idx i = 0; i < cpyInst.nbrDim(); i++ ) {
        NodeId id = nodeId( cpyInst.variable( i ) );
        srcInst.chgVal( source.variable( id ), cpyInst.val( i ) );
      }
      copyPtr->set( cpyInst, ( **cptIter )[srcInst] );
    }
    // We add the CPT to the CPT's hashmap
    __probaMap.insert( cptIter.key(), copyPtr );
  }
}

template<typename T_DATA>
BayesNet<T_DATA>&
BayesNet<T_DATA>::operator=( const BayesNet<T_DATA>& source ) {
  if (this != &source) {
    AbstractBayesNet<T_DATA>::operator=(source);
    __dag = source.dag();
    __varMap = source.__varMap;
    // Removing previous potentials
    for ( HashTableConstIterator< NodeId, Potential<T_DATA>* > iter = __probaMap.begin(); iter != __probaMap.end(); ++iter ) {
      delete *iter;
    }
    __probaMap.clear();
    // Copying potentials
    typedef HashTableConstIterator<NodeId, Potential<T_DATA>*> PotIterator;
    for (PotIterator cptIter = source.__probaMap.begin(); cptIter != source.__probaMap.end(); ++cptIter) {
      // First we build the node's CPT
      Potential<T_DATA>* copyPtr = new Potential<T_DATA>();
      ( *copyPtr ) << variable( cptIter.key() );
      // Add it's parents
      const ArcSet& parentArcs = __dag.parents( cptIter.key() );
      for ( ArcSetIterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter ) {
        ( *copyPtr ) << variable(( *arcIter ).tail() );
      }
      // Second, we fill the CPT
      Instantiation srcInst( **cptIter );
      Instantiation cpyInst( *copyPtr );
      // Slowest but safest
      for ( cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc() ) {
        for ( Idx i = 0; i < cpyInst.nbrDim(); i++ ) {
          NodeId id = nodeId( cpyInst.variable( i ) );
          srcInst.chgVal( source.variable( id ), cpyInst.val( i ) );
        }
        copyPtr->set( cpyInst, ( **cptIter )[srcInst] );
      }
      // We add the CPT to the CPT's hashmap
      __probaMap.insert( cptIter.key(), copyPtr );
    }
    ( *__topologicalOrder ) = *( source.__topologicalOrder );
    ( *__moralGraph ) = *( source.__moralGraph );
  }
  return *this;
}

template<typename T_DATA>
BayesNet<T_DATA>::~BayesNet() {
  GUM_DESTRUCTOR( BayesNet );
  for ( HashTableConstIterator<NodeId, Potential<T_DATA>*> iter = __probaMap.begin();
        iter != __probaMap.end();
        ++iter ) {
    delete *iter;
  }
  delete __moralGraph;
  delete __topologicalOrder;
}

template<typename T_DATA> INLINE
const DiscreteVariable&  BayesNet<T_DATA>::variable( NodeId id ) const {
  return __varMap.get( id );
}

template<typename T_DATA> INLINE
void BayesNet<T_DATA>::changeVariableName(NodeId id,const std::string& new_name) {
  __varMap.changeName(id,new_name);
}

template<typename T_DATA> INLINE
NodeId BayesNet<T_DATA>::nodeId( const DiscreteVariable &var ) const {
  return __varMap.get( var );
}

template<typename T_DATA> INLINE
NodeId
BayesNet<T_DATA>::add( const DiscreteVariable& var , NodeId id ) {
  MultiDimArray<T_DATA>* ptr = new MultiDimArray<T_DATA>();
  NodeId res = 0;
  try {
    res = add( var, ptr, id );
  } catch ( Exception& e ) {
    delete ptr;
    throw;
  }
  return res;
}

template<typename T_DATA>
NodeId
BayesNet<T_DATA>::add( const DiscreteVariable& var, MultiDimImplementation<T_DATA> *aContent , NodeId id ) {
  // this code is not thread safe !!!!
  NodeId proposedId;
  if ( id == 0 ) {
    proposedId = __dag.nextNodeId();
  } else {
    proposedId = id;
  }
  __varMap.insert( proposedId, var );
  __dag.insertNode( proposedId );
  // end of not thread safe code !!!
  Potential<T_DATA> *cpt = new Potential<T_DATA>( aContent );
  ( *cpt ) << variable( proposedId );
  __probaMap.insert( proposedId, cpt );
  return proposedId;
}

template<typename T_DATA> INLINE
NodeId
BayesNet<T_DATA>::addVariable( const DiscreteVariable& var ) {
  return add( var );
}

template<typename T_DATA> INLINE
NodeId
BayesNet<T_DATA>::addVariable( const DiscreteVariable& var,
                               MultiDimImplementation<T_DATA> *aContent ) {
  return add( var, aContent );
}

template<typename T_DATA> INLINE
NodeId BayesNet<T_DATA>::idFromName( const std::string& name ) const {
  return __varMap.idFromName(name);
}

template<typename T_DATA> INLINE
const DiscreteVariable& BayesNet<T_DATA>::variableFromName( const std::string& name ) const {
  return __varMap.variableFromName(name);
}

template<typename T_DATA> INLINE
const Potential<T_DATA>&
BayesNet<T_DATA>::cpt( NodeId varId ) const { return *( __probaMap[varId] ); }

template<typename T_DATA> INLINE
const DAG&
BayesNet<T_DATA>::dag() const { return __dag; }

template<typename T_DATA> INLINE
const VariableNodeMap&
BayesNet<T_DATA>::variableNodeMap() const { return __varMap; }

template<typename T_DATA> INLINE
Idx
BayesNet<T_DATA>::size() const { return __dag.size(); }

template<typename T_DATA> INLINE
bool
BayesNet<T_DATA>::empty() const { return size() == 0; }

template<typename T_DATA> INLINE
void
BayesNet<T_DATA>::eraseVariable( NodeId varId ) { erase( varId ); }

template<typename T_DATA> INLINE
void
BayesNet<T_DATA>::erase( const DiscreteVariable& var ) {
  erase( __varMap.get( var ) );
}

template<typename T_DATA>
void
BayesNet<T_DATA>::erase( NodeId varId ) {
  if ( __varMap.exists( varId ) ) {
    // Reduce the variable child's CPT
    for ( ArcSetIterator iter = __dag.children( varId ).begin(); iter != __dag.children( varId ).end(); ++iter ) {
      __probaMap[iter->head()]->erase( variable( varId ) );
    }
    delete __probaMap[varId];
    __probaMap.erase( varId );
    __varMap.erase( varId );
    __dag.eraseNode( varId );
  }
}

template<typename T_DATA> INLINE
void
BayesNet<T_DATA>::insertArc( NodeId tail, NodeId head ) {
  __dag.insertArc( tail, head );
  // Add parent in the child's CPT
  ( *( __probaMap[head] ) ) << variable( tail );
}

template<typename T_DATA> INLINE
void
BayesNet<T_DATA>::eraseArc( const Arc& arc ) {
  if ( __varMap.exists( arc.tail() ) && __varMap.exists( arc.head() ) ) {
    NodeId head = arc.head(), tail = arc.tail();
    __dag.eraseArc( arc );
    // Remove parent froms child's CPT
    ( *( __probaMap[head] ) ) >> variable( tail );
  }
}

template<typename T_DATA> INLINE
void
BayesNet<T_DATA>::eraseArc( NodeId tail, NodeId head ) {
  eraseArc( Arc( tail, head ) );
}

template<typename T_DATA>
const UndiGraph&
BayesNet<T_DATA>::moralGraph( bool clear ) const {
  if ( clear or __moralGraph == 0 ) {
    if ( __moralGraph != 0 ) {delete __moralGraph;}
    __moralGraph = new UndiGraph();
    AbstractBayesNet<T_DATA>::_moralGraph(*__moralGraph);
  }
  return *__moralGraph;
}

template<typename T_DATA>
const Sequence<NodeId>&
BayesNet<T_DATA>::getTopologicalOrder( bool clear ) const {
  if ( clear or( __topologicalOrder->empty() ) ) {
    __topologicalOrder->clear();
    AbstractBayesNet<T_DATA>::_topologicalOrder(*__topologicalOrder);
  }
  return *__topologicalOrder;
}

template<typename T_DATA> INLINE
NodeId
BayesNet<T_DATA>::addNoisyOR( const DiscreteVariable& var , T_DATA external_weight, NodeId id ) {
  return add( var, new MultiDimNoisyOR<T_DATA>( external_weight ) , id );
}

template<typename T_DATA>
void
BayesNet<T_DATA>::insertArcNoisyOR( NodeId tail, NodeId head, T_DATA causalWeight ) {
  const MultiDimAdressable& content = cpt( head ).getMasterRef();
  const MultiDimNoisyOR<T_DATA>* noisy = dynamic_cast<const MultiDimNoisyOR<T_DATA>*>( &content );
  if ( noisy == 0 ) {
    GUM_ERROR( InvalidArc, "This head is not a noisyOR variable !" );
  }
  // or is OK
  insertArc( tail, head );
  noisy->causalWeight( variable( tail ), causalWeight );
}

template<typename T_DATA>
std::ostream&
operator<<( std::ostream& output, const AbstractBayesNet<T_DATA>& map ) {
  output << map.toString();
  return output;
}

} /* namespace gum */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
