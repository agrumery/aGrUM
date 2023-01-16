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
 * @brief Class for simulating a discrete time stochastic process.
 *
 * @author Jean-Christophe MAGNAN
 */
#ifndef GUM_ABSTRACT_SIMULATOR_H
#define GUM_ABSTRACT_SIMULATOR_H
//======================================================================
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
    INLINE void setInitialState(const Instantiation& initialState) { currentState_ = initialState; }
    void        setInitialStateRandomly();

    protected:
    /// Choses a random state as the first test for a run
    virtual Instantiation randomState_();

    public:
    /// Sets the final states upon which a run is over
    INLINE void setEndState(const Instantiation& endState) { endState_ = endState; }

    /// Tests if end state has been reached
    virtual bool hasReachEnd();

    ///
    INLINE const Instantiation& currentState() { return currentState_; }

    ///
    virtual double reward() = 0;

    ///
    virtual void perform(Idx) = 0;

    /// @}

    // ===========================================================================
    /// @name Variables
    // ===========================================================================
    /// @{

    virtual const DiscreteVariable* primeVar(const DiscreteVariable* mainVar) = 0;

    /// Iteration over the variables of the simulated probleme
    virtual SequenceIteratorSafe< const DiscreteVariable* > beginVariables() = 0;
    virtual SequenceIteratorSafe< const DiscreteVariable* > endVariables()   = 0;

    /// @}

    // ===========================================================================
    /// @name Actions
    // ===========================================================================
    /// @{

    virtual const std::string& actionName(Idx) = 0;

    /// Iteration over the variables of the simulated probleme
    virtual SequenceIteratorSafe< Idx > beginActions() = 0;
    virtual SequenceIteratorSafe< Idx > endActions()   = 0;


    /// @}

    /// Tha state in which the system currently is
    Instantiation currentState_, endState_;
  };

} /* namespace gum */


#endif   //  _GUM_ABSTRACT_SIMULATOR_H
