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
/** @file
 * @brief A class for detecting directed cycles in DAGs when trying to apply
 * many changes to the graph
 *
 * When trying to assess whether multiple changes applied to a given DAG would
 * induce cycles, use class DAGCycleDetector instead of trying to apply the
 * modifications to the DAG itself and check whether and exception is raised:
 * the class is designed to be fast for such modifications. However, the number
 * of modifications checked should be higher than at least 3 for this class to
 * be competitive.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/DAGCycleDetector.h>
#include <agrum/core/sequence.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/DAGCycleDetector.inl>
#endif // GUM_NOINLINE

namespace gum {

  /// sets the initial DAG from which changes shall be applied
  void DAGCycleDetector::setDAG(const DAG &dag) {
    // sets the dag
    __dag = dag;

    // get the set of roots and leaves of the dag
    List<NodeId> roots, leaves;
    NodeProperty<unsigned int> nb_parents, nb_children;

    for (const auto node : dag) {
      unsigned int nb_ch = dag.children(node).size();
      nb_children.insert(node, nb_ch);

      if (!nb_ch)
        leaves.insert(node);

      unsigned int nb_pa = dag.parents(node).size();
      nb_parents.insert(node, nb_pa);

      if (!nb_pa)
        roots.insert(node);
    }

    // recompute the set of ancestors
    NodeProperty<unsigned int> empty_set;
    __ancestors.clear();

    for (const auto node : dag) {
      __ancestors.insert(node, empty_set);
    }

    while (!roots.empty()) {
      // get a node and update the ancestors of its children
      NodeId node = roots.front();
      NodeProperty<unsigned int> node_ancestors = __ancestors[node];
      node_ancestors.insert(node, 1);
      const NodeSet &node_children = dag.children(node);
      roots.popFront();

      for (const auto ch : node_children) {
        __addWeightedSet(__ancestors[ch], node_ancestors, 1);
        --nb_parents[ch];

        if (!nb_parents[ch]) {
          roots.insert(ch);
        }
      }
    }

    // recompute the set of descendants
    __descendants.clear();

    for (const auto node : dag) {
      __descendants.insert(node, empty_set);
    }

    while (!leaves.empty()) {
      // get a node and update the descendants of its parents
      NodeId node = leaves.front();
      NodeProperty<unsigned int> node_descendants = __descendants[node];
      node_descendants.insert(node, 1);
      const NodeSet &node_parents = dag.parents(node);
      leaves.popFront();

      for (const auto pa : node_parents) {
        __addWeightedSet(__descendants[pa], node_descendants, 1);
        --nb_children[pa];

        if (!nb_children[pa]) {
          leaves.insert(pa);
        }
      }
    }
  }

  /// indicates whether a set of modifications would create a cycle
  bool DAGCycleDetector::hasCycleFromModifications(
      const std::vector<Change> &modifs) const {
    // first, we separate deletions and insertions (reversals are cut into
    // a deletion + an insertion) and we check that no insertion also exists
    // as a deletion (if so, we remove both operations). In addition, if
    // we try to add an arc (X,X) we return that it induces a cycle
    HashTable<Arc, unsigned int> deletions(modifs.size());
    HashTable<Arc, unsigned int> additions(modifs.size());

    for (const auto &modif : modifs) {
      Arc arc(modif.tail(), modif.head());

      switch (modif.type()) {
        case ChangeType::ARC_DELETION:
          if (deletions.exists(arc))
            ++deletions[arc];
          else
            deletions.insert(arc, 1);

          break;

        case ChangeType::ARC_ADDITION:
          if (modif.tail() == modif.head())
            return true;

          if (additions.exists(arc))
            ++additions[arc];
          else
            additions.insert(arc, 1);

          break;

        case ChangeType::ARC_REVERSAL:
          if (deletions.exists(arc))
            ++deletions[arc];
          else
            deletions.insert(arc, 1);

          arc = Arc(modif.head(), modif.tail());

          if (additions.exists(arc))
            ++additions[arc];
          else
            additions.insert(arc, 1);

          break;

        default:
          GUM_ERROR(OperationNotAllowed, "undefined change type");
      }
    }

    for (auto iter = additions.beginSafe(); // safe iterator needed here
         iter != additions.endSafe(); ++iter) {
      if (deletions.exists(iter.key())) {
        unsigned int &nb_del = deletions[iter.key()];
        unsigned int &nb_add = iter.val();

        if (nb_del > nb_add) {
          additions.erase(iter);
          nb_del -= nb_add;
        } else if (nb_del < nb_add) {
          deletions.erase(iter.key());
          nb_add -= nb_del;
        } else {
          deletions.erase(iter.key());
          additions.erase(iter);
        }
      }
    }

    // get the set of nodes involved in the modifications
    NodeSet extremities;

    for (const auto &modif : modifs) {
      extremities.insert(modif.tail());
      extremities.insert(modif.head());
    }

    // we now retrieve the set of ancestors and descendants of all the
    // extremities of the arcs involved in the modifications. We also
    // keep track of all the children and parents of these nodes
    NodeProperty<NodeProperty<unsigned int>> ancestors, descendants;

    for (const auto &modif : modifs) {
      if (!ancestors.exists(modif.tail())) {
        NodeProperty<unsigned int> &anc =
            ancestors.insert(modif.tail(), NodeProperty<unsigned int>()).second;
        __restrictWeightedSet(anc, __ancestors[modif.tail()], extremities);

        NodeProperty<unsigned int> &desc =
            descendants.insert(modif.tail(), NodeProperty<unsigned int>()).second;
        __restrictWeightedSet(desc, __descendants[modif.tail()], extremities);
      }

      if (!ancestors.exists(modif.head())) {
        NodeProperty<unsigned int> &anc =
            ancestors.insert(modif.head(), NodeProperty<unsigned int>()).second;
        __restrictWeightedSet(anc, __ancestors[modif.head()], extremities);

        NodeProperty<unsigned int> &desc =
            descendants.insert(modif.head(), NodeProperty<unsigned int>()).second;
        __restrictWeightedSet(desc, __descendants[modif.head()], extremities);
      }
    }

    // we apply all the suppressions:
    // assume that the modif concerns arc (X,Y). Then, after removing
    // arc (X,Y), the sets of descendants of the nodes T in
    // ( X + ancestors (X) ) are updated by subtracting the number of paths
    // leading to Y and its set of descendants. These numbers are equal to
    // the numbers found in descendants[X] * the numbers of paths leading
    // to X, i.e., descendants[T][X].
    // By symmetry, the set of ancestors of nodes T in ( Y + descendants (Y) ) are
    // updated by subtracting the number of paths leading to X and its ancestors.
    for (auto iter = deletions.cbegin(); iter != deletions.cend(); ++iter) {
      const Arc &arc = iter.key();
      const NodeId tail = arc.tail();
      const NodeProperty<unsigned int> &anc_tail = ancestors[tail];
      const NodeId head = arc.head();
      const NodeProperty<unsigned int> &desc_head = descendants[head];

      // update the set of descendants
      NodeProperty<unsigned int> set_to_del = desc_head;
      set_to_del.insert(head, 1);
      __delWeightedSet(descendants[tail], set_to_del, 1);

      for (auto iter = anc_tail.cbegin(); iter != anc_tail.cend(); ++iter) {
        __delWeightedSet(descendants[iter.key()], set_to_del,
                         descendants[iter.key()][tail]);
      }

      // update the set of ancestors
      set_to_del = anc_tail;
      set_to_del.insert(tail, 1);
      __delWeightedSet(ancestors[head], set_to_del, 1);

      for (auto iter = desc_head.cbegin(); iter != desc_head.cend(); ++iter) {
        __delWeightedSet(ancestors[iter.key()], set_to_del,
                         ancestors[iter.key()][head]);
      }
    }

    // now we apply all the additions of arcs (including the additions after
    // arc reversals). After adding arc (X,Y), the set of ancestors T of Y and its
    // descendants shall be updated by adding the number of paths leading to
    // X and its ancestors, i.e., ancestors[X] * the number of paths leading
    // to Y, i.e., ancestors[T][Y]. Similarly, the set of descendants of X and its
    // ancestors should be updated by adding the number of paths leading to Y and
    // its descendants. Finally, an arc (X,Y) can be added if and
    // only if Y does not belong to the ancestor set of X
    for (auto iter = additions.cbegin(); iter != additions.cend(); ++iter) {
      const Arc &arc = iter.key();
      NodeId tail = arc.tail();
      NodeId head = arc.head();

      const NodeProperty<unsigned int> &anc_tail = ancestors[tail];

      if (anc_tail.exists(head)) {
        return true;
      }

      const NodeProperty<unsigned int> &desc_head = descendants[head];

      // update the set of ancestors
      NodeProperty<unsigned int> set_to_add = anc_tail;
      set_to_add.insert(tail, 1);
      __addWeightedSet(ancestors[head], set_to_add, 1);

      for (auto iter = desc_head.cbegin(); iter != desc_head.cend(); ++iter) {
        __addWeightedSet(ancestors[iter.key()], set_to_add,
                         ancestors[iter.key()][head]);
      }

      // update the set of descendants
      set_to_add = desc_head;
      set_to_add.insert(head, 1);
      __addWeightedSet(descendants[tail], set_to_add, 1);

      for (auto iter = anc_tail.cbegin(); iter != anc_tail.cend(); ++iter) {
        __addWeightedSet(descendants[iter.key()], set_to_add,
                         descendants[iter.key()][tail]);
      }
    }

    return false;
  }

  /// adds a new arc to the current DAG
  void DAGCycleDetector::insertArc(NodeId tail, NodeId head) { addArc(tail, head); }
  /// adds a new arc to the current DAG
  void DAGCycleDetector::addArc(NodeId tail, NodeId head) {
    // check that the arc does not already exist
    if (__dag.existsArc(tail, head))
      return;

    // check that the arc would not create a cycle
    if (hasCycleFromAddition(tail, head)) {
      GUM_ERROR(InvalidDirectedCycle, "the arc would create a directed into a DAG");
    }

    __dag.addArc(tail, head);

    // now we apply the addition of the arc as done in method
    // hasCycleFromModifications
    const NodeProperty<unsigned int> &anc_tail = __ancestors[tail];
    const NodeProperty<unsigned int> &desc_head = __descendants[head];

    // update the set of ancestors
    NodeProperty<unsigned int> set_to_add = anc_tail;
    set_to_add.insert(tail, 1);
    __addWeightedSet(__ancestors[head], set_to_add, 1);

    for (auto iter = desc_head.cbegin(); iter != desc_head.cend(); ++iter) {
      __addWeightedSet(__ancestors[iter.key()], set_to_add,
                       __ancestors[iter.key()][head]);
    }

    // update the set of descendants
    set_to_add = desc_head;
    set_to_add.insert(head, 1);
    __addWeightedSet(__descendants[tail], set_to_add, 1);

    for (auto iter = anc_tail.cbegin(); iter != anc_tail.cend(); ++iter) {
      __addWeightedSet(__descendants[iter.key()], set_to_add,
                       __descendants[iter.key()][tail]);
    }
  }

  /// removes an arc from the current DAG
  void DAGCycleDetector::eraseArc(NodeId tail, NodeId head) {
    // check that the arc exists
    if (!__dag.existsArc(tail, head))
      return;

    __dag.eraseArc(Arc(tail, head));

    // we apply the deletion of the arc as done in method
    // hasCycleFromModifications
    const NodeProperty<unsigned int> &anc_tail = __ancestors[tail];
    const NodeProperty<unsigned int> &desc_head = __descendants[head];

    // update the set of descendants
    NodeProperty<unsigned int> set_to_del = desc_head;
    set_to_del.insert(head, 1);
    __delWeightedSet(__descendants[tail], set_to_del, 1);

    for (auto iter = anc_tail.cbegin(); iter != anc_tail.cend(); ++iter) {
      __delWeightedSet(__descendants[iter.key()], set_to_del,
                       __descendants[iter.key()][tail]);
    }

    // update the set of ancestors
    set_to_del = anc_tail;
    set_to_del.insert(tail, 1);
    __delWeightedSet(__ancestors[head], set_to_del, 1);

    for (auto iter = desc_head.cbegin(); iter != desc_head.cend(); ++iter) {
      __delWeightedSet(__ancestors[iter.key()], set_to_del,
                       __ancestors[iter.key()][head]);
    }
  }

} /* namespace gum */
