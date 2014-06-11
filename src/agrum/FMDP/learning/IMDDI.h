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

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_IMDDI_H
#define GUM_IMDDI_H
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/nodeDatabase.h>
// =========================================================================

namespace gum {

  /**
   * @class IMDDI IMDDI.h <agrum/FMDP/planning/IMDDI.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template<typename GUM_SCALAR>
  class IMDDI {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        IMDDI ( MultiDimDecisionGraph<GUM_SCALAR>* target , double learningThreshold );

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~IMDDI();

      /// @}

      // ==========================================================================
      /// @name Incrementals methods
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Adds a new observation to the structure
        // ###################################################################
        void addObservation ( const Observation<GUM_SCALAR>* );

        // ###################################################################
        /// Updates the tree after a new observation has been added
        // ###################################################################
        void updateOrderedTree();

      /// @}

  private :

      // ==========================================================================
      /// @name Updating methods
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
        void __updateNodeSet( Set<NodeId>&, const DiscreteVariable*, MultiPriorityQueue<const DiscreteVariable*, double, std::greater>& );

        // ###################################################################
        /// Turn the given node into a leaf if not already so
        // ###################################################################
        void __turnIntoLeaf(NodeId);

      /// @}

      /// The final diagram we're building
      MultiDimDecisionGraph<GUM_SCALAR>* __target;

      /// The intermediate ordered decision tree
      MultiDimDecisionGraph<GUM_SCALAR>* __intermediate;

      /// This hashtable binds every node to an associated NodeDatabase
      /// which handles every observation that concerns that node
      HashTable<NodeId, NodeDatabase<GUM_SCALAR>*> __nodeId2Database;

      /// The total number of observation added to this tree
      Idx __nbTotalObservation;

      /// The threshold above which we consider variables to be dependant
      double __dependenceThreshold;

  };

  extern template class SPUDDPlanning<float>;
  extern template class SPUDDPlanning<double>;
} /* namespace gum */


#include <agrum/FMDP/learning/IMDDI.tcc>

#endif // GUM_IMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

