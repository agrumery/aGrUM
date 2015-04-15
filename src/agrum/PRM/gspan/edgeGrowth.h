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
 * @brief Headers of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_EDGE_GROWTH_H
#define GUM_EDGE_GROWTH_H

#include <utility>
#include <cmath>
#include <list>
#include <vector>
#include <ostream>

#include <agrum/core/sequence.h>
#include <agrum/core/set.h>
#include <agrum/core/bijection.h>

#include <agrum/graphs/diGraph.h>

#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/gspan/pattern.h>

namespace gum {
  namespace prm {
    namespace gspan {
      template <typename GUM_SCALAR> class DFSTree;

      template <typename GUM_SCALAR> class EdgeGrowth {
        public:
        friend class DFSTree<GUM_SCALAR>;
        /// Constructor.
        EdgeGrowth(NodeId a_u, LabelData *an_edge, LabelData *a_l_v, NodeId a_v = 0);
        /// Copy constructor.
        EdgeGrowth(const EdgeGrowth &from);
        /// Destructor.
        ~EdgeGrowth();
        /// The NodeId of the node from which we grow an edge.
        NodeId u;
        /// Returns the number of matches in the interface graph
        /// for this edge growth.
        // Size count() const;
        /// The LabelData over the edge of this edge growth.
        LabelData *edge;
        /// The LabelData over the node of this edge growth.
        LabelData *l_v;
        /// If the growth is backward you must assigned the subscript of v,
        /// otherwise 0 is assigned (recall that subscripts start from 1)
        NodeId v;
        /// Add the pair (u,v) as a match for the current growth.
        void insert(Instance<GUM_SCALAR> *u, Instance<GUM_SCALAR> *v);
        /// The mapping between the u and v for each match in the interface graph.
        NodeProperty<std::pair<Instance<GUM_SCALAR> *, Instance<GUM_SCALAR> *>>
            matches;
        /// Return a string representation of this
        std::string toString();

        private:
        /// The iso graph for computing the maximum independent set of matches.
        UndiGraph iso_graph;
        /// Vector used for computation
        std::vector<NodeId> *degree_list;
        /// The max indep set of matches
        Set<NodeId> max_indep_set;
      };

      template <typename GUM_SCALAR>
      std::ostream &operator<<(std::ostream &out,
                               const EdgeGrowth<GUM_SCALAR> &edge);

      extern template class EdgeGrowth<double>;
    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/gspan/edgeGrowth.tcc>

#endif /* GUM_EDGE_GROWTH_H */
