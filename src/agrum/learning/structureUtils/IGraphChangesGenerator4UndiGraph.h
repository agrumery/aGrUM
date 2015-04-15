/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The base class for all GraphChangesGenerators for undirected structures
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H
#define GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H

#include <agrum/config.h>
#include <agrum/core/set.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    class IGraphChangesGenerator4UndiGraph {
      public:
      /// the iterator for parsing the list of possible graph change operators
      using iterator = typename Set<GraphChange>::const_iterator;

      /// the const iterator for parsing the list of graph change operators
      using const_iterator = iterator;

      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /// returns an (unsafe) iterator on the beginning of the list of operators
      virtual iterator begin() const = 0;

      /// returns an (unsafe) iterator on the end of the list of operators
      virtual const iterator &end() const = 0;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which the operator will compute possible changes
      virtual void setGraph(const UndiGraph &graph) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const EdgeAddition &change) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const EdgeDeletion &change) = 0;

      /// notify the operator set of a change applied to the graph
      virtual void modifyGraph(const GraphChange &change) = 0;

      /// empty the set of possible change operators that can be applied
      virtual void clearChanges() noexcept = 0;

      /// notifies the generator that we have parsed all its legal changes
      virtual void notifyGetCompleted() = 0;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_IGRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H */
