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
 * @brief Headers of the SPUMDD planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPUMDD_H
#define GUM_SPUMDD_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/core/functors.h>
#include <agrum/core/argMaxSet.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// =========================================================================

namespace gum {  

  /**
   * @struct ArgumentMaximisesAction spumdd.h <agrum/FMDP/planning/spumdd.h>
   * @brief Argument Maximization function object class
   * @ingroup core
   *
   * Returns the set that has the maximal value between its two arguments sets
   */
  template <typename GUM_SCALAR>
  struct ArgumentMaximisesAction {

  // ###########################################################################
  /// @name Operator()
  // ###########################################################################
  /// @{

    const GUM_SCALAR& operator() (
        const GUM_SCALAR& x,
        const GUM_SCALAR& y) const {

       if( x > y ){
         return x;
       }
       if( x < y ){
         return y;
       }

       __temp = x;
       __temp += y;
       return __temp;
    }

  private :
    mutable GUM_SCALAR __temp;
  };


  /**
   * @class ActionSet spumdd.h <agrum/FMDP/planning/spumdd.h>
   * @brief A class to store the optimal actions.
   * @ingroup fmdp_group
   *
   * Stores the ids of optimal actions. To be used as leaves on optimal policy tree or decision graph
   *
   */
  class ActionSet {

  public :

    // ###########################################################################
    /// @name CNL
    // ###########################################################################
    /// @{

      // ============================================================================
      /// Constructor
      // ============================================================================
      ActionSet( ){
        GUM_CONSTRUCTOR(ActionSet)
        __actionSeq = new Sequence<Idx>();
      }

      ActionSet( const ActionSet& src ){
        GUM_CONSTRUCTOR(ActionSet)
        __actionSeq = new Sequence<Idx>();
        for( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
          __actionSeq->insert(*idi);
      }

      // ============================================================================
      /// Destructor
      // ============================================================================
      ~ActionSet(){
        GUM_DESTRUCTOR(ActionSet)
        delete __actionSeq;
      }

      // ============================================================================
      /// Allocators and Deallocators redefinition
      // ============================================================================
      void* operator new(size_t s){ return SmallObjectAllocator::instance().allocate(s);}
      void operator delete(void* p){ SmallObjectAllocator::instance().deallocate(p, sizeof(ActionSet));}

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

      // ============================================================================
      /// Iterator beginning
      // ============================================================================
      SequenceIteratorSafe<Idx> beginSafe() const { return __actionSeq->beginSafe(); }

      // ============================================================================
      /// Iterator end
      // ============================================================================
      SequenceIteratorSafe<Idx> endSafe() const { return __actionSeq->endSafe(); }

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

      // ============================================================================
      /// Ajout d'un élément
      // ============================================================================
      ActionSet& operator += ( const Idx& elem ) {
        __actionSeq->insert(elem);
        return *this;
      }

      // ============================================================================
      /// Use to insert the content of another set inside this one
      // ============================================================================
      ActionSet& operator += ( const ActionSet& src) {
        for( auto iter = src.beginSafe(); iter != src.endSafe(); ++iter )
          if( ! __actionSeq->exists(*iter) )
            __actionSeq->insert(*iter);
        return *this;
      }

      // ============================================================================
      /// Gives the ith element
      // ============================================================================
      const Idx& operator [] ( const Idx i ) const { return __actionSeq->atPos(i); }

      // ============================================================================
      /// Compares two ActionSet to check if they are equals
      // ============================================================================
      bool operator == ( const ActionSet& compared ) const {
        for( auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter )
          if( ! __actionSeq->exists(*iter) )
            return false;
        for( auto iter = this->beginSafe(); iter != this->endSafe(); ++iter )
          if( ! compared.exists(*iter) )
            return false;
        return true;
      }
      bool operator !=( const ActionSet& compared ) const {return !( *this == compared );}

    /// @}

      // ============================================================================
      /// Gives the size
      // ============================================================================
      Idx size() const { return __actionSeq->size(); }

      bool exists( const Idx& elem ) const { return __actionSeq->exists( elem ); }

    private :
      /// The very bone of the ActionSet
      Sequence<Idx>* __actionSeq;

      friend std::ostream& operator << (std::ostream& streamy, const ActionSet& objy ){
        streamy  << objy.__actionSeq->toString();
        return streamy;
      }
  };


  /**
   * @class SPUMDD spumdd.h <agrum/FMDP/planning/spumdd.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SPUDD planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class SPUMDD {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

       /**
        * Default constructor
        */
        SPUMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

       /**
        * Default destructor
        */
        ~SPUMDD();

      /// @}

      // ==========================================================================
      /// @name Miscelleanous methods
      // ==========================================================================
      /// @{

        /// Returns a const ptr on the Factored Markov Decision Process on which we're planning
        INLINE const FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp() { return __fmdp; }

        /// Returns a const ptr on the value function computed so far
        INLINE const MultiDimDecisionGraph<GUM_SCALAR>* vFunction() { return __vFunction; }

        /// Returns the best policy obtained so far
        INLINE const MultiDimDecisionGraph<ActionSet, SetTerminalNodePolicy>* optimalPolicy() { return __optimalPolicy; }

      /// @}

      // ==========================================================================
      /// @name Planning Methods
      // ==========================================================================
      /// @{

        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        void initialize();

        /**
         * Performs a value iteration
         *
         * @param nbStep : enables you to specify how many value iterations you wish to do.
         * makePlanning will then stop whether when optimal value function is reach or when nbStep have been performed
         */
        void makePlanning(Idx nbStep = 1000000);

  private:

        /// Performs a single step of value iteration
        MultiDimDecisionGraph< GUM_SCALAR >* __valueIteration();

  public:

        /// Add computed Qaction to table
        void addQaction( MultiDimDecisionGraph<GUM_SCALAR>* );
        void addQaction( MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* );

        /**
         * Returns an iterator reference to the beginning of the var eleminstation sequence
         * @warning in reverse mode (from end to beginning)
         */
        INLINE SequenceIteratorSafe<const DiscreteVariable*> beginVarElimination() { return __elVarSeq.rbeginSafe(); }

        /**
         * Returns an iterator to the end
         * @warning in reverse mode (from end to beginning)
         */
        INLINE SequenceIteratorSafe<const DiscreteVariable*> endVarElemination() {return __elVarSeq.rendSafe();}

  private:

        /**
         * Terminates a value iteration by multiplying by discount factor the so far computed value function.
         * Then adds reward function
         */
        MultiDimDecisionGraph< GUM_SCALAR >* __addReward ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold );

      /// @}

      // ==========================================================================
      /// @name Optimal policy evaluation methods
      // ==========================================================================
      /// @{

        /**
         * Once value ieration is over, this methods is call to find an associated optimal policy
         */
        void __evalPolicy ();

       /**
        * Creates a copy of given in parameter decision diagram and replaces leaves of that diagram by a pair containing value of the leaf and
        * action to which is bind this diagram (given in parameter).
        */
        MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* __createArgMaxCopy (
            const MultiDimDecisionGraph<GUM_SCALAR>* Vaction,
            Idx actionId );

       /**
        * Once final V is computed upon arg max on last Vactions, this function creates a diagram where all leaves tied to the same action are merged together.
        * Since this function is a recursive one to ease the merge of all identic nodes to converge toward a cannonical policy, a factory and the current node are needed to build
        * resulting diagram. Also we need an exploration table to avoid exploration of already visited sub-graph part.
        */
       void __extractOptimalPolicy ( const MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* optimalValueFunction );
       void __transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>&, ActionSet&);


      /// @}

      /// The Factored Markov Decision Process describing our planning situation
      /// (NB : this one must have decision graph as transitions and reward functions )
      const FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;

      /// The Value Function computed iteratively
      MultiDimDecisionGraph<GUM_SCALAR>* __vFunction;

      /// The associated optimal policy
      /// @warning This decision graph has Idx as leaf.
      /// Indeed, its leaves are a match to the fmdp action ids
      MultiDimDecisionGraph< ActionSet, SetTerminalNodePolicy >* __optimalPolicy;

      /// A table giving for every action the associated Q function
      std::vector<MultiDimDecisionGraph<GUM_SCALAR>*> __qFunctionSet;
      std::vector<MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >*> __argMaxQFunctionSet;

      /// A locker on the above table for multitreading purposes
      std::mutex __qSetMutex;

      /// The threshold value
      /// Whenever | Vn - Vn+1 | < threshold, we consider that V ~ V*
      GUM_SCALAR __threshold;

      /// A sequence to eleminate primed variables
      Sequence< const DiscreteVariable* > __elVarSeq;

      bool __firstTime;
  };

  template<typename GUM_SCALAR>
  MultiDimDecisionGraph<GUM_SCALAR>*
  evalQaction( SPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph<GUM_SCALAR>* Vold, Idx actionId );

//  template<typename GUM_SCALAR>
//  void evalQactionForArgMax( SPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph<GUM_SCALAR>* Vold, Idx actionId );

  extern template class SPUMDD<float>;
  extern template class SPUMDD<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/spumdd.tcc>

#endif // GUM_SPUMDD_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

