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
 * @brief Class for implementation of factored markov decision process.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_FACTORED_MARKOV_DECISION_PROCESS_H
#define GUM_FACTORED_MARKOV_DECISION_PROCESS_H
//======================================================================
#include <string>
#include <vector>
//======================================================================
#include <agrum/core/bijection.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
//======================================================================
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
//======================================================================

namespace gum {
  /**
   * @class FactoredMarkovDecisionProcess
   * @brief This class is used to implement factored decision process.
   * @ingroup fmdp_group
   * This class supports a mechanism which allows user to give, for each
   * variable ivolved in the process, a default transition probability table
   * and to describe for specific actions a different table.
   */

  template <typename GUM_SCALAR> class FactoredMarkovDecisionProcess {

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    FactoredMarkovDecisionProcess();

    /**
     * Default destructor.
     */
    ~FactoredMarkovDecisionProcess();

    /// @}

    // ===========================================================================
    /// @name FMDP instantiation methods.
    // ===========================================================================
    /// @{

    void setProperty(const std::string &propName, const std::string &propValue){};

    /**
     * Adds a variable to FMDP description
     * @throw DuplicateElement if a similar variable already exists
     */
    void addVariable(const DiscreteVariable *var);

    /**
     * Adds a primed variable to FMDP description
     * @throw DuplicateElement if a similar variable already exists
     */
    void addPrimedVariable(const DiscreteVariable *var,
                           const DiscreteVariable *mainVar);

    /**
     * Adds an action to FMDP description
     * @throw DuplicateElement if an action with same name already exists
     */
    void addAction(const std::string &action);

    /**
     * Adds a variable transition table to specified action
     * @throw NotFound if action or var does not exists
     * @throw DuplicateElement if variable already has a transition for this action
     */
    void addTransitionForAction(const DiscreteVariable *var,
                                const MultiDimImplementation<GUM_SCALAR> *transition,
                                const std::string &action);

    /**
     * Adds a default variable transition
     * @throw NotFound if var does not exists
     * @throw DuplicateElement if variable already has a default transition
     */
    void addTransition(const DiscreteVariable *var,
                       const MultiDimImplementation<GUM_SCALAR> *transition);

    /**
     * Adds a cost table to specified action
     * @throw NotFound if action does not exists
     */
    void addCostForAction(const MultiDimImplementation<GUM_SCALAR> *cost,
                          const std::string &action);

    /**
     * Adds a default variable cost
     * @throw DuplicateElement if a default cost exists already
     */
    void addCost(const MultiDimImplementation<GUM_SCALAR> *cost);

    /**
     * Adds a default variable reward
     * @throw DuplicateElement if a default reward exists already
     */
    void addReward(const MultiDimImplementation<GUM_SCALAR> *reward);

    /**
     * Precises the discount factor for that mdp
     */
    void addDiscount(GUM_SCALAR discount);

    /// @}

    // ===========================================================================
    /// @name FMPD manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns the discount factor of mdp
     */
    const GUM_SCALAR discount() const;

    /**
     * Returns the reward table of mdp
     */
    const MultiDimImplementation<GUM_SCALAR> *reward() const;

    /**
     * Resets the action iterator
     */
    void resetActionsIterator();

    /**
     * Indicates if iterator reached end of actions (false then ) or not (true )
     */
    bool hasAction() const;

    /**
     * Deplaces iterator onto next action
     */
    void nextAction();

    /**
     * Returns the id of current action pointed by actionIterator
     */
    Idx actionIterId() const;

    /**
     * Returns name of action given in parameter
     */
    const std::string actionName(Idx actionId) const;

    /**
     * Resets the variable iterator
     */
    void resetVariablesIterator();

    /**
     * Indicates if iterator reached end of variables (false then ) or not (true )
     */
    bool hasVariable() const;

    /**
     * Deplaces iterator onto next variable
     */
    void nextVariable();

    /**
     * Returns current primed variable pointed by variable iterator
     */
    const DiscreteVariable *variable() const;

    /**
     * Returns transition associated to current variable pointed by variable iterator
     * and current action poinbted by action iterator
     */
    const MultiDimImplementation<GUM_SCALAR> *transition() const;

    /**
     * Returns transition associated to given in parameter variable
     */
    const MultiDimImplementation<GUM_SCALAR> *
    transition(const DiscreteVariable *v) const;

    /**
     * Returns set of primed variable (variable at next instant )
     */
    const Set<const DiscreteVariable *> &primedVariables() const;

    /**
     * Returns the map on main variable and their primed version
     */
    const Bijection<const DiscreteVariable *, const DiscreteVariable *> &
    main2prime() const;

    /// @}
    std::string show() const;

    private:
    /**
      * Returns action id
      */
    const Idx &__actionId(const std::string &) const;

    /// Sequence de variables and its iterator
    Sequence<const DiscreteVariable *> __varSeq;
    SequenceIterator<const DiscreteVariable *> __varIter;

    /// Variable default transition cpt table
    HashTable<const DiscreteVariable *, const MultiDimImplementation<GUM_SCALAR> *>
        __defaultTransitionTable;

    /// Table which give for each action a table containing variables transition cpt
    HashTable<Idx, HashTable<const DiscreteVariable *,
                             const MultiDimImplementation<GUM_SCALAR> *> *>
        __actionTransitionTable;

    /// default cost table
    const MultiDimImplementation<GUM_SCALAR> *__defaultCostTable;

    /// Table which give for each action cost table
    HashTable<Idx, const MultiDimImplementation<GUM_SCALAR> *> __actionCostTable;

    /// Bijection mapping an action name to its id
    Bijection<Idx, const std::string *> __actionMap;

    /// default reward table
    const MultiDimImplementation<GUM_SCALAR> *__defaultRewardTable;

    Set<const DiscreteVariable *> __primedVariablesSet;
    Bijection<const DiscreteVariable *, const DiscreteVariable *> __main2primed;

    /// FMDP discount factor
    GUM_SCALAR __discount;

    /// Gives the next action id
    Idx __nextActionId;

    /// Iterator on actions
    HashTableConstIterator<
        Idx, HashTable<const DiscreteVariable *,
                       const MultiDimImplementation<GUM_SCALAR> *> *> __actionIter;
  };
} /* namespace gum */

#include <agrum/FMDP/FactoredMarkovDecisionProcess.tcc>

#endif // GUM_FACTORED_MARKOV_DECISION_PROCESS_H
