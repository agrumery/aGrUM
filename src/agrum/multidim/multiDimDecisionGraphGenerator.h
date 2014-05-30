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
 * @brief Headers of gum::MultiDimDecisionGraphGenerator.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_GENERATOR_H
#define GUM_MULTI_DIM_DECISION_GRAPH_GENERATOR_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// ============================================================================

namespace gum {
  /**
   * @class MultiDimDecisionGraphGenerator multiDimDecisionGraphGenerator.h <agrum/multidim/multiDimDecisionGraphGenerator.h>
   *
   * @brief Class implementing a decision graph generator with template type :double
   *
   * @ingroup multidim_group
   */
  class MultiDimDecisionGraphGenerator {

  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimDecisionGraphGenerator(Idx maxVar, Idx minVar, const Sequence<const DiscreteVariable *>& varSeq);

      // ============================================================================
      /// destructor.
      // ============================================================================
      ~MultiDimDecisionGraphGenerator();

    /// @}

      MultiDimDecisionGraph<double>* generate();



    private:

      bool __createLeaf(NodeId currentNodeId , HashTable<NodeId, Idx> &node2MinVar);
      Idx __generateVarPos(Idx offset, Idx span);


      Idx __minNbVarInDiagram;
      Idx __maxNbVarInDiagram;
      const Sequence<const DiscreteVariable*> __varSeq;
      Idx __nbTotalVar;
      static Idx __genSeed;
  };
} /* end of namespace */
#endif /* GUM_MULTI_DIM_DECISION_GRAPH_GENERATOR_H */
