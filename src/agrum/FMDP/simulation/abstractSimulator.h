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
 * @brief Class for simulating a discrete time stochastic process.
 *
 * @author Jean-Christophe MAGNAN
 */
#ifndef GUM_ABSTRACT_SIMULATOR_H
#define GUM_ABSTRACT_SIMULATOR_H
//======================================================================
#include <agrum/multidim/instantiation.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================
//======================================================================

namespace gum {
  /**
   * @class AbstractSimulator abstractSimulator.h
   * <agrum/FMDP/simulation/abstractSimulator.h>
   * @brief A class to simulate a reinforcement learning problem.
   * @ingroup fmdp_group
   *
   *
   *
   */
  class AbstractSimulator {

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    AbstractSimulator();

    /**
     * Default destructor.
     */
    virtual ~AbstractSimulator();

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    /// Sets the intial statefrom which we begun the simulation
    INLINE void setInitialState( const Instantiation& initialState ) {
      _currentState = initialState;
    }
    void setInitialStateRandomly();

    protected:
    /// Choses a random state as the first test for a run
    virtual Instantiation _randomState();

    public:
    /// Sets the final states upon which a run is over
    INLINE void setEndState( const Instantiation& endState ) {
      _endState = endState;
    }

    /// Tests if end state has been reached
    virtual bool hasReachEnd();

    ///
    INLINE const Instantiation& currentState() { return _currentState; }

    ///
    virtual double reward() = 0;

    ///
    virtual void perform( Idx ) = 0;

    /// @}

    // ===========================================================================
    /// @name Variables
    // ===========================================================================
    /// @{

    virtual const DiscreteVariable*
    primeVar( const DiscreteVariable* mainVar ) = 0;

    /// Iteration over the variables of the simulated probleme
    virtual SequenceIteratorSafe<const DiscreteVariable*> beginVariables() = 0;
    virtual SequenceIteratorSafe<const DiscreteVariable*> endVariables() = 0;

    /// @}

    // ===========================================================================
    /// @name Actions
    // ===========================================================================
    /// @{

    virtual const std::string& actionName( Idx ) = 0;

    /// Iteration over the variables of the simulated probleme
    virtual SequenceIteratorSafe<Idx> beginActions() = 0;
    virtual SequenceIteratorSafe<Idx> endActions() = 0;


    /// @}

    /// Tha state in which the system currently is
    Instantiation _currentState, _endState;
  };

} /* namespace gum */


#endif  // GUM__ABSTRACT_SIMULATOR_H
