/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief The base class for all GraphChangesGenerators for directed structures
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_DIGRAPH_H
#define GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_DIGRAPH_H

#include <agrum/agrum.h>
#include <agrum/tools/graphs/diGraph.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    class IGraphChangesGenerator4DiGraph {
      public:
      /// the iterator for parsing the list of possible graph change operators
      using iterator = typename Set< GraphChange >::const_iterator;

      /// the const iterator for parsing the list of graph change operators
      using const_iterator = iterator;

      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /// returns an (unsafe) iterator on the beginning of the list of operators
      virtual iterator begin() const = 0;

      /// returns an (unsafe) iterator on the end of the list of operators
      virtual const iterator& end() const = 0;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which the operator will compute possible changes
      virtual void setGraph(const DiGraph& graph) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const ArcAddition& change) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const ArcDeletion& change) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const ArcReversal& change) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const GraphChange& change) = 0;

      /// empty the set of possible change operators that can be applied
      virtual void clearChanges() noexcept = 0;

      /// notifies the generator that we have parsed all its legal changes
      virtual void notifyGetCompleted() = 0;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_DIGRAPH_H */
