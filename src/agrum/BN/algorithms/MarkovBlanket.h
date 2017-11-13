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
 * @brief Class building the markovBlanket from a DAGmodel and a node name
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#ifndef GUM_MARKOVBLANKET_H
#define GUM_MARKOVBLANKET_H

#include <agrum/graphicalModels/DAGmodel.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/graphElements.h>

namespace gum {

  /**
   * @class MarkovBlanket MarkovBlanket.h <agrum/BN/algorithms/MarokovBlanket.h>
   * @brief Class building the markov Blanket from a BN and a nodeName.
   * @ingroup bn_group

   * The main goal of this class is to build and to encapsulate the DiGraph which
   represents the Markov Blanket.
   *
   */
  class MarkovBlanket {
    public:
    MarkovBlanket(const DAGmodel& m, NodeId n);
    MarkovBlanket(const DAGmodel& m, const std::string& name);

    ~MarkovBlanket();

    /// @return a copy of the mixed graph
    DiGraph mb();

    // @return a dot representation of this MarkovBlanket
    // node of interest is in red
    // special arcs (not used during the construction of the Markov Blanket) are in
    // grey
    std::string toDot(void) const;

    /// wrapping @ref DiGraph::parents(id)
    const NodeSet& parents(const NodeId id) const;

    /// wrapping @ref DiGraph::parents(id)
    const NodeSet& children(const NodeId id) const;

    /// wrapping @ref DiGraph::sizeArcs()
    Size sizeArcs() const;

    /// wrapping @ref DiGraph::arcs()
    const ArcSet& arcs() const;

    /// wrapping @ref DiGraph::sizeNodes()
    Size sizeNodes() const;

    /// wrapping @ref DiGraph::size()
    Size size() const;

    /// wrapping @ref DiGraph::nodes()
    const NodeGraphPart& nodes() const;

    /// @return true if all the named node are the same and all the named arcs are
    /// the same
    bool hasSameStructure(const DAGmodel& other);

    private:
    void __buildMarkovBlanket();

    const DAGmodel& __model;
    DiGraph         __mb;
    const NodeId    __node;
    ArcSet          __specialArcs;
  };
}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif  // GUM_NOINLINE

#endif  // GUM_MARKOVBLANKET_H
