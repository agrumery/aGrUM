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
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <cstdio>
#include <iostream>
// ============================================================================

namespace gum {
	  

  /* ************************************************************************************************** **/
  /* **													**/
  /* **				Constructors / Destructors						**/
  /* **													**/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Default constructor.
    // ===========================================================================
    template<typename T_DATA> INLINE
    FactoredMarkovDecisionProcess<T_DATA>::FactoredMarkovDecisionProcess() : __varIter( __varSeq ) {
      GUM_CONSTRUCTOR ( FactoredMarkovDecisionProcess );
      __defaultCostTable = NULL;
      __defaultRewardTable = NULL;
      __discount = 0;
      __nextActionId = 1;
      resetActionsIterator();
      resetVariablesIterator();
    }

    // ===========================================================================
    // Destructor.
    // ===========================================================================
    template<typename T_DATA>
    FactoredMarkovDecisionProcess<T_DATA>::~FactoredMarkovDecisionProcess() {
    
      GUM_DESTRUCTOR ( FactoredMarkovDecisionProcess );
      
      /// Table which give for each action a table containing variables transition cpt
      for( HashTableIterator< Idx, HashTable< const DiscreteVariable*, const MultiDimImplementation< T_DATA >* >* > iterA = __actionTransitionTable.begin();
		      iterA != __actionTransitionTable.end(); ++iterA ){
	for( HashTableIterator<  const DiscreteVariable*, const MultiDimImplementation< T_DATA >* > iterH = (*iterA)->begin();
			iterH != (*iterA)->end(); ++iterH )
	  delete *iterH;
	delete *iterA;
      }

      if( __defaultCostTable )
	delete __defaultCostTable;

      for( HashTableIterator< Idx, const MultiDimImplementation< T_DATA >* > iterA = __actionCostTable.begin(); iterA != __actionCostTable.end(); ++iterA )
	delete *iterA;

      for( BijectionIterator< Idx, const std::string* > iterId = __actionMap.begin(); iterId != __actionMap.end(); ++iterId )
	delete iterId.second();

      if( __defaultRewardTable )
	delete __defaultRewardTable;		
      
      for( HashTableIterator< const DiscreteVariable*, const MultiDimImplementation< T_DATA >* > iter = __defaultTransitionTable.begin();
		      iter != __defaultTransitionTable.end(); ++iter ){
	if( *iter != NULL )
	  delete *iter;
	delete iter.key();
      }
      
      for( SetIterator< const DiscreteVariable* > primedIter = __primedVariablesSet.begin(); primedIter != __primedVariablesSet.end(); ++primedIter )
	delete *primedIter;
    }


  /* ************************************************************************************************** **/
  /* **													**/
  /* **				FMDP instantiation methods.						**/
  /* **													**/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Adds a variable to FMDP description
    // @throw DuplicateElement if a similar variable already exists
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addVariable( const DiscreteVariable* var ){
	    
      if( __defaultTransitionTable.exists( var ) )
	GUM_ERROR( DuplicateElement, " Variable " << var->name() << " has already been inserted in FMDP." );
	      
      __defaultTransitionTable.insert( var, NULL );
      
      
      __varSeq.insert( var );
	    
    }

    // ===========================================================================
    // Adds a variable to FMDP description
    // @throw DuplicateElement if a similar variable already exists
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addPrimedVariable( const DiscreteVariable* var, const DiscreteVariable* mainVar ){
	    
      if( __primedVariablesSet.exists( var ) )
	GUM_ERROR( DuplicateElement, " Variable " << var->name() << " has already been inserted in FMDP." );

      __primedVariablesSet.insert( var );
      __main2primed.insert( mainVar, var );
	    
    }

    // ===========================================================================
    // Adds an action to FMDP description
    // @throw DuplicateElement if an action with same name already exists
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addAction( const std::string& action ){
    
      for( BijectionIterator< Idx, const std::string* > actIter = __actionMap.begin(); actIter != __actionMap.end(); ++actIter )
	if( *(actIter.second()) == action ){
	  GUM_ERROR( DuplicateElement, " Action " << action << " has already been inserted in FMDP." );
	}
      
      Idx newActionId = __nextActionId;
      ++__nextActionId;
      __actionMap.insert( newActionId, new std::string( action ) );
      
      __actionTransitionTable.insert( newActionId, new HashTable< const DiscreteVariable*, const MultiDimImplementation< T_DATA >* >() );
      __actionCostTable.insert( newActionId, NULL );
    } 

    // ===========================================================================
    // Adds a variable transition table to specified action
    // @throw NotFound if action or var does not exists
    // @throw DuplicateElement if variable already has a transition for this action
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addTransitionForAction( const DiscreteVariable* var, const MultiDimImplementation<T_DATA>* transition, const std::string& action ){

      Idx actionId = __actionId( action );

      if( !__defaultTransitionTable.exists( var ) )
	GUM_ERROR( NotFound, " Variable " << var->name() << " has not been declared before." );

      if( __actionTransitionTable[actionId]->exists( var ) )
	GUM_ERROR( DuplicateElement, " Variable " << var->name() << " already has a transition table in " << actionId << " table." );

      __actionTransitionTable[actionId]->insert( var, transition );
    }

    // ===========================================================================
    // Adds a default variable transition
    // @throw NotFound if var does not exists
    // @throw DuplicateElement if variable already has a default transition
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addTransition( const DiscreteVariable* var, const MultiDimImplementation<T_DATA>* transition ){

      if( !__defaultTransitionTable.exists( var ) )
	GUM_ERROR( NotFound, " Variable " << var->name() << " has not been declared before." );

      if( __defaultTransitionTable[var] != NULL )
	GUM_ERROR( DuplicateElement, " Variable " << var->name() << " already has a default transition table" );

      __defaultTransitionTable.insert( var, transition );
    }

    // ===========================================================================
    // Adds a cost table to specified action
    // @throw NotFound if action does not exists
    // @throw DuplicateElement if action already has a cost
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addCostForAction( const MultiDimImplementation<T_DATA>* cost, const std::string& action ){

      Idx actionId = __actionId( action );

      if( __actionCostTable[ actionId ] != NULL )
	GUM_ERROR( DuplicateElement, " Action " << action << " already has a cost table" );

      __actionCostTable.insert( actionId, cost );
    }

    // ===========================================================================
    // Adds a default variable cost
    // @throw DuplicateElement if a default cost exists already
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addCost( const MultiDimImplementation<T_DATA>* cost ){

      if( __defaultCostTable != NULL )
	GUM_ERROR( DuplicateElement, " Default cost table already specified " );

      __defaultCostTable = cost;
    }

    // ===========================================================================
    // Adds a default variable reward
    // @throw DuplicateElement if a default reward exists already
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addReward( const MultiDimImplementation<T_DATA>* reward ){

      if( __defaultRewardTable != NULL )
	GUM_ERROR( DuplicateElement, " Default reward table already specified " );

      __defaultRewardTable = reward;
    }

    // ===========================================================================
    // Precises the discount factor for that mdp
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::addDiscount( T_DATA discount ){
      __discount = discount;
    }


  /* ************************************************************************************************** **/
  /* **													**/
  /* **				FMPD manipulation methods.						**/
  /* **													**/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Returns the discount factor for that mdp
    // ===========================================================================
    template<typename T_DATA> INLINE
    const T_DATA
    FactoredMarkovDecisionProcess<T_DATA>::discount(  ) const{
      return __discount;
    }

    // ===========================================================================
    // Returns the reward table of mdp
    // ===========================================================================
    template<typename T_DATA> INLINE
    const MultiDimImplementation< T_DATA >*
    FactoredMarkovDecisionProcess<T_DATA>::reward(  ) const{
      return __defaultRewardTable;
    }



    // ===========================================================================
    // Resets the action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::resetActionsIterator(){
      this->__actionIter = this->__actionTransitionTable.begin();
    }

    // ===========================================================================
    // Resets the action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    bool
    FactoredMarkovDecisionProcess<T_DATA>::hasAction() const{
      return __actionIter != __actionTransitionTable.end();
    }

    // ===========================================================================
    // Resets the action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::nextAction(){
      ++__actionIter;
    }



    // ===========================================================================
    // Returns the id of current action pointed by actionIterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    Idx
    FactoredMarkovDecisionProcess<T_DATA>::actionIterId() const{
      return __actionIter.key();
    }

    // ===========================================================================
    // Returns name of action given in parameter
    // ===========================================================================
    template<typename T_DATA> INLINE
    const std::string
    FactoredMarkovDecisionProcess<T_DATA>::actionName( Idx actionId ) const{
    
      if( !__actionMap.existsFirst( actionId ) )
	GUM_ERROR( NotFound, "No action with "<< actionId << " as identifiant." );

      const std::string* ret = __actionMap.second( actionId );
      return *ret;
    }



    // ===========================================================================
    // Resets the variable iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::resetVariablesIterator(){
//       __varIter = __defaultTransitionTable.begin();
	__varIter = __varSeq.begin();
    }

    // ===========================================================================
    // Resets the action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    bool
    FactoredMarkovDecisionProcess<T_DATA>::hasVariable() const{
//       return __varIter != __defaultTransitionTable.end();
      return __varIter != __varSeq.end();
    }

    // ===========================================================================
    // Returns current primed variable pointed by variable iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    const DiscreteVariable*
    FactoredMarkovDecisionProcess<T_DATA>::variable() const {
//       return __main2primed.second( __varIter.key() );
      return __main2primed.second( *__varIter );
    }

    // ===========================================================================
    // Resets the action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    void
    FactoredMarkovDecisionProcess<T_DATA>::nextVariable(){
      ++__varIter;
    }



    // ===========================================================================
    // Returns transition associated to current variable pointed by variable iterator 
    // and current action poinbted by action iterator
    // ===========================================================================
    template<typename T_DATA> INLINE
    const MultiDimImplementation< T_DATA >*
    FactoredMarkovDecisionProcess<T_DATA>::transition() const{

//       if( (*__actionIter)->exists( __varIter.key() ) )
      if( (*__actionIter)->exists( *__varIter ) )
// 	return (*(*__actionIter))[ __varIter.key() ];
	  return (*(*__actionIter))[ *__varIter ];
      else
// 	return *__varIter;
	  return __defaultTransitionTable[ *__varIter ];
    }

    // ===========================================================================
    // Returns set of primed variable (variable at next instant )
    // ===========================================================================
    template<typename T_DATA> INLINE
    const Set< const DiscreteVariable* >&
    FactoredMarkovDecisionProcess<T_DATA>::primedVariables() const{
      return __primedVariablesSet;
    }

    // ===========================================================================
    // Returns the map on main variable and their primed version
    // ===========================================================================
    template<typename T_DATA> INLINE
    const Bijection< const DiscreteVariable*, const DiscreteVariable*>&
    FactoredMarkovDecisionProcess<T_DATA>::main2prime() const{
      return __main2primed;
    }


    template<typename T_DATA> INLINE
    std::string
    FactoredMarkovDecisionProcess<T_DATA>::show( ) const{
      
      std::stringstream fmdpCore;
      for( HashTableConstIterator< Idx, HashTable< const DiscreteVariable*, const MultiDimImplementation< T_DATA >* >* > actionIter = __actionTransitionTable.begin(); actionIter != __actionTransitionTable.end(); ++actionIter ) {
	for( HashTableConstIterator< const DiscreteVariable*, const MultiDimImplementation< T_DATA >* > tableIter = (*actionIter)->begin(); tableIter != (*actionIter)->end(); ++tableIter ){
	  std::stringstream graphName;
	  graphName << " ACTION : " << actionName( actionIter.key() ) << " VARIABLE : " << tableIter.key()->name();
	  fmdpCore << std::endl << reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( *tableIter )->toDot(graphName.str());
	}
      }

      fmdpCore << std::endl << reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __defaultRewardTable )->toDot("REWARD");
      return fmdpCore.str();
    }



  /* ************************************************************************************************** **/
  /* **													**/
  /* **					Protected methods.						**/
  /* **													**/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Returns action id
    // ===========================================================================
    template<typename T_DATA> INLINE
    const Idx&
    FactoredMarkovDecisionProcess<T_DATA>::__actionId( const std::string& action) const{

      for( BijectionIterator< Idx, const std::string* > actIter = __actionMap.begin(); actIter != __actionMap.end(); ++actIter )
	if( *(actIter.second()) == action ){
	  return actIter.first();
	}

      GUM_ERROR( NotFound, " Action " << action << " has not been declared before." );
    }
  
}
