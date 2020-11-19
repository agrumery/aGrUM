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


///**
// * @file
// * @brief Sources of
// *
// * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
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
//    xPos__ = new LabelizedVariable("xPos", "Position horizontale du
//    FactorySimulator");
//    yPos__ = new LabelizedVariable("yPos", "Position verticale du
//    FactorySimulator");
//    xPos__->eraseLabels();
//    yPos__->eraseLabels();
//    for( Idx pos = 0; pos < 5; pos++){
//      std::stringstream ss;
//      ss << pos;
//      xPos__->addLabel(ss.str());
//      yPos__->addLabel(ss.str());
//    }

//    // Position et destination passager
//    passengerPos__ = new LabelizedVariable("PassengerPos", "Position du
//    Passager", 5);
//    passengerDest__ = new LabelizedVariable("PassengerDest", "Destination du
//    Passager", 4);
//    passengerPos__->changeLabel(HOME, "Home");
//    passengerDest__->changeLabel(HOME, "Home");
//    passengerPos__->changeLabel(WORK, "Work");
//    passengerDest__->changeLabel(WORK, "Work");
//    passengerPos__->changeLabel(THEATER, "Theater");
//    passengerDest__->changeLabel(THEATER, "Theater");
//    passengerPos__->changeLabel(CLUB, "Club");
//    passengerDest__->changeLabel(CLUB, "Club");
//    passengerPos__->changeLabel(Factory, "Factory");

////    fuelLevel__ = new LabelizedVariable("FuelLevel", "Niveau du réservoir",
/// 14);

//    // Ajout à séquence
//    FactoryVars__.insert(xPos__);
//    FactoryVars__.insert(yPos__);
//    FactoryVars__.insert(passengerPos__);
//    FactoryVars__.insert(passengerDest__);
////    FactoryVars__.insert(fuelLevel__);

//    // Prime version creation
//    for( SequenceIteratorSafe<const DiscreteVariable*> varIter =
//    this->beginVariables(); varIter != this->endVariables(); ++varIter){
//      DiscreteVariable* primeVar = (*varIter)->clone();
//      primeVar->setName((*varIter)->name() + "'");
//      primeMap__.insert((*varIter), primeVar);
//    }

//    //
//    *****************************************************************************************

//    //
//    *****************************************************************************************
//    // Défintion des actions du problème
//    FactoryActions__.insert(GoNorth);
//    actionMap__.insert(GoNorth, new std::string ("Go North"));
//    FactoryActions__.insert(GoEast);
//    actionMap__.insert(GoEast, new std::string ("Go East"));
//    FactoryActions__.insert(GoSouth);
//    actionMap__.insert(GoSouth, new std::string ("Go South"));
//    FactoryActions__.insert(GoWest);
//    actionMap__.insert(GoWest, new std::string ("Go West"));
//    FactoryActions__.insert(PickUp);
//    actionMap__.insert(PickUp, new std::string ("Pick Up"));
//    FactoryActions__.insert(PutDown);
//    actionMap__.insert(PutDown, new std::string ("Put Down"));
////    FactoryActions__.insert(FillUp);
////    actionMap__.insert(FillUp, new std::string ("FillUp"));
//  }

//  FactorySimulator::~FactorySimulator(){
//    GUM_DESTRUCTOR(FactorySimulator)

//    for( BijectionIteratorSafe<const DiscreteVariable*, const
//    DiscreteVariable*> varIter = primeMap__.beginSafe();
//        varIter != primeMap__.endSafe(); ++varIter ){
//      delete varIter.first();
//      delete varIter.second();
//    }

//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  Instantiation  FactorySimulator::randomState_(){
//    Instantiation randy = AbstractSimulator::randomState_();
////    Idx curFuelLevel = randy.valFromPtr(fuelLevel__);
////    while(curFuelLevel > 12 || curFuelLevel < 5)
////      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) * 7.0)
///+ 5;
////    randy.chgVal(fuelLevel__, curFuelLevel);

//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    randy.valFromPtr(passengerPos__);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    randy.valFromPtr(passengerDest__);
//    while( passPos == passDest || passPos == Factory )
//      passPos = (FactorySimulationLandmark) (((double)std::rand( ) /
//      (double)RAND_MAX) * 3.0);
//    randy.chgVal(passengerPos__, passPos);

//    return randy;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  bool FactorySimulator::hasReachEnd(){
//    if( currentState_.valFromPtr(passengerPos__) ==
//    currentState_.valFromPtr(passengerDest__) )
//      return true;

////    if( currentState_.valFromPtr(fuelLevel__) == 0 )
////      return true;

//    return false;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  double FactorySimulator::reward(){
//    if( currentState_.valFromPtr(passengerPos__) ==
//    currentState_.valFromPtr(passengerDest__) ) {
//      return 20.0;
//    }

//    if( lastAction__ == PickUp ){
//      if( currentState_.valFromPtr(passengerPos__) != Factory )
//        return -10.0;
//      else
//        return 10.0;
//    }

//    if( lastAction__ == PutDown )
//      return -10.0;

////    if( currentState_.valFromPtr(fuelLevel__) == 0 )
////      return -10.0;

//    return 0.0;//-1.0;
//  }

//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  void FactorySimulator::perform(Idx actionId){

//    lastAction__ = (FactorySimulationAction) actionId;

////    Idx curFuelLevel = currentState_.valFromPtr(fuelLevel__);
////    if( curFuelLevel > 0 )
////      currentState_.chgVal(fuelLevel__, --curFuelLevel);

//    switch(actionId){
//      case GoNorth : return goNorth__( );
//      case GoEast : return goEast__( );
//      case GoSouth : return goSouth__( );
//      case GoWest : return goWest__( );
//      case PickUp : return pickUp__( );
//      case PutDown : return putDown__( );
//      case FillUp : return fillUp__( );
//    }
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go North
//  //
//  ==================================================================================================================
//  void FactorySimulator::goNorth__(){
//    Idx curPos = this->currentState_.valFromPtr(yPos__);
//    if( curPos < 4 )
//      currentState_.chgVal(yPos__, ++curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go east
//  //
//  ==================================================================================================================
//  void FactorySimulator::goEast__(){
//    Idx xCurPos = this->currentState_.valFromPtr(xPos__);
//    Idx yCurPos = this->currentState_.valFromPtr(yPos__);

//    if(xCurPos == 4)
//      return;
//    if(xCurPos == 0 || xCurPos == 2)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 1)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    currentState_.chgVal(xPos__, ++xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go south
//  //
//  ==================================================================================================================
//  void FactorySimulator::goSouth__(){
//    Idx curPos = this->currentState_.valFromPtr(yPos__);
//    if( curPos > 0 )
//      currentState_.chgVal(yPos__, --curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go west
//  //
//  ==================================================================================================================
//  void FactorySimulator::goWest__(){
//    Idx xCurPos = this->currentState_.valFromPtr(xPos__);
//    Idx yCurPos = this->currentState_.valFromPtr(yPos__);

//    if(xCurPos == 0)
//      return;
//    if(xCurPos == 1 || xCurPos == 3)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 2)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    currentState_.chgVal(xPos__, --xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go pick up sb
//  //
//  ==================================================================================================================
//  void FactorySimulator::pickUp__(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr(xPos__);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr(yPos__);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr(passengerPos__);
//    switch(passPos){
//      case HOME :  {
//                  if( xCurPos == HOMEX && yCurPos == HOMEY )
//                    currentState_.chgVal(passengerPos__, Factory);
//                  return;
//                }
//      case WORK :  {
//                  if( xCurPos == WORKX && yCurPos == WORKY )
//                    currentState_.chgVal(passengerPos__, Factory);
//                  return;
//                }
//      case THEATER :  {
//                  if( xCurPos == THEATERX && yCurPos == THEATERY )
//                    currentState_.chgVal(passengerPos__, Factory);
//                  return;
//                }
//      case CLUB :  {
//                  if( xCurPos == CLUBX && yCurPos == CLUBY )
//                    currentState_.chgVal(passengerPos__, Factory);
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
//  void FactorySimulator::putDown__(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr(xPos__);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr(yPos__);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr(passengerPos__);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr(passengerDest__);
//    if(passPos == Factory ){
//        switch(passDest){
//          case HOME :  {
//                      if( xCurPos == HOMEX && yCurPos == HOMEY )
//                        currentState_.chgVal(passengerPos__, HOME);
//                      return;
//                    }
//          case WORK :  {
//                      if( xCurPos == WORKX && yCurPos == WORKY )
//                        currentState_.chgVal(passengerPos__, WORK);
//                      return;
//                    }
//          case THEATER :  {
//                      if( xCurPos == THEATERX && yCurPos == THEATERY )
//                        currentState_.chgVal(passengerPos__, THEATER);
//                      return;
//                    }
//          case CLUB :  {
//                      if( xCurPos == CLUBX && yCurPos == CLUBY )
//                        currentState_.chgVal(passengerPos__, CLUB);
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
//  void FactorySimulator::fillUp__(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr(xPos__);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr(yPos__);

//    if( xCurPos == STATIONX && yCurPos == STATIONY )
//      currentState_.chgVal(fuelLevel__, 13);
//  }
//} // End of namespace gum
