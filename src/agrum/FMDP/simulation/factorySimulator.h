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
#ifndef GUM_FACTORY_SIMULATOR_H
#define GUM_FACTORY_SIMULATOR_H
//======================================================================
#include <agrum/multidim/instantiation.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
#include <agrum/FMDP/simulation/abstractSimulator.h>
//======================================================================
#include <agrum/variables/discreteVariable.h>
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
  /**
   * @class Factory factorySimulator.h
   * <agrum/FMDP/simulation/factorySimulator.h>
   * @brief A class to simulate the Factory problem
   * @ingroup fmdp_group
   *
   *
   *
   */
  class FactorySimulator : public AbstractSimulator {

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
    Instantiation _randomState();

    public:
    bool hasReachEnd();
    ///
    double reward();
    void   perform( Idx );

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
      return __FactoryVars.beginSafe();
    }
    SequenceIteratorSafe<const DiscreteVariable*> endVariables() {
      return __FactoryVars.endSafe();
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
    SequenceIteratorSafe<Idx> beginActions() {
      return __FactoryActions.beginSafe();
    }
    SequenceIteratorSafe<Idx> endActions() { return __FactoryActions.endSafe(); }


    /// @}


    private:
    void __goNorth();
    void __goEast();
    void __goSouth();
    void __goWest();
    void __pickUp();
    void __putDown();
    void __fillUp();

    /// Variables data structures
    Sequence<const DiscreteVariable*> __FactoryVars;
    Bijection<const DiscreteVariable*, const DiscreteVariable*> __primeMap;
    DiscreteVariable* __xPos;
    DiscreteVariable* __yPos;
    DiscreteVariable* __passengerPos;
    DiscreteVariable* __passengerDest;
    DiscreteVariable* __fuelLevel;

    /// Actions
    Sequence<Idx> __FactoryActions;
    HashTable<Idx, std::string*> __actionMap;  //__actionMap.insert ( actionId,
    // new std::string ( action ) );
    FactorySimulationAction __lastAction;
  };

} /* namespace gum */


#endif  // GUM_FACTORY_SIMULATOR_H
