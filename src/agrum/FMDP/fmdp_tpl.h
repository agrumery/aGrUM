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
* @brief Template implementation of FMDP/FMDP.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

//======================================================================
#include <cstdio>
#include <iostream>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
#define RECAST( x ) reinterpret_cast<const MultiDimFunctionGraph<GUM_SCALAR>*>( x )

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
  INLINE FMDP<GUM_SCALAR>::FMDP( bool onDestructionDeleteVar ) {
    GUM_CONSTRUCTOR( FMDP );
    __onDestructionDeleteVars = onDestructionDeleteVar;

    // Default Action initialisation
    __actionMap.insert( 0, new std::string( "DEFAULT" ) );
    __actionTransitionTable.insert( 0, new VarTransitionTable<GUM_SCALAR>() );
    __actionCostTable.insert( 0, nullptr );
    __actionRewardTable.insert( 0, nullptr );
  }

  // ===========================================================================
  // Destructor.
  // ===========================================================================
  template <typename GUM_SCALAR>
  FMDP<GUM_SCALAR>::~FMDP() {

    // Action Transition Graph deletion
    for ( auto iterA = __actionTransitionTable.beginSafe();
          iterA != __actionTransitionTable.endSafe();
          ++iterA ) {
      if ( iterA.val() ) {
        for ( auto iterH = ( iterA.val() )->beginSafe();
              iterH != ( iterA.val() )->endSafe();
              ++iterH )
          if ( iterH.val() ) delete iterH.val();
        delete iterA.val();
      }
    }

    // Action cost graph deletion
    for ( auto iterA = __actionCostTable.beginSafe();
          iterA != __actionCostTable.endSafe();
          ++iterA )
      if ( iterA.val() ) delete iterA.val();

    // Action reward graph deletion
    for ( auto iterA = __actionRewardTable.beginSafe();
          iterA != __actionRewardTable.endSafe();
          ++iterA )
      if ( iterA.val() ) delete iterA.val();

    // Action Name deletion
    for ( auto iterId = __actionMap.beginSafe(); iterId != __actionMap.endSafe();
          ++iterId )
      delete iterId.second();

    // Primed Variables deletion
    for ( auto varIter = __main2primed.beginSafe();
          varIter != __main2primed.endSafe();
          ++varIter ) {
      delete varIter.second();
      if ( __onDestructionDeleteVars ) delete varIter.first();
    }

    GUM_DESTRUCTOR( FMDP );
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
  template <typename GUM_SCALAR>
  INLINE void FMDP<GUM_SCALAR>::addVariable( const DiscreteVariable* var ) {

    if ( __varSeq.exists( var ) )
      GUM_ERROR( DuplicateElement,
                 " Variable " << var->name()
                              << " has already been inserted in FMDP." );


    __varSeq.insert( var );

    // Prime version creation
    DiscreteVariable* primeVar = var->clone();
    primeVar->setName( var->name() + "'" );
    __main2primed.insert( var, primeVar );
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
  template <typename GUM_SCALAR>
  INLINE void FMDP<GUM_SCALAR>::addAction( Idx                actionId,
                                           const std::string& action ) {

    if ( actionId == 0 )
      GUM_ERROR( DuplicateElement, " Action Id 0 is reserved." );

    for ( BijectionIteratorSafe<Idx, const std::string*> actIter =
              __actionMap.beginSafe();
          actIter != __actionMap.endSafe();
          ++actIter )
      if ( *( actIter.second() ) == action )
        GUM_ERROR( DuplicateElement,
                   " Action "
                       << action
                       << " has already been inserted in FMDP with this name." );

    if ( __actionMap.existsFirst( actionId ) )
      GUM_ERROR( DuplicateElement,
                 " An action with same id has already been inserted." );

    __actionMap.insert( actionId, new std::string( action ) );

    __actionTransitionTable.insert( actionId,
                                    new VarTransitionTable<GUM_SCALAR>() );
    __actionCostTable.insert( actionId, nullptr );
    __actionRewardTable.insert( actionId, nullptr );

    __actionSeq.insert( actionId );
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
  template <typename GUM_SCALAR>
  INLINE void FMDP<GUM_SCALAR>::addTransitionForAction(
      Idx                                       actionId,
      const DiscreteVariable*                   var,
      const MultiDimImplementation<GUM_SCALAR>* transition ) {

    if ( !__varSeq.exists( var ) )
      GUM_ERROR( NotFound,
                 " Variable " << var->name() << " has not been declared before." );

    if ( !__actionTransitionTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionTransitionTable[actionId]->exists( var ) )
      GUM_ERROR( DuplicateElement,
                 " Variable " << var->name()
                              << " already has a transition table in "
                              << actionId
                              << " table." );

    __actionTransitionTable[actionId]->insert( var, transition );
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR>*
  FMDP<GUM_SCALAR>::transition( Idx actionId, const DiscreteVariable* v ) const {

    if ( !__actionTransitionTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionTransitionTable[actionId]->exists( v ) )
      return ( *__actionTransitionTable[actionId] )[v];
    else
      return ( *__actionTransitionTable[0] ).exists( v )
                 ? ( *__actionTransitionTable[0] )[v]
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
  template <typename GUM_SCALAR>
  INLINE void FMDP<GUM_SCALAR>::addCostForAction(
      Idx actionId, const MultiDimImplementation<GUM_SCALAR>* cost ) {

    if ( !__actionCostTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionCostTable[actionId] != nullptr )
      GUM_ERROR( DuplicateElement,
                 " Action " << actionName( actionId )
                            << " already has a cost table" );

    __actionCostTable[actionId] = cost;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR>*
  FMDP<GUM_SCALAR>::cost( Idx actionId ) const {

    if ( !__actionCostTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionCostTable[actionId] ) return __actionCostTable[actionId];
    return __actionCostTable[0];
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
  template <typename GUM_SCALAR>
  INLINE void FMDP<GUM_SCALAR>::addRewardForAction(
      Idx actionId, const MultiDimImplementation<GUM_SCALAR>* reward ) {

    if ( !__actionRewardTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionRewardTable[actionId] != nullptr )
      GUM_ERROR( DuplicateElement,
                 " Action " << actionName( actionId )
                            << " already has a reward table" );

    __actionRewardTable[actionId] = reward;
  }


  // ===========================================================================
  // Returns transition associated to given in parameter variable and given
  // action
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR>*
  FMDP<GUM_SCALAR>::reward( Idx actionId ) const {

    if ( !__actionRewardTable.exists( actionId ) )
      GUM_ERROR( NotFound,
                 " Action " << actionName( actionId )
                            << " has not been declared before." );

    if ( __actionRewardTable[actionId] ) return __actionRewardTable[actionId];
    return __actionRewardTable[0];
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
  template <typename GUM_SCALAR>
  INLINE const std::string& FMDP<GUM_SCALAR>::actionName( Idx actionId ) const {

    if ( !__actionMap.existsFirst( actionId ) )
      GUM_ERROR( NotFound, "No action with " << actionId << " as identifiant." );

    return *( __actionMap.second( actionId ) );
  }

  // ===========================================================================
  // Returns action id
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE Idx FMDP<GUM_SCALAR>::actionId( const std::string& action ) const {

    for ( BijectionIterator<Idx, const std::string*> actIter = __actionMap.begin();
          actIter != __actionMap.end();
          ++actIter )
      if ( *( actIter.second() ) == action ) {
        return actIter.first();
      }

    GUM_ERROR( NotFound,
               " Action " << action << " has not been declared before." );
  }


  template <typename GUM_SCALAR>
  INLINE std::string FMDP<GUM_SCALAR>::toString() const {
    std::stringstream fmdpCore;

    for ( auto actionIter = beginActions(); actionIter != endActions();
          ++actionIter ) {
      for ( auto varIter = beginVariables(); varIter != endVariables(); ++varIter )
        if ( this->transition( *actionIter, *varIter ) )
          fmdpCore << RECAST( this->transition( *actionIter, *varIter ) )->toDot()
                   << std::endl;
      if ( this->reward( *actionIter ) )
        fmdpCore << RECAST( this->reward( *actionIter ) )->toDot() << std::endl;
    }

    for ( auto varIter = beginVariables(); varIter != endVariables(); ++varIter )
      if ( this->transition( 0, *varIter ) )
        fmdpCore << RECAST( this->transition( 0, *varIter ) )->toDot()
                 << std::endl;
    if ( this->reward() )
      fmdpCore << RECAST( this->reward() )->toDot() << std::endl;
    return fmdpCore.str();
  }


  template <typename GUM_SCALAR>
  INLINE Size FMDP<GUM_SCALAR>::size() const {
    Size s = 0;
    for ( auto actionIter = beginActions(); actionIter != endActions();
          ++actionIter ) {
      for ( auto varIter = beginVariables(); varIter != endVariables(); ++varIter )
        if ( this->transition( *actionIter, *varIter ) )
          s += this->transition( *actionIter, *varIter )->realSize();
      if ( this->reward( *actionIter ) )
        s += this->reward( *actionIter )->realSize();
    }

    for ( auto varIter = beginVariables(); varIter != endVariables(); ++varIter )
      if ( this->transition( 0, *varIter ) )
        s += this->transition( 0, *varIter )->realSize();
    if ( this->reward() ) s += this->reward()->realSize();
    return s;
  }
}
