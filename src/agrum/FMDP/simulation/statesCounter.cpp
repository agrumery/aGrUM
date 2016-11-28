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

#include <agrum/FMDP/simulation/statesCounter.h>


namespace gum {


  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::StatesCounter()
      : __counter( MultiDimFunctionGraph<Size>::getTreeInstance() ) {
    GUM_CONSTRUCTOR( StatesCounter )
  }


  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::~StatesCounter() {
    delete __counter;
    GUM_DESTRUCTOR( StatesCounter )
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::reset( const Instantiation& initialState ) {

    __counter->clear();
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              initialState.variablesSequence().beginSafe();
          varIter != initialState.variablesSequence().endSafe();
          ++varIter )
      __counter->add( **varIter );

    __counter->manager()->setRootNode(
        __counter->manager()->addTerminalNode( 0 ) );

    __incState( initialState, 0, 0, 0 );
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::incState( const Instantiation& state ) {

    Idx nbVisits = __counter->get( state );

    NodeId parId = __counter->root();
    Idx    parModa = state.valFromPtr( __counter->node( parId )->nodeVar() );
    while (
        !__counter->isTerminalNode( __counter->node( parId )->son( parModa ) ) ) {
      parId = __counter->node( parId )->son( parModa );
      parModa = state.valFromPtr( __counter->node( parId )->nodeVar() );
    }
    __incState( state, parId, parModa, nbVisits );
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::__incState( const Instantiation& state,
                                  NodeId               parentId,
                                  Idx                  parentModa,
                                  Size                 nbVisits ) {

    Idx varIter = 0;
    if ( parentId )
      varIter =
          state.variablesSequence().pos( __counter->node( parentId )->nodeVar() ) +
          1;


    for ( ; varIter < state.variablesSequence().size(); ++varIter ) {

      const DiscreteVariable* curVar = state.variablesSequence().atPos( varIter );
      NodeId varId = __counter->manager()->addInternalNode( curVar );
      if ( parentId )
        __counter->manager()->setSon( parentId, parentModa, varId );
      else
        __counter->manager()->setRootNode( varId );
      for ( Idx moda = 0; moda < curVar->domainSize(); ++moda ) {
        if ( moda == state.valFromPtr( curVar ) )
          parentModa = moda;
        else
          __counter->manager()->setSon(
              varId, moda, __counter->terminalNodeId( nbVisits ) );
      }
      parentId = varId;
    }
    __counter->manager()->setSon(
        parentId,
        parentModa,
        __counter->manager()->addTerminalNode( nbVisits + 1 ) );
  }
}  // End of namespace gum
