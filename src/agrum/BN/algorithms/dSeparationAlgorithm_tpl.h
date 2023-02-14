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
 * @brief d-separation analysis (as described in Koller & Friedman 2009)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */


namespace gum {


  // update a set of potentials, keeping only those d-connected with
  // query variables given evidence
  template < typename GUM_SCALAR, class TABLE >
  void dSeparationAlgorithm::relevantPotentials(const IBayesNet< GUM_SCALAR >& bn,
                                                const NodeSet&                 query,
                                                const NodeSet&                 hardEvidence,
                                                const NodeSet&                 softEvidence,
                                                Set< const TABLE* >&           potentials) {
    const DAG& dag = bn.dag();

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

    /// for relevant potentials: indicate which tables contain a variable
    /// (nodeId)
    HashTable< NodeId, Set< const TABLE* > > node2potentials;
    for (const auto pot: potentials) {
      const Sequence< const DiscreteVariable* >& vars = pot->variablesSequence();
      for (const auto var: vars) {
        const NodeId id = bn.nodeId(*var);
        if (!node2potentials.exists(id)) { node2potentials.insert(id, Set< const TABLE* >()); }
        node2potentials[id].insert(pot);
      }
    }

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
    while (!nodes_to_visit.empty() && !node2potentials.empty()) {
      // get the next node to visit
      const NodeId node      = nodes_to_visit.front().first;
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

      // if the node belongs to the query, update  _node2potentials_: remove all
      // the potentials containing the node
      if (node2potentials.exists(node)) {
        auto& pot_set = node2potentials[node];
        for (const auto pot: pot_set) {
          const auto& vars = pot->variablesSequence();
          for (const auto var: vars) {
            const NodeId id = bn.nodeId(*var);
            if (id != node) {
              node2potentials[id].erase(pot);
              if (node2potentials[id].empty()) { node2potentials.erase(id); }
            }
          }
        }
        node2potentials.erase(node);

        // if  _node2potentials_ is empty, no need to go on: all the potentials
        // are d-connected to the query
        if (node2potentials.empty()) return;
      }

      // if this is the first time we meet the node, then visit it
      if (!already_visited) {
        // mark the node as reachable if this is not a hard evidence
        const bool is_hard_evidence = hardEvidence.exists(node);

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

    // here, all the potentials that belong to  _node2potentials_ are d-separated
    // from the query
    for (const auto& elt: node2potentials) {
      for (const auto pot: elt.second) {
        potentials.erase(pot);
      }
    }
  }


} /* namespace gum */
