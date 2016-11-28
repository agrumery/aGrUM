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
 * @brief Sources of
 *
 * @author Jean-Christophe Magnan
 *
 */
// =====================================================================
#include <cstdlib>
#include <random>
//======================================================================
#include <agrum/FMDP/simulation/taxiSimulator.h>
//======================================================================

namespace gum {

  TaxiSimulator::TaxiSimulator()
      : AbstractSimulator() {
    GUM_CONSTRUCTOR( TaxiSimulator )

    // *****************************************************************************************
    // Défintion des variables du problème

    // Position TaxiSimulator
    __xPos =
        new LabelizedVariable( "xPos", "Position horizontale du TaxiSimulator" );
    __yPos =
        new LabelizedVariable( "yPos", "Position verticale du TaxiSimulator" );
    __xPos->eraseLabels();
    __yPos->eraseLabels();
    for ( Idx pos = 0; pos < 5; pos++ ) {
      std::stringstream ss;
      ss << pos;
      __xPos->addLabel( ss.str() );
      __yPos->addLabel( ss.str() );
    }

    // Position et destination passager
    __passengerPos =
        new LabelizedVariable( "PassengerPos", "Position du Passager", 5 );
    __passengerDest =
        new LabelizedVariable( "PassengerDest", "Destination du Passager", 4 );
    __passengerPos->changeLabel( HOME, "Home" );
    __passengerDest->changeLabel( HOME, "Home" );
    __passengerPos->changeLabel( WORK, "Work" );
    __passengerDest->changeLabel( WORK, "Work" );
    __passengerPos->changeLabel( THEATER, "Theater" );
    __passengerDest->changeLabel( THEATER, "Theater" );
    __passengerPos->changeLabel( CLUB, "Club" );
    __passengerDest->changeLabel( CLUB, "Club" );
    __passengerPos->changeLabel( TAXI, "Taxi" );

    __fuelLevel = new LabelizedVariable( "FuelLevel", "Niveau du réservoir", 14 );

    // Ajout à séquence
    __taxiVars.insert( __xPos );
    __taxiVars.insert( __yPos );
    __taxiVars.insert( __passengerPos );
    __taxiVars.insert( __passengerDest );
    __taxiVars.insert( __fuelLevel );

    // Prime version creation
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              this->beginVariables();
          varIter != this->endVariables();
          ++varIter ) {
      DiscreteVariable* primeVar = ( *varIter )->clone();
      primeVar->setName( ( *varIter )->name() + "'" );
      __primeMap.insert( ( *varIter ), primeVar );
    }

    // *****************************************************************************************

    // *****************************************************************************************
    // Défintion des actions du problème
    __taxiActions.insert( GoNorth );
    __actionMap.insert( GoNorth, new std::string( "Go North" ) );
    __taxiActions.insert( GoEast );
    __actionMap.insert( GoEast, new std::string( "Go East" ) );
    __taxiActions.insert( GoSouth );
    __actionMap.insert( GoSouth, new std::string( "Go South" ) );
    __taxiActions.insert( GoWest );
    __actionMap.insert( GoWest, new std::string( "Go West" ) );
    __taxiActions.insert( PickUp );
    __actionMap.insert( PickUp, new std::string( "Pick Up" ) );
    __taxiActions.insert( PutDown );
    __actionMap.insert( PutDown, new std::string( "Put Down" ) );
    __taxiActions.insert( FillUp );
    __actionMap.insert( FillUp, new std::string( "FillUp" ) );
  }

  TaxiSimulator::~TaxiSimulator() {
    GUM_DESTRUCTOR( TaxiSimulator )

    for ( BijectionIteratorSafe<const DiscreteVariable*, const DiscreteVariable*>
              varIter = __primeMap.beginSafe();
          varIter != __primeMap.endSafe();
          ++varIter ) {
      delete varIter.first();
      delete varIter.second();
    }
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  Instantiation TaxiSimulator::_randomState() {
    Instantiation randy = AbstractSimulator::_randomState();
    //    Idx curFuelLevel = randy.valFromPtr(__fuelLevel);
    //    while(curFuelLevel > 12 || curFuelLevel < 5)
    //      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) *
    //      7.0) + 5;
    //    randy.chgVal(__fuelLevel, curFuelLevel);

    //    TaxiSimulationLandmark passPos = (TaxiSimulationLandmark)
    //    randy.valFromPtr(__passengerPos);
    //    TaxiSimulationLandmark passDest = (TaxiSimulationLandmark)
    //    randy.valFromPtr(__passengerDest);
    //    while( passPos == passDest || passPos == TAXI )
    //      passPos = (TaxiSimulationLandmark) (((double)std::rand( ) /
    //      (double)RAND_MAX) * 3.0);
    //    randy.chgVal(__passengerPos, passPos);

    return randy;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  bool TaxiSimulator::hasReachEnd() {
    //    if( _currentState.valFromPtr(__passengerPos) ==
    //    _currentState.valFromPtr(__passengerDest) )
    //      return true;

    //    if( _currentState.valFromPtr(__fuelLevel) == 0 )
    //      return true;

    return false;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  double TaxiSimulator::reward() { return __reward; }

  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::perform( Idx actionId ) {

    __lastAction = (TaxiSimulationAction)actionId;

    __evalReward();

    Idx curFuelLevel = _currentState.valFromPtr( __fuelLevel );
    if ( curFuelLevel > 0 ) _currentState.chgVal( __fuelLevel, --curFuelLevel );

    switch ( actionId ) {
      case GoNorth:
        return __performGoNorth();
      case GoEast:
        return __performGoEast();
      case GoSouth:
        return __performGoSouth();
      case GoWest:
        return __performGoWest();
      case PickUp:
        return __performPickUp();
      case PutDown:
        return __performPutDown();
      case FillUp:
        return __performFillUp();
    }
  }


  // ==================================================================================================================
  // Transition if you go North
  // ==================================================================================================================
  void TaxiSimulator::__performGoNorth() {
    Idx curPos = this->_currentState.valFromPtr( __yPos );
    if ( curPos < 4 ) _currentState.chgVal( __yPos, ++curPos );
  }


  // ==================================================================================================================
  // Transition if you go east
  // ==================================================================================================================
  void TaxiSimulator::__performGoEast() {
    Idx xCurPos = this->_currentState.valFromPtr( __xPos );
    Idx yCurPos = this->_currentState.valFromPtr( __yPos );

    if ( xCurPos == 4 ) return;
    if ( xCurPos == 0 || xCurPos == 2 )
      if ( yCurPos == 0 || yCurPos == 1 ) return;
    if ( xCurPos == 1 )
      if ( yCurPos == 3 || yCurPos == 4 ) return;

    _currentState.chgVal( __xPos, ++xCurPos );
  }


  // ==================================================================================================================
  // Transition if you go south
  // ==================================================================================================================
  void TaxiSimulator::__performGoSouth() {
    Idx curPos = this->_currentState.valFromPtr( __yPos );
    if ( curPos > 0 ) _currentState.chgVal( __yPos, --curPos );
  }


  // ==================================================================================================================
  // Transition if you go west
  // ==================================================================================================================
  void TaxiSimulator::__performGoWest() {
    Idx xCurPos = this->_currentState.valFromPtr( __xPos );
    Idx yCurPos = this->_currentState.valFromPtr( __yPos );

    if ( xCurPos == 0 ) return;
    if ( xCurPos == 1 || xCurPos == 3 )
      if ( yCurPos == 0 || yCurPos == 1 ) return;
    if ( xCurPos == 2 )
      if ( yCurPos == 3 || yCurPos == 4 ) return;

    _currentState.chgVal( __xPos, --xCurPos );
  }


  // ==================================================================================================================
  // Transition if you go pick up sb
  // ==================================================================================================================
  void TaxiSimulator::__performPickUp() {
    TaxiSimulationLandmarkX xCurPos =
        ( TaxiSimulationLandmarkX ) this->_currentState.valFromPtr( __xPos );
    TaxiSimulationLandmarkY yCurPos =
        ( TaxiSimulationLandmarkY ) this->_currentState.valFromPtr( __yPos );
    TaxiSimulationLandmark passPos =
        ( TaxiSimulationLandmark ) this->_currentState.valFromPtr(
            __passengerPos );
    switch ( passPos ) {
      case HOME: {
        if ( xCurPos == HOMEX && yCurPos == HOMEY )
          _currentState.chgVal( __passengerPos, TAXI );
        return;
      }
      case WORK: {
        if ( xCurPos == WORKX && yCurPos == WORKY )
          _currentState.chgVal( __passengerPos, TAXI );
        return;
      }
      case THEATER: {
        if ( xCurPos == THEATERX && yCurPos == THEATERY )
          _currentState.chgVal( __passengerPos, TAXI );
        return;
      }
      case CLUB: {
        if ( xCurPos == CLUBX && yCurPos == CLUBY )
          _currentState.chgVal( __passengerPos, TAXI );
        return;
      }
      case TAXI:
        return;
    }
  }


  // ==================================================================================================================
  // Transition if you go put down sb
  // ==================================================================================================================
  void TaxiSimulator::__performPutDown() {
    TaxiSimulationLandmarkX xCurPos =
        ( TaxiSimulationLandmarkX ) this->_currentState.valFromPtr( __xPos );
    TaxiSimulationLandmarkY yCurPos =
        ( TaxiSimulationLandmarkY ) this->_currentState.valFromPtr( __yPos );
    TaxiSimulationLandmark passPos =
        ( TaxiSimulationLandmark ) this->_currentState.valFromPtr(
            __passengerPos );
    TaxiSimulationLandmark passDest =
        ( TaxiSimulationLandmark ) this->_currentState.valFromPtr(
            __passengerDest );
    if ( passPos == TAXI ) {
      switch ( passDest ) {
        case HOME: {
          if ( xCurPos == HOMEX && yCurPos == HOMEY )
            _currentState.chgVal( __passengerPos, HOME );
          return;
        }
        case WORK: {
          if ( xCurPos == WORKX && yCurPos == WORKY )
            _currentState.chgVal( __passengerPos, WORK );
          return;
        }
        case THEATER: {
          if ( xCurPos == THEATERX && yCurPos == THEATERY )
            _currentState.chgVal( __passengerPos, THEATER );
          return;
        }
        case CLUB: {
          if ( xCurPos == CLUBX && yCurPos == CLUBY )
            _currentState.chgVal( __passengerPos, CLUB );
          return;
        }
        case TAXI:
          return;
      }
    }
  }


  // ==================================================================================================================
  // Transition if you go reffill
  // ==================================================================================================================
  void TaxiSimulator::__performFillUp() {
    TaxiSimulationLandmarkX xCurPos =
        ( TaxiSimulationLandmarkX ) this->_currentState.valFromPtr( __xPos );
    TaxiSimulationLandmarkY yCurPos =
        ( TaxiSimulationLandmarkY ) this->_currentState.valFromPtr( __yPos );

    if ( xCurPos == STATIONX && yCurPos == STATIONY )
      _currentState.chgVal( __fuelLevel, 13 );
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::__evalReward() {

    TaxiSimulationLandmarkX xCurPos =
        ( TaxiSimulationLandmarkX ) this->_currentState.valFromPtr( __xPos );
    TaxiSimulationLandmarkY yCurPos =
        ( TaxiSimulationLandmarkY ) this->_currentState.valFromPtr( __yPos );
    TaxiSimulationLandmark passPos =
        ( TaxiSimulationLandmark ) this->_currentState.valFromPtr(
            __passengerPos );
    TaxiSimulationLandmark passDest =
        ( TaxiSimulationLandmark ) this->_currentState.valFromPtr(
            __passengerDest );

    if ( __lastAction == PutDown ) {
      if ( passPos == TAXI ) {
        if ( __isAtDestination( passDest, xCurPos, yCurPos ) )
          __reward = 30.0;
        else
          __reward = 0.0;
        return;
      }
      __reward = 0;
      return;
    }

    if ( __lastAction == PickUp ) {
      if ( __isAtMeetPoint( passPos, xCurPos, yCurPos ) )
        __reward = 20.0;
      else
        __reward = 0.0;
      return;
    }

    if ( _currentState.valFromPtr( __fuelLevel ) == 0 && __lastAction != FillUp ) {
      __reward = 0.0;
      return;
    }

    if ( __lastAction == FillUp &&
         ( xCurPos != STATIONX || yCurPos != STATIONY ) ) {
      __reward = 0.0;
      return;
    }

    __reward = 10.0;  //-1.0;
  }

  bool TaxiSimulator::__isAtDestination( TaxiSimulationLandmark  passDest,
                                         TaxiSimulationLandmarkX xCurPos,
                                         TaxiSimulationLandmarkY yCurPos ) {
    switch ( passDest ) {
      case HOME: {
        if ( xCurPos == HOMEX && yCurPos == HOMEY ) return true;
        break;
      }
      case WORK: {
        if ( xCurPos == WORKX && yCurPos == WORKY ) return true;
        break;
      }
      case THEATER: {
        if ( xCurPos == THEATERX && yCurPos == THEATERY ) return true;
        break;
      }
      case CLUB: {
        if ( xCurPos == CLUBX && yCurPos == CLUBY ) return true;
        break;
      }
      case TAXI:
        return false;
    }
    return false;
  }

  bool TaxiSimulator::__isAtMeetPoint( TaxiSimulationLandmark  passPos,
                                       TaxiSimulationLandmarkX xCurPos,
                                       TaxiSimulationLandmarkY yCurPos ) {
    switch ( passPos ) {
      case HOME: {
        if ( xCurPos == HOMEX && yCurPos == HOMEY ) return true;
        break;
      }
      case WORK: {
        if ( xCurPos == WORKX && yCurPos == WORKY ) return true;
        break;
      }
      case THEATER: {
        if ( xCurPos == THEATERX && yCurPos == THEATERY ) return true;
        break;
      }
      case CLUB: {
        if ( xCurPos == CLUBX && yCurPos == CLUBY ) return true;
        break;
      }
      case TAXI:
        return false;
    }
    return false;
  }
}  // End of namespace gum
