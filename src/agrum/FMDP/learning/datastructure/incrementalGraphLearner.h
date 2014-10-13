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
 * @brief Headers of the interface specifying functions to be implemented by any incremental learner.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_INCREMENTAL_GRAPH_LEARNER_H
#define GUM_INCREMENTAL_GRAPH_LEARNER_H
// =========================================================================
#include <agrum/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>
// =========================================================================

namespace gum {

  template < int v >
  struct Int2Type {
    enum { value = v };
  };

  /**
   * @class IncrementalGraphLearner incrementalGraphLearner.h <agrum/FMDP/learning/datastructure/incrementalGraphLearner>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */
  template < bool isScalar = false >
  class IncrementalGraphLearner {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        IncrementalGraphLearner ( MultiDimDecisionGraph<double>* target,
                             Set<const DiscreteVariable*> ,
                             const DiscreteVariable*);

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~IncrementalGraphLearner();

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
        virtual void updateGraph() = 0;

      /// @}

  protected :

      // ==========================================================================
      /// @name Update protected methods
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Turn the given node into a leaf if not already so
        // ###################################################################
        void _convertNode2Leaf( NodeId );

        // ###################################################################
        /// Install given variable to the given node, ensuring that the variable
        /// is not present in its subtree
        // ###################################################################
        void _transpose( NodeId, const DiscreteVariable* );

  public :

        // ###################################################################
        ///
        // ###################################################################
        virtual void toDG();

        // ###################################################################
        ///
        // ###################################################################
        Size size(){ return _nodeVarMap.size(); }

  protected :

    /// @}

        /// For debuggibg purposes only
        void _showMap();

      // ==========================================================================
      /// @name Model handling datastructures
      // ==========================================================================
      /// @{

        // ###################################################################
        /// The source of nodeId
        // ###################################################################
        NodeGraphPart _model;

        // ###################################################################
        /// The root of the ordered tree
        // ###################################################################
        NodeId _root;

        // ###################################################################
        /// Gives for any node its associated variable
        // ###################################################################
        HashTable<NodeId, const DiscreteVariable*> _nodeVarMap;

        // ###################################################################
        /// A table giving for any node a table mapping to its son
        /// idx is the modality of associated variable
        // ###################################################################
        HashTable<NodeId, NodeId*> _nodeSonsMap;

        // ###################################################################
        /// Associates to any variable the list of all nodes associated to
        /// this variable
        // ###################################################################
        HashTable<const DiscreteVariable*, List<NodeId>*> _var2Node;

        // ###################################################################
        /// This hashtable binds every node to an associated NodeDatabase
        /// which handles every observation that concerns that node
        // ###################################################################
        HashTable<NodeId, NodeDatabase*> _nodeId2Database;

      /// @}


      /// The final diagram we're building
      MultiDimDecisionGraph<double>* _target;

      Set<const DiscreteVariable*> _setOfVars;
      const DiscreteVariable* _value;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.tcc>

#endif // GUM_INCREMENTAL_GRAPH_LEARNER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

