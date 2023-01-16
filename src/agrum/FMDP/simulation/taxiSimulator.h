/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class for simulating a markov decision process.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_TAXI_SIMULATOR_H
#define GUM_TAXI_SIMULATOR_H
//======================================================================
//======================================================================
//======================================================================
#include <agrum/FMDP/simulation/abstractSimulator.h>
//======================================================================
#include <agrum/tools/variables/labelizedVariable.h>
//======================================================================

namespace gum {


  enum TaxiSimulationLandmark : Idx {
    HOME    = 0,
    WORK    = 1,
    THEATER = 2,
    CLUB    = 3,
    TAXI    = 4
  };
  enum TaxiSimulationLandmarkX : Idx {
    HOMEX    = 0,
    WORKX    = 0,
    THEATERX = 3,
    CLUBX    = 4,
    STATIONX = 2
  };
  enum TaxiSimulationLandmarkY : Idx {
    HOMEY    = 0,
    WORKY    = 4,
    THEATERY = 0,
    CLUBY    = 4,
    STATIONY = 1
  };
  enum TaxiSimulationAction : Idx {
    GoNorth = 1,
    GoEast  = 2,
    GoSouth = 3,
    GoWest  = 4,
    PickUp  = 5,
    PutDown = 6,
    FillUp  = 7
  };

  // clang-format off
  /**
* @class TaxiSimulator
* @headerfile taxiSimulator.h <agrum/FMDP/simulation/taxiSimulator.h>
   * @brief A class to simulate the Taxi problem
   * @ingroup fmdp_group
   */
  // clang-format on
  class TaxiSimulator: public AbstractSimulator {
    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    TaxiSimulator();

    /**
     * Default destructor.
     */
    ~TaxiSimulator();

    /// @}

    // ===========================================================================
    /// @name States
    // ===========================================================================
    /// @{

    protected:
    /// Choses a random state as the first test for a run
    Instantiation randomState_();

    public:
    bool hasReachEnd();

    /// @}

    // ===========================================================================
    /// @name Variables
    // ===========================================================================
    /// @{

    const DiscreteVariable* primeVar(const DiscreteVariable* mainVar) {
      return _primeMap_.second(mainVar);
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< const DiscreteVariable* > beginVariables() {
      return _taxiVars_.beginSafe();
    }
    SequenceIteratorSafe< const DiscreteVariable* > endVariables() { return _taxiVars_.endSafe(); }

    /// @}

    // ===========================================================================
    /// @name Actions
    // ===========================================================================
    /// @{

    const std::string& actionName(Idx actionId) { return *_actionMap_[actionId]; }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< Idx > beginActions() { return _taxiActions_.beginSafe(); }
    SequenceIteratorSafe< Idx > endActions() { return _taxiActions_.endSafe(); }


    void perform(Idx);

    private:
    void _performGoNorth_();
    void _performGoEast_();
    void _performGoSouth_();
    void _performGoWest_();
    void _performPickUp_();
    void _performPutDown_();
    void _performFillUp_();

    /// @}

    // ===========================================================================
    /// @name Rewards
    // ===========================================================================
    /// @{
    public:
    double reward();

    private:
    void _evalReward_();
    bool _isAtDestination_(TaxiSimulationLandmark  passDest,
                           TaxiSimulationLandmarkX xCurPos,
                           TaxiSimulationLandmarkY yCurPos);
    bool _isAtMeetPoint_(TaxiSimulationLandmark  passpos,
                         TaxiSimulationLandmarkX xCurPos,
                         TaxiSimulationLandmarkY yCurPos);

    /// @}

    /// Variables data structures
    Sequence< const DiscreteVariable* >                           _taxiVars_;
    Bijection< const DiscreteVariable*, const DiscreteVariable* > _primeMap_;
    LabelizedVariable*                                            _xPos_;
    LabelizedVariable*                                            _yPos_;
    LabelizedVariable*                                            _passengerPos_;
    LabelizedVariable*                                            _passengerDest_;
    LabelizedVariable*                                            _fuelLevel_;

    /// Actions
    Sequence< Idx >                _taxiActions_;
    HashTable< Idx, std::string* > _actionMap_;   // __actionMap.insert ( actionId,
    // new std::string ( action ) );
    TaxiSimulationAction _lastAction_;

    /// Reward
    double _reward_;
  };

} /* namespace gum */


#endif   // GUM_TAXI_SIMULATOR_H
