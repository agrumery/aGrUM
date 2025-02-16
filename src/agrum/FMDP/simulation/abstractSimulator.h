/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

    void setInitialStateRandomly();

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
