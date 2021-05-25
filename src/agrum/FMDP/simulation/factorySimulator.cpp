/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
// * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and
// Christophe GONZALES(@AMU)
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
//    GUM_CONSTRUCTOR(FactorySimulator);

//    //
//    *****************************************************************************************
//    // Défintion des variables du problème

//    // Position FactorySimulator
//     _xPos_ = new LabelizedVariable("xPos", "Position horizontale du
//    FactorySimulator");
//     _yPos_ = new LabelizedVariable("yPos", "Position verticale du
//    FactorySimulator");
//     _xPos_->eraseLabels();
//     _yPos_->eraseLabels();
//    for( Idx pos = 0; pos < 5; pos++){
//      std::stringstream ss;
//      ss << pos;
//       _xPos_->addLabel(ss.str());
//       _yPos_->addLabel(ss.str());
//    }

//    // Position et destination passager
//     _passengerPos_ = new LabelizedVariable("PassengerPos", "Position du
//    Passager", 5);
//     _passengerDest_ = new LabelizedVariable("PassengerDest", "Destination du
//    Passager", 4);
//     _passengerPos_->changeLabel(HOME, "Home");
//     _passengerDest_->changeLabel(HOME, "Home");
//     _passengerPos_->changeLabel(WORK, "Work");
//     _passengerDest_->changeLabel(WORK, "Work");
//     _passengerPos_->changeLabel(THEATER, "Theater");
//     _passengerDest_->changeLabel(THEATER, "Theater");
//     _passengerPos_->changeLabel(CLUB, "Club");
//     _passengerDest_->changeLabel(CLUB, "Club");
//     _passengerPos_->changeLabel(Factory, "Factory");

////     _fuelLevel_ = new LabelizedVariable("FuelLevel", "Niveau du réservoir",
/// 14);

//    // Ajout à séquence
//     _FactoryVars_.insert( _xPos_);
//     _FactoryVars_.insert( _yPos_);
//     _FactoryVars_.insert( _passengerPos_);
//     _FactoryVars_.insert( _passengerDest_);
////     _FactoryVars_.insert( _fuelLevel_);

//    // Prime version creation
//    for( SequenceIteratorSafe<const DiscreteVariable*> varIter =
//    this->beginVariables(); varIter != this->endVariables(); ++varIter){
//      DiscreteVariable* primeVar = (*varIter)->clone();
//      primeVar->setName((*varIter)->name() + "'");
//       _primeMap_.insert((*varIter), primeVar);
//    }

//    //
//    *****************************************************************************************

//    //
//    *****************************************************************************************
//    // Défintion des actions du problème
//     _FactoryActions_.insert(GoNorth);
//     _actionMap_.insert(GoNorth, new std::string ("Go North"));
//     _FactoryActions_.insert(GoEast);
//     _actionMap_.insert(GoEast, new std::string ("Go East"));
//     _FactoryActions_.insert(GoSouth);
//     _actionMap_.insert(GoSouth, new std::string ("Go South"));
//     _FactoryActions_.insert(GoWest);
//     _actionMap_.insert(GoWest, new std::string ("Go West"));
//     _FactoryActions_.insert(PickUp);
//     _actionMap_.insert(PickUp, new std::string ("Pick Up"));
//     _FactoryActions_.insert(PutDown);
//     _actionMap_.insert(PutDown, new std::string ("Put Down"));
////     _FactoryActions_.insert(FillUp);
////     _actionMap_.insert(FillUp, new std::string ("FillUp"));
//  }

//  FactorySimulator::~FactorySimulator(){
//    GUM_DESTRUCTOR(FactorySimulator);

//    for( BijectionIteratorSafe<const DiscreteVariable*, const
//    DiscreteVariable*> varIter =  _primeMap_.beginSafe();
//        varIter !=  _primeMap_.endSafe(); ++varIter ){
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
////    Idx curFuelLevel = randy.valFromPtr( _fuelLevel_);
////    while(curFuelLevel > 12 || curFuelLevel < 5)
////      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) * 7.0)
///+ 5;
////    randy.chgVal( _fuelLevel_, curFuelLevel);

//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    randy.valFromPtr( _passengerPos_);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    randy.valFromPtr( _passengerDest_);
//    while( passPos == passDest || passPos == Factory )
//      passPos = (FactorySimulationLandmark) (((double)std::rand( ) /
//      (double)RAND_MAX) * 3.0);
//    randy.chgVal( _passengerPos_, passPos);

//    return randy;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  bool FactorySimulator::hasReachEnd(){
//    if( currentState_.valFromPtr( _passengerPos_) ==
//    currentState_.valFromPtr( _passengerDest_) )
//      return true;

////    if( currentState_.valFromPtr( _fuelLevel_) == 0 )
////      return true;

//    return false;
//  }


//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  double FactorySimulator::reward(){
//    if( currentState_.valFromPtr( _passengerPos_) ==
//    currentState_.valFromPtr( _passengerDest_) ) {
//      return 20.0;
//    }

//    if(  _lastAction_ == PickUp ){
//      if( currentState_.valFromPtr( _passengerPos_) != Factory )
//        return -10.0;
//      else
//        return 10.0;
//    }

//    if(  _lastAction_ == PutDown )
//      return -10.0;

////    if( currentState_.valFromPtr( _fuelLevel_) == 0 )
////      return -10.0;

//    return 0.0;//-1.0;
//  }

//  //
//  ==================================================================================================================
//  // Reward according to the situation
//  //
//  ==================================================================================================================
//  void FactorySimulator::perform(Idx actionId){

//     _lastAction_ = (FactorySimulationAction) actionId;

////    Idx curFuelLevel = currentState_.valFromPtr( _fuelLevel_);
////    if( curFuelLevel > 0 )
////      currentState_.chgVal( _fuelLevel_, --curFuelLevel);

//    switch(actionId){
//      case GoNorth : return  _goNorth_( );
//      case GoEast : return  _goEast_( );
//      case GoSouth : return  _goSouth_( );
//      case GoWest : return  _goWest_( );
//      case PickUp : return  _pickUp_( );
//      case PutDown : return  _putDown_( );
//      case FillUp : return  _fillUp_( );
//    }
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go North
//  //
//  ==================================================================================================================
//  void FactorySimulator:: _goNorth_(){
//    Idx curPos = this->currentState_.valFromPtr( _yPos_);
//    if( curPos < 4 )
//      currentState_.chgVal( _yPos_, ++curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go east
//  //
//  ==================================================================================================================
//  void FactorySimulator:: _goEast_(){
//    Idx xCurPos = this->currentState_.valFromPtr( _xPos_);
//    Idx yCurPos = this->currentState_.valFromPtr( _yPos_);

//    if(xCurPos == 4)
//      return;
//    if(xCurPos == 0 || xCurPos == 2)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 1)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    currentState_.chgVal( _xPos_, ++xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go south
//  //
//  ==================================================================================================================
//  void FactorySimulator:: _goSouth_(){
//    Idx curPos = this->currentState_.valFromPtr( _yPos_);
//    if( curPos > 0 )
//      currentState_.chgVal( _yPos_, --curPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go west
//  //
//  ==================================================================================================================
//  void FactorySimulator:: _goWest_(){
//    Idx xCurPos = this->currentState_.valFromPtr( _xPos_);
//    Idx yCurPos = this->currentState_.valFromPtr( _yPos_);

//    if(xCurPos == 0)
//      return;
//    if(xCurPos == 1 || xCurPos == 3)
//      if(yCurPos == 0 || yCurPos == 1)
//        return;
//    if(xCurPos == 2)
//      if(yCurPos == 3 || yCurPos == 4)
//        return;

//    currentState_.chgVal( _xPos_, --xCurPos);
//  }


//  //
//  ==================================================================================================================
//  // Transition if you go pick up sb
//  //
//  ==================================================================================================================
//  void FactorySimulator:: _pickUp_(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr( _xPos_);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr( _yPos_);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr( _passengerPos_);
//    switch(passPos){
//      case HOME :  {
//                  if( xCurPos == HOMEX && yCurPos == HOMEY )
//                    currentState_.chgVal( _passengerPos_, Factory);
//                  return;
//                }
//      case WORK :  {
//                  if( xCurPos == WORKX && yCurPos == WORKY )
//                    currentState_.chgVal( _passengerPos_, Factory);
//                  return;
//                }
//      case THEATER :  {
//                  if( xCurPos == THEATERX && yCurPos == THEATERY )
//                    currentState_.chgVal( _passengerPos_, Factory);
//                  return;
//                }
//      case CLUB :  {
//                  if( xCurPos == CLUBX && yCurPos == CLUBY )
//                    currentState_.chgVal( _passengerPos_, Factory);
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
//  void FactorySimulator:: _putDown_(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr( _xPos_);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr( _yPos_);
//    FactorySimulationLandmark passPos = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr( _passengerPos_);
//    FactorySimulationLandmark passDest = (FactorySimulationLandmark)
//    this->currentState_.valFromPtr( _passengerDest_);
//    if(passPos == Factory ){
//        switch(passDest){
//          case HOME :  {
//                      if( xCurPos == HOMEX && yCurPos == HOMEY )
//                        currentState_.chgVal( _passengerPos_, HOME);
//                      return;
//                    }
//          case WORK :  {
//                      if( xCurPos == WORKX && yCurPos == WORKY )
//                        currentState_.chgVal( _passengerPos_, WORK);
//                      return;
//                    }
//          case THEATER :  {
//                      if( xCurPos == THEATERX && yCurPos == THEATERY )
//                        currentState_.chgVal( _passengerPos_, THEATER);
//                      return;
//                    }
//          case CLUB :  {
//                      if( xCurPos == CLUBX && yCurPos == CLUBY )
//                        currentState_.chgVal( _passengerPos_, CLUB);
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
//  void FactorySimulator:: _fillUp_(){
//    FactorySimulationLandmarkX xCurPos = (FactorySimulationLandmarkX)
//    this->currentState_.valFromPtr( _xPos_);
//    FactorySimulationLandmarkY yCurPos = (FactorySimulationLandmarkY)
//    this->currentState_.valFromPtr( _yPos_);

//    if( xCurPos == STATIONX && yCurPos == STATIONY )
//      currentState_.chgVal( _fuelLevel_, 13);
//  }
//} // End of namespace gum
