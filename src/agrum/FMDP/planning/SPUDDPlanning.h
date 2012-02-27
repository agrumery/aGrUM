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
 * @brief Headers of the SPUDDPlanning class.
 *
 * @author Jean-Christophe Magnan
 */
// ============================================================================
// #define  TRACE_ALGO
// ============================================================================
#ifndef GUM_SPUDD_PLANNING_H
#define GUM_SPUDD_PLANNING_H
// ============================================================================
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
// ============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// ============================================================================

namespace gum {

  /**
   * @class SPUDDPlanning SPUDDPlanning.h <agrum/FMDP/planning/SPUDDPlanning.h>
   * @brief A class to find optimal policy for a given FMDP.
   *
   * Perform a SPUDD planning on given in parameter factored markov decision process
   *
   */

  template<typename T_DATA>

  class SPUDDPlanning {

    public:
      
      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

          /**
          * Default constructor
          */
          SPUDDPlanning ( FactoredMarkovDecisionProcess<T_DATA>* fmdp, T_DATA epsilon = 0.001 );

          /**
          * Default destructor
          */
          ~SPUDDPlanning();

      /// @}
      // ==========================================================================
      /// @name Planning Methods
      // ==========================================================================
      /// @{

          /**
          * Makes a spudd planning on FMDP
          */
          void makePlanning ();
          
          MultiDimDecisionDiagramBase< T_DATA >* makePlanningAlgoEvaluation( const std::string saveFilesName, Idx mode = 1 );
      /// @}
      
    private:
      
      /**
       * Evals the policy corresponding to the given value function
       */      
       std::pair< MultiDimDecisionDiagramBase< T_DATA >*, HashTable< NodeId, List<Idx>* >* > __evalPolicy( const MultiDimDecisionDiagramBase< T_DATA >* V );

      /**
       * Evals value function for a given action
       */
        MultiDimDecisionDiagramBase< T_DATA >* __evalActionValue( const MultiDimDecisionDiagramBase< T_DATA >* Vold );

      /**
       * Evals final value function by multiplying by discount reward and adding reward
       */
        MultiDimDecisionDiagramBase< T_DATA >* __addReward( const MultiDimDecisionDiagramBase< T_DATA >* Vold );
        
        
        
        MultiDimDecisionDiagramBase< T_DATA >* __evalActionValueAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode );
        MultiDimDecisionDiagramBase< T_DATA >* __addRewardAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode );
        std::pair<Idx,Idx> __evalNbRetrograde( const MultiDimDecisionDiagramBase<T_DATA>* t1, const MultiDimDecisionDiagramBase<T_DATA>* t2 );
        
        
    
      /// The Factored Markov Decision Process on which we do our planning (NB : this one must have decision diagram as transitions and reward functions )
      FactoredMarkovDecisionProcess<T_DATA>* __fmdp;
      
      /// List of action indexes which corresponds to best action for a given terminal node in value function diagram
      HashTable< NodeId, List<Idx>* > __bestPolicy; 
      
      /// The hyperparameter determining when we do stop value iteration.
      T_DATA __epsilon;
      
      /// RetroVar number save file
      std::ofstream __traceAlgoSaveFile;

  };
} /* namespace gum */

// ============================================================================
#include <agrum/FMDP/planning/SPUDDPlanning.tcc>
// ============================================================================
#endif // GUM_SPUDD_PLANNING_H
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;

