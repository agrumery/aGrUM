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
    _xPos_ = new LabelizedVariable("xPos", "Position horizontale du TaxiSimulator");
    _yPos_ = new LabelizedVariable("yPos", "Position verticale du TaxiSimulator");
    _xPos_->eraseLabels();
    _yPos_->eraseLabels();
    for (Idx pos = 0; pos < 5; pos++) {
      std::stringstream ss;
      ss << pos;
      _xPos_->addLabel(ss.str());
      _yPos_->addLabel(ss.str());
    }

    // Position et destination passager
    _passengerPos_  = new LabelizedVariable("PassengerPos", "Position du Passager", 5);
    _passengerDest_ = new LabelizedVariable("PassengerDest", "Destination du Passager", 4);
    _passengerPos_->changeLabel(HOME, "Home");
    _passengerDest_->changeLabel(HOME, "Home");
    _passengerPos_->changeLabel(WORK, "Work");
    _passengerDest_->changeLabel(WORK, "Work");
    _passengerPos_->changeLabel(THEATER, "Theater");
    _passengerDest_->changeLabel(THEATER, "Theater");
    _passengerPos_->changeLabel(CLUB, "Club");
    _passengerDest_->changeLabel(CLUB, "Club");
    _passengerPos_->changeLabel(TAXI, "Taxi");

    _fuelLevel_ = new LabelizedVariable("FuelLevel", "Niveau du réservoir", 14);

    // Ajout à séquence
    _taxiVars_.insert(_xPos_);
    _taxiVars_.insert(_yPos_);
    _taxiVars_.insert(_passengerPos_);
    _taxiVars_.insert(_passengerDest_);
    _taxiVars_.insert(_fuelLevel_);

    // Prime version creation
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter = this->beginVariables();
         varIter != this->endVariables();
         ++varIter) {
      DiscreteVariable* primeVar = (*varIter)->clone();
      primeVar->setName((*varIter)->name() + "'");
      _primeMap_.insert((*varIter), primeVar);
    }

    // *****************************************************************************************

    // *****************************************************************************************
    // Défintion des actions du problème
    _taxiActions_.insert(GoNorth);
    _actionMap_.insert(GoNorth, new std::string("Go North"));
    _taxiActions_.insert(GoEast);
    _actionMap_.insert(GoEast, new std::string("Go East"));
    _taxiActions_.insert(GoSouth);
    _actionMap_.insert(GoSouth, new std::string("Go South"));
    _taxiActions_.insert(GoWest);
    _actionMap_.insert(GoWest, new std::string("Go West"));
    _taxiActions_.insert(PickUp);
    _actionMap_.insert(PickUp, new std::string("Pick Up"));
    _taxiActions_.insert(PutDown);
    _actionMap_.insert(PutDown, new std::string("Put Down"));
    _taxiActions_.insert(FillUp);
    _actionMap_.insert(FillUp, new std::string("FillUp"));
  }

  TaxiSimulator::~TaxiSimulator() {
    GUM_DESTRUCTOR(TaxiSimulator);

    for (BijectionIteratorSafe< const DiscreteVariable*, const DiscreteVariable* > varIter
         = _primeMap_.beginSafe();
         varIter != _primeMap_.endSafe();
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
    //    Idx curFuelLevel = randy.valFromPtr( _fuelLevel_);
    //    while(curFuelLevel > 12 || curFuelLevel < 5)
    //      curFuelLevel = (Idx)(((double)std::rand( ) / (double)RAND_MAX) *
    //      7.0) + 5;
    //    randy.chgVal( _fuelLevel_, curFuelLevel);

    //    TaxiSimulationLandmark passPos = (TaxiSimulationLandmark)
    //    randy.valFromPtr( _passengerPos_);
    //    TaxiSimulationLandmark passDest = (TaxiSimulationLandmark)
    //    randy.valFromPtr( _passengerDest_);
    //    while( passPos == passDest || passPos == TAXI )
    //      passPos = (TaxiSimulationLandmark) (((double)std::rand( ) /
    //      (double)RAND_MAX) * 3.0);
    //    randy.chgVal( _passengerPos_, passPos);

    return randy;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  bool TaxiSimulator::hasReachEnd() {
    //    if( currentState_.valFromPtr( _passengerPos_) ==
    //    currentState_.valFromPtr( _passengerDest_) )
    //      return true;

    //    if( currentState_.valFromPtr( _fuelLevel_) == 0 )
    //      return true;

    return false;
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  double TaxiSimulator::reward() { return _reward_; }

  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::perform(Idx actionId) {
    _lastAction_ = (TaxiSimulationAction)actionId;

    _evalReward_();

    Idx curFuelLevel = currentState_.valFromPtr(_fuelLevel_);
    if (curFuelLevel > 0) currentState_.chgVal(_fuelLevel_, --curFuelLevel);

    switch (actionId) {
      case GoNorth:
        return _performGoNorth_();
      case GoEast:
        return _performGoEast_();
      case GoSouth:
        return _performGoSouth_();
      case GoWest:
        return _performGoWest_();
      case PickUp:
        return _performPickUp_();
      case PutDown:
        return _performPutDown_();
      case FillUp:
        return _performFillUp_();
    }
  }


  // ==================================================================================================================
  // Transition if you go North
  // ==================================================================================================================
  void TaxiSimulator::_performGoNorth_() {
    Idx curPos = this->currentState_.valFromPtr(_yPos_);
    if (curPos < 4) currentState_.chgVal(_yPos_, ++curPos);
  }


  // ==================================================================================================================
  // Transition if you go east
  // ==================================================================================================================
  void TaxiSimulator::_performGoEast_() {
    Idx xCurPos = this->currentState_.valFromPtr(_xPos_);
    Idx yCurPos = this->currentState_.valFromPtr(_yPos_);

    if (xCurPos == 4) return;
    if (xCurPos == 0 || xCurPos == 2)
      if (yCurPos == 0 || yCurPos == 1) return;
    if (xCurPos == 1)
      if (yCurPos == 3 || yCurPos == 4) return;

    currentState_.chgVal(_xPos_, ++xCurPos);
  }


  // ==================================================================================================================
  // Transition if you go south
  // ==================================================================================================================
  void TaxiSimulator::_performGoSouth_() {
    Idx curPos = this->currentState_.valFromPtr(_yPos_);
    if (curPos > 0) currentState_.chgVal(_yPos_, --curPos);
  }


  // ==================================================================================================================
  // Transition if you go west
  // ==================================================================================================================
  void TaxiSimulator::_performGoWest_() {
    Idx xCurPos = this->currentState_.valFromPtr(_xPos_);
    Idx yCurPos = this->currentState_.valFromPtr(_yPos_);

    if (xCurPos == 0) return;
    if (xCurPos == 1 || xCurPos == 3)
      if (yCurPos == 0 || yCurPos == 1) return;
    if (xCurPos == 2)
      if (yCurPos == 3 || yCurPos == 4) return;

    currentState_.chgVal(_xPos_, --xCurPos);
  }


  // ==================================================================================================================
  // Transition if you go pick up sb
  // ==================================================================================================================
  void TaxiSimulator::_performPickUp_() {
    TaxiSimulationLandmarkX xCurPos
       = (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(_xPos_);
    TaxiSimulationLandmarkY yCurPos
       = (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(_yPos_);
    TaxiSimulationLandmark passPos
       = (TaxiSimulationLandmark)this->currentState_.valFromPtr(_passengerPos_);
    switch (passPos) {
      case HOME: {
        if (xCurPos == HOMEX && yCurPos == HOMEY) currentState_.chgVal(_passengerPos_, TAXI);
        return;
      }
      case WORK: {
        if (xCurPos == WORKX && yCurPos == WORKY) currentState_.chgVal(_passengerPos_, TAXI);
        return;
      }
      case THEATER: {
        if (xCurPos == THEATERX && yCurPos == THEATERY) currentState_.chgVal(_passengerPos_, TAXI);
        return;
      }
      case CLUB: {
        if (xCurPos == CLUBX && yCurPos == CLUBY) currentState_.chgVal(_passengerPos_, TAXI);
        return;
      }
      case TAXI:
        return;
    }
  }


  // ==================================================================================================================
  // Transition if you go put down sb
  // ==================================================================================================================
  void TaxiSimulator::_performPutDown_() {
    TaxiSimulationLandmarkX xCurPos
       = (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(_xPos_);
    TaxiSimulationLandmarkY yCurPos
       = (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(_yPos_);
    TaxiSimulationLandmark passPos
       = (TaxiSimulationLandmark)this->currentState_.valFromPtr(_passengerPos_);
    TaxiSimulationLandmark passDest
       = (TaxiSimulationLandmark)this->currentState_.valFromPtr(_passengerDest_);
    if (passPos == TAXI) {
      switch (passDest) {
        case HOME: {
          if (xCurPos == HOMEX && yCurPos == HOMEY) currentState_.chgVal(_passengerPos_, HOME);
          return;
        }
        case WORK: {
          if (xCurPos == WORKX && yCurPos == WORKY) currentState_.chgVal(_passengerPos_, WORK);
          return;
        }
        case THEATER: {
          if (xCurPos == THEATERX && yCurPos == THEATERY)
            currentState_.chgVal(_passengerPos_, THEATER);
          return;
        }
        case CLUB: {
          if (xCurPos == CLUBX && yCurPos == CLUBY) currentState_.chgVal(_passengerPos_, CLUB);
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
  void TaxiSimulator::_performFillUp_() {
    TaxiSimulationLandmarkX xCurPos
       = (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(_xPos_);
    TaxiSimulationLandmarkY yCurPos
       = (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(_yPos_);

    if (xCurPos == STATIONX && yCurPos == STATIONY) currentState_.chgVal(_fuelLevel_, 13);
  }


  // ==================================================================================================================
  // Reward according to the situation
  // ==================================================================================================================
  void TaxiSimulator::_evalReward_() {
    TaxiSimulationLandmarkX xCurPos
       = (TaxiSimulationLandmarkX)this->currentState_.valFromPtr(_xPos_);
    TaxiSimulationLandmarkY yCurPos
       = (TaxiSimulationLandmarkY)this->currentState_.valFromPtr(_yPos_);
    TaxiSimulationLandmark passPos
       = (TaxiSimulationLandmark)this->currentState_.valFromPtr(_passengerPos_);
    TaxiSimulationLandmark passDest
       = (TaxiSimulationLandmark)this->currentState_.valFromPtr(_passengerDest_);

    if (_lastAction_ == PutDown) {
      if (passPos == TAXI) {
        if (_isAtDestination_(passDest, xCurPos, yCurPos))
          _reward_ = 30.0;
        else
          _reward_ = 0.0;
        return;
      }
      _reward_ = 0;
      return;
    }

    if (_lastAction_ == PickUp) {
      if (_isAtMeetPoint_(passPos, xCurPos, yCurPos))
        _reward_ = 20.0;
      else
        _reward_ = 0.0;
      return;
    }

    if (currentState_.valFromPtr(_fuelLevel_) == 0 && _lastAction_ != FillUp) {
      _reward_ = 0.0;
      return;
    }

    if (_lastAction_ == FillUp && (xCurPos != STATIONX || yCurPos != STATIONY)) {
      _reward_ = 0.0;
      return;
    }

    _reward_ = 10.0;   //-1.0;
  }

  bool TaxiSimulator::_isAtDestination_(TaxiSimulationLandmark  passDest,
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
      case TAXI:
        return false;
    }
    return false;
  }

  bool TaxiSimulator::_isAtMeetPoint_(TaxiSimulationLandmark  passPos,
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
      case TAXI:
        return false;
    }
    return false;
  }
}   // End of namespace gum
