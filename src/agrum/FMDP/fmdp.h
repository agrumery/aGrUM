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
 * @brief Class for implementation of factored markov decision process.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_FACTORED_MARKOV_DECISION_PROCESS_H
#define GUM_FACTORED_MARKOV_DECISION_PROCESS_H
//======================================================================
#include <string>
#include <vector>
//======================================================================
//======================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
//======================================================================

namespace gum {
  /**
   * @class FMDP
   * @brief This class is used to implement factored decision process.
   * @ingroup fmdp_group
   *
   * This class supports a mechanism which allows user to give, for each
   * variable ivolved in the process, a default transition probability table
   * and to describe for specific actions a different table.
   *
   */

  template < typename GUM_SCALAR >
  class FMDP {
    template < typename GUM_SCALAR_O >
    using VarTransitionTable
       = HashTable< const DiscreteVariable*, const MultiDimImplementation< GUM_SCALAR_O >* >;

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    FMDP(bool onDestructionDeleteVar = false);

    /**
     * Default destructor.
     */
    ~FMDP();

    /// @}

    // ===========================================================================
    /// @name Variable Handling Methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a variable to FMDP description
     * @throw DuplicateElement if a similar variable already exists
     */
    void addVariable(const DiscreteVariable* var);

    /**
     * Returns an iterator reference to he beginning of the list of variables
     */
    SequenceIteratorSafe< const DiscreteVariable* > beginVariables() const {
      return _varSeq_.beginSafe();
    }

    /**
     * Returns an iterator reference to the end of the list of variables
     */
    SequenceIteratorSafe< const DiscreteVariable* > endVariables() const {
      return _varSeq_.endSafe();
    }

    /**
     * Returns the primed variable associate to the given main variable
     */
    const DiscreteVariable* main2prime(const DiscreteVariable* mainVar) const {
      return _main2primed_.second(mainVar);
    }

    /**
     * Returns the map binding main variables and prime variables
     */
    INLINE const Bijection< const DiscreteVariable*, const DiscreteVariable* >&
                 mapMainPrime() const {
      return _main2primed_;
    }

    /// @}

    // ===========================================================================
    /// @name Action Handling Methods.
    // ===========================================================================
    /// @{

    /**
     * Adds an action to FMDP description
     * @throw DuplicateElement if an action with same name already exists
     */
    void addAction(Idx actionId, const std::string& action);

    /**
     * Returns an iterator reference to he beginning of the list of actions
     */
    SequenceIteratorSafe< Idx > beginActions() const { return _actionSeq_.beginSafe(); }

    /**
     * Returns an iterator reference to the end of the list of actions
     */
    SequenceIteratorSafe< Idx > endActions() const { return _actionSeq_.endSafe(); }

    /// @}

    // ===========================================================================
    /// @name Transition methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a variable transition table to specified action
     * @throw NotFound if action or var does not exists
     * @throw DuplicateElement if variable already has a transition for this
     * action
     */
    void addTransitionForAction(Idx                                         actionId,
                                const DiscreteVariable*                     var,
                                const MultiDimImplementation< GUM_SCALAR >* transition);

    /**
     * Adds a default variable transition
     * @throw NotFound if var does not exists
     * @throw DuplicateElement if variable already has a default transition
     */
    void addTransition(const DiscreteVariable*                     var,
                       const MultiDimImplementation< GUM_SCALAR >* transition) {
      this->addTransitionForAction(0, var, transition);
    }

    /**
     * Returns transition associated to given in parameter variable and the
     * given action
     */
    const MultiDimImplementation< GUM_SCALAR >* transition(Idx                     actionId,
                                                           const DiscreteVariable* v) const;

    /// @}

    // ===========================================================================
    /// @name Cost methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a cost table to specified action
     * @throw NotFound if action does not exists
     */
    void addCostForAction(Idx actionId, const MultiDimImplementation< GUM_SCALAR >* cost);

    /**
     * Adds a default variable cost
     * @throw DuplicateElement if a default cost exists already
     */
    void addCost(const MultiDimImplementation< GUM_SCALAR >* cost) {
      this->addCostForAction(0, cost);
    }

    /**
     * Returns the reward table of mdp
     */
    const MultiDimImplementation< GUM_SCALAR >* cost(Idx actionId = 0) const;

    /// @}

    // ===========================================================================
    /// @name Reward methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a default variable reward
     * @throw DuplicateElement if a default reward exists already
     */
    void addRewardForAction(Idx actionId, const MultiDimImplementation< GUM_SCALAR >* reward);

    /**
     * Adds a default variable reward
     * @throw DuplicateElement if a default reward exists already
     */
    void addReward(const MultiDimImplementation< GUM_SCALAR >* reward) {
      this->addRewardForAction(0, reward);
    }

    /**
     * Returns the reward table of mdp
     */
    const MultiDimImplementation< GUM_SCALAR >* reward(Idx actionId = 0) const;

    /// @}

    // ===========================================================================
    /// @name FMPD miscelleanous methods.
    // ===========================================================================
    /// @{

    /**
     * Returns name of action given in parameter
     */
    const std::string& actionName(Idx actionId) const;

    /**
     * Returns action id
     */
    Idx actionId(const std::string&) const;

    /**
     * Displays the FMDP in a Dot format
     */
    std::string toString() const;

    /**
     * Returns the map binding main variables and prime variables
     */
    Size size() const;


    /// @}

    private:
    /// Sequence de variables and its iterator
    Sequence< const DiscreteVariable* > _varSeq_;

    Sequence< Idx > _actionSeq_;

    /// Mapping from a main variable to its associated primed version
    Bijection< const DiscreteVariable*, const DiscreteVariable* > _main2primed_;

    /// Bijection mapping an action name to its id
    Bijection< Idx, const std::string* > _actionMap_;

    /// Table which give for each action a table containing variables transition
    /// cpt
    HashTable< Idx, VarTransitionTable< GUM_SCALAR >* > _actionTransitionTable_;

    /// Table which give for each action cost table
    HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* > _actionCostTable_;

    /// Table which give for each action reward table
    HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* > _actionRewardTable_;

    /// Boolean indicates whether or not main variables should be deleted on
    /// destruction of this instance
    /// Usually the case when fmdp has been initialized with the factory
    bool _onDestructionDeleteVars_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class FMDP< double >;
#endif


} /* namespace gum */


#include <agrum/FMDP/fmdp_tpl.h>

#endif   // GUM_FACTORED_MARKOV_DECISION_PROCESS_H
