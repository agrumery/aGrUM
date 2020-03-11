
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief d-separation analysis (as described in Koller & Friedman 2009)
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/BN/algorithms/dSeparation.h>
#include <agrum/tools/core/list.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/dSeparation_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // Fill 'requisite' with the requisite nodes in dag given a query and
  // evidence.
  void dSeparation::requisiteNodes(const DAG&     dag,
                                   const NodeSet& query,
                                   const NodeSet& hardEvidence,
                                   const NodeSet& softEvidence,
                                   NodeSet&       requisite) {
    // for the moment, no node is requisite
    requisite.clear();

    // mark the set of ancestors of the evidence
    NodeSet ev_ancestors(dag.size());
    {
      List< NodeId > anc_to_visit;
      for (const auto node: hardEvidence)
        anc_to_visit.insert(node);
      for (const auto node: softEvidence)
        anc_to_visit.insert(node);
      while (!anc_to_visit.empty()) {
        const NodeId node = anc_to_visit.front();
        anc_to_visit.popFront();

        if (!ev_ancestors.exists(node)) {
          ev_ancestors.insert(node);
          for (const auto par: dag.parents(node)) {
            anc_to_visit.insert(par);
          }
        }
      }
    }

    // create the marks indicating that we have visited a node
    NodeSet visited_from_child(dag.size());
    NodeSet visited_from_parent(dag.size());

    // indicate that we will send the ball to all the query nodes (as children):
    // in list nodes_to_visit, the first element is the next node to send the
    // ball to and the Boolean indicates whether we shall reach it from one of
    // its children (true) or from one parent (false)
    List< std::pair< NodeId, bool > > nodes_to_visit;
    for (const auto node: query) {
      nodes_to_visit.insert(std::pair< NodeId, bool >(node, true));
    }

    // perform the bouncing ball until there is no node in the graph to send
    // the ball to
    while (!nodes_to_visit.empty()) {
      // get the next node to visit
      const NodeId node = nodes_to_visit.front().first;
      const bool   direction = nodes_to_visit.front().second;
      nodes_to_visit.popFront();

      // check if the node has not already been visited in the same direction
      bool already_visited;
      if (direction) {
        already_visited = visited_from_child.exists(node);
        if (!already_visited) { visited_from_child.insert(node); }
      } else {
        already_visited = visited_from_parent.exists(node);
        if (!already_visited) { visited_from_parent.insert(node); }
      }

      // if this is the first time we meet the node, then visit it
      if (!already_visited) {
        // mark the node as reachable if this is not a hard evidence
        const bool is_hard_evidence = hardEvidence.exists(node);
        if (!is_hard_evidence) { requisite.insert(node); }

        // bounce the ball toward the neighbors
        if (direction && !is_hard_evidence) {   // visit from a child
          // visit the parents
          for (const auto par: dag.parents(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(par, true));
          }

          // visit the children
          for (const auto chi: dag.children(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(chi, false));
          }
        } else {   // visit from a parent
          if (!hardEvidence.exists(node)) {
            // visit the children
            for (const auto chi: dag.children(node)) {
              nodes_to_visit.insert(std::pair< NodeId, bool >(chi, false));
            }
          }
          if (ev_ancestors.exists(node)) {
            // visit the parents
            for (const auto par: dag.parents(node)) {
              nodes_to_visit.insert(std::pair< NodeId, bool >(par, true));
            }
          }
        }
      }
    }
  }

} /* namespace gum */
