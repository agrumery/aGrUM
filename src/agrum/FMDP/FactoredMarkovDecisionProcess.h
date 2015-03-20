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
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
//======================================================================

namespace gum {
  /**
   * @class FactoredMarkovDecisionProcess
   * @brief This class is used to implement factored decision process.
   * @ingroup fmdp_group
   *
   * This class supports a mechanism which allows user to give, for each
   * variable ivolved in the process, a default transition probability table
   * and to describe for specific actions a different table.
   *
   */

  template<typename GUM_SCALAR>
  class FactoredMarkovDecisionProcess {

    public:

      // ===========================================================================
      /// @name Constructors, Destructors.
      // ===========================================================================
      /// @{

        /**
         * Default constructor.
         */
        FactoredMarkovDecisionProcess(bool onDestructionDeleteVar = false);

        /**
         * Default destructor.
         */
        ~FactoredMarkovDecisionProcess();

      /// @}

      // ===========================================================================
      /// @name FMDP instantiation methods.
      // ===========================================================================
      /// @{

        /**
         * Adds a variable to FMDP description
         * @throw DuplicateElement if a similar variable already exists
         */
        void addVariable ( const DiscreteVariable* var );

        /**
         * Adds an action to FMDP description
         * @throw DuplicateElement if an action with same name already exists
         */
        void addAction ( Idx actionId, const std::string& action );

        /**
         * Adds a variable transition table to specified action
         * @throw NotFound if action or var does not exists
         * @throw DuplicateElement if variable already has a transition for this action
         */
        void addTransitionForAction(const std::string& action ,
                                    const DiscreteVariable* var,
                                    const MultiDimImplementation<GUM_SCALAR>* transition);

        /**
         * Adds a default variable transition
         * @throw NotFound if var does not exists
         * @throw DuplicateElement if variable already has a default transition
         */
        void addTransition ( const DiscreteVariable* var, const MultiDimImplementation<GUM_SCALAR>* transition );

        /**
         * Adds a cost table to specified action
         * @throw NotFound if action does not exists
         */
        void addCostForAction ( const std::string& action, const MultiDimImplementation<GUM_SCALAR>* cost  );

        /**
         * Adds a default variable cost
         * @throw DuplicateElement if a default cost exists already
         */
        void addCost ( const MultiDimImplementation<GUM_SCALAR>* cost );

        /**
         * Adds a default variable reward
         * @throw DuplicateElement if a default reward exists already
         */
        void addReward ( const MultiDimImplementation<GUM_SCALAR>* reward );

        /**
         * Precises the discount factor for that mdp
         */
        void setDiscount ( GUM_SCALAR discount );

      /// @}

      // ===========================================================================
      /// @name FMPD manipulation methods.
      // ===========================================================================
      /// @{

        /**
         * Returns the discount factor of mdp
         */
        const GUM_SCALAR discount( ) const;

        /**
         * Returns the reward table of mdp
         */
        const MultiDimImplementation< GUM_SCALAR >* reward( ) const;

        /**
         * Returns name of action given in parameter
         */
        const std::string& actionName ( Idx actionId ) const;


      /// @}

      // ===========================================================================
      /// @name Action Iterator Handling Methods.
      // ===========================================================================
      /// @{

        /**
         * Returns an iterator reference to he beginning of the list of actions
         */
        SequenceIteratorSafe<Idx> beginActions() const {return __actionSeq.beginSafe(); }

        /**
         * Returns an iterator reference to the end of the list of actions
         */
        SequenceIteratorSafe<Idx> endActions() const { return __actionSeq.endSafe(); }

      /// @}

      // ===========================================================================
      /// @name Variable Iterator Handling Methods.
      // ===========================================================================
      /// @{

        /**
         * Returns an iterator reference to he beginning of the list of variables
         */
        SequenceIteratorSafe< const DiscreteVariable* > beginVariables() const {return __varSeq.beginSafe(); }

        /**
         * Returns an iterator reference to the end of the list of variables
         */
        SequenceIteratorSafe< const DiscreteVariable* > endVariables() const { return __varSeq.endSafe(); }

      /// @}

      /**
       * Returns transition associated to given in parameter variable and the given action
       */
      const MultiDimImplementation< GUM_SCALAR >* transition ( Idx actionId, const DiscreteVariable* v ) const;

      /**
       * Returns set of the main variables
       */
      Set< const DiscreteVariable* > variables() const;

      /**
       * Returns the primed variable associate to the given main variable
       */
      const DiscreteVariable* main2prime(const DiscreteVariable*) const;

      /**
       * Returns the map binding main variables and prime variables
       */
      INLINE const Bijection<const DiscreteVariable*, const DiscreteVariable*>& mapMainPrime() const { return __main2primed; }

      /// @}

      /**
       * Displays the FMDP in a Dot format
       */
      std::string toString() const;

      /**
       * Returns the map binding main variables and prime variables
       */
      Size size() const;

    private :

      /**
        * Returns action id
        */
      const Idx& __actionId( const std::string& ) const;

      /// Sequence de variables and its iterator
      Sequence< const DiscreteVariable* > __varSeq;

      Sequence<Idx> __actionSeq;

      /// Mapping from a main variable to its associated primed version
      Bijection< const DiscreteVariable*, const DiscreteVariable*> __main2primed;

      /// Bijection mapping an action name to its id
      Bijection< Idx, const std::string* > __actionMap;

      /// Table which give for each action a table containing variables transition cpt
      HashTable< Idx, HashTable< const DiscreteVariable*, const MultiDimImplementation< GUM_SCALAR >* >* > __actionTransitionTable;

      /// Table which give for each action cost table
      HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* > __actionCostTable;

      /// Variable default transition cpt table
      HashTable< const DiscreteVariable*, const MultiDimImplementation< GUM_SCALAR >* > __defaultTransitionTable;

      /// default cost table
      const MultiDimImplementation< GUM_SCALAR >* __defaultCostTable;

      /// default reward table
      const MultiDimImplementation< GUM_SCALAR >* __defaultRewardTable;

      /// FMDP discount factor
      GUM_SCALAR __discount;

      /// Boolean indicates whether or not main variables should be deleted on destruction of this instance
      /// Usually the case when fmdp has been initialized with the factory
      bool __onDestructionDeleteVars;
  };

//  extern template class FactoredMarkovDecisionProcess<float>;
//  extern template class FactoredMarkovDecisionProcess<double>;

} /* namespace gum */


#include <agrum/FMDP/FactoredMarkovDecisionProcess.tcc>

#endif // GUM_FACTORED_MARKOV_DECISION_PROCESS_H

