/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template implementation of FMDP/FMDP.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

//======================================================================
#include <cstdio>
#include <iostream>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
#define RECAST(x) reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(x)

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
  template < typename GUM_SCALAR >
  INLINE FMDP< GUM_SCALAR >::FMDP(bool onDestructionDeleteVar) {
    GUM_CONSTRUCTOR(FMDP);
    onDestructionDeleteVars__ = onDestructionDeleteVar;

    // Default Action initialisation
    actionMap__.insert(0, new std::string("DEFAULT"));
    actionTransitionTable__.insert(0, new VarTransitionTable< GUM_SCALAR >());
    actionCostTable__.insert(0, nullptr);
    actionRewardTable__.insert(0, nullptr);
  }

  // ===========================================================================
  // Destructor.
  // ===========================================================================
  template < typename GUM_SCALAR >
  FMDP< GUM_SCALAR >::~FMDP() {
    // Action Transition Graph deletion
    for (auto iterA = actionTransitionTable__.beginSafe();
         iterA != actionTransitionTable__.endSafe();
         ++iterA) {
      if (iterA.val()) {
        for (auto iterH = (iterA.val())->beginSafe();
             iterH != (iterA.val())->endSafe();
             ++iterH)
          if (iterH.val()) delete iterH.val();
        delete iterA.val();
      }
    }

    // Action cost graph deletion
    for (auto iterA = actionCostTable__.beginSafe();
         iterA != actionCostTable__.endSafe();
         ++iterA)
      if (iterA.val()) delete iterA.val();

    // Action reward graph deletion
    for (auto iterA = actionRewardTable__.beginSafe();
         iterA != actionRewardTable__.endSafe();
         ++iterA)
      if (iterA.val()) delete iterA.val();

    // Action Name deletion
    for (auto iterId = actionMap__.beginSafe(); iterId != actionMap__.endSafe();
         ++iterId)
      delete iterId.second();

    // Primed Variables deletion
    for (auto varIter = main2primed__.beginSafe();
         varIter != main2primed__.endSafe();
         ++varIter) {
      delete varIter.second();
      if (onDestructionDeleteVars__) delete varIter.first();
    }

    GUM_DESTRUCTOR(FMDP);
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Variable Handling Methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds a variable to FMDP description
  // @throw DuplicateElement if a similar variable already exists
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE void FMDP< GUM_SCALAR >::addVariable(const DiscreteVariable* var) {
    if (varSeq__.exists(var))
      GUM_ERROR(DuplicateElement,
                " Variable " << var->name()
                             << " has already been inserted in FMDP.");


    varSeq__.insert(var);

    // Prime version creation
    DiscreteVariable* primeVar = var->clone();
    primeVar->setName(var->name() + "'");
    main2primed__.insert(var, primeVar);
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Variable Handling Methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds an action to FMDP description
  // @throw DuplicateElement if an action with same name already exists
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE void FMDP< GUM_SCALAR >::addAction(Idx                actionId,
                                            const std::string& action) {
    if (actionId == 0) GUM_ERROR(DuplicateElement, " Action Id 0 is reserved.")

    for (BijectionIteratorSafe< Idx, const std::string* > actIter
         = actionMap__.beginSafe();
         actIter != actionMap__.endSafe();
         ++actIter)
      if (*(actIter.second()) == action)
        GUM_ERROR(DuplicateElement,
                  " Action "
                     << action
                     << " has already been inserted in FMDP with this name.");

    if (actionMap__.existsFirst(actionId))
      GUM_ERROR(DuplicateElement,
                " An action with same id (" << actionId
                                            << ") has already been inserted.");

    actionMap__.insert(actionId, new std::string(action));

    actionTransitionTable__.insert(actionId,
                                   new VarTransitionTable< GUM_SCALAR >());
    actionCostTable__.insert(actionId, nullptr);
    actionRewardTable__.insert(actionId, nullptr);

    actionSeq__.insert(actionId);
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Transition methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds a variable transition table to specified action
  // @throw NotFound if action or var does not exists
  // @throw DuplicateElement if variable already has a transition for this
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE void FMDP< GUM_SCALAR >::addTransitionForAction(
     Idx                                         actionId,
     const DiscreteVariable*                     var,
     const MultiDimImplementation< GUM_SCALAR >* transition) {
    if (!varSeq__.exists(var))
      GUM_ERROR(NotFound,
                " Variable " << var->name() << " has not been declared before.");

    if (!actionTransitionTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionTransitionTable__[actionId]->exists(var))
      GUM_ERROR(DuplicateElement,
                " Variable " << var->name()
                             << " already has a transition table in " << actionId
                             << " table.");

    actionTransitionTable__[actionId]->insert(var, transition);
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               FMDP< GUM_SCALAR >::transition(Idx                     actionId,
                                    const DiscreteVariable* v) const {
    if (!actionTransitionTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionTransitionTable__[actionId]->exists(v))
      return (*actionTransitionTable__[actionId])[v];
    else
      return (*actionTransitionTable__[0]).exists(v)
                        ? (*actionTransitionTable__[0])[v]
                        : nullptr;
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Cost methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds a cost table to specified action
  // @throw NotFound if action does not exists
  // @throw DuplicateElement if action already has a cost
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE void FMDP< GUM_SCALAR >::addCostForAction(
     Idx                                         actionId,
     const MultiDimImplementation< GUM_SCALAR >* cost) {
    if (!actionCostTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionCostTable__[actionId] != nullptr)
      GUM_ERROR(DuplicateElement,
                " Action " << actionName(actionId) << " already has a cost table");

    actionCostTable__[actionId] = cost;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               FMDP< GUM_SCALAR >::cost(Idx actionId) const {
    if (!actionCostTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionCostTable__[actionId]) return actionCostTable__[actionId];
    return actionCostTable__[0];
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Cost methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Adds a default variable reward
  // @throw DuplicateElement if a default reward exists already
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE void FMDP< GUM_SCALAR >::addRewardForAction(
     Idx                                         actionId,
     const MultiDimImplementation< GUM_SCALAR >* reward) {
    if (!actionRewardTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionRewardTable__[actionId] != nullptr)
      GUM_ERROR(DuplicateElement,
                " Action " << actionName(actionId)
                           << " already has a reward table");

    actionRewardTable__[actionId] = reward;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               FMDP< GUM_SCALAR >::reward(Idx actionId) const {
    if (!actionRewardTable__.exists(actionId))
      GUM_ERROR(NotFound,
                " Action " << actionName(actionId)
                           << " has not been declared before.");

    if (actionRewardTable__[actionId]) return actionRewardTable__[actionId];
    return actionRewardTable__[0];
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                               Miscelleanous methods. **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Returns name of action given in parameter
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const std::string& FMDP< GUM_SCALAR >::actionName(Idx actionId) const {
    if (!actionMap__.existsFirst(actionId))
      GUM_ERROR(NotFound, "No action with " << actionId << " as identifiant.")

    return *(actionMap__.second(actionId));
  }

  // ===========================================================================
  // Returns action id
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE Idx FMDP< GUM_SCALAR >::actionId(const std::string& action) const {
    for (BijectionIterator< Idx, const std::string* > actIter
         = actionMap__.begin();
         actIter != actionMap__.end();
         ++actIter)
      if (*(actIter.second()) == action) { return actIter.first(); }

    GUM_ERROR(NotFound, " Action " << action << " has not been declared before.")
  }


  template < typename GUM_SCALAR >
  INLINE std::string FMDP< GUM_SCALAR >::toString() const {
    std::stringstream fmdpCore;

    for (auto actionIter = beginActions(); actionIter != endActions();
         ++actionIter) {
      for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
        if (this->transition(*actionIter, *varIter))
          fmdpCore << RECAST(this->transition(*actionIter, *varIter))->toDot()
                   << std::endl;
      if (this->reward(*actionIter))
        fmdpCore << RECAST(this->reward(*actionIter))->toDot() << std::endl;
    }

    for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
      if (this->transition(0, *varIter))
        fmdpCore << RECAST(this->transition(0, *varIter))->toDot() << std::endl;
    if (this->reward()) fmdpCore << RECAST(this->reward())->toDot() << std::endl;
    return fmdpCore.str();
  }


  template < typename GUM_SCALAR >
  INLINE Size FMDP< GUM_SCALAR >::size() const {
    Size s = 0;
    for (auto actionIter = beginActions(); actionIter != endActions();
         ++actionIter) {
      for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
        if (this->transition(*actionIter, *varIter))
          s += this->transition(*actionIter, *varIter)->realSize();
      if (this->reward(*actionIter)) s += this->reward(*actionIter)->realSize();
    }

    for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
      if (this->transition(0, *varIter))
        s += this->transition(0, *varIter)->realSize();
    if (this->reward()) s += this->reward()->realSize();
    return s;
  }
}   // namespace gum
