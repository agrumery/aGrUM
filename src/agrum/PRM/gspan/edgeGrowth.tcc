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
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/gspan/edgeGrowth.h>

namespace gum {
  namespace prm {
    namespace gspan {

      template <typename GUM_SCALAR>
      INLINE EdgeGrowth<GUM_SCALAR>::EdgeGrowth(NodeId a_u, LabelData *an_edge,
                                                LabelData *a_l_v, NodeId a_v)
          : u(a_u), edge(an_edge), l_v(a_l_v), v(a_v),
            degree_list(new std::vector<NodeId>()) {
        GUM_CONSTRUCTOR(EdgeGrowth);
      }

      template <typename GUM_SCALAR>
      INLINE EdgeGrowth<GUM_SCALAR>::EdgeGrowth(const EdgeGrowth<GUM_SCALAR> &from)
          : u(from.u), edge(from.edge), v(from.v), matches(from.matches),
            iso_graph(from.iso_graph), degree_list(0),
            max_indep_set(from.max_indep_set) {
        GUM_CONS_CPY(EdgeGrowth);

        if (from.degree_list != 0) {
          degree_list = new std::vector<NodeId>(*(from.degree_list));
        }
      }

      template <typename GUM_SCALAR> INLINE EdgeGrowth<GUM_SCALAR>::~EdgeGrowth() {
        GUM_DESTRUCTOR(EdgeGrowth);

        if (degree_list != 0) {
          delete degree_list;
        }
      }

      template <typename GUM_SCALAR>
      INLINE std::string EdgeGrowth<GUM_SCALAR>::toString() {
        std::stringstream str;
        str << u << "-" << edge << "-" << l_v << "-" << v;
        return str.str();
      }

      template <typename GUM_SCALAR>
      void EdgeGrowth<GUM_SCALAR>::insert(Instance<GUM_SCALAR> *u,
                                          Instance<GUM_SCALAR> *v) {
        NodeId id = iso_graph.addNode();
        degree_list->push_back(id);

        for (const auto &elt : matches) {
          if ((elt.second.first == u) or (elt.second.second == u) or
              (elt.second.first == v) or (elt.second.second == v)) {
            iso_graph.addEdge(elt.first, id);
          }
        }

        // The order between u and v is important ! DO NOT INVERSE IT !
        matches.insert(id, std::make_pair(u, v));
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
