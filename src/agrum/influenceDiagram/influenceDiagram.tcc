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
* @brief Template implementation of InfluenceDiagram/InfluenceDiagram.h classes.
*
* @author Jean-Christophe Magnan & Pierre_Henri Wuillemin
*/
// ============================================================================
#include <agrum/influenceDiagram/influenceDiagram.h>
#include <cstdio>
#include <iostream>
// ============================================================================

namespace gum {

// ===========================================================================
// Constructors / Destructors
// ===========================================================================

  /*
  * Default constructor.
  */
  template<typename T_DATA> INLINE
  InfluenceDiagram<T_DATA>::InfluenceDiagram() :
      AbstractBayesNet<T_DATA>(), __topologicalOrder( 0 ) {
    GUM_CONSTRUCTOR( InfluenceDiagram );
    __topologicalOrder = new Sequence<NodeId>();
  }

  /*
  * Destructor.
  */
  template<typename T_DATA>
  InfluenceDiagram<T_DATA>::~InfluenceDiagram() {
    GUM_DESTRUCTOR( InfluenceDiagram );
    _removeTables();
    delete __topologicalOrder;
  }

  /*
  * Copy Constructor
  */
  template<typename T_DATA>
  InfluenceDiagram<T_DATA>::InfluenceDiagram( const InfluenceDiagram<T_DATA>& source ) :
      AbstractBayesNet<T_DATA> ( source ), __dag( source.__dag ), __variableMap( source.__variableMap ), __topologicalOrder( 0 ) {
    GUM_CONS_CPY( InfluenceDiagram );
    _copyTables( source );
    __topologicalOrder = new Sequence<NodeId> ( * ( source.__topologicalOrder ) );

  }

  /*
  * Copy Operator
  */
  template<typename T_DATA>
  InfluenceDiagram<T_DATA>&
  InfluenceDiagram<T_DATA>::operator= ( const InfluenceDiagram<T_DATA>& source ) {
    if ( this != &source ) {
      AbstractBayesNet<T_DATA>::operator= ( source );
      // Removing previous potentials
      _removeTables();
      __potentialMap.clear();
      __utilityMap.clear();

      __dag = source.dag();
      __variableMap = source.__variableMap;

      //Copying tables
      _copyTables( source );

      ( *__topologicalOrder ) = * ( source.__topologicalOrder );
    }

    return *this;
  }

  /*
  * Removing ancient table
  */
  template<typename T_DATA>
  void InfluenceDiagram<T_DATA>::_removeTables() {
    for ( gum::DAG::NodeIterator iter = dag().beginNodes(); iter != dag().endNodes(); ++iter ) {
      if ( isChanceNode( *iter ) )
        delete &cpt( *iter );
      else if ( isUtilityNode( *iter ) )
        delete &utility( *iter );
    }
  }

  /*
  * Copying tables from another influence diagram
  */
  template<typename T_DATA>
  void InfluenceDiagram<T_DATA>::_copyTables( const InfluenceDiagram<T_DATA>& IDsource ) {
    // Copying potentials
    Potential<T_DATA> * potentialCpy = NULL;

    for ( typename Property<Potential<T_DATA>*>::onNodes::const_iterator potentialIter = IDsource.__potentialMap.begin(); potentialIter != IDsource.__potentialMap.end(); ++potentialIter ) {
      // Instanciation of the node's CPT
      potentialCpy = new Potential<T_DATA>;
      ( *potentialCpy ) << variable( potentialIter.key() );

      // Addition of the parents
      const NodeSet& parentArcs = __dag.parents( potentialIter.key() );

      for ( NodeSet::const_iterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter )
        ( *potentialCpy ) << variable( *arcIter );

      // Filling up of the table
      Instantiation srcInst( **potentialIter );

      Instantiation cpyInst( *potentialCpy );

      for ( cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc() ) {
        for ( Idx i = 0; i < cpyInst.nbrDim(); i++ ) {
          NodeId id = nodeId( cpyInst.variable( i ) );
          srcInst.chgVal( IDsource.variable( id ), cpyInst.val( i ) );
        }

        potentialCpy->set( cpyInst, ( **potentialIter )[srcInst] );
      }

      // Adding cpt to cpt map
      __potentialMap.set( potentialIter.key(), potentialCpy );
    }

    // Copying Utilities
    UtilityTable<T_DATA> *utilityCpy;

    for ( typename Property<UtilityTable<T_DATA>*>::onNodes::const_iterator utilityIter = IDsource.__utilityMap.begin(); utilityIter != IDsource.__utilityMap.end(); ++utilityIter ) {
      // Instanciation of the node's CPT
      utilityCpy = new UtilityTable<T_DATA>;
      ( *utilityCpy ) << variable( utilityIter.key() );

      // Addition of the parents
      const NodeSet& parentArcs = __dag.parents( utilityIter.key() );

      for ( NodeSet::const_iterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter )
        ( *utilityCpy ) << variable( *arcIter );

      // Filling up of the table
      Instantiation srcInst( **utilityIter );

      Instantiation cpyInst( *utilityCpy );

      for ( cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc() ) {
        for ( Idx i = 0; i < cpyInst.nbrDim(); i++ ) {
          NodeId id = nodeId( cpyInst.variable( i ) );
          srcInst.chgVal( IDsource.variable( id ), cpyInst.val( i ) );
        }

        utilityCpy->set( cpyInst, ( **utilityIter )[srcInst] );
      }

      // Adding cpt to cpt map
      __utilityMap.set( utilityIter.key(), utilityCpy );
    }
  }

  /*
  * To string editor
  */
  template<typename T_DATA> INLINE
  std::string
  InfluenceDiagram<T_DATA>::toDot( void ) const {
    std::stringstream output;
    std::stringstream decisionNode;
    std::stringstream utilityNode;
    std::stringstream chanceNode;
    std::stringstream arcstream;
    output << "digraph \"";

    try {
      output << this->property( "name" ) << "\" {" << std::endl;
    } catch ( NotFound& ) {
      output << "no_name\" {" << std::endl;
    }

    decisionNode << "node [shape = box];" << std::endl;

    utilityNode << "node [shape = diamond];" << std::endl;
    chanceNode << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for ( gum::DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
      if ( isChanceNode( *node_iter ) )
        chanceNode << tab << variable( *node_iter ).name() << ";";
      else if ( isUtilityNode( *node_iter ) )
        utilityNode << tab << variable( *node_iter ).name() << ";";
      else
        decisionNode << tab << variable( *node_iter ).name() << ";";

      if ( dag().children( *node_iter ).size() > 0 ) {
        const NodeSet& children = dag().children( *node_iter );

        for ( NodeSetIterator arc_iter = children.begin(); arc_iter != children.end(); ++arc_iter )
          arcstream << tab <<  variable( *node_iter ).name() << " -> " << variable( *arc_iter ).name() << ";" << std::endl;
      }
    }

    output << decisionNode.str() << std::endl << utilityNode.str() << std::endl << chanceNode.str() << std::endl << arcstream.str() << std::endl << "}" << std::endl;

    return output.str();


  }



// ===========================================================================
// Variable manipulation methods.
// ===========================================================================

  /*
  * Returns the CPT of a chance variable.
  */
  template<typename T_DATA> INLINE
  const Potential<T_DATA>&
  InfluenceDiagram<T_DATA>::cpt( NodeId varId ) const {
    return * ( __potentialMap[varId] );
  }

  /*
  * Returns the utility table of a utility node.
  */
  template<typename T_DATA> INLINE
  const UtilityTable<T_DATA>&
  InfluenceDiagram<T_DATA>::utility( NodeId varId ) const {
    return * ( __utilityMap[varId] );
  }

  /*
  * Return true if node is a utility one
  */
  template<typename T_DATA> INLINE
  bool
  InfluenceDiagram<T_DATA>::isUtilityNode( NodeId varId ) const {
    return __utilityMap.exists( varId );
  }

  /*
  * Return true if node is a utility one
  */
  template<typename T_DATA> INLINE
  bool
  InfluenceDiagram<T_DATA>::isDecisionNode( NodeId varId ) const {
    bool ret = true;

    if ( isUtilityNode( varId ) || isChanceNode( varId ) )
      ret = false;

    return ret;
  }

  /*
  * Return true if node is a chance one
  */
  template<typename T_DATA> INLINE
  bool
  InfluenceDiagram<T_DATA>::isChanceNode( NodeId varId ) const {
    return __potentialMap.exists( varId );
  }

  /*
  * Returns the number of utility nodes
  */
  template<typename T_DATA> INLINE
  Size
  InfluenceDiagram<T_DATA>::utilityNodeSize() const {
    return __utilityMap.size();
  }

  /*
  * Returns the number of chance nodes
  */
  template<typename T_DATA> INLINE
  Size
  InfluenceDiagram<T_DATA>::chanceNodeSize() const {
    return __potentialMap.size();
  }

  /*
  * Returns the number of decision nodes
  */
  template<typename T_DATA> INLINE
  Size
  InfluenceDiagram<T_DATA>::decisionNodeSize() const {
    return ( size() - __utilityMap.size() - __potentialMap.size() );
  }

  /*
  * Returns a constant reference to the dag of this Influence Diagram.
  */
  template<typename T_DATA> INLINE
  const DAG&
  InfluenceDiagram<T_DATA>::dag() const {
    return __dag;
  }

  /*
  * Returns a constant reference to the VariableNodeMap of this Influence Diagram
  */
  template<typename T_DATA> INLINE
  const VariableNodeMap&
  InfluenceDiagram<T_DATA>::variableNodeMap() const {
    return __variableMap;
  }

  /*
  * Returns the number of variables in this Influence Diagram.
  */
  template<typename T_DATA> INLINE
  Idx
  InfluenceDiagram<T_DATA>::size() const {
    return __dag.size();
  }

  /*
  * Retursn true if this Influence Diagram is empty.
  */
  template<typename T_DATA> INLINE
  bool
  InfluenceDiagram<T_DATA>::empty() const {
    return size() == 0;
  }

  /*
  * Returns a constant reference over a variabe given it's node id.
  */
  template<typename T_DATA> INLINE
  const DiscreteVariable&
  InfluenceDiagram<T_DATA>::variable( NodeId id ) const {
    return __variableMap[id];
  }

  /*
  * Return id node from discrete var pointer.
  */
  template<typename T_DATA> INLINE
  NodeId InfluenceDiagram<T_DATA>::nodeId( const DiscreteVariable &var ) const {
    return __variableMap.get( var );
  }

// Getter by name
  template<typename T_DATA> INLINE
  NodeId InfluenceDiagram<T_DATA>::idFromName( const std::string& name ) const {
    return __variableMap.idFromName( name );
  }

// Getter by name
  template<typename T_DATA> INLINE
  const DiscreteVariable& InfluenceDiagram<T_DATA>::variableFromName( const std::string& name ) const {
    return __variableMap.variableFromName( name );
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template<typename T_DATA>
  NodeId InfluenceDiagram<T_DATA>::add( const DiscreteVariable& var, NodeId varId ) {
    return addChanceNode( var, varId );
  }

  /*
  * Add a utility variable, it's associate node and it's UT. The id of the new
  * variable is automatically generated.
  * @Throws : Gum::InvalidArgument if var has more than one state
  */
  template<typename T_DATA>
  NodeId InfluenceDiagram<T_DATA>::addUtilityNode( const DiscreteVariable& var, NodeId varId ) {
    MultiDimArray<T_DATA>*  newMultiDim = new MultiDimArray<T_DATA>();
    NodeId res = 0;

    try {
      res = addUtilityNode( var, newMultiDim, varId );
    } catch ( Exception& e ) {
      delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
  * Add a decision variable. The id of the new
  * variable is automatically generated.
  */
  template<typename T_DATA>
  NodeId InfluenceDiagram<T_DATA>::addDecisionNode( const DiscreteVariable& var, NodeId varId ) {
    return _addNode( var, varId );
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template<typename T_DATA>
  NodeId InfluenceDiagram<T_DATA>::addChanceNode( const DiscreteVariable& var, NodeId varId ) {
    MultiDimArray<T_DATA>*  newMultiDim = new MultiDimArray<T_DATA>();
    NodeId res = 0;

    try {
      res = addChanceNode( var, newMultiDim, varId );
    } catch ( Exception& e ) {
      delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template<typename T_DATA>
  NodeId
  InfluenceDiagram<T_DATA>::addChanceNode( const DiscreteVariable& var, MultiDimImplementation<T_DATA> *aContent, NodeId DesiredId ) {
    NodeId proposedId = _addNode( var, DesiredId );

    Potential<T_DATA> *varcpt = new Potential<T_DATA> ( aContent );
    ( *varcpt ) << variable( proposedId );
    __potentialMap.insert( proposedId, varcpt );

    return proposedId;
  }


  /*
  * Add a utility variable, it's associate node and it's UT. The id of the new
  * variable is automatically generated.
  * @Throws : Gum::InvalidArgument if var has more than one state
  */
  template<typename T_DATA>
  NodeId
  InfluenceDiagram<T_DATA>::addUtilityNode( const DiscreteVariable& var, MultiDimImplementation<T_DATA> *aContent, NodeId DesiredId ) {
    if ( var.domainSize() != 1 ) {
      GUM_ERROR( InvalidArgument, "Utility var have no state ( which implicates a single label for data output reasons )." );
    }

    NodeId proposedId = _addNode( var, DesiredId );

    UtilityTable<T_DATA> *varut = new UtilityTable<T_DATA> ( aContent );

    ( *varut ) << variable( proposedId );

    __utilityMap.insert( proposedId, varut );

    return proposedId;
  }

  /*
  * Add a node
  */
  template<typename T_DATA>
  NodeId InfluenceDiagram<T_DATA>::_addNode( const DiscreteVariable& variableType, NodeId DesiredId ) {
    // None thread safe code!
    NodeId proposedId;

    if ( DesiredId == 0 )
      proposedId = __dag.nextNodeId();
    else
      proposedId = DesiredId;

    __variableMap.insert( proposedId, variableType );

    __dag.insertNode( proposedId );

    // end critical section
    return proposedId;
  }

  /*
  * Erase a Variable from the network and remove the variable from
  * all childs of id.
  * If no variable matches the id, then nothing is done.
  */
  template<typename T_DATA>
  void
  InfluenceDiagram<T_DATA>::erase( NodeId varId ) {
    if ( __variableMap.exists( varId ) ) {
      // Reduce the variable child's CPT or Utility Table if necessary
      const NodeSet& childrenArcs = __dag.children( varId );

      for ( NodeSetIterator iter = childrenArcs.begin(); iter != childrenArcs.end(); ++iter ) {
        if ( isChanceNode( *iter ) )
          __potentialMap[*iter ]->erase( variable( varId ) );
        else if ( isUtilityNode( *iter ) )
          __utilityMap[*iter]->erase( variable( varId ) );
      }

      if ( isChanceNode( varId ) ) {
        delete __potentialMap[varId];
        __potentialMap.erase( varId );
      } else if ( isUtilityNode( varId ) ) {
        delete __utilityMap[varId];
        __utilityMap.erase( varId );
      }

      __variableMap.erase( varId );

      __dag.eraseNode( varId );
    }
  }

  /*
  * Erase a Variable from the network and remove the variable from
  * all childs of var.
  * If no variable matches, then nothing is done.
  */
  template<typename T_DATA> INLINE
  void
  InfluenceDiagram<T_DATA>::erase( const DiscreteVariable& var ) {
    erase( __variableMap.get( var ) );
  }

  /* we allow the user to change the name of a variable
  */
  template<typename T_DATA> INLINE
  void InfluenceDiagram<T_DATA>::changeVariableName( NodeId id, const std::string& new_name ) {
    __variableMap.changeName( id, new_name );
  }



// ===========================================================================
// @name Arc manipulation methods.
// ===========================================================================

  /*
  * Add an arc in the ID, and update diagram's chance nodes cpt if necessary.
  */
  template<typename T_DATA> INLINE
  void
  InfluenceDiagram<T_DATA>::insertArc( NodeId tail, NodeId head ) {
    if ( isUtilityNode( tail ) ) {
      GUM_ERROR( InvalidArc, "Tail cannot be a utility node" );
    }

    __dag.insertArc( tail, head );

    if ( isChanceNode( head ) )
      // Add parent in the child's CPT
      ( * ( __potentialMap[head] ) ) << variable( tail );
    else if ( isUtilityNode( head ) )
      // Add parent in the child's UT
      ( * ( __utilityMap[head] ) ) << variable( tail );
  }

  /*
  * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
  *
  * If (tail, head) doesn't exist, the nothing happens.
  */
  template<typename T_DATA> INLINE
  void
  InfluenceDiagram<T_DATA>::eraseArc( const Arc& arc ) {
    if ( dag().existsArc( arc ) ) {
      NodeId head = arc.head(), tail = arc.tail();
      __dag.eraseArc( arc );

      if ( isChanceNode( head ) )
        // Removes parent in the child's CPT
        ( * ( __potentialMap[head] ) ) >> variable( tail );
      else if ( isUtilityNode( head ) )
        // Removes parent in the child's UT
        ( * ( __utilityMap[head] ) ) >> variable( tail );
    }
  }

  /*
  * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
  *
  * If (tail, head) doesn't exist, the nothing happens.
  */
  template<typename T_DATA> INLINE
  void
  InfluenceDiagram<T_DATA>::eraseArc( NodeId tail, NodeId head ) {
    eraseArc( Arc( tail, head ) );
  }


// ===========================================================================
// Graphical methods
// ===========================================================================

  /*
  * The node's id are coherent with the variables and nodes of the topology.
  */
  template<typename T_DATA>
  const UndiGraph&
  InfluenceDiagram<T_DATA>::moralGraph( bool clear ) const {
    if ( clear ) {
      __moralGraph.clear();
      //DAG directGraph( dag() );

      for ( NodeGraphPartIterator nodeIter = __dag.beginNodes(); nodeIter != __dag.endNodes(); ++nodeIter )
        if ( !isUtilityNode( *nodeIter ) ) __moralGraph.insertNode( *nodeIter );

      for ( NodeGraphPartIterator nodeIter = __dag.beginNodes(); nodeIter != __dag.endNodes(); ++nodeIter ) {
        if ( !isDecisionNode( *nodeIter ) ) {
          const NodeSet& parents = __dag.parents( *nodeIter );

          for ( NodeSetIterator arcIter = parents.begin(); arcIter != parents.end(); ++arcIter ) {
            if ( isChanceNode( *nodeIter ) ) __moralGraph.insertEdge( *nodeIter, *arcIter );

            for ( NodeSetIterator arcIterPrime = arcIter; arcIterPrime != parents.end(); ++arcIterPrime )
              if ( *arcIter != *arcIterPrime ) __moralGraph.insertEdge( *arcIter, *arcIterPrime );
          }
        }
      }
    }

    return __moralGraph;
  }

  /*
  * The topological order stays the same as long as no variable or arcs are
  * added or erased from the topology.
  */
  template<typename T_DATA>
  const Sequence<NodeId>&
  InfluenceDiagram<T_DATA>::getTopologicalOrder( bool clear ) const {
    if ( clear or( __topologicalOrder->empty() ) ) {
      __topologicalOrder->clear();
      AbstractBayesNet<T_DATA>::_topologicalOrder( *__topologicalOrder );
    }

    return *__topologicalOrder;
  }


  /*
  * True if a directed path exist with all decison nodes
  */
  template<typename T_DATA>
  bool
  InfluenceDiagram<T_DATA>::decisionOrderExists() const {
    Sequence<NodeId> order = getTopologicalOrder( true );

    //Finding first decision node
    Sequence<NodeId>::const_iterator orderIter = order.begin();

    while (( orderIter != order.end() ) && ( !isDecisionNode( *orderIter ) ) )
      ++orderIter;

    if ( orderIter == order.end() )
      return true;

    NodeId parentDecision = ( *orderIter );

    ++orderIter;

    // Checking path between decisions nodes
    while ( orderIter != order.end() ) {
      if ( isDecisionNode( *orderIter ) ) {
        if ( ! existsPathBetween( parentDecision, *orderIter ) )
          return false;

        parentDecision = *orderIter;

      }

      ++orderIter;
    }

    return true;
  }


  /*
  * Returns true if a path exists between source and destination
  */
  template<typename T_DATA>
  bool
  InfluenceDiagram<T_DATA>::existsPathBetween( NodeId src, NodeId dest ) const {
    List<NodeId> nodeFIFO;
    // mark[node] contains 0 if not visited
    // mark[node] = predecessor if visited
    Property<int>::onNodes mark = __dag.nodesProperty(( int ) - 1 );
    NodeId current;

    mark[src] = ( int ) src;

    nodeFIFO.pushBack( src );

    while ( ! nodeFIFO.empty() ) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      const NodeSet& child = __dag.children( current );

      for ( NodeSet::const_iterator ite = child.begin(); ite != child.end(); ++ite ) {
        NodeId new_one = *ite;

        if ( mark[new_one] != -1 ) continue; // if this node is already marked, continue

        mark[new_one] = ( int ) current;

        if ( new_one == dest ) break; // if we reach *orderIter, stop.

        nodeFIFO.pushBack( new_one );
      }
    }

    if ( mark[dest] == -1 )
      return false;

    return true;
  }

  /*
  * Returns the decision graph
  */
  template<typename T_DATA>
  gum::DAG*
  InfluenceDiagram<T_DATA>::getDecisionGraph() const {
    gum::DAG* temporalGraph = new gum::DAG();

    for ( NodeGraphPartIterator graphIter = dag().beginNodes(); graphIter != dag().endNodes(); ++graphIter ) {
      if ( isDecisionNode( *graphIter ) ) {
        if ( !temporalGraph->existsNode( *graphIter ) )
          temporalGraph->insertNode( *graphIter );

        Sequence<NodeId>* childrenSequence = _getChildrenDecision( *graphIter );

        for ( Sequence<NodeId>::const_iterator childrenSeqIter = childrenSequence->begin();
              childrenSeqIter != childrenSequence->end(); ++childrenSeqIter ) {
          if ( !temporalGraph->existsNode( *childrenSeqIter ) )
            temporalGraph->insertNode( *childrenSeqIter );

          temporalGraph->insertArc( *graphIter, *childrenSeqIter );
        }

        delete childrenSequence;
      }
    }

    return temporalGraph;
  }



  /*
  * Returns the list of children decision for a given nodeId
  */
  template<typename T_DATA>
  Sequence<NodeId>*
  InfluenceDiagram<T_DATA>::_getChildrenDecision( NodeId parentDecision ) const {
    Sequence<NodeId>* childrenSeq = new Sequence<NodeId>();

    List<NodeId> nodeFIFO;
    NodeId current;

    // mark[node] contains false if not visited
    // mark[node] contains true if visited
    Property<bool>::onNodes mark = __dag.nodesProperty( false );

    mark[parentDecision] = true;

    nodeFIFO.pushBack( parentDecision );

    while ( ! nodeFIFO.empty() ) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      const ArcSet& set = __dag.children( current );

      for ( ArcSet::const_iterator childIte = set.begin(); childIte != set.end(); ++childIte ) {
        NodeId new_one = childIte->head();

        if ( mark[new_one] ) continue; // if this node is already marked, continue

        mark[new_one] = true;

        if ( !isDecisionNode( new_one ) )
          nodeFIFO.pushBack( new_one );
        else
          childrenSeq->insert( new_one );
      }
    }

    return childrenSeq;
  }


  /*
  * Returns the sequence of decision nodes
  * @throw NotFound if such a sequence does not exist
  */
  template<typename T_DATA>
  std::vector<NodeId>*
  InfluenceDiagram<T_DATA>::getDecisionOrder() const {
    if ( !decisionOrderExists() ) {
      GUM_ERROR( NotFound, "No decision path exists" );
    }

    Sequence<NodeId> order = getTopologicalOrder( false );

    std::vector<NodeId>*  decisionSequence = new std::vector<NodeId>();

    for ( Sequence<NodeId>::const_iterator orderIter = order.begin(); orderIter != order.end(); ++orderIter )
      if ( isDecisionNode( *orderIter ) )
        decisionSequence->push_back( *orderIter );

    return decisionSequence;

  }

  /*
  * Returns partial temporal ordering
  * @throw NotFound if such a sequence does not exist
  */
  template<typename T_DATA>
  const List< NodeSet >&
  InfluenceDiagram<T_DATA>::getPartialTemporalOrder( bool clear ) const {
    if ( clear ) {
      __temporalOrder.clear();

      if ( !decisionOrderExists() ) {
        GUM_ERROR( NotFound, "No decision path exists" );
      }

      std::vector< NodeId >* decisionOrder = getDecisionOrder();

      NodeSet nodeList = dag().nodes();

      for ( unsigned int i = 0; i < decisionOrder->size(); i++ ) {
        NodeSet partialOrderedSet ;

        const NodeSet& parentArcs = __dag.parents( decisionOrder->at( i ) );

        for ( NodeSetIterator arcIter = parentArcs.begin(); arcIter != parentArcs.end(); ++arcIter ) {
          if ( nodeList.contains( *arcIter ) && isChanceNode( *arcIter ) ) {
            partialOrderedSet.insert( *arcIter );
            nodeList.erase( *arcIter );
          }
        }

        if ( ! partialOrderedSet.empty() )
          __temporalOrder.pushFront( partialOrderedSet );

        NodeSet decisionSet ;

        decisionSet.insert( decisionOrder->at( i ) );

        __temporalOrder.pushFront( decisionSet );
      }

      NodeSet lastSet ;//= new gum::NodeSet();

      for ( NodeSetIterator nodeIter = nodeList.begin(); nodeIter != nodeList.end(); ++nodeIter )
        if ( isChanceNode( *nodeIter ) )
          lastSet.insert( *nodeIter );

      if ( ! lastSet.empty() )
        __temporalOrder.pushFront( lastSet );
    }

    return __temporalOrder;
  }

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
