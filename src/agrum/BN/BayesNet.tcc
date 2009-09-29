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
//                              AbstractBayesNet
// ============================================================================

template <typename T_DATA> INLINE
AbstractBayesNet<T_DATA>::AbstractBayesNet():
  __propertiesMap(0)
{
  GUM_CONSTRUCTOR( AbstractBayesNet );
}

template <typename T_DATA> INLINE
AbstractBayesNet<T_DATA>::AbstractBayesNet(const AbstractBayesNet<T_DATA>& from):
  __propertiesMap(0)
{
  GUM_CONS_CPY( AbstractBayesNet );
  if ( from.__propertiesMap != 0 ) {
    __propertiesMap = new HashTable<std::string, std::string>( *( from.__propertiesMap ) );
  }
}

template <typename T_DATA> INLINE
AbstractBayesNet<T_DATA>::~AbstractBayesNet() {
  GUM_DESTRUCTOR( AbstractBayesNet );
  // Removing previous properties
  if ( __propertiesMap != 0 ) {
    delete __propertiesMap;
  }

}

template <typename T_DATA>
AbstractBayesNet<T_DATA>&
AbstractBayesNet<T_DATA>::operator=( const AbstractBayesNet<T_DATA>& source ) {
  if (this != &source) {
    if ( __propertiesMap != 0) {
      delete __propertiesMap;
      __propertiesMap = 0;
    }
    if ( source.__propertiesMap != 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>( *( source.__propertiesMap ) );
    }
  }
  return *this;
}

template<typename T_DATA> INLINE
const std::string&
AbstractBayesNet<T_DATA>::property( const std::string& name ) const {
  try {
    return ( __properties() )[name];
  } catch ( NotFound& ) {
    std::string msg = "The following property does not exists: ";
    GUM_ERROR( NotFound, msg + name );
  }
}

template<typename T_DATA> INLINE
void
AbstractBayesNet<T_DATA>::setProperty( const std::string& name, const std::string& value ) {
  try {
    __properties()[name] = value;
  } catch ( NotFound& ) {
    __properties().insert( name, value );
  }
}

template<typename T_DATA> INLINE
const DAG::NodeIterator
AbstractBayesNet<T_DATA>::beginNodes() const { return dag().beginNodes(); }

template<typename T_DATA> INLINE
const DAG::NodeIterator
AbstractBayesNet<T_DATA>::endNodes() const { return dag().endNodes(); }

template<typename T_DATA> INLINE
const DAG::ArcIterator
AbstractBayesNet<T_DATA>::beginArcs() const { return dag().beginArcs(); }

template<typename T_DATA> INLINE
const DAG::ArcIterator&
AbstractBayesNet<T_DATA>::endArcs() const { return dag().endArcs(); }

template <typename T_DATA>
void
AbstractBayesNet<T_DATA>::_moralGraph(UndiGraph& graph) const
{
  graph.populateNodes( dag() );
  // transform the arcs into edges
  for ( DAG::ArcIterator iter = dag().beginArcs(); iter != dag().endArcs(); ++iter ) {
    graph.insertEdge( iter->first(), iter->second() );
  }
  // mary the parents
  for ( DAG::NodeIterator iter = beginNodes(); iter != endNodes(); ++iter ) {
    for ( ArcSetIterator it1 = dag().parents( *iter ).begin(); it1 != dag().parents( *iter ).end(); ++it1 ) {
      ArcSetIterator it2 = it1;
      for ( ++it2; it2 != dag().parents( *iter ).end(); ++it2 ) {
        // will automatically check if this edge already exists
        graph.insertEdge( it1->tail(), it2->tail() );
      }
    }
  }
}

template <typename T_DATA>
void
AbstractBayesNet<T_DATA>::_topologicalOrder(Sequence<NodeId>& order) const
{
  // Add all root nodes in the list
  NodeSet nodeList = dag().nodes();
  for ( NodeSetIterator iter = nodeList.begin(); iter != nodeList.end(); ++iter ) {
    if ( dag().parents( *iter ).empty() ) {
      order.insert( *iter );
      nodeList.erase( *iter );
    }
  }
  // Check if the graph has at least one root node
  if ( order.empty() ) {
    GUM_ERROR( OperationNotAllowed, "No root node in DAG." );
  }
  // Add nodes in topological order
  while (not nodeList.empty()) {
    bool add;
    // Parsing all nodes in nodeList
    for ( NodeSetIterator node = nodeList.begin(); node != nodeList.end(); ++node ) {
      add = true;
      // Parsing all parents of current node
      for ( ArcSetIterator arc = dag().parents(*node).begin(); arc != dag().parents(*node).end(); ++arc ) {
        add = add && order.exists(arc->tail() );
      }
      // If current node's parent are all in order. then we add it
      if ( add ) {
        order.insert( *node );
        nodeList.erase( *node );
      }
    }
  }
}

template<typename T_DATA> INLINE
HashTable<std::string, std::string>&
AbstractBayesNet<T_DATA>::__properties() const {
  if ( __propertiesMap == 0 ) {
    __propertiesMap = new HashTable<std::string, std::string>();
  }
  return *__propertiesMap;
}

template<typename T_DATA> INLINE
std::string
AbstractBayesNet<T_DATA>::toString( void ) const {
  Size dSize = 1;
  Size param = 0;
  for ( DAG::NodeIterator it = beginNodes();it != endNodes();++it ) {
    dSize *= variable( *it ).domainSize();
    param += (( const MultiDimImplementation<T_DATA> & )cpt( *it ).getMasterRef() ).realSize();
  }
  int compressionRatio = (int)(100.0 * (( float )1.0 - (( float )param ) / (( float )dSize )) );
  std::stringstream s;
  s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", domainSize: " << dSize << ", parameters: " << param << ", compression ratio: " << compressionRatio << "% }";
  return s.str();
}

template<typename T_DATA> INLINE
std::string
AbstractBayesNet<T_DATA>::toDot( void ) const {
  std::stringstream output;
  output << "digraph \"";
  try {
    output << property( "name" ) << "\" {" << std::endl;
  } catch ( NotFound& ) {
    output << "no_name\" {" << std::endl;
  }
  std::string tab = "  ";
  for ( gum::DAG::NodeIterator node_iter = dag().beginNodes();
        node_iter != dag().endNodes(); ++node_iter ) {
    if ( dag().children( *node_iter ).size() > 0 ) {
      for ( gum::DAG::ArcIterator arc_iter = dag().children( *node_iter ).begin();
            arc_iter != dag().children( *node_iter ).end(); ++arc_iter ) {
        output << tab << "\"" << variable( *node_iter ).name() << "\" -> "
        << "\"" << variable( arc_iter->head() ).name() << "\";" << std::endl;
      }
    } else if ( dag().parents( *node_iter ).size() == 0 ) {
      output << tab << "\"" << variable( *node_iter ).name() << "\";" << std::endl;
    }
  }
  output << "}" << std::endl;
  return output.str();
}

template <typename T_DATA>
bool
AbstractBayesNet<T_DATA>::operator==(const AbstractBayesNet<T_DATA>& from) const
{
  if (dag() == from.dag()) {
    for (DAG::NodeIterator node = beginNodes(); node != endNodes(); ++node) {
      // We don't use Potential::operator== because BN's don't share
      // DiscreteVariable's pointers.
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
      bijection.insert(&(variable(*node)), &(from.variable(*node)));
      for (DAG::ArcIterator arc = dag().parents(*node).begin(); arc != dag().parents(*node).end(); ++arc) {
        bijection.insert(&(variable(arc->tail())), &(from.variable(arc->tail())));
      }
      Instantiation i(cpt(*node));
      Instantiation j(from.cpt(*node));
      for (i.setFirst(); not i.end(); i.inc()) {
        typedef Bijection<const DiscreteVariable*, const DiscreteVariable*>::iterator BiIter;
        for (BiIter iter = bijection.begin(); iter != bijection.end(); ++iter) {
          j.chgVal(*(iter.second()), i.val(*(iter.first())));
        }
        if (std::pow(cpt(*node).get(i) - from.cpt(*node).get(j), (T_DATA) 2) > (T_DATA) 1e-6) {
          return false;
        }
      }
    }
    return true;
  }
  return false;
}

template <typename T_DATA>
bool
AbstractBayesNet<T_DATA>::operator!=(const AbstractBayesNet<T_DATA>& from) const
{
  return not this->operator==(from);
}

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
