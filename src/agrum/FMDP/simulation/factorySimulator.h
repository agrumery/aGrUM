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
 * @brief Class for simulating a markov decision process.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
 */
#ifndef GUM_FACTORY_SIMULATOR_H
#define GUM_FACTORY_SIMULATOR_H
//======================================================================
#include <agrum/tools/multidim/instantiation.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
#include <agrum/FMDP/simulation/abstractSimulator.h>
//======================================================================
#include <agrum/tools/variables/discreteVariable.h>
//======================================================================

namespace gum {


  enum FactorySimulationLandmark : Idx {
    HOME = 0,
    WORK = 1,
    THEATER = 2,
    CLUB = 3,
    Factory = 4
  };
  enum FactorySimulationLandmarkX : Idx {
    HOMEX = 0,
    WORKX = 0,
    THEATERX = 3,
    CLUBX = 4,
    STATIONX = 2
  };
  enum FactorySimulationLandmarkY : Idx {
    HOMEY = 0,
    WORKY = 4,
    THEATERY = 0,
    CLUBY = 4,
    STATIONY = 1
  };
  enum FactorySimulationAction : Idx {
    GoNorth = 0,
    GoEast = 1,
    GoSouth = 2,
    GoWest = 3,
    PickUp = 4,
    PutDown = 5,
    FillUp = 6
  };

  // clang-format off
  /**
* @class FactorySimulator
* @headerfile factorySimulator.h <agrum/FMDP/simulation/factorySimulator.h>
   * @brief A class to simulate the Factory problem
   * @ingroup fmdp_group
   */
  // clang-format on
  class FactorySimulator: public AbstractSimulator {
    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    FactorySimulator();

    /**
     * Default destructor.
     */
    ~FactorySimulator();

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    protected:
    /// Choses a random state as the first test for a run
    Instantiation randomState_();

    public:
    bool hasReachEnd();
    ///
    double reward();
    void   perform(Idx);

    /// @}

    // ===========================================================================
    /// @name Variables
    // ===========================================================================
    /// @{

    const DiscreteVariable* primeVar(const DiscreteVariable* mainVar) {
      return primeMap__.second(mainVar);
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< const DiscreteVariable* > beginVariables() {
      return FactoryVars__.beginSafe();
    }
    SequenceIteratorSafe< const DiscreteVariable* > endVariables() {
      return FactoryVars__.endSafe();
    }

    /// @}

    // ===========================================================================
    /// @name Actions
    // ===========================================================================
    /// @{

    const std::string& actionName(Idx actionId) { return *actionMap__[actionId]; }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< Idx > beginActions() {
      return FactoryActions__.beginSafe();
    }
    SequenceIteratorSafe< Idx > endActions() { return FactoryActions__.endSafe(); }


    /// @}


    private:
    void goNorth__();
    void goEast__();
    void goSouth__();
    void goWest__();
    void pickUp__();
    void putDown__();
    void fillUp__();

    /// Variables data structures
    Sequence< const DiscreteVariable* >                           FactoryVars__;
    Bijection< const DiscreteVariable*, const DiscreteVariable* > primeMap__;
    DiscreteVariable*                                             xPos__;
    DiscreteVariable*                                             yPos__;
    DiscreteVariable*                                             passengerPos__;
    DiscreteVariable*                                             passengerDest__;
    DiscreteVariable*                                             fuelLevel__;

    /// Actions
    Sequence< Idx >                FactoryActions__;
    HashTable< Idx, std::string* > actionMap__;   //__actionMap.insert ( actionId,
    // new std::string ( action ) );
    FactorySimulationAction lastAction__;
  };

} /* namespace gum */


#endif   // GUM_FACTORY_SIMULATOR_H
