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

#include <agrum/BN/BayesNet.h>
// ============================================================================

namespace gum {

// Default constructor
  template<typename T_DATA> INLINE
  BayesNet<T_DATA>::BayesNet():
      __propertiesMap( 0 ), __moralGraph( 0 ), __topologicalOrder( 0 ) {
    GUM_CONSTRUCTOR( BayesNet );
    __topologicalOrder = new Sequence<NodeId>();
    __moralGraph = new UndiGraph();
  }

// Copy constructor
  template<typename T_DATA> BayesNet<T_DATA>::BayesNet( const BayesNet<T_DATA>& source ) :
      __propertiesMap( 0 ),
      __dag( source.__dag ),
      __varMap( source.__varMap ),
      __moralGraph( 0 ), __topologicalOrder( 0 ) {
    GUM_CONSTRUCTOR( BayesNet );

    if ( source.__propertiesMap != 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>( *( source.__propertiesMap ) );
    }

    Potential<T_DATA> *sourcePtr = 0;

    Potential<T_DATA> *copyPtr = 0;
    __moralGraph = new UndiGraph( *( source.__moralGraph ) );
    __topologicalOrder = new Sequence<NodeId>( *( source.__topologicalOrder ) );

    for ( HashTableConstIterator<NodeId, Potential<T_DATA>*> cptIter = source.__probaMap.begin(); cptIter != source.__probaMap.end(); ++cptIter ) {
      // First we build the node's CPT
      copyPtr = new Potential<T_DATA>();
      ( *copyPtr ) << variable( cptIter.key() );
      // Add it's parents
      const ArcSet& parentArcs = __dag.parents( cptIter.key() );

      for ( ArcSetIterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter ) {
        ( *copyPtr ) << variable(( *arcIter ).tail() );
      }

      // Second, we fill the CPT
      sourcePtr = *cptIter;

      Instantiation srcInst( *sourcePtr );

      Instantiation cpInst( *copyPtr );

      // Slowest but safest
      for ( cpInst.setFirst(); !cpInst.end(); cpInst.inc() ) {
        for ( Idx i = 0; i < cpInst.nbrDim(); i++ ) {
          NodeId id = nodeId( cpInst.variable( i ) );
          srcInst.chgVal( source.variable( id ), cpInst.val( i ) );
        }

        copyPtr->set( cpInst, ( *sourcePtr )[srcInst] );
      }

      // We add the CPT to the CPT's hashmap
      __probaMap.insert( cptIter.key(), copyPtr );
    }
  }

// Copy Operator
  template<typename T_DATA>
  BayesNet<T_DATA>&
  BayesNet<T_DATA>::operator=( const BayesNet<T_DATA>& source ) {
    // Removing previous properties
    if ( __propertiesMap != 0 ) {
      delete __propertiesMap;
    }

    if ( source.__propertiesMap != 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>( *( source.__propertiesMap ) );
    }

    // Removing previous potentials
    for ( HashTableConstIterator< NodeId, Potential<T_DATA>* > iter = __probaMap.begin(); iter != __probaMap.end(); ++iter ) {
      delete *iter;
    }

    __varMap = source.__varMap;

    __probaMap.clear();

    Potential<T_DATA> *sourcePtr, *copyPtr;

    for ( typename Property<Potential<T_DATA>*>::onNodes::const_iterator iter = source.__probaMap.begin(); iter != source.__probaMap.end(); ++iter ) {
      sourcePtr = *iter;
      copyPtr = new Potential<T_DATA>( *sourcePtr );
      __probaMap.insert( iter.key(), copyPtr );
    }

    __topologicalOrder->clear();

    ( *__topologicalOrder ) = *( source->__topologicalOrder );
    __moralGraph->clear();
    ( *__moralGraph ) = *( source->__moralGraph );

    return *this;
  }

// Destructor
  template<typename T_DATA>
  BayesNet<T_DATA>::~BayesNet() {
    GUM_DESTRUCTOR( BayesNet );

    if ( __propertiesMap != 0 ) {
      delete __propertiesMap;
    }

    for ( HashTableConstIterator<NodeId, Potential<T_DATA>*> iter = __probaMap.begin();
          iter != __probaMap.end();
          ++iter ) {
      delete *iter;
    }

    delete __moralGraph;

    delete __topologicalOrder;
  }

  // Returns a constant reference over a variabe given it's node id.
  template<typename T_DATA> INLINE
  const DiscreteVariable&  BayesNet<T_DATA>::variable( NodeId id ) const {
    return __varMap.get( id );
  }

  // Return id node from discrete var pointer.
  template<typename T_DATA> INLINE
  NodeId BayesNet<T_DATA>::nodeId( const DiscreteVariable &var ) const {
    return __varMap.get( var );
  }

// Return the value of the property "name" of this BayesNet.
// @throw NotFound Raised if no "name" property is found.
  template<typename T_DATA> INLINE
  const std::string&
  BayesNet<T_DATA>::property( const std::string& name ) const {
    try {
      return ( __properties() )[name];
    } catch ( NotFound& ) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR( NotFound, msg + name );
    }
  }

// Add or change a property of this BayesNet.
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::setProperty( const std::string& name, const std::string& value ) {
    try {
      __properties()[name] = value;
    } catch ( NotFound& ) {
      __properties().insert( name, value );
    }
  }

// Add a variable, it's associate node and it's CPT
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

// Add a variable, it's associate node and it's CPT
  template<typename T_DATA>
  NodeId
  BayesNet<T_DATA>::add( const DiscreteVariable& var, MultiDimImplementation<T_DATA> *aContent , NodeId id ) {
    // this code is not thread safe !!!!
    NodeId proposedId;

    if ( id == 0 )
      proposedId = __dag.nextNodeId();
    else
      proposedId = id;

    __varMap.insert( proposedId, var );

    __dag.insertNode( proposedId );

    // end of not thread safe code !!!

    Potential<T_DATA> *cpt = new Potential<T_DATA>( aContent );

    ( *cpt ) << variable( proposedId );

    __probaMap.insert( proposedId, cpt );

    return proposedId;
  }

// Deprecated!
  template<typename T_DATA> INLINE
  NodeId
  BayesNet<T_DATA>::addVariable( const DiscreteVariable& var ) {
    return add( var );
  }

// Deprecated!
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

  // Returns the CPT of a variable
  template<typename T_DATA> INLINE
  const Potential<T_DATA>&
  BayesNet<T_DATA>::cpt( NodeId varId ) const { return *( __probaMap[varId] ); }

// Returns the DAG of this bayes net
  template<typename T_DATA> INLINE
  const DAG&
  BayesNet<T_DATA>::dag() const { return __dag; }

// Returns the number of variables in this bayes net.
  template<typename T_DATA> INLINE
  Idx
  BayesNet<T_DATA>::size() const { return __dag.size(); }

// Retursn true if this bayes net is empty.
  template<typename T_DATA> INLINE
  bool
  BayesNet<T_DATA>::empty() const { return size() == 0; }

// Deprecated!
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::eraseVariable( NodeId varId ) { erase( varId ); }

// Erase a variable and update dependent CPTs.
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::erase( const DiscreteVariable& var ) {
    erase( VariableNodeMap::get( var ) );
  }

// Erase a variable and update dependent CPTs.
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

// Add an arc in the BN, and update arc.head's CPT
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::insertArc( NodeId tail, NodeId head ) {
    __dag.insertArc( tail, head );
    // Add parent in the child's CPT
    ( *( __probaMap[head] ) ) << variable( tail );
  }

// Removes an arc in the BN, and update head's CTP
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

// Removes an arc in the BN, and update head's CTP
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::eraseArc( NodeId tail, NodeId head ) {
    eraseArc( Arc( tail, head ) );
  }

// Shortcut for this->dag().beginNodes().
  template<typename T_DATA> INLINE
  const DAG::NodeIterator
  BayesNet<T_DATA>::beginNodes() const { return __dag.beginNodes(); }

// Shortcut for this->dag().endNodes().
  template<typename T_DATA> INLINE
  const DAG::NodeIterator
  BayesNet<T_DATA>::endNodes() const { return __dag.endNodes(); }

// Shortcut for this->dag().beginArcs().
  template<typename T_DATA> INLINE
  const DAG::ArcIterator
  BayesNet<T_DATA>::beginArcs() const { return __dag.beginArcs(); }

// Shortcut for this->dag().endArcs().
  template<typename T_DATA> INLINE
  const DAG::ArcIterator&
  BayesNet<T_DATA>::endArcs() const { return __dag.endArcs(); }

// The node's id are coherent with the variables and nodes of the topology.
  template<typename T_DATA>
  const UndiGraph&
  BayesNet<T_DATA>::moralGraph( bool clear ) const {
    if ( clear or __moralGraph == 0 ) {
      if ( __moralGraph != 0 ) delete __moralGraph;

      __moralGraph = new UndiGraph();

      __moralGraph->populateNodes( __dag );

      // transform the arcs into edges
      for ( DAG::ArcIterator iter = __dag.beginArcs(); iter != __dag.endArcs(); ++iter ) {
        __moralGraph->insertEdge( iter->first(), iter->second() );
      }

      // mary the parents
      for ( DAG::NodeIterator iter = beginNodes(); iter != endNodes(); ++iter ) {
        //const ArcSet& parentsList = __dag.parents(*iter);
        for ( ArcSetIterator it1 = __dag.parents( *iter ).begin(); it1 != __dag.parents( *iter ).end(); ++it1 ) {
          ArcSetIterator it2 = it1;

          for ( ++it2; it2 != __dag.parents( *iter ).end(); ++it2 ) {
            // will automatically check if this edge already exists
            __moralGraph->insertEdge( it1->tail(), it2->tail() );
          }
        }
      }
    }

    return *__moralGraph;
  }

// The topological order stays the same as long as no variable or arcs are
// added or erased from the topology.
  template<typename T_DATA>
  const Sequence<NodeId>&
  BayesNet<T_DATA>::getTopologicalOrder( bool clear ) const {
    if ( clear or( __topologicalOrder->empty() ) ) {
      __topologicalOrder->clear();
      // Add all root nodes in the list
      NodeSet nodeList = __dag.nodes();
      __getRootTopologyLevel( nodeList );
      // Check if the graph has at least one root node

      if ( __topologicalOrder->empty() )
        GUM_ERROR( OperationNotAllowed, "No root node in DAG." );

      // Add nodes in topological order
      while ( !nodeList.empty() )
        __getNextTopologyLevel( nodeList );
    }

    return *__topologicalOrder;
  }

// Return the properties of this BayesNet and initialize the hash table is
// necessary.
  template<typename T_DATA> INLINE
  HashTable<std::string, std::string>&
  BayesNet<T_DATA>::__properties() {
    if ( __propertiesMap == 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>();
    }

    return *__propertiesMap;
  }

// Return the properties of this BayesNet and initialize the hash table is
// necessary.
  template<typename T_DATA> INLINE
  const HashTable<std::string, std::string>&
  BayesNet<T_DATA>::__properties() const {
    if ( __propertiesMap == 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>();
    }

    return *__propertiesMap;
  }

// Add all the dag's root nodes in __topologicalOrder
  template<typename T_DATA> INLINE
  void
  BayesNet<T_DATA>::__getRootTopologyLevel( NodeSet& uncheckedNodes ) const {
    // Add all root nodes in the list
    for ( NodeSetIterator iter = uncheckedNodes.begin(); iter != uncheckedNodes.end(); ++iter ) {
      if ( __dag.parents( *iter ).empty() ) {
        __topologicalOrder->insert( *iter );
        uncheckedNodes.erase( *iter );
      }
    }
  }

// Add the next level of nodes in the topological order
  template<typename T_DATA>
  void
  BayesNet<T_DATA>::__getNextTopologyLevel( NodeSet& uncheckedNodes ) const {
    bool add;
    // Parsing all nodes in uncheckedNodes

    for ( NodeSetIterator nodeIter = uncheckedNodes.begin(); nodeIter != uncheckedNodes.end(); ++nodeIter ) {
      add = true;
      // Parsing all parents of current node
      const ArcSet& parentsList = __dag.parents( *nodeIter );

      for ( ArcSetIterator arcIter = parentsList.begin(); arcIter != parentsList.end(); ++arcIter ) {
        add = add && __topologicalOrder->exists(( *arcIter ).tail() );
      }

      // If current node's parent are all in __topologicalOrder, then we add it
      if ( add ) {
        __topologicalOrder->insert( *nodeIter );
        uncheckedNodes.erase( *nodeIter );
      }
    }
  }

  // Add a noisyOR variable
  template<typename T_DATA> INLINE
  NodeId
  BayesNet<T_DATA>::addNoisyOR( const DiscreteVariable& var , T_DATA external_weight, NodeId id ) {
    return add( var, new MultiDimNoisyOR<T_DATA>( external_weight ) , id );
  }

  // Add an arc in the BN, and update arc.head's CPT.
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

  template<typename T_DATA> INLINE
  std::string BayesNet<T_DATA>::toString( void ) const {
    Size dSize = 1;
    Size param = 0;

    for ( DAG::NodeIterator it = beginNodes();it != endNodes();++it ) {
      dSize *= variable( *it ).domainSize();
      param += (( const MultiDimImplementation<T_DATA> & )cpt( *it ).getMasterRef() ).realSize();
    }

    int compressionRatio = 100 * (( float )1.0 - (( float )param ) / (( float )dSize ) );

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", domainSize: " << dSize << ", parameters: " << param << ", compression ratio: " << compressionRatio << "% }";

    return s.str();
  }


  template<typename T_DATA> INLINE
  std::string BayesNet<T_DATA>::toDot( void ) const {
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

  template<typename T_DATA>
  std::ostream&
  operator<<( std::ostream& output, const BayesNet<T_DATA>& map ) {
    output << map.toString();
    return output;
  }


} /* namespace gum */

// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
