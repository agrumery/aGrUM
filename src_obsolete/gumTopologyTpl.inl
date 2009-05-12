/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
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


/** @file
 * @brief Inlined implementation of gumTopologyTpl
 *
 * @author Lionel Torti
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ===                       CONSTRUCTORS AND DESTRUCTOR                      === */
/* ============================================================================== */

// Default Constructor
INLINE
gumTopologyTpl::gumTopologyTpl(gumId firstId):
    __rebuildTopologicalOrder( true ), __topologicalOrder( NULL ),
    __rebuildMoralGraph( true ), __moralGraph( NULL ),
    __rebuiltEndIterator( true ), __endIter( NULL )
{
  GUM_CONSTRUCTOR( gumTopologyTpl );
  __nextId = firstId;
}

/* ============================================================================== */
/* ===                          GETTERS AND SETTERS                           === */
/* ============================================================================== */

// Getter for the subjacent DAG of *this.
// @return A constant reference on the subjacent DAG of *this.
INLINE
const gumDAGTpl<gumNode, gumArc>&
gumTopologyTpl::getDAG() const {
  return _dag;
}

// Getter for the structure's variables.
// @param nodeID The associated node Id.
// @return A reference on the variable, where nodeID is the id of
// it's associated node.
// @throws gumNotFound Raised if node node has nodeID as id.
INLINE
gumDiscreteVariable&
gumTopologyTpl::variable( gumId varId )
{
  return *( _variableMap[varId] );
}

// Getter for the structure's variables.
// @param nodeID The associated node Id.
// @return A reference on the variable, where nodeID is the id of
// it's associated node.
// @throws gumNotFound Raised if node node has nodeID as id.
INLINE
const gumDiscreteVariable&
gumTopologyTpl::variable( gumId varId ) const
{
  return *( _variableMap[varId] );
}

// return id node from discrete var pointer
// @param var the discrete var pointer
// @return A gumId& which is the id of it's associated node.
// @throws gumNotFound raised if this var pointer is not in the variables list.
INLINE
gumId
gumTopologyTpl::getIdFromVariable(const gumDiscreteVariable& var ) const
{
  return _variableMap.keyByVal(const_cast<gumDiscreteVariable*>(&var));
}

/* ============================================================================== */
/* ===                         ADD AND ERASE METHODS                          === */
/* ============================================================================== */

// Copy the variable, create it's associated node and added it to the topology.
// @param variable The variable added to the topology.
// @param varId The id of the variable in the topology. If it equals 0, then a the
// id is generated (which is the default value).
// @throw gumDuplicateElement Raised if id is already a variable's id.
// @return A constant reference on the id of the new variable (and node).
/*INLINE
gumId
gumTopologyTpl::addVariable(const gumDiscreteVariable * const variable, gumId varId) {
  if (_variableMap.exists(varId))
    GUM_ERROR(gumDuplicateElement, "Duplicate id.");

  gumDiscreteVariable* varPtr = variable->copyFactory();
  gumId id = varId;
  gumNode node( id, varPtr->name() );

  varPtr->setDiscreteVariableListener(( gumDiscreteVariableListener* ) this );
  _variableMap.insert( id, varPtr );
  _dag.insertNode( node );

  _topologyChanged();

  return id;
}*/
INLINE
gumId
gumTopologyTpl::addVariable(const gumDiscreteVariable * const variable) {
  gumDiscreteVariable* varPtr = variable->copyFactory();
  gumId id = _nextId();
  gumNode node( id, varPtr->name() );

  varPtr->setDiscreteVariableListener(( gumDiscreteVariableListener* ) this );
  _variableMap.insert( id, varPtr );
  _dag.insertNode( node );

  _topologyChanged();

  return id;
}


// Erase a Variable from the topology.
// @param id The id of the variable to erase.
INLINE
void
gumTopologyTpl::eraseVariable( gumId id ) {
  _dag.eraseNode( id );
  delete _variableMap[id];
  _variableMap.erase( id );

  _topologyChanged();
}

// Add a new arc in the topology.
// @param The arc which is copied in the topoloy.
// @return A constant reference of the copied version of the arc.
// @throw Raised if the arc reference nodes not found in the topology.
INLINE
const gumArc&
gumTopologyTpl::insertArc( gumId tail, gumId head )
{
  if ( _variableMap.exists( tail ) && _variableMap.exists( head ) ) {
    gumArc a( tail, head );
    const gumArc& realArc = _dag.insertArc( a );
    _topologyChanged();

    return realArc;
  } else {
    GUM_ERROR( gumInvalidEdge, "The arc is invalid." );
  }
}

// Erase an arc from the topology.
// @param tail The source of the arc.
// @param head The destination of the arc.
INLINE
void
gumTopologyTpl::eraseArc( gumId tail, gumId head ) {
  if ( _dag.existsArc( tail, head ) ) {
    _dag.eraseArc( tail, head );
    _topologyChanged();
  }
}

/* ============================================================================== */
/* ===                           ITERATION METHODS                            === */
/* ============================================================================== */
// @return Returns the number of variables in the topology.
INLINE
const gumSize
gumTopologyTpl::size() {
  return _variableMap.size();
}

// @return Returns true if the topology is empty.
INLINE
const bool
gumTopologyTpl::empty() const {
  return _variableMap.empty();
}

// Return true if id matches a variable
INLINE
bool
gumTopologyTpl::exists( gumId id ) const {
  return _variableMap.exists( id );
}

// @return Returns an iterator pointing at the begining of the Topology.
INLINE
gumTopologyTpl::iterator
gumTopologyTpl::begin() const {
  gumHashTable<gumId, gumDiscreteVariable*>::iterator iter = _variableMap.begin();
  return gumTopologyTplIterator( iter );
}

/**
 * @return Returna an iterator pointing at the end of the Topology.
 */
INLINE
const gumTopologyTpl::iterator&
gumTopologyTpl::end() const {
  if ( __endIter == NULL )
    __endIter = new gumTopologyTplIterator( _variableMap.end() );

  if ( __rebuiltEndIterator )
    *__endIter = gumTopologyTplIterator( _variableMap.end() );

  return *__endIter;
}

/* ============================================================================== */
/* ===                   GUM_DISCRETE_VARIABLE_LISTENER METHODS               === */
/* ============================================================================== */
// @see gumDiscreteVariableListener
INLINE
void
gumTopologyTpl::variableDomainChanged( gumDiscreteVariableEvent& ) {
  // Empty !
}

// @see gumDiscreteVariableListener
INLINE
void
gumTopologyTpl::variableNameChanged( gumDiscreteVariableEvent& ) {
  // Empty !
}

// @see gumDiscreteVariableListener
INLINE
void
gumTopologyTpl::variableDescriptionChanged( gumDiscreteVariableEvent& ) {
  // Empty !
}

// Call this method if the topology has changed.
INLINE
void
gumTopologyTpl::_topologyChanged() const {
  __rebuildTopologicalOrder = true;
  __rebuildMoralGraph = true;
  __rebuiltEndIterator = true;
}

/* ============================================================================== */
/* ===                                  OPERATORS                             === */
/* ============================================================================== */

/**
 * Copy operator.
 */
INLINE
gumTopologyTpl&
gumTopologyTpl::operator=( const gumTopologyTpl& source)
{
  _dag = source._dag;
  __nextId = source.__nextId;

  gumDiscreteVariable *copyPtr;
  // Copying variables and ids
  for ( gumNodePROPERTY(gumDiscreteVariable*)::iterator iter = source._variableMap.begin();
        iter != source._variableMap.end();
        ++iter ) {
    copyPtr = (*iter)->copyFactory();
    copyPtr->setDiscreteVariableListener(( gumDiscreteVariableListener* ) this );
    _variableMap.insert( iter.key(), copyPtr );
  }
  return *this;
}

/* ============================================================================== */
/* ===               GUM_TOPOLOGY PROTECTED INLINE METHODS                    === */
/* ============================================================================== */

// @return Returns the next id
INLINE
gumId
gumTopologyTpl::_nextId() {
  while (_variableMap.exists(++__nextId)) {
    // Do nothing!
  }

  return __nextId;
}

/* ============================================================================== */
/* ===                 GUM_TOPOLOGY PRIVATE INLINE METHODS                    === */
/* ============================================================================== */

// Add all the DAG's root nodes in __topologicalOrder
INLINE
void
gumTopologyTpl::__getRootTopologyLevel( gumNodeList<gumNode>& nodeList ) const {
  unsigned int before = __topologicalOrder->size();

  // Add all root nodes in the list

  for ( gumNodeList<gumNode>::iterator iter = nodeList.begin();
        iter != nodeList.end();
        ++iter ) {
    if ( _dag.getParentArcs( iter->getID() ).empty() ) {
      __topologicalOrder->insert( iter->getID() );
    }
  }

  unsigned int after = __topologicalOrder->size();

  // Cleaning nodeList

  for ( unsigned int i = after; i > before; i-- ) {
    nodeList.erase(( *__topologicalOrder )[i - 1] );
  }
}

// Add the next level of nodes in the topological order
INLINE
void
gumTopologyTpl::__getNextTopologyLevel( gumNodeList<gumNode>& nodeList ) const {
  unsigned int before = __topologicalOrder->size();

  bool add;
  // Parsing all nodes in nodeList

  for ( gumNodeList<gumNode>::iterator nodeIter = nodeList.begin();
        nodeIter != nodeList.end();
        ++nodeIter ) {
    add = true;
    // Parsing all parents of current node
    const gumList< gumRefPtr<gumArc> >& parentsList = _dag.getParentArcs( nodeIter->getID() );

    for ( gumList< gumRefPtr<gumArc> >::iterator arcIter = parentsList.begin();
          arcIter != parentsList.end();
          ++arcIter ) {
      add = add && __topologicalOrder->exists(( *arcIter )->tail() );
    }

    // If current node's parent are all in __topologicalOrder, then we add it
    if ( add ) __topologicalOrder->insert( nodeIter->getID() );
  }

  unsigned int after = __topologicalOrder->size();

  // Cleaning nodeList

  for ( unsigned int i = after; i > before; i-- ) {
    nodeList.erase(( *__topologicalOrder )[i - 1] );
  }
}

/* ============================================================================== */
/* ===                        GUM_TOPOLOGY_ITERATOR                           === */
/* ============================================================================== */

/* ============================================================================== */
/* ===                     CONSTRUCTORS AND DESTUCTORS                        === */
/* ============================================================================== */
// Default Construtor.
INLINE
gumTopologyTplIterator::gumTopologyTplIterator() {
  GUM_CONSTRUCTOR( gumTopologyTplIterator );
}

// Copy Constructor.
INLINE
gumTopologyTplIterator::gumTopologyTplIterator( const gumTopologyTplIterator& source ):
    iterator( source.iterator ) {
  GUM_CONSTRUCTOR( gumTopologyTplIterator );
}

// Builds an iterator based on the hashtable iterator.
INLINE
gumTopologyTplIterator::gumTopologyTplIterator( const gumHashTableIterator<gumId, gumDiscreteVariable*>& iter ):
    iterator( iter ) {
  GUM_CONSTRUCTOR( gumTopologyTplIterator );
}

// Destructor.
INLINE
gumTopologyTplIterator::~gumTopologyTplIterator() {
  GUM_DESTRUCTOR( gumTopologyTplIterator );
}

/* ============================================================================== */
/* ===                         GETTERS AND SETTERS                            === */
/* ============================================================================== */
// @return Returns the id corresponding to the variable pointed by the iterator.
// @throw gumUndefinedIteratorValue if there's no variable pointed by the iterator.
INLINE
gumId
gumTopologyTplIterator::getId()
{
  return iterator.key();
}

// Makes the iterator point to nothing.
INLINE
void
gumTopologyTplIterator::clear() {
  iterator.clear();
}

/* ============================================================================== */
/* ===                              OPERATORS                                 === */
/* ============================================================================== */
// Copy operator.
INLINE
gumTopologyTplIterator&
gumTopologyTplIterator::operator=( const gumTopologyTplIterator & source )
{
  iterator = source.iterator;
  return *this;
}

// Increment operator.
INLINE
gumTopologyTplIterator&
gumTopologyTplIterator::operator++()
{
  ++iterator;
  return *this;
}

// Decrement operator.
INLINE
gumTopologyTplIterator&
gumTopologyTplIterator::operator--()
{
  --iterator;
  return *this;
}

// Checks wether two iterators are pointing toward different elements.
INLINE
const bool
gumTopologyTplIterator::operator!=( const gumTopologyTplIterator& source ) const
{
  return iterator != source.iterator;
}

// Checks wether two iterators are pointing toward the same elements.
INLINE
const bool
gumTopologyTplIterator::operator==( const gumTopologyTplIterator& source ) const
{
  return iterator == source.iterator;
}

// @return Returns the variable pointed by the iterator.
INLINE
gumDiscreteVariable&
gumTopologyTplIterator::operator*()
{
  return *( *iterator );
}

// Copy operator.
INLINE
const gumDiscreteVariable&
gumTopologyTplIterator::operator*() const
{
  return *( *iterator );
}

// Dereference the variable pointed by the iterator.
INLINE
gumDiscreteVariable*
gumTopologyTplIterator::operator->() const
{
  return *iterator;
}

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

