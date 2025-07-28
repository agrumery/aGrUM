/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of the BayesBall class.
 */

namespace gum {


  // update a set of tensors, keeping only those d-connected with
  // query variables
  template < typename GUM_SCALAR, class TABLE >
  void BayesBall::relevantTensors(const IBayesNet< GUM_SCALAR >& bn,
                                  const NodeSet&                 query,
                                  const NodeSet&                 hardEvidence,
                                  const NodeSet&                 softEvidence,
                                  Set< const TABLE* >&           tensors) {
    const DAG& dag = bn.dag();

    // create the marks (top = first and bottom = second)
    NodeProperty< std::pair< bool, bool > > marks(dag.size());
    const std::pair< bool, bool >           empty_mark(false, false);

    /// for relevant tensors: indicate which tables contain a variable
    /// (nodeId)
    HashTable< NodeId, Set< const TABLE* > > node2tensors;
    for (const auto pot: tensors) {
      const Sequence< const DiscreteVariable* >& vars = pot->variablesSequence();
      for (const auto var: vars) {
        const NodeId id = bn.nodeId(*var);
        if (!node2tensors.exists(id)) { node2tensors.insert(id, Set< const TABLE* >()); }
        node2tensors[id].insert(pot);
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

    // perform the bouncing ball until  _node2tensors_ becomes empty (which
    // means that we have reached all the tensors and, therefore, those
    // are d-connected to query) or until there is no node in the graph to send
    // the ball to
    while (!nodes_to_visit.empty() && !node2tensors.empty()) {
      // get the next node to visit
      NodeId node = nodes_to_visit.front().first;

      // if the marks of the node do not exist, create them
      if (!marks.exists(node)) marks.insert(node, empty_mark);

      // if the node belongs to the query, update  _node2tensors_: remove all
      // the tensors containing the node
      if (node2tensors.exists(node)) {
        auto& pot_set = node2tensors[node];
        for (const auto pot: pot_set) {
          const auto& vars = pot->variablesSequence();
          for (const auto var: vars) {
            const NodeId id = bn.nodeId(*var);
            if (id != node) {
              node2tensors[id].erase(pot);
              if (node2tensors[id].empty()) { node2tensors.erase(id); }
            }
          }
        }
        node2tensors.erase(node);

        // if  _node2tensors_ is empty, no need to go on: all the tensors
        // are d-connected to the query
        if (node2tensors.empty()) return;
      }


      // bounce the ball toward the neighbors
      if (nodes_to_visit.front().second) {   // visit from a child
        nodes_to_visit.popFront();

        if (hardEvidence.exists(node)) { continue; }

        if (!marks[node].first) {
          marks[node].first = true;   // top marked
          for (const auto par: dag.parents(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(par, true));
          }
        }

        if (!marks[node].second) {
          marks[node].second = true;   // bottom marked
          for (const auto chi: dag.children(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(chi, false));
          }
        }
      } else {   // visit from a parent
        nodes_to_visit.popFront();

        const bool is_hard_evidence = hardEvidence.exists(node);
        const bool is_evidence      = is_hard_evidence || softEvidence.exists(node);

        if (is_evidence && !marks[node].first) {
          marks[node].first = true;

          for (const auto par: dag.parents(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(par, true));
          }
        }

        if (!is_hard_evidence && !marks[node].second) {
          marks[node].second = true;

          for (const auto chi: dag.children(node)) {
            nodes_to_visit.insert(std::pair< NodeId, bool >(chi, false));
          }
        }
      }
    }


    // here, all the tensors that belong to  _node2tensors_ are d-separated
    // from the query
    for (const auto& elt: node2tensors) {
      for (const auto pot: elt.second) {
        tensors.erase(pot);
      }
    }
  }


} /* namespace gum */
