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
* @brief Template implementation of FMDP/FactoredMarkovDecisionProcess.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <cstdio>
#include <iostream>

namespace gum {

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Constructors / Destructors **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Default constructor.
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE FactoredMarkovDecisionProcess<GUM_SCALAR>::FactoredMarkovDecisionProcess()
      : __varIter(__varSeq) {
    GUM_CONSTRUCTOR(FactoredMarkovDecisionProcess);
    __defaultCostTable = nullptr;
    __defaultRewardTable = nullptr;
    __discount = 0;
    __nextActionId = 1;
    resetActionsIterator();
    resetVariablesIterator();
  }

  // ===========================================================================
  // Destructor.
  // ===========================================================================
  template <typename GUM_SCALAR>
  FactoredMarkovDecisionProcess<GUM_SCALAR>::~FactoredMarkovDecisionProcess() {

    GUM_DESTRUCTOR(FactoredMarkovDecisionProcess);

    /// Table which give for each action a table containing variables transition cpt
    for (const auto &action : __actionTransitionTable) {
      for (const auto &elt : *action.second)
        delete elt.second;

      delete action.second;
    }

    if (__defaultCostTable != nullptr)
      delete __defaultCostTable;

    for (const auto &action : __actionCostTable)
      delete action.second;

    for (BijectionIterator<Idx, const std::string *> actIter = __actionMap.begin();
         actIter != __actionMap.end(); ++actIter)
      delete actIter.second();

    if (__defaultRewardTable != nullptr)
      delete __defaultRewardTable;

    for (const auto &elt : __defaultTransitionTable) {
      if (elt.second != nullptr)
        delete elt.second;

      delete elt.first;
    }

    for (const auto &primed : __primedVariablesSet)
      delete primed;
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               FMDP instantiation methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds a variable to FMDP description
  // @throw DuplicateElement if a similar variable already exists
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addVariable(
      const DiscreteVariable *var) {

    if (__defaultTransitionTable.exists(var))
      GUM_ERROR(DuplicateElement, " Variable "
                                      << var->name()
                                      << " has already been inserted in FMDP.");

    __defaultTransitionTable.insert(var, nullptr);

    __varSeq.insert(var);
  }

  // ===========================================================================
  // Adds a variable to FMDP description
  // @throw DuplicateElement if a similar variable already exists
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addPrimedVariable(
      const DiscreteVariable *var, const DiscreteVariable *mainVar) {

    if (__primedVariablesSet.exists(var))
      GUM_ERROR(DuplicateElement, " Variable "
                                      << var->name()
                                      << " has already been inserted in FMDP.");

    __primedVariablesSet.insert(var);
    __main2primed.insert(mainVar, var);
  }

  // ===========================================================================
  // Adds an action to FMDP description
  // @throw DuplicateElement if an action with same name already exists
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void
  FactoredMarkovDecisionProcess<GUM_SCALAR>::addAction(const std::string &action) {

    for (BijectionIterator<Idx, const std::string *> actIter = __actionMap.begin();
         actIter != __actionMap.end(); ++actIter)
      if (*(actIter.second()) == action) {
        GUM_ERROR(DuplicateElement,
                  " Action " << action << " has already been inserted in FMDP.");
      }

    Idx newActionId = __nextActionId;
    ++__nextActionId;
    __actionMap.insert(newActionId, new std::string(action));

    __actionTransitionTable.insert(
        newActionId, new HashTable<const DiscreteVariable *,
                                   const MultiDimImplementation<GUM_SCALAR> *>());
    __actionCostTable.insert(newActionId, nullptr);
  }

  // ===========================================================================
  // Adds a variable transition table to specified action
  // @throw NotFound if action or var does not exists
  // @throw DuplicateElement if variable already has a transition for this action
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addTransitionForAction(
      const DiscreteVariable *var,
      const MultiDimImplementation<GUM_SCALAR> *transition,
      const std::string &action) {

    Idx actionId = __actionId(action);

    if (!__defaultTransitionTable.exists(var))
      GUM_ERROR(NotFound, " Variable " << var->name()
                                       << " has not been declared before.");

    if (__actionTransitionTable[actionId]->exists(var))
      GUM_ERROR(DuplicateElement,
                " Variable " << var->name() << " already has a transition table in "
                             << actionId << " table.");

    __actionTransitionTable[actionId]->insert(var, transition);
  }

  // ===========================================================================
  // Adds a default variable transition
  // @throw NotFound if var does not exists
  // @throw DuplicateElement if variable already has a default transition
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addTransition(
      const DiscreteVariable *var,
      const MultiDimImplementation<GUM_SCALAR> *transition) {

    if (!__defaultTransitionTable.exists(var))
      GUM_ERROR(NotFound, " Variable " << var->name()
                                       << " has not been declared before.");

    if (__defaultTransitionTable[var] != nullptr)
      GUM_ERROR(DuplicateElement, " Variable "
                                      << var->name()
                                      << " already has a default transition table");

    __defaultTransitionTable.insert(var, transition);
  }

  // ===========================================================================
  // Adds a cost table to specified action
  // @throw NotFound if action does not exists
  // @throw DuplicateElement if action already has a cost
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addCostForAction(
      const MultiDimImplementation<GUM_SCALAR> *cost, const std::string &action) {

    Idx actionId = __actionId(action);

    if (__actionCostTable[actionId] != nullptr)
      GUM_ERROR(DuplicateElement, " Action " << action
                                             << " already has a cost table");

    __actionCostTable.insert(actionId, cost);
  }

  // ===========================================================================
  // Adds a default variable cost
  // @throw DuplicateElement if a default cost exists already
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addCost(
      const MultiDimImplementation<GUM_SCALAR> *cost) {

    if (__defaultCostTable != nullptr)
      GUM_ERROR(DuplicateElement, " Default cost table already specified ");

    __defaultCostTable = cost;
  }

  // ===========================================================================
  // Adds a default variable reward
  // @throw DuplicateElement if a default reward exists already
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::addReward(
      const MultiDimImplementation<GUM_SCALAR> *reward) {

    if (__defaultRewardTable != nullptr)
      GUM_ERROR(DuplicateElement, " Default reward table already specified ");

    __defaultRewardTable = reward;
  }

  // ===========================================================================
  // Precises the discount factor for that mdp
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void
  FactoredMarkovDecisionProcess<GUM_SCALAR>::addDiscount(GUM_SCALAR discount) {
    __discount = discount;
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               FMPD manipulation methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Returns the discount factor for that mdp
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR
      FactoredMarkovDecisionProcess<GUM_SCALAR>::discount() const {
    return __discount;
  }

  // ===========================================================================
  // Returns the reward table of mdp
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR> *
  FactoredMarkovDecisionProcess<GUM_SCALAR>::reward() const {
    return __defaultRewardTable;
  }

  // ===========================================================================
  // Resets the action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::resetActionsIterator() {
    this->__actionIter = this->__actionTransitionTable.begin();
  }

  // ===========================================================================
  // Resets the action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE bool FactoredMarkovDecisionProcess<GUM_SCALAR>::hasAction() const {
    return __actionIter != __actionTransitionTable.end();
  }

  // ===========================================================================
  // Resets the action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::nextAction() {
    ++__actionIter;
  }

  // ===========================================================================
  // Returns the id of current action pointed by actionIterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE Idx FactoredMarkovDecisionProcess<GUM_SCALAR>::actionIterId() const {
    return __actionIter.key();
  }

  // ===========================================================================
  // Returns name of action given in parameter
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const std::string
  FactoredMarkovDecisionProcess<GUM_SCALAR>::actionName(Idx actionId) const {

    if (!__actionMap.existsFirst(actionId))
      GUM_ERROR(NotFound, "No action with " << actionId << " as identifiant.");

    const std::string *ret = __actionMap.second(actionId);
    return *ret;
  }

  // ===========================================================================
  // Resets the variable iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::resetVariablesIterator() {
    __varIter = __varSeq.begin();
  }

  // ===========================================================================
  // Resets the action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE bool FactoredMarkovDecisionProcess<GUM_SCALAR>::hasVariable() const {
    //       return __varIter != __defaultTransitionTable.end();
    return __varIter != __varSeq.end();
  }

  // ===========================================================================
  // Returns current primed variable pointed by variable iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable *
  FactoredMarkovDecisionProcess<GUM_SCALAR>::variable() const {
    //       return __main2primed.second( __varIter.key() );
    return __main2primed.second(*__varIter);
  }

  // ===========================================================================
  // Resets the action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void FactoredMarkovDecisionProcess<GUM_SCALAR>::nextVariable() {
    ++__varIter;
  }

  // ===========================================================================
  // Returns transition associated to current variable pointed by variable iterator
  // and current action poinbted by action iterator
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR> *
  FactoredMarkovDecisionProcess<GUM_SCALAR>::transition() const {

    if ((__actionIter.val())->exists(*__varIter))
      return (*(__actionIter.val()))[*__varIter];
    else
      return __defaultTransitionTable[*__varIter];
  }

  // ===========================================================================
  // Returns transition associated to given in parameter variable
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR> *
  FactoredMarkovDecisionProcess<GUM_SCALAR>::transition(
      const DiscreteVariable *v) const {

    if ((__actionIter.val())->exists(v))
      return (*(__actionIter.val()))[v];
    else
      return __defaultTransitionTable[v];
  }

  // ===========================================================================
  // Returns set of primed variable (variable at next instant )
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const Set<const DiscreteVariable *> &
  FactoredMarkovDecisionProcess<GUM_SCALAR>::primedVariables() const {
    return __primedVariablesSet;
  }

  // ===========================================================================
  // Returns the map on main variable and their primed version
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const Bijection<const DiscreteVariable *, const DiscreteVariable *> &
  FactoredMarkovDecisionProcess<GUM_SCALAR>::main2prime() const {
    return __main2primed;
  }

  template <typename GUM_SCALAR>
  INLINE std::string FactoredMarkovDecisionProcess<GUM_SCALAR>::show() const {

    std::stringstream fmdpCore;

    for (const auto &action : __actionTransitionTable) {
      for (const auto &elt : *action.second) {
        std::stringstream graphName;
        graphName << " ACTION : " << actionName(action.first)
                  << " VARIABLE : " << elt.first->name();
        fmdpCore
            << std::endl
            << reinterpret_cast<const MultiDimDecisionDiagramBase<GUM_SCALAR> *>(
                   elt.second)->toDot(graphName.str());
      }
    }

    fmdpCore << std::endl
             << reinterpret_cast<const MultiDimDecisionDiagramBase<GUM_SCALAR> *>(
                    __defaultRewardTable)->toDot("REWARD");
    return fmdpCore.str();
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Protected methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Returns action id
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const Idx &FactoredMarkovDecisionProcess<GUM_SCALAR>::__actionId(
      const std::string &action) const {

    for (BijectionIterator<Idx, const std::string *> actIter = __actionMap.begin();
         actIter != __actionMap.end(); ++actIter)
      if (*(actIter.second()) == action) {
        return actIter.first();
      }

    GUM_ERROR(NotFound, " Action " << action << " has not been declared before.");
  }
}
