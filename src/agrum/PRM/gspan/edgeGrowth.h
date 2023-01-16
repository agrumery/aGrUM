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


/**
 * @file
 * @brief Headers of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_EDGE_GROWTH_H
#define GUM_EDGE_GROWTH_H

#include <list>
#include <ostream>
#include <utility>
#include <vector>


#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/PRM/gspan/pattern.h>

namespace gum {
  namespace prm {
    namespace gspan {
      template < typename GUM_SCALAR >
      class DFSTree;

      /**
       * @class EdgeGrowth
       * @headerfile DFSTree.h <agrum/PRM/DFSTree.h>
       * This class is used to define an edge growth of a pattern
       * in this DFSTree.
       */
      template < typename GUM_SCALAR >
      class EdgeGrowth {
        public:
        friend class DFSTree< GUM_SCALAR >;
        /// Constructor.
        EdgeGrowth(NodeId a_u, LabelData* an_edge, LabelData* a_l_v, NodeId a_v = 0);
        /// Copy constructor.
        EdgeGrowth(const EdgeGrowth& from);
        /// Destructor.
        ~EdgeGrowth();
        /// The id of the node from which we grow an edge.
        NodeId u;
        /// The LabelData over the edge of this edge growth.
        LabelData* edge;
        /// The LabelData over the node of this edge growth.
        LabelData* l_v;
        /// If the growth is backward you must assigned the subscript of v,
        /// otherwise 0 is assigned (recall that subscripts start from 1)
        NodeId v;
        /// Add the pair (u,v) as a match for the current growth.
        void insert(PRMInstance< GUM_SCALAR >* u, PRMInstance< GUM_SCALAR >* v);
        /// The mapping between the u and v for each match in the interface
        /// graph.
        NodeProperty< std::pair< PRMInstance< GUM_SCALAR >*, PRMInstance< GUM_SCALAR >* > > matches;
        /// Return a string representation of this
        std::string toString();

        private:
        /// The iso graph for computing the maximum independent set of matches.
        UndiGraph iso_graph;
        /// Vector used for computation
        std::vector< NodeId >* degree_list;
        /// The max indep set of matches
        Set< NodeId > max_indep_set;
      };

      template < typename GUM_SCALAR >
      std::ostream& operator<<(std::ostream& out, const EdgeGrowth< GUM_SCALAR >& edge);


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class EdgeGrowth< double >;
#endif


    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/gspan/edgeGrowth_tpl.h>

#endif /* GUM_EDGE_GROWTH_H */
