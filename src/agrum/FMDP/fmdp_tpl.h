/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
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
    _onDestructionDeleteVars_ = onDestructionDeleteVar;

    // Default Action initialisation
    _actionMap_.insert(0, new std::string("DEFAULT"));
    _actionTransitionTable_.insert(0, new VarTransitionTable< GUM_SCALAR >());
    _actionCostTable_.insert(0, nullptr);
    _actionRewardTable_.insert(0, nullptr);
  }

  // ===========================================================================
  // Destructor.
  // ===========================================================================
  template < typename GUM_SCALAR >
  FMDP< GUM_SCALAR >::~FMDP() {
    // Action Transition Graph deletion
    for (auto iterA = _actionTransitionTable_.beginSafe();
         iterA != _actionTransitionTable_.endSafe();
         ++iterA) {
      if (iterA.val()) {
        for (auto iterH = (iterA.val())->beginSafe(); iterH != (iterA.val())->endSafe(); ++iterH)
          if (iterH.val()) delete iterH.val();
        delete iterA.val();
      }
    }

    // Action cost graph deletion
    for (auto iterA = _actionCostTable_.beginSafe(); iterA != _actionCostTable_.endSafe(); ++iterA)
      if (iterA.val()) delete iterA.val();

    // Action reward graph deletion
    for (auto iterA = _actionRewardTable_.beginSafe(); iterA != _actionRewardTable_.endSafe();
         ++iterA)
      if (iterA.val()) delete iterA.val();

    // Action Name deletion
    for (auto iterId = _actionMap_.beginSafe(); iterId != _actionMap_.endSafe(); ++iterId)
      delete iterId.second();

    // Primed Variables deletion
    for (auto varIter = _main2primed_.beginSafe(); varIter != _main2primed_.endSafe(); ++varIter) {
      delete varIter.second();
      if (_onDestructionDeleteVars_) delete varIter.first();
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
    if (_varSeq_.exists(var))
      GUM_ERROR(DuplicateElement,
                " Variable " << var->name() << " has already been inserted in FMDP.");


    _varSeq_.insert(var);

    // Prime version creation
    DiscreteVariable* primeVar = var->clone();
    primeVar->setName(var->name() + "'");
    _main2primed_.insert(var, primeVar);
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
  INLINE void FMDP< GUM_SCALAR >::addAction(Idx actionId, const std::string& action) {
    if (actionId == 0) GUM_ERROR(DuplicateElement, " Action Id 0 is reserved.")

    for (BijectionIteratorSafe< Idx, const std::string* > actIter = _actionMap_.beginSafe();
         actIter != _actionMap_.endSafe();
         ++actIter)
      if (*(actIter.second()) == action)
        GUM_ERROR(DuplicateElement,
                  " Action " << action << " has already been inserted in FMDP with this name.");

    if (_actionMap_.existsFirst(actionId))
      GUM_ERROR(DuplicateElement,
                " An action with same id (" << actionId << ") has already been inserted.");

    _actionMap_.insert(actionId, new std::string(action));

    _actionTransitionTable_.insert(actionId, new VarTransitionTable< GUM_SCALAR >());
    _actionCostTable_.insert(actionId, nullptr);
    _actionRewardTable_.insert(actionId, nullptr);

    _actionSeq_.insert(actionId);
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
    if (!_varSeq_.exists(var))
      GUM_ERROR(NotFound, " Variable " << var->name() << " has not been declared before.")

    if (!_actionTransitionTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionTransitionTable_[actionId]->exists(var))
      GUM_ERROR(DuplicateElement,
                " Variable " << var->name() << " already has a transition table in " << actionId
                             << " table.");

    _actionTransitionTable_[actionId]->insert(var, transition);
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               FMDP< GUM_SCALAR >::transition(Idx actionId, const DiscreteVariable* v) const {
    if (!_actionTransitionTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionTransitionTable_[actionId]->exists(v))
      return (*_actionTransitionTable_[actionId])[v];
    else
      return (*_actionTransitionTable_[0]).exists(v) ? (*_actionTransitionTable_[0])[v] : nullptr;
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
  INLINE void
     FMDP< GUM_SCALAR >::addCostForAction(Idx                                         actionId,
                                          const MultiDimImplementation< GUM_SCALAR >* cost) {
    if (!_actionCostTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionCostTable_[actionId] != nullptr)
      GUM_ERROR(DuplicateElement, " Action " << actionName(actionId) << " already has a cost table")

    _actionCostTable_[actionId] = cost;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >* FMDP< GUM_SCALAR >::cost(Idx actionId) const {
    if (!_actionCostTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionCostTable_[actionId]) return _actionCostTable_[actionId];
    return _actionCostTable_[0];
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
  INLINE void
     FMDP< GUM_SCALAR >::addRewardForAction(Idx                                         actionId,
                                            const MultiDimImplementation< GUM_SCALAR >* reward) {
    if (!_actionRewardTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionRewardTable_[actionId] != nullptr)
      GUM_ERROR(DuplicateElement,
                " Action " << actionName(actionId) << " already has a reward table");

    _actionRewardTable_[actionId] = reward;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               FMDP< GUM_SCALAR >::reward(Idx actionId) const {
    if (!_actionRewardTable_.exists(actionId))
      GUM_ERROR(NotFound, " Action " << actionName(actionId) << " has not been declared before.");

    if (_actionRewardTable_[actionId]) return _actionRewardTable_[actionId];
    return _actionRewardTable_[0];
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
    if (!_actionMap_.existsFirst(actionId))
      GUM_ERROR(NotFound, "No action with " << actionId << " as identifiant.")

    return *(_actionMap_.second(actionId));
  }

  // ===========================================================================
  // Returns action id
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE Idx FMDP< GUM_SCALAR >::actionId(const std::string& action) const {
    for (BijectionIterator< Idx, const std::string* > actIter = _actionMap_.begin();
         actIter != _actionMap_.end();
         ++actIter)
      if (*(actIter.second()) == action) { return actIter.first(); }

    GUM_ERROR(NotFound, " Action " << action << " has not been declared before.")
  }


  template < typename GUM_SCALAR >
  INLINE std::string FMDP< GUM_SCALAR >::toString() const {
    std::stringstream fmdpCore;

    for (auto actionIter = beginActions(); actionIter != endActions(); ++actionIter) {
      for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
        if (this->transition(*actionIter, *varIter))
          fmdpCore << RECAST(this->transition(*actionIter, *varIter))->toDot() << std::endl;
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
    for (auto actionIter = beginActions(); actionIter != endActions(); ++actionIter) {
      for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
        if (this->transition(*actionIter, *varIter))
          s += this->transition(*actionIter, *varIter)->realSize();
      if (this->reward(*actionIter)) s += this->reward(*actionIter)->realSize();
    }

    for (auto varIter = beginVariables(); varIter != endVariables(); ++varIter)
      if (this->transition(0, *varIter)) s += this->transition(0, *varIter)->realSize();
    if (this->reward()) s += this->reward()->realSize();
    return s;
  }
}   // namespace gum
