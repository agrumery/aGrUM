///***************************************************************************
// *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
// *   {prenom.nom}_at_lip6.fr                                               *
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// *   This program is distributed in the hope that it will be useful,       *
// *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
// *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
// *   GNU General Public License for more details.                          *
// *                                                                         *
// *   You should have received a copy of the GNU General Public License     *
// *   along with this program; if not, write to the                         *
// *   Free Software Foundation, Inc.,                                       *
// *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
// ***************************************************************************/
///**
// * @file
// * @brief Sources of
// *
// * @author Jean-Christophe Magnan
// *
// */
//// =====================================================================
//#include <random>
//#include <cstdlib>
////======================================================================
//#include <agrum/FMDP/simulation/factorySimulator.h>
////======================================================================

// namespace gum {

//  FactorySimulator::FactorySimulator():AbstractSimulator(){
//    GUM_CONSTRUCTOR(FactorySimulator)

//    //
//    *****************************************************************************************
//    // Défintion des variables du problème

//    // Position FactorySimulator
//    __xPos = new LabelizedVariable("xPos", "Position horizontale du
//    FactorySimulator");
//    __yPos = new LabelizedVariable("yPos", "Position verticale du
//    FactorySimulator");
//    __xPos->eraseLabels();
//    __yPos->eraseLabels();
//    for( Idx pos = 0; pos < 5; pos++){
//      std::stringstream ss;
//      ss << pos;
//      __xPos->addLabel(ss.str());
//      __yPos->addLabel(ss.str());
//    }

//    // Position et destination passager
//    __passengerPos = new LabelizedVariable("PassengerPos", "Position du
//    Passager", 5);
//    __passengerDest = new LabelizedVariable("PassengerDest", "Destination du
//    Passager", 4);
//    __passengerPos->changeLabel(HOME, "Home");
//    __passengerDest->changeLabel(HOME, "Home");
//    __passengerPos->changeLabel(WORK, "Work");
//    __passengerDest->changeLabel(WORK, "Work");
//    __passengerPos->changeLabel(THEATER, "Theater");
//    __passengerDest->changeLabel(THEATER, "Theater");
//    __passengerPos->changeLabel(CLUB, "Club");
//    __passengerDest->changeLabel(CLUB, "Club");
//    __passengerPos->changeLabel(Factory, "Factory");

////    __fuelLevel = new LabelizedVariable("FuelLevel", "Niveau du réservoir",
/// 14);

//    // Ajout à séquence
//    __FactoryVars.insert(__xPos);
//    __FactoryVars.insert(__yPos);
//    __FactoryVars.insert(__passengerPos);
//    __FactoryVars.insert(__passengerDest);
////    __FactoryVars.insert(__fuelLevel);

//    // Prime version creation
//    for( SequenceIteratorSafe<const DiscreteVariable*> varIter =
//    this->beginVariables(); varIter != this->endVariables(); ++varIter){
//      DiscreteVariable* primeVar = (*varIter)->clone();
//      primeVar->setName((*varIter)->name() + "'");
//      __primeMap.insert((*varIter), primeVar);
//    }

//    //
//    *****************************************************************************************

//    //
//    *****************************************************************************************
//    // Défintion des actions du problème
//    __FactoryActions.insert(GoNorth);
//    __actionMap.insert(GoNorth, new std::string ("Go North"));
//    __FactoryActions.insert(GoEast);
//    __actionMap.insert(GoEast, new std::string ("Go East"));
//    __FactoryActions.insert(GoSouth);
//    __actionMap.insert(GoSouth, new std::string ("Go South"));
//    __FactoryActions.insert(GoWest);
//    __actionMap.insert(GoWest, new std::string ("Go West"));
//    __FactoryActions.insert(PickUp);
//    __actionMap.insert(PickUp, new std::string ("Pick Up"));
//    __FactoryActions.insert(PutDown);
//    __actionMap.insert(PutDown, new std::string ("Put Down"));
////    __FactoryActions.insert(FillUp);
////    __actionMap.insert(FillUp, new std::string ("FillUp"));
//  }

//  FactorySimulator::~FactorySimulator(){
//    GUM_DESTRUCTOR(FactorySimulator)

//    for( BijectionIteratorSafe<const DiscreteVariable*, const
//    DiscreteVariable*> varIter = __primeMap.beginSafe();
//        varIter != __primeMap.endSafe(); ++varIter ){
//      delete varIter.first();
//      delete varIter.second();
//    }

//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  Instantiation  FactorySimulator::_randomState(){
//    Instantiation randy = AbstractSimulator::_randomState();
////    Idx curFuelLevel = randy.valFromPtr(__fuelLevel);
////    while(curFuelLevel > 12 || curFuelLevel < 5)
////      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) * 7.0)
///+ 5;
////    randy.chgVal(__fuelLevel, curFuelLevel);

//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    randy.valFromPtr(__passengerPos);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    randy.valFromPtr(__passengerDest);
//    while( passPos == passDest || passPos == Factory )
//      passPos = (FactorySimulationLandmark) (((double)std::rand( ) /
//      (double)RAND_MAX) * 3.0);
//    randy.chgVal(__passengerPos, passPos);

//    return randy;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  bool FactorySimulator::hasReachEnd(){
//    if( _currentState.valFromPtr(__passengerPos) ==
//    _currentState.valFromPtr(__passengerDest) )
//      return true;

////    if( _currentState.valFromPtr(__fuelLevel) == 0 )
////      return true;

//    return false;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  double FactorySimulator::reward(){
//    if( _currentState.valFromPtr(__passengerPos) ==
//    _currentState.valFromPtr(__passengerDest) ) {
//      return 20.0;
//    }

//    if( __lastAction == PickUp ){
//      if( _currentState.valFromPtr(__passengerPos) != Factory )
//        return -10.0;
//      else
//        return 10.0;
//    }

//    if( __lastAction == PutDown )
//      return -10.0;

////    if( _currentState.valFromPtr(__fuelLevel) == 0 )
////      return -10.0;

//    return 0.0;//-1.0;
//  }

//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  void FactorySimulator::perform(Idx actionId){

//    __lastAction = (FactorySimulationAction) actionId;

////    Idx curFuelLevel = _currentState.valFromPtr(__fuelLevel);
////    if( curFuelLevel > 0 )
////      _currentState.chgVal(__fuelLevel, --curFuelLevel);

//    switch(actionId){
//      case GoNorth : return __goNorth( );
//      case GoEast : return __goEast( );
//      case GoSouth : return __goSouth( );
//      case GoWest : return __goWest( );
//      case PickUp : return __pickUp( );
//      case PutDown : return __putDown( );
//      case FillUp : return __fillUp( );
//    }
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go North
//  //
//  ==================================================================================================================
//  void FactorySimulator::__goNorth(){
//    Idx curPos = this->_currentState.valFromPtr(__yPos);
//    if( curPos < 4 )
//      _currentState.chgVal(__yPos, ++curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go east
//  //
//  ==================================================================================================================
//  void FactorySimulator::__goEast(){
//    Idx xCurPos = this->_currentState.valFromPtr(__xPos);
//    Idx yCurPos = this->_currentState.valFromPtr(__yPos);

//    if(xCurPos == 4)
//      return;
//    if(xCurPos == 0 || xCurPos == 2)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 1)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    _currentState.chgVal(__xPos, ++xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go south
//  //
//  ==================================================================================================================
//  void FactorySimulator::__goSouth(){
//    Idx curPos = this->_currentState.valFromPtr(__yPos);
//    if( curPos > 0 )
//      _currentState.chgVal(__yPos, --curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go west
//  //
//  ==================================================================================================================
//  void FactorySimulator::__goWest(){
//    Idx xCurPos = this->_currentState.valFromPtr(__xPos);
//    Idx yCurPos = this->_currentState.valFromPtr(__yPos);

//    if(xCurPos == 0)
//      return;
//    if(xCurPos == 1 || xCurPos == 3)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 2)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    _currentState.chgVal(__xPos, --xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go pick up sb
//  //
//  ==================================================================================================================
//  void FactorySimulator::__pickUp(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->_currentState.valFromPtr(__xPos);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->_currentState.valFromPtr(__yPos);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->_currentState.valFromPtr(__passengerPos);
//    switch(passPos){
//      case HOME :  {
//                  if( xCurPos == HOMEX && yCurPos == HOMEY )
//                    _currentState.chgVal(__passengerPos, Factory);
//                  return;
//                }
//      case WORK :  {
//                  if( xCurPos == WORKX && yCurPos == WORKY )
//                    _currentState.chgVal(__passengerPos, Factory);
//                  return;
//                }
//      case THEATER :  {
//                  if( xCurPos == THEATERX && yCurPos == THEATERY )
//                    _currentState.chgVal(__passengerPos, Factory);
//                  return;
//                }
//      case CLUB :  {
//                  if( xCurPos == CLUBX && yCurPos == CLUBY )
//                    _currentState.chgVal(__passengerPos, Factory);
//                  return;
//                }
//      case Factory :  return;
//    }
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go put down sb
//  //
//  ==================================================================================================================
//  void FactorySimulator::__putDown(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->_currentState.valFromPtr(__xPos);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->_currentState.valFromPtr(__yPos);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->_currentState.valFromPtr(__passengerPos);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    this->_currentState.valFromPtr(__passengerDest);
//    if(passPos == Factory ){
//        switch(passDest){
//          case HOME :  {
//                      if( xCurPos == HOMEX && yCurPos == HOMEY )
//                        _currentState.chgVal(__passengerPos, HOME);
//                      return;
//                    }
//          case WORK :  {
//                      if( xCurPos == WORKX && yCurPos == WORKY )
//                        _currentState.chgVal(__passengerPos, WORK);
//                      return;
//                    }
//          case THEATER :  {
//                      if( xCurPos == THEATERX && yCurPos == THEATERY )
//                        _currentState.chgVal(__passengerPos, THEATER);
//                      return;
//                    }
//          case CLUB :  {
//                      if( xCurPos == CLUBX && yCurPos == CLUBY )
//                        _currentState.chgVal(__passengerPos, CLUB);
//                      return;
//                    }
//          case Factory :  return;
//        }

//    }
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go reffill
//  //
//  ==================================================================================================================
//  void FactorySimulator::__fillUp(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->_currentState.valFromPtr(__xPos);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->_currentState.valFromPtr(__yPos);

//    if( xCurPos == STATIONX && yCurPos == STATIONY )
//      _currentState.chgVal(__fuelLevel, 13);
//  }
//} // End of namespace gum
