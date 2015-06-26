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
 * @brief Headers of the RMax planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_RMAX_PLANER_H
#define GUM_RMAX_PLANER_H
// =========================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/learning/fmdpLearner.h>
#include <agrum/FMDP/planning/structuredPlaner.h>
#include <agrum/FMDP/simulation/statesCounter.h>
// =========================================================================

namespace gum {  

  /**
   * @class RMaxPlaner rMaxPlaner.h <agrum/FMDP/planning/rMaxPlaner.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a RMax planning on given in parameter factored markov decision process
   *
   */
  class RMaxPlaner : public StructuredPlaner<double>, public IDecisionStrategy {


      // ###################################################################
      /// @name
      // ###################################################################
      /// @{
    public :
        // ==========================================================================
        ///
        // ==========================================================================
        static RMaxPlaner* ReducedAndOrderedInstance(
                        const ILearningStrategy* learner,
                        double discountFactor = 0.9,
                        double epsilon = 0.00001)
              { return new RMaxPlaner( new MDDOperatorStrategy<double>(), discountFactor, epsilon, learner);}

        // ==========================================================================
        ///
        // ==========================================================================
        static RMaxPlaner* TreeInstance(
                        const ILearningStrategy* learner,
                        double discountFactor = 0.9,
                        double epsilon = 0.00001)
              { return new RMaxPlaner( new TreeOperatorStrategy<double>(), discountFactor, epsilon, learner);}

      /// @}

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{
    private:
        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        RMaxPlaner(IOperatorStrategy<double>* opi,
                    double discountFactor,
                    double epsilon,
                    const ILearningStrategy* learner);

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
    public:
        ~RMaxPlaner();

      /// @}



      // ###################################################################
      /// @name Planning Methods
      // ###################################################################
      /// @{

  public:

        // ==========================================================================
        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        // ==========================================================================
        void initialize( FMDP<double>* fmdp );


        // ==========================================================================
        /**
         * Performs a value iteration
         *
         * @param nbStep : enables you to specify how many value iterations you wish to do.
         * makePlanning will then stop whether when optimal value function is reach or when nbStep have been performed
         */
        // ==========================================================================
        void makePlanning(Idx nbStep = 1000000);

      /// @}



      // ###################################################################
      /// @name Value Iteration Methods
      // ###################################################################
      /// @{

    protected:        
        // ==========================================================================
        ///
        // ==========================================================================
        virtual void _initVFunction();

        // ==========================================================================
        /// Performs a single step of value iteration
        // ==========================================================================
        virtual MultiDimFunctionGraph< double >* _valueIteration();

      /// @}



      // ###################################################################
      /// @name Optimal policy extraction methods
      // ###################################################################
      /// @{

    protected:
        // ==========================================================================
        /// Perform the required tasks to extract an optimal policy
        // ==========================================================================
        virtual void _evalPolicy ();

      /// @}

    private :
      void __makeRMaxFunctionGraphs();

      std::pair<NodeId,NodeId> __visitLearner( const IVisitableGraphLearner*,
                                               NodeId currentNodeId,
                                               MultiDimFunctionGraph<double>*,
                                               MultiDimFunctionGraph<double>* );
      void __clearTables();

    private :

      HashTable<Idx, MultiDimFunctionGraph<double>*> __actionsRMaxTable;
      HashTable<Idx, MultiDimFunctionGraph<double>*> __actionsBoolTable;
      const ILearningStrategy* __fmdpLearner;

      double __rThreshold;
      double __rmax;


      // ###################################################################
      /// @name Incremental methods
      // ###################################################################
      /// @{
    public:
        void checkState( const Instantiation& newState, Idx actionId ){
          if( !__initializedTable[actionId] ){
            __counterTable[actionId]->reset(newState);
            __initializedTable[actionId] = true;
          } else
            __counterTable[actionId]->incState(newState);
        }

    private :
        HashTable<Idx, StatesCounter*> __counterTable;
        HashTable<Idx, bool> __initializedTable;

        bool __initialized;
      /// @}

  };

} /* namespace gum */


#include <agrum/FMDP/planning/rMaxPlaner.tcc>

#endif // GUM_RMAX_PLANER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

