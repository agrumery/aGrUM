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
 * @brief Class for simulating a markov decision process.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_TAXI_SIMULATOR_H
#define GUM_TAXI_SIMULATOR_H
//======================================================================
#include <agrum/multidim/instantiation.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
#include <agrum/FMDP/simulation/abstractSimulator.h>
//======================================================================
#include <agrum/variables/labelizedVariable.h>
//======================================================================

namespace gum {


  enum TaxiSimulationLandmark : Idx {
    HOME = 0,
    WORK = 1,
    THEATER = 2,
    CLUB = 3,
    TAXI = 4
  };
  enum TaxiSimulationLandmarkX : Idx {
    HOMEX = 0,
    WORKX = 0,
    THEATERX = 3,
    CLUBX = 4,
    STATIONX = 2
  };
  enum TaxiSimulationLandmarkY : Idx {
    HOMEY = 0,
    WORKY = 4,
    THEATERY = 0,
    CLUBY = 4,
    STATIONY = 1
  };
  enum TaxiSimulationAction : Idx {
    GoNorth = 1,
    GoEast = 2,
    GoSouth = 3,
    GoWest = 4,
    PickUp = 5,
    PutDown = 6,
    FillUp = 7
  };
  /**
   * @class Taxi taxiSimulator.h <agrum/FMDP/simulation/taxiSimulator.h>
   * @brief A class to simulate the Taxi problem
   * @ingroup fmdp_group
   *
   *
   *
   */
  class TaxiSimulator : public AbstractSimulator {

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
    Instantiation _randomState();

    public:
    bool hasReachEnd();

    /// @}

    // ===========================================================================
    /// @name Variables
    // ===========================================================================
    /// @{

    const DiscreteVariable* primeVar( const DiscreteVariable* mainVar ) {
      return __primeMap.second( mainVar );
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe<const DiscreteVariable*> beginVariables() {
      return __taxiVars.beginSafe();
    }
    SequenceIteratorSafe<const DiscreteVariable*> endVariables() {
      return __taxiVars.endSafe();
    }

    /// @}

    // ===========================================================================
    /// @name Actions
    // ===========================================================================
    /// @{

    const std::string& actionName( Idx actionId ) {
      return *__actionMap[actionId];
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe<Idx> beginActions() { return __taxiActions.beginSafe(); }
    SequenceIteratorSafe<Idx> endActions() { return __taxiActions.endSafe(); }


    void perform( Idx );

    private:
    void __performGoNorth();
    void __performGoEast();
    void __performGoSouth();
    void __performGoWest();
    void __performPickUp();
    void __performPutDown();
    void __performFillUp();

    /// @}

    // ===========================================================================
    /// @name Rewards
    // ===========================================================================
    /// @{
    public:
    double reward();

    private:
    void __evalReward();
    bool __isAtDestination( TaxiSimulationLandmark  passDest,
                            TaxiSimulationLandmarkX xCurPos,
                            TaxiSimulationLandmarkY yCurPos );
    bool __isAtMeetPoint( TaxiSimulationLandmark  passpos,
                          TaxiSimulationLandmarkX xCurPos,
                          TaxiSimulationLandmarkY yCurPos );

    /// @}

    /// Variables data structures
    Sequence<const DiscreteVariable*> __taxiVars;
    Bijection<const DiscreteVariable*, const DiscreteVariable*> __primeMap;
    LabelizedVariable* __xPos;
    LabelizedVariable* __yPos;
    LabelizedVariable* __passengerPos;
    LabelizedVariable* __passengerDest;
    LabelizedVariable* __fuelLevel;

    /// Actions
    Sequence<Idx> __taxiActions;
    HashTable<Idx, std::string*> __actionMap;  //__actionMap.insert ( actionId,
    // new std::string ( action ) );
    TaxiSimulationAction __lastAction;

    /// Reward
    double __reward;
  };

} /* namespace gum */


#endif  // GUM_TAXI_SIMULATOR_H
