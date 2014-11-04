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
 * @brief Headers of the ITI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_ITI_H
#define GUM_ITI_H
// =========================================================================
#include <agrum/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
// =========================================================================
#include <agrum/variables/labelizedVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class ITI iti.h <agrum/FMDP/planning/decisionGraph/iti.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template <TESTNAME AttributeSelection, bool isScalar = false >
  class ITI : public IncrementalGraphLearner<AttributeSelection, isScalar>{

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Variable Learner constructor
        // ###################################################################
        ITI ( MultiDimDecisionGraph<double>* target,
                double attributeSelectionThreshold,
                Set<const DiscreteVariable*> attributeListe,
                const DiscreteVariable* learnedValue );

        // ###################################################################
        /// Reward Learner constructor
        // ###################################################################
        ITI ( MultiDimDecisionGraph<double>* target,
                double attributeSelectionThreshold,
                Set<const DiscreteVariable*> attributeListe );

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~ITI(){GUM_DESTRUCTOR(ITI);}

      /// @}

      // ==========================================================================
      /// @name Incrementals methods
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Adds a new observation to the structure
        // ###################################################################
        void addObservation ( const Observation* );

    protected :
        void __updateNodeWithObservation( const Observation* newObs, NodeId currentNodeId );


    public :

        // ###################################################################
        /// Updates the tree after a new observation has been added
        // ###################################################################
        void updateGraph();

    protected :
        void _insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                                  const DiscreteVariable* boundVar,
                                  NodeId* sonsMap );

        void _chgNodeBoundVar( NodeId chgedNodeId, const DiscreteVariable* desiredVar );

        void _removeNode( NodeId removedNodeId );

      /// @}

    public :

        // ###################################################################
        ///
        // ###################################################################
        void updateDecisionGraph();

      HashTable<NodeId, bool> __staleTable;

      /// The total number of observation added to this tree
      Idx __nbTotalObservation;

      /// The threshold above which we consider variables to be dependant
      double __attributeSelectionThreshold;
  };


} /* namespace gum */



#endif // GUM_ITI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

