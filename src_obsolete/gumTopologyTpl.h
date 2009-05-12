/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
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
 * @brief Class representing a topology on variables.
 *
 * @author Lionel Torti
 */

#ifndef GUM_TOPOLOGYTPL_H
#define GUM_TOPOLOGYTPL_H

#include <iostream>
#include <string>

#include <agrum/gum_utils.h>

#include <agrum/gumDebug.h>
#include <agrum/gumDiscreteVariable.h>
#include <agrum/gumEdgeList.h>
#include <agrum/gumExceptions.h>
#include <agrum/gumGraph.h>
#include <agrum/utils/gumHashTable.h>
#include <agrum/gumNodeList.h>

class gumTopologyTpl;

class gumTopologyTplIterator;

/* ============================================================================== */
/* ===                 CLASS FOR REPRESENTING A TOPOLOGY ON VARIABLE          === */
/* ============================================================================== */

/** @class gumTopologyTpl
 * @brief Class representing a Topoloy on variables.
 *
 * This class is used to represent the main functionalities for
 * representing a set of variables related through a graph. The graph used here is
 * a DAG, i.e. a Directed Acyclic Graph.
 *
 * Each gumNode in the underlying DAG is associated with a gumDiscreteVariable.
 * Variables are added by copy, and a gumId is returned to keep track of the newly
 * added variables. Since gumDiscreteVariable is an abstract class, the factory
 * pattern is used to produce copies of added variables.
 *
 * The methods getMoralGraph, getTopologicalOrder uses a lazy instantiation,
 * meaning if the topology is changed, it is necessary to call getMoralGraph(),
 * resp. getTopologicalOrder(), to rebuild the moral graph, i.e. the topology
 * order. Since constant reference are used, the following code is correct:
 *
 * @code
 * gumTopologyTpl topology;
 * // Fill the topology
 * const gumUndiGraphTpl<gumNode,*gumEdge> &moralGraph = topology.getMoralGraph();
 *
 * // Changing the topology
 * topology.getMoralGraph();
 *
 * // The reference isn't changed, moralGraph is correctly updated
 * std::cerr << moralGraph;
 * @endcode
 *
 * A tricky thing about gumTopologyTpl is the generation of variable's id. By default,
 * the first id generated is 1, then the number is incremented for each added variable.
 * However, it is possible to chose the variable's id using the addVariable method
 * argument's varId, which default value is 0. By doing this, you will not affect the
 * next default id value. An example will make things clearer:
 *
 * @code
 * gumTopologyTpl topo;
 * gumLabelledVariable var1("A"), var2("B"), var3("C"), var100("X"), var101("Y");
 *
 * topo.addVariable(var1);        // var1 is given the id 1
 * topo.addVariable(var100, 100): // var100 is given the id 100
 * topo.addVariable(var101, 101); // var101 is given the id 101
 * topo.addVariable(var2);        // var2 is given the id 2
 *
 * for (int i = 4; i < 100; i++)
 * {
 *    topo.addVariable(gumLabelledVariable("plop");
 * }
 * // Last variable added had 99 as id
 * topo.addVariable(var3);        // var3 is given the id 102, since 100 and 101 are
 *                                // already used
 * @endcode
 *
 *
 * @see gumDAGTpl, gumDiscreteVariable
 */

class gumTopologyTpl: public gumDiscreteVariableListener {

  friend class gumTopologyTplIterator;

  public:
// ##############################################################################
/// @name Constructors / Destructors
// ##############################################################################
/// @{

    /**
     * Default constructor.
     *
     * Creates an empty topology.
     * @param firstId The beginning number to generate ids. By default the first id will
     * be equal to 1.
     */
    gumTopologyTpl(gumId firstId = 0);

    /**
     * Copy constructor.
     *
     * A deep copy is used.
     *
     * @param source The object which is copied.
     */
    gumTopologyTpl( const gumTopologyTpl& source );

    /**
     * Destructor.
     */
    ~gumTopologyTpl();

// @}

// ##############################################################################
/// @name Network Manipulation Methods
// ##############################################################################
/// @{

    /**
     * Getter for the subjacent DAG of *this.
     *
     * @return A constant reference on the subjacent DAG of *this.
     */
    const gumDAGTpl<gumNode, gumArc>& getDAG() const;

    /**
     * Getter for the structure's variables.
     * The variable can be freely modified, since *this will be warned for any
     * important changes.
     *
     * @param varId The associated node Id.
     * @return Returns a reference on the variable, where varId is the id of
     * it's associated node.
     * @throws gumNotFound Raised if no node has varId as id.
     */
    gumDiscreteVariable& variable( gumId varId );

    /** Getter for the structure's variables.
    *
    *  @param nodeID The associated node Id.
    *  @return A reference on the variable, where nodeID is the id of
    *  it's associated node.
    *  @throws gumElement²NotFound Raised if node node has nodeID as id.
    */
    const gumDiscreteVariable& variable( gumId varId ) const;

    /** Return id node from discrete var pointer.
     *
     * @param var The discrete var pointer.
     * @return A gumId& which is the id of it's associated node.
     * @throws gumNotFound Raised if this var pointer is not in the variables list.
     */
    gumId getIdFromVariable(const gumDiscreteVariable& var ) const;

    /**
     * Copy the variable, create it's associated node and added it to the topology.
     *
     * @param variable A pointer on the variable added to the topology.
     * @param varId The id of the variable in the topology.
     * @throw gumDuplicateElement Raised if id is already a variable's id.
     * @return A constant reference on the id of the new variable (and node).
     */
    //virtual gumId addVariable(const gumDiscreteVariable * const variable,gumId varId);

  /**
     * Copy the variable, create it's associated node and added it to the topology.
     *
     *  The id of the new variable is automatically generated.
     * @param variable A pointer on the variable added to the topology.
     * @return A constant reference on the id of the new variable (and node).
     */
    virtual gumId addVariable(const gumDiscreteVariable * const variable);

    /**
     * Erase a Variable from the topology.
     *
     * The variable and it's corresponding node are erased from the topology.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    virtual void eraseVariable( gumId id );

    /**
     * Add a new arc in the topology.
     * The arc inserted is a copy of the parameter.
     *
     * @param head and
     * @param tail as gumId
     * @return A constant reference of the copied version of the arc.
     * @throw gumInvalidEdge Raised if the arc reference nodes not found in the topology.
     */
    virtual const gumArc& insertArc( gumId head,gumId tail );

    /**
     * Erase an arc from the topology.
     * If no arc matches the ids, then nothing is done.
     *
     * @param from The source of the arc.
     * @param to The destination of the arc.
     */
    virtual void eraseArc( gumId tail, gumId head );

    /**
     * Clear every variables, nodes, and arcs of this topology.
     */
    void clear();

    /**
     * The node's id are coherent with the variables and nodes of the topology.
     *
     * @return The moral graph built from this topology.
     */
    const gumUndiGraphTpl<gumNode, gumEdge>& getMoralGraph() const;

    /**
     * The topological order stays the same as long as no variable or arcs are
     * added or erased from the topology.
     *
     * A lazy instantiation is used, so the topological order is computed only
     * on demand.
     *
     * @return Returns a constant reference on a list of the variable's
     * id in topological order.
     * @throw gumOperationNotAllowed Raised if for some reasons the topological order
     * can not be built.
     */
    const gumList<gumId>& getTopologicalOrder() const;

// @}

// ##############################################################################
/// @name Iteration Methods
// ##############################################################################
/// @{

    /**
     * @return Returns the number of variables in the topology.
     */
    const gumSize size();

    /**
     * @return Returns true if the topology is empty.
     */
    const bool empty() const;

    /**
     * Returns true if id matches a variable.
     */
    bool exists( gumId id ) const;

    /**
     * Iterator on a topology.
     */
    typedef gumTopologyTplIterator iterator;

    /**
     * @return Returns an iterator pointing at the begining of the Topology.
     */
    iterator begin() const;

    /**
     * @return Returna an iterator pointing at the end of the Topology.
     */
    const iterator& end() const;

// @}

// ##############################################################################
/// @name gumDiscreteVariable Implementation
// ##############################################################################
/// @{

    /**
     * @see gumDiscreteVariableListener
     */
    void  variableDomainChanged( gumDiscreteVariableEvent &e );

    /**
     * @see gumDiscreteVariableListener
     */
    void  variableNameChanged( gumDiscreteVariableEvent &e );

    /**
     * @see gumDiscreteVariableListener
     */
    void  variableDescriptionChanged( gumDiscreteVariableEvent &e );

// @}

// ##############################################################################
/// @name Operators
// ##############################################################################
/// @{

    /**
     * Copy operator.
     */
    gumTopologyTpl& operator=( const gumTopologyTpl& source );

// @}

  protected:
    /**
     * The graph representing the relations between the variables
     */
    gumDAGTpl<gumNode, gumArc> _dag;

    /**
     * Mapping between the node's gumIds and the variables
     */
    gumHashTable<gumId, gumDiscreteVariable*> _variableMap;

    /**
     * Call this method if the topology has changed.
     */
    void _topologyChanged() const;

    /**
     * Ids generation are generated following a given policy, chosen when
     * the constructor is called.
     * @return Returns the next id.
     */
    gumId _nextId();

  private:
    // The highest value among the variables id's in the topology
    gumId __nextId;

    // Serves to rebuild the topological order after the topology changed
    mutable bool __rebuildTopologicalOrder;

    // List of the variable's id in topological order.
    mutable gumList<gumId>* __topologicalOrder;

    // Serves to rebuild the moral graph after the topology changed
    mutable bool __rebuildMoralGraph;

    // The moral graph of this->dag
    mutable gumUndiGraphTpl<gumNode, gumEdge> *__moralGraph;

    // Serves to rebuild the end iterator after the topology changed
    mutable bool __rebuiltEndIterator;

    // Used for iterations on gumTopologyTpl
    mutable iterator* __endIter;

    // Used by getTopologicalOrder to insert root nodes in topologyChanged
    void __getRootTopologyLevel( gumNodeList<gumNode>& nodeList ) const;

    // Used by getTopologicalOrder to insert the next level of the topological order
    void __getNextTopologyLevel( gumNodeList<gumNode>& nodeList ) const;
};

/* ============================================================================== */
/* ===                        GUM_TOPOLOGY_ITERATOR                           === */
/* ============================================================================== */
class gumTopologyTplIterator {
  public:
    /* ============================================================================== */
    /* ===                     CONSTRUCTORS AND DESTUCTOR                         === */
    /* ============================================================================== */

    /**
     * Default Construtor.
     *
     * Builds an iterator pointing to nothing.
     */
    gumTopologyTplIterator();

    /**
     * Copy Constructor.
     */
    gumTopologyTplIterator( const gumTopologyTplIterator& source );

    /**
     * Builds an iterator based on the hashtable iterator.
     */
    gumTopologyTplIterator( const gumHashTableIterator<gumId, gumDiscreteVariable*>& iter );

    /**
     * Destructor.
     */
    ~gumTopologyTplIterator();


    /* ============================================================================== */
    /* ===                         GETTERS AND SETTERS                            === */
    /* ============================================================================== */

    /**
     * @return Returns the id corresponding to the variable pointed by the iterator.
     * @throw gumUndefinedIteratorValue if there's no variable pointed by the iterator.
     */
    gumId getId();

    /**
     * Makes the iterator point to nothing.
     */
    void clear();


    /* ============================================================================== */
    /* ===                              OPERATORS                                 === */
    /* ============================================================================== */

    /**
     * Copy operator.
     */
    gumTopologyTplIterator& operator=( const gumTopologyTplIterator& source );

    /**
     * Increment operator.
     */
    gumTopologyTplIterator& operator++();

    /**
     * Decrement operator.
     */
    gumTopologyTplIterator& operator--();

    /**
     * Checks wether two iterators are pointing toward different elements.
     */
    const bool operator!=( const gumTopologyTplIterator& source ) const;

    /**
     * Checks wether two iterators are pointing toward the same elements.
     */
    const bool operator==( const gumTopologyTplIterator& source ) const;

    /**
     * @return Returns the variable pointed by the iterator.
     * @throw gumUndefinedIteratorValue
     */
    gumDiscreteVariable& operator*();

    /**
     * Copy operator.
     * @throw gumUndefinedIteratorValue
     */
    const gumDiscreteVariable& operator*() const ;

    /**
     * Dereference the variable pointed by the iterator.
     * @throw gumUndefinedIteratorValue
     */
    gumDiscreteVariable* operator->() const;


  protected:
    /**
     * The hastable's iterator decorated by this class.
     */
    gumHashTableIterator<gumId, gumDiscreteVariable*> iterator;

};


#ifndef GUM_NO_INLINE
#include "gumTopologyTpl.inl"
#endif /* GUM_NO_INLINE */

#endif /* GUM_TOPOLOGYTPL_H*/

