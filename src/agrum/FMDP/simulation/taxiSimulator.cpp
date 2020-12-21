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
 * @brief Sources of
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 *
 */
// =====================================================================
#include <cstdlib>
#include <random>
//======================================================================
#include <agrum/FMDP/simulation/taxiSimulator.h>
//======================================================================

namespace gum {

  TaxiSimulator::TaxiSimulator() : AbstractSimulator() {
    GUM_CONSTRUCTOR(TaxiSimulator);

    // *****************************************************************************************
    // Défintion des variables du problème

    // Position TaxiSimulator
    xPos__ =
       new LabelizedVariable("xPos", "Position horizontale du TaxiSimulator");
    yPos__ = new LabelizedVariable("yPos", "Position verticale du TaxiSimulator");
    xPos__->eraseLabels();
    yPos__->eraseLabels();
    for (Idx pos = 0; pos < 5; pos++) {
      std::stringstream ss;
      ss << pos;
      xPos__->addLabel(ss.str());
      yPos__->addLabel(ss.str());
    }

    // Position et destination passager
    passengerPos__ =
       new LabelizedVariable("PassengerPos", "Position du Passager", 5);
    passengerDest__ =
       new LabelizedVariable("PassengerDest", "Destination du Passager", 4);
    passengerPos__->changeLabel(HOME, "Home");
    passengerDest__->changeLabel(HOME, "Home");
    passengerPos__->changeLabel(WORK, "Work");
    passengerDest__->changeLabel(WORK, "Work");
    passengerPos__->changeLabel(THEATER, "Theater");
    passengerDest__->changeLabel(THEATER, "Theater");
    passengerPos__->changeLabel(CLUB, "Club");
    passengerDest__->changeLabel(CLUB, "Club");
    passengerPos__->changeLabel(TAXI, "Taxi");

    fuelLevel__ = new LabelizedVariable("FuelLevel", "Niveau du réservoir", 14);

    // Ajout à séquence
    taxiVars__.insert(xPos__);
    taxiVars__.insert(yPos__);
    taxiVars__.insert(passengerPos__);
    taxiVars__.insert(passengerDest__);
    taxiVars__.insert(fuelLevel__);

    // Prime version creation
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            this->beginVariables();
         varIter != this->endVariables();
         ++varIter) {
      DiscreteVariable* primeVar = (*varIter)->clone();
      primeVar->setName((*varIter)->name() + "'");
      primeMap__.insert((*varIter), primeVar);
    }

    // *****************************************************************************************

    // *****************************************************************************************
    // Défintion des actions du problème
    taxiActions__.insert(GoNorth);
    actionMap__.insert(GoNorth, new std::string("Go North"));
    taxiActions__.insert(GoEast);
    actionMap__.insert(GoEast, new std::string("Go East"));
    taxiActions__.insert(GoSouth);
    actionMap__.insert(GoSouth, new std::string("Go South"));
    taxiActions__.insert(GoWest);
    actionMap__.insert(GoWest, new std::string("Go West"));
    taxiActions__.insert(PickUp);
    actionMap__.insert(PickUp, new std::string("Pick Up"));
    taxiActions__.insert(PutDown);
    actionMap__.insert(PutDown, new std::string("Put Down"));
    taxiActions__.insert(FillUp);
    actionMap__.insert(FillUp, new std::string("FillUp"));
  }

  TaxiSimulator::~TaxiSimulator() {
    GUM_DESTRUCTOR(TaxiSimulator);

    for (BijectionIteratorSafe< const DiscreteVariable*, const DiscreteVariable* >
            varIter = primeMap__.beginSafe();
         varIter != primeMap__.endSafe();
         ++varIter) {
      delete varIter.first();
      delete varIter.second();
    }
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  Instantiation TaxiSimulator::randomState_() {
    Instantiation randy = AbstractSimulator::randomState_();
    //    Idx curFuelLevel = randy.valFromPtr(fuelLevel__);
    //    while(curFuelLevel > 12 || curFuelLevel < 5)
    //      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) *
    //      7.0) + 5;
    //    randy.chgVal(fuelLevel__, curFuelLevel);

    //    TaxiSimulationLandmark passPos = (TaxiSimulationLandmark)
    //    randy.valFromPtr(passengerPos__);
    //    TaxiSimulationLandmark passDest = (TaxiSimulationLandmark)
    //    randy.valFromPtr(passengerDest__);
    //    while( passPos == passDest || passPos == TAXI )
    //      passPos = (TaxiSimulationLandmark) (((double)std::rand( ) /
    //      (double)RAND_MAX) * 3.0);
    //    randy.chgVal(passengerPos__, passPos);

    return randy;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  bool TaxiSimulator::hasReachEnd() {
    //    if( currentState_.valFromPtr(passengerPos__) ==
    //    currentState_.valFromPtr(passengerDest__) )
    //      return true;

    //    if( currentState_.valFromPtr(fuelLevel__) == 0 )
    //      return true;

    return false;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  double TaxiSimulator::reward() { return reward__; }

  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::perform(Idx actionId) {
    lastAction__ = (TaxiSimulationAction)actionId;

    evalReward__();

    Idx curFuelLevel = currentState_.valFromPtr(fuelLevel__);
    if (curFuelLevel > 0) currentState_.chgVal(fuelLevel__, --curFuelLevel);

    switch (actionId) {
      case GoNorth: return performGoNorth__();
      case GoEast: return performGoEast__();
      case GoSouth: return performGoSouth__();
      case GoWest: return performGoWest__();
      case PickUp: return performPickUp__();
      case PutDown: return performPutDown__();
      case FillUp: return performFillUp__();
    }
  }


  // ==================================================================================================================
  // Transition if you go North
  // ==================================================================================================================
  void TaxiSimulator::performGoNorth__() {
    Idx curPos = this->currentState_.valFromPtr(yPos__);
    if (curPos < 4) currentState_.chgVal(yPos__, ++curPos);
  }


  // ==================================================================================================================
  // Transition if you go east
  // ==================================================================================================================
  void TaxiSimulator::performGoEast__() {
    Idx xCurPos = this->currentState_.valFromPtr(xPos__);
    Idx yCurPos = this->currentState_.valFromPtr(yPos__);

    if (xCurPos == 4) return;
    if (xCurPos == 0 || xCurPos == 2)
      if (yCurPos == 0 || yCurPos == 1) return;
    if (xCurPos == 1)
      if (yCurPos == 3 || yCurPos == 4) return;

    currentState_.chgVal(xPos__, ++xCurPos);
  }


  // ==================================================================================================================
  // Transition if you go south
  // ==================================================================================================================
  void TaxiSimulator::performGoSouth__() {
    Idx curPos = this->currentState_.valFromPtr(yPos__);
    if (curPos > 0) currentState_.chgVal(yPos__, --curPos);
  }


  // ==================================================================================================================
  // Transition if you go west
  // ==================================================================================================================
  void TaxiSimulator::performGoWest__() {
    Idx xCurPos = this->currentState_.valFromPtr(xPos__);
    Idx yCurPos = this->currentState_.valFromPtr(yPos__);

    if (xCurPos == 0) return;
    if (xCurPos == 1 || xCurPos == 3)
      if (yCurPos == 0 || yCurPos == 1) return;
    if (xCurPos == 2)
      if (yCurPos == 3 || yCurPos == 4) return;

    currentState_.chgVal(xPos__, --xCurPos);
  }


  // ==================================================================================================================
  // Transition if you go pick up sb
  // ==================================================================================================================
  void TaxiSimulator::performPickUp__() {
    TaxiSimulationLandmarkX xCurPos =
       (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(xPos__);
    TaxiSimulationLandmarkY yCurPos =
       (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(yPos__);
    TaxiSimulationLandmark passPos =
       (TaxiSimulationLandmark)this->currentState_.valFromPtr(passengerPos__);
    switch (passPos) {
      case HOME: {
        if (xCurPos == HOMEX && yCurPos == HOMEY)
          currentState_.chgVal(passengerPos__, TAXI);
        return;
      }
      case WORK: {
        if (xCurPos == WORKX && yCurPos == WORKY)
          currentState_.chgVal(passengerPos__, TAXI);
        return;
      }
      case THEATER: {
        if (xCurPos == THEATERX && yCurPos == THEATERY)
          currentState_.chgVal(passengerPos__, TAXI);
        return;
      }
      case CLUB: {
        if (xCurPos == CLUBX && yCurPos == CLUBY)
          currentState_.chgVal(passengerPos__, TAXI);
        return;
      }
      case TAXI: return;
    }
  }


  // ==================================================================================================================
  // Transition if you go put down sb
  // ==================================================================================================================
  void TaxiSimulator::performPutDown__() {
    TaxiSimulationLandmarkX xCurPos =
       (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(xPos__);
    TaxiSimulationLandmarkY yCurPos =
       (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(yPos__);
    TaxiSimulationLandmark passPos =
       (TaxiSimulationLandmark)this->currentState_.valFromPtr(passengerPos__);
    TaxiSimulationLandmark passDest =
       (TaxiSimulationLandmark)this->currentState_.valFromPtr(passengerDest__);
    if (passPos == TAXI) {
      switch (passDest) {
        case HOME: {
          if (xCurPos == HOMEX && yCurPos == HOMEY)
            currentState_.chgVal(passengerPos__, HOME);
          return;
        }
        case WORK: {
          if (xCurPos == WORKX && yCurPos == WORKY)
            currentState_.chgVal(passengerPos__, WORK);
          return;
        }
        case THEATER: {
          if (xCurPos == THEATERX && yCurPos == THEATERY)
            currentState_.chgVal(passengerPos__, THEATER);
          return;
        }
        case CLUB: {
          if (xCurPos == CLUBX && yCurPos == CLUBY)
            currentState_.chgVal(passengerPos__, CLUB);
          return;
        }
        case TAXI: return;
      }
    }
  }


  // ==================================================================================================================
  // Transition if you go reffill
  // ==================================================================================================================
  void TaxiSimulator::performFillUp__() {
    TaxiSimulationLandmarkX xCurPos =
       (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(xPos__);
    TaxiSimulationLandmarkY yCurPos =
       (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(yPos__);

    if (xCurPos == STATIONX && yCurPos == STATIONY)
      currentState_.chgVal(fuelLevel__, 13);
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::evalReward__() {
    TaxiSimulationLandmarkX xCurPos =
       (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(xPos__);
    TaxiSimulationLandmarkY yCurPos =
       (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(yPos__);
    TaxiSimulationLandmark passPos =
       (TaxiSimulationLandmark)this->currentState_.valFromPtr(passengerPos__);
    TaxiSimulationLandmark passDest =
       (TaxiSimulationLandmark)this->currentState_.valFromPtr(passengerDest__);

    if (lastAction__ == PutDown) {
      if (passPos == TAXI) {
        if (isAtDestination__(passDest, xCurPos, yCurPos))
          reward__ = 30.0;
        else
          reward__ = 0.0;
        return;
      }
      reward__ = 0;
      return;
    }

    if (lastAction__ == PickUp) {
      if (isAtMeetPoint__(passPos, xCurPos, yCurPos))
        reward__ = 20.0;
      else
        reward__ = 0.0;
      return;
    }

    if (currentState_.valFromPtr(fuelLevel__) == 0 && lastAction__ != FillUp) {
      reward__ = 0.0;
      return;
    }

    if (lastAction__ == FillUp && (xCurPos != STATIONX || yCurPos != STATIONY)) {
      reward__ = 0.0;
      return;
    }

    reward__ = 10.0;   //-1.0;
  }

  bool TaxiSimulator::isAtDestination__(TaxiSimulationLandmark  passDest,
                                        TaxiSimulationLandmarkX xCurPos,
                                        TaxiSimulationLandmarkY yCurPos) {
    switch (passDest) {
      case HOME: {
        if (xCurPos == HOMEX && yCurPos == HOMEY) return true;
        break;
      }
      case WORK: {
        if (xCurPos == WORKX && yCurPos == WORKY) return true;
        break;
      }
      case THEATER: {
        if (xCurPos == THEATERX && yCurPos == THEATERY) return true;
        break;
      }
      case CLUB: {
        if (xCurPos == CLUBX && yCurPos == CLUBY) return true;
        break;
      }
      case TAXI: return false;
    }
    return false;
  }

  bool TaxiSimulator::isAtMeetPoint__(TaxiSimulationLandmark  passPos,
                                      TaxiSimulationLandmarkX xCurPos,
                                      TaxiSimulationLandmarkY yCurPos) {
    switch (passPos) {
      case HOME: {
        if (xCurPos == HOMEX && yCurPos == HOMEY) return true;
        break;
      }
      case WORK: {
        if (xCurPos == WORKX && yCurPos == WORKY) return true;
        break;
      }
      case THEATER: {
        if (xCurPos == THEATERX && yCurPos == THEATERY) return true;
        break;
      }
      case CLUB: {
        if (xCurPos == CLUBX && yCurPos == CLUBY) return true;
        break;
      }
      case TAXI: return false;
    }
    return false;
  }
}   // End of namespace gum
