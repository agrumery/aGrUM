
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_FMDP_SIMULATOR_H
#define GUM_FMDP_SIMULATOR_H
//======================================================================
#include <agrum/multidim/instantiation.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/simulation/abstractSimulator.h>
//======================================================================

namespace gum {
  /**
   * @class FMDPSimulator fmdpSimulator.h
   * <agrum/FMDP/simulation/fmdpSimulator.h>
   * @brief A class to simulate a Factored Markov Decision Process.
   * @ingroup fmdp_group
   *
   *
   *
   */
  class FMDPSimulator : public AbstractSimulator {
    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    FMDPSimulator(const FMDP< double >* fmdp);
    FMDPSimulator(const std::string& resource);

    /**
     * Default destructor.
     */
    ~FMDPSimulator();

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    ///
    double reward() { return __fmdp->reward()->get(this->_currentState); }

    void perform(Idx);

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    const DiscreteVariable* primeVar(const DiscreteVariable* mainVar) {
      return __fmdp->main2prime(mainVar);
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< const DiscreteVariable* > beginVariables() {
      return __fmdp->beginVariables();
    }
    SequenceIteratorSafe< const DiscreteVariable* > endVariables() {
      return __fmdp->endVariables();
    }

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    virtual const std::string& actionName(Idx actionId) {
      return __fmdp->actionName(actionId);
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< Idx > beginActions() { return __fmdp->beginActions(); }
    SequenceIteratorSafe< Idx > endActions() { return __fmdp->endActions(); }
    /// @}

    protected:
    virtual double _transitionProbability(const DiscreteVariable* var,
                                          const Instantiation&    transit,
                                          Idx                     actionId) {
      return reinterpret_cast< const MultiDimFunctionGraph< double >* >(
                __fmdp->transition(actionId, var))
         ->get(transit);
    }

    private:
    /// The Factored Markov Decision Process that describes how the system
    /// evolves
    FMDP< double >* __fmdp;

    /// Just to know if it should be deleted in the end
    const bool __loaded;
  };

} /* namespace gum */


#endif   // GUM_FMDP_SIMULATOR_H
