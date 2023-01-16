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
#ifndef GUM_FMDP_SIMULATOR_H
#define GUM_FMDP_SIMULATOR_H
//======================================================================
//======================================================================
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
  class FMDPSimulator: public AbstractSimulator {
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
    double reward() { return _fmdp_->reward()->get(this->currentState_); }

    void perform(Idx);

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    const DiscreteVariable* primeVar(const DiscreteVariable* mainVar) {
      return _fmdp_->main2prime(mainVar);
    }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< const DiscreteVariable* > beginVariables() {
      return _fmdp_->beginVariables();
    }
    SequenceIteratorSafe< const DiscreteVariable* > endVariables() {
      return _fmdp_->endVariables();
    }

    /// @}

    // ===========================================================================
    /// @name
    // ===========================================================================
    /// @{

    virtual const std::string& actionName(Idx actionId) { return _fmdp_->actionName(actionId); }

    /// Iteration over the variables of the simulated probleme
    SequenceIteratorSafe< Idx > beginActions() { return _fmdp_->beginActions(); }
    SequenceIteratorSafe< Idx > endActions() { return _fmdp_->endActions(); }
    /// @}

    protected:
    virtual double transitionProbability_(const DiscreteVariable* var,
                                          const Instantiation&    transit,
                                          Idx                     actionId) {
      return reinterpret_cast< const MultiDimFunctionGraph< double >* >(
                _fmdp_->transition(actionId, var))
         ->get(transit);
    }

    private:
    /// The Factored Markov Decision Process that describes how the system
    /// evolves
    FMDP< double >* _fmdp_;

    /// Just to know if it should be deleted in the end
    const bool _loaded_;
  };

} /* namespace gum */


#endif   // GUM_FMDP_SIMULATOR_H
