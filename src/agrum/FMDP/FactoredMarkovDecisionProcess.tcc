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

//======================================================================
#include <cstdio>
#include <iostream>
//======================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
//======================================================================

namespace gum {


/* ************************************************************************************************** **/
/* **                                                                                                 **/
/* **                               Constructors / Destructors                                        **/
/* **                                                                                                 **/
/* ************************************************************************************************** **/

    // ===========================================================================
    // Default constructor.
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    FactoredMarkovDecisionProcess<GUM_SCALAR>::FactoredMarkovDecisionProcess(bool onDestructionDeleteVar) {
        GUM_CONSTRUCTOR ( FactoredMarkovDecisionProcess );
        __defaultCostTable = nullptr;
        __defaultRewardTable = nullptr;
        __discount = 0;
        __onDestructionDeleteVars = onDestructionDeleteVar;
    }

    // ===========================================================================
    // Destructor.
    // ===========================================================================
    template<typename GUM_SCALAR>
    FactoredMarkovDecisionProcess<GUM_SCALAR>::~FactoredMarkovDecisionProcess() {

        // Action Transition Graph deletion
        for ( auto iterA = __actionTransitionTable.beginSafe(); iterA != __actionTransitionTable.endSafe(); ++iterA ) {
            for ( auto iterH = ( iterA.val() )->beginSafe(); iterH != ( iterA.val() )->endSafe(); ++iterH )
                delete iterH.val();
            delete iterA.val();
        }

        // Default Transition Graph Deletion
        for ( auto iter = __defaultTransitionTable.beginSafe(); iter != __defaultTransitionTable.endSafe(); ++iter )
            if ( iter.val() != nullptr )
                delete iter.val();

        // Action cost graph deletion
        for ( auto iterA = __actionCostTable.beginSafe(); iterA != __actionCostTable.endSafe(); ++iterA )
            delete iterA.val();

        // Default cost graph deletion
        if ( __defaultCostTable )
            delete __defaultCostTable;

        // Reward graph deletion
        if ( __defaultRewardTable )
            delete __defaultRewardTable;

        // Action Name deletion
        for ( auto iterId = __actionMap.beginSafe(); iterId != __actionMap.endSafe(); ++iterId )
            delete iterId.second();

        // Primed Variables deletion
        for( auto varIter = __main2primed.beginSafe(); varIter != __main2primed.endSafe(); ++varIter ){
            delete varIter.second();
            if(__onDestructionDeleteVars)
                delete varIter.first();
        }

        GUM_DESTRUCTOR ( FactoredMarkovDecisionProcess );
    }


  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                               FMDP instantiation methods.                                       **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Adds a variable to FMDP description
    // @throw DuplicateElement if a similar variable already exists
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addVariable ( const DiscreteVariable* var ) {

        if ( __defaultTransitionTable.exists ( var ) )
            GUM_ERROR ( DuplicateElement, " Variable " << var->name() << " has already been inserted in FMDP." );

        __defaultTransitionTable.insert ( var, nullptr );

        __varSeq.insert ( var );

        // Prime version creation
        DiscreteVariable* primeVar = var->clone();
        primeVar->setName(var->name() + "'");
        __main2primed.insert(var, primeVar);
    }

    // ===========================================================================
    // Adds an action to FMDP description
    // @throw DuplicateElement if an action with same name already exists
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void FactoredMarkovDecisionProcess<GUM_SCALAR>::addAction( Idx actionId, const std::string& action ) {

        for ( BijectionIteratorSafe< Idx, const std::string* > actIter = __actionMap.beginSafe(); actIter != __actionMap.endSafe(); ++actIter )
            if ( * ( actIter.second() ) == action )
                GUM_ERROR ( DuplicateElement, " Action " << action << " has already been inserted in FMDP with this name." );

        if ( __actionMap.existsFirst(actionId) )
            GUM_ERROR ( DuplicateElement, " An action with same id has already been inserted." );

        __actionMap.insert ( actionId, new std::string ( action ) );

        __actionTransitionTable.insert ( actionId, new HashTable< const DiscreteVariable*, const MultiDimImplementation< GUM_SCALAR >* >() );
        __actionCostTable.insert ( actionId, nullptr );

        __actionSeq.insert(actionId);
    }

    // ===========================================================================
    // Adds a variable transition table to specified action
    // @throw NotFound if action or var does not exists
    // @throw DuplicateElement if variable already has a transition for this action
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addTransitionForAction ( const std::string& action,
                                                                        const DiscreteVariable* var,
                                                                        const MultiDimImplementation<GUM_SCALAR>* transition ) {

        Idx actionId = __actionId ( action );

        if ( !__defaultTransitionTable.exists ( var ) )
            GUM_ERROR ( NotFound, " Variable " << var->name() << " has not been declared before." );

        if ( __actionTransitionTable[actionId]->exists ( var ) )
            GUM_ERROR ( DuplicateElement, " Variable " << var->name() << " already has a transition table in " << actionId << " table." );

        __actionTransitionTable[actionId]->insert ( var, transition );
    }

    // ===========================================================================
    // Adds a default variable transition
    // @throw NotFound if var does not exists
    // @throw DuplicateElement if variable already has a default transition
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addTransition ( const DiscreteVariable* var,
                                                               const MultiDimImplementation<GUM_SCALAR>* transition ) {

        if ( !__defaultTransitionTable.exists ( var ) )
            GUM_ERROR ( NotFound, " Variable " << var->name() << " has not been declared before." );

        if ( __defaultTransitionTable[var] != nullptr )
            GUM_ERROR ( DuplicateElement, " Variable " << var->name() << " already has a default transition table" );

        __defaultTransitionTable.insert ( var, transition );
    }

    // ===========================================================================
    // Adds a cost table to specified action
    // @throw NotFound if action does not exists
    // @throw DuplicateElement if action already has a cost
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addCostForAction (const std::string& action , const MultiDimImplementation<GUM_SCALAR>* cost) {

        Idx actionId = __actionId ( action );

        if ( __actionCostTable[ actionId ] != nullptr )
            GUM_ERROR ( DuplicateElement, " Action " << action << " already has a cost table" );

        __actionCostTable.insert ( actionId, cost );
    }

    // ===========================================================================
    // Adds a default variable cost
    // @throw DuplicateElement if a default cost exists already
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addCost ( const MultiDimImplementation<GUM_SCALAR>* cost ) {

        if ( __defaultCostTable != nullptr )
            GUM_ERROR ( DuplicateElement, " Default cost table already specified " );

        __defaultCostTable = cost;
    }

    // ===========================================================================
    // Adds a default variable reward
    // @throw DuplicateElement if a default reward exists already
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::addReward ( const MultiDimImplementation<GUM_SCALAR>* reward ) {

        if ( __defaultRewardTable != nullptr )
            GUM_ERROR ( DuplicateElement, " Default reward table already specified " );

        __defaultRewardTable = reward;
    }

    // ===========================================================================
    // Precises the discount factor for that mdp
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    void
    FactoredMarkovDecisionProcess<GUM_SCALAR>::setDiscount ( GUM_SCALAR discount ) {
        __discount = discount;
    }


/* ************************************************************************************************** **/
/* **                                                                                                 **/
/* **                               FMPD manipulation methods.                                        **/
/* **                                                                                                 **/
/* ************************************************************************************************** **/

    // ===========================================================================
    // Returns the discount factor for that mdp
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    const GUM_SCALAR
    FactoredMarkovDecisionProcess<GUM_SCALAR>::discount( ) const {
        return __discount;
    }

    // ===========================================================================
    // Returns the reward table of mdp
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    const MultiDimImplementation< GUM_SCALAR >*
    FactoredMarkovDecisionProcess<GUM_SCALAR>::reward( ) const {
        return __defaultRewardTable;
    }


    // ===========================================================================
    // Returns name of action given in parameter
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    const std::string
    FactoredMarkovDecisionProcess<GUM_SCALAR>::actionName ( Idx actionId ) const {

        if ( !__actionMap.existsFirst ( actionId ) )
            GUM_ERROR ( NotFound, "No action with " << actionId << " as identifiant." );

        return *(__actionMap.second ( actionId ));
    }

    // ===========================================================================
    // Returns transition associated to given in parameter variable and given action
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    const MultiDimImplementation< GUM_SCALAR >*
    FactoredMarkovDecisionProcess<GUM_SCALAR>::transition ( Idx actionId, const DiscreteVariable* v ) const {

        if ( __actionTransitionTable[actionId]->exists ( v ) )
            return ( * __actionTransitionTable[actionId] ) [ v ];
        else
            return __defaultTransitionTable[ v ];
    }

    template<typename GUM_SCALAR> INLINE
    Set<const DiscreteVariable*>
    FactoredMarkovDecisionProcess<GUM_SCALAR>::variables( ) const {
        Set<const DiscreteVariable*> ret;
        for (auto varIter = __main2primed.beginSafe(); varIter != __main2primed.endSafe(); ++varIter)
            ret << varIter.first();
        return ret;
    }

    // ===========================================================================
    // Returns the map on main variable and their primed version
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    const DiscreteVariable*
    FactoredMarkovDecisionProcess<GUM_SCALAR>::main2prime( const DiscreteVariable* mainVar ) const {
        return __main2primed.second(mainVar);
    }


    template<typename GUM_SCALAR> INLINE
    std::string
    FactoredMarkovDecisionProcess<GUM_SCALAR>::show( ) const {
        std::stringstream fmdpCore;

        for ( auto actionIter = __actionTransitionTable.beginSafe(); actionIter != __actionTransitionTable.endSafe(); ++actionIter ) {
            for ( auto tableIter = ( actionIter.val() )->beginSafe(); tableIter != ( actionIter.val() )->endSafe(); ++tableIter ) {
                fmdpCore << std::endl << reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*>(tableIter.val())->toDot();
            }
        }

        fmdpCore << std::endl << reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*>(__defaultRewardTable)->toDot();
        return fmdpCore.str();
    }


    template<typename GUM_SCALAR> INLINE
    Size
    FactoredMarkovDecisionProcess<GUM_SCALAR>::size( ) const {
        Size s = 0;
        for ( auto actionIter = beginActions(); actionIter != endActions(); ++actionIter )
            for ( auto varIter = beginVariables(); varIter != endVariables(); ++varIter )
                s += this->transition(*actionIter, *varIter)->realSize();
        s += __defaultRewardTable->realSize();
        return s;
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                               Protected methods.                                                **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

// ===========================================================================
// Returns action id
// ===========================================================================
  template<typename GUM_SCALAR> INLINE
  const Idx&
  FactoredMarkovDecisionProcess<GUM_SCALAR>::__actionId ( const std::string& action ) const {

    for ( BijectionIteratorSafe< Idx, const std::string* > actIter = __actionMap.beginSafe(); actIter != __actionMap.endSafe(); ++actIter )
      if ( * ( actIter.second() ) == action ) {
        return actIter.first();
      }

    GUM_ERROR ( NotFound, " Action " << action << " has not been declared before." );
  }

}
