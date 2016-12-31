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
 * @brief Class for implementation of factored markov decision process.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_FACTORED_MARKOV_DECISION_PROCESS_H
#define GUM_FACTORED_MARKOV_DECISION_PROCESS_H
//======================================================================
#include <string>
#include <vector>
//======================================================================
#include <agrum/core/bijection.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
//======================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/variables/discreteVariable.h>
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

  template <typename GUM_SCALAR>
  class FMDP {

    template <typename GUM_SCALAR_O>
    using VarTransitionTable =
        HashTable<const DiscreteVariable*,
                  const MultiDimImplementation<GUM_SCALAR_O>*>;

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    FMDP( bool onDestructionDeleteVar = false );

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
    void addVariable( const DiscreteVariable* var );

    /**
     * Returns an iterator reference to he beginning of the list of variables
     */
    SequenceIteratorSafe<const DiscreteVariable*> beginVariables() const {
      return __varSeq.beginSafe();
    }

    /**
     * Returns an iterator reference to the end of the list of variables
     */
    SequenceIteratorSafe<const DiscreteVariable*> endVariables() const {
      return __varSeq.endSafe();
    }

    /**
     * Returns the primed variable associate to the given main variable
     */
    const DiscreteVariable* main2prime( const DiscreteVariable* mainVar ) const {
      return __main2primed.second( mainVar );
    }

    /**
     * Returns the map binding main variables and prime variables
     */
    INLINE const Bijection<const DiscreteVariable*, const DiscreteVariable*>&
                 mapMainPrime() const {
      return __main2primed;
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
    void addAction( Idx actionId, const std::string& action );

    /**
     * Returns an iterator reference to he beginning of the list of actions
     */
    SequenceIteratorSafe<Idx> beginActions() const {
      return __actionSeq.beginSafe();
    }

    /**
     * Returns an iterator reference to the end of the list of actions
     */
    SequenceIteratorSafe<Idx> endActions() const { return __actionSeq.endSafe(); }

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
    void
    addTransitionForAction( Idx                                       actionId,
                            const DiscreteVariable*                   var,
                            const MultiDimImplementation<GUM_SCALAR>* transition );

    /**
     * Adds a default variable transition
     * @throw NotFound if var does not exists
     * @throw DuplicateElement if variable already has a default transition
     */
    void addTransition( const DiscreteVariable*                   var,
                        const MultiDimImplementation<GUM_SCALAR>* transition ) {
      this->addTransitionForAction( 0, var, transition );
    }

    /**
     * Returns transition associated to given in parameter variable and the
     * given action
     */
    const MultiDimImplementation<GUM_SCALAR>*
    transition( Idx actionId, const DiscreteVariable* v ) const;

    /// @}

    // ===========================================================================
    /// @name Cost methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a cost table to specified action
     * @throw NotFound if action does not exists
     */
    void addCostForAction( Idx                                       actionId,
                           const MultiDimImplementation<GUM_SCALAR>* cost );

    /**
     * Adds a default variable cost
     * @throw DuplicateElement if a default cost exists already
     */
    void addCost( const MultiDimImplementation<GUM_SCALAR>* cost ) {
      this->addCostForAction( 0, cost );
    }

    /**
     * Returns the reward table of mdp
     */
    const MultiDimImplementation<GUM_SCALAR>* cost( Idx actionId = 0 ) const;

    /// @}

    // ===========================================================================
    /// @name Reward methods.
    // ===========================================================================
    /// @{

    /**
     * Adds a default variable reward
     * @throw DuplicateElement if a default reward exists already
     */
    void addRewardForAction( Idx                                       actionId,
                             const MultiDimImplementation<GUM_SCALAR>* reward );

    /**
     * Adds a default variable reward
     * @throw DuplicateElement if a default reward exists already
     */
    void addReward( const MultiDimImplementation<GUM_SCALAR>* reward ) {
      this->addRewardForAction( 0, reward );
    }

    /**
     * Returns the reward table of mdp
     */
    const MultiDimImplementation<GUM_SCALAR>* reward( Idx actionId = 0 ) const;

    /// @}

    // ===========================================================================
    /// @name FMPD miscelleanous methods.
    // ===========================================================================
    /// @{

    /**
     * Returns name of action given in parameter
     */
    const std::string& actionName( Idx actionId ) const;

    /**
      * Returns action id
      */
    Idx actionId( const std::string& ) const;

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
    Sequence<const DiscreteVariable*> __varSeq;

    Sequence<Idx> __actionSeq;

    /// Mapping from a main variable to its associated primed version
    Bijection<const DiscreteVariable*, const DiscreteVariable*> __main2primed;

    /// Bijection mapping an action name to its id
    Bijection<Idx, const std::string*> __actionMap;

    /// Table which give for each action a table containing variables transition
    /// cpt
    HashTable<Idx, VarTransitionTable<GUM_SCALAR>*> __actionTransitionTable;

    /// Table which give for each action cost table
    HashTable<Idx, const MultiDimImplementation<GUM_SCALAR>*> __actionCostTable;

    /// Table which give for each action reward table
    HashTable<Idx, const MultiDimImplementation<GUM_SCALAR>*> __actionRewardTable;

    /// Boolean indicates whether or not main variables should be deleted on
    /// destruction of this instance
    /// Usually the case when fmdp has been initialized with the factory
    bool __onDestructionDeleteVars;
  };


  extern template class FMDP<float>;
  extern template class FMDP<double>;


} /* namespace gum */


#include <agrum/FMDP/fmdp_tpl.h>

#endif  // GUM_FACTORED_MARKOV_DECISION_PROCESS_H
