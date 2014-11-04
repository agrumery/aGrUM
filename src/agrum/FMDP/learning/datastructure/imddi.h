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
 * @brief Headers of the IMDDI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_IMDDI_H
#define GUM_IMDDI_H
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
   * @class IMDDI imddi.h <agrum/FMDP/planning/decisionGraph/imddi.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template <TESTNAME AttributeSelection, bool isScalar = false >
  class IMDDI : public IncrementalGraphLearner<AttributeSelection, isScalar>{

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Variable Learner constructor
        // ###################################################################
        IMDDI ( MultiDimDecisionGraph<double>* target,
                double attributeSelectionThreshold,
                double pairSelectionThreshold,
                Set<const DiscreteVariable*> attributeListe,
                const DiscreteVariable* learnedValue ) ;

        // ###################################################################
        /// Reward Learner constructor
        // ###################################################################
        IMDDI ( MultiDimDecisionGraph<double>* target,
                double attributeSelectionThreshold,
                double pairSelectionThreshold,
                Set<const DiscreteVariable*> attributeListe );

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~IMDDI(){GUM_DESTRUCTOR(IMDDI);}

      /// @}

      // ==========================================================================
      /// @name Incrementals methods
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Adds a new observation to the structure
        // ###################################################################
        void addObservation ( const Observation* );

        // ###################################################################
        /// Updates the tree after a new observation has been added
        // ###################################################################
        void updateGraph();

      /// @}

  private :

      // ==========================================================================
      /// @name Updating private methods
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Computes the score of the given variables for the given node
        // ###################################################################
        double __score( const DiscreteVariable*, NodeId );

        // ###################################################################
        /// For each node in the given set, this methods checks whether or not
        /// we should installed the given variable as a test.
        /// If so, the node is updated
        // ###################################################################
        void __updateNodeSet( Set<NodeId>&, const DiscreteVariable*,
                              MultiPriorityQueue<const DiscreteVariable*,
                              double,
                              std::greater<double>>& );

  public :

        // ###################################################################
        ///
        // ###################################################################
        void updateDecisionGraph();

  private :

        // ###################################################################
        ///
        // ###################################################################
        void __mergeLeaves( HashTable<NodeId, NodeId>& toTarget, bool rewardLeaf );

        double __evalPair( double* feffectif, double ftotal, double* seffectif, double stotal );

        /// @}

      Sequence<const DiscreteVariable*> __varOrder;

      /// The total number of observation added to this tree
      Idx __nbTotalObservation;

      /// The threshold above which we consider variables to be dependant
      double __attributeSelectionThreshold;

      /// The threshold above which two leaves does not share the same probability distribution
      double __pairSelectionThreshold;
  };


} /* namespace gum */



#endif // GUM_IMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

