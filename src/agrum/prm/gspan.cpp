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
 * @brief Implementation of gspan.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/gspan.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

void
GSpan::discoverPatterns() {
  __sortNodesAndEdges();
  gspan::InterfaceGraph graph(*__graph);
  for (std::list<NodeId>::iterator root = __tree.roots().begin(); root != __tree.roots().end(); ++root) {
    gspan::Pattern& p = __tree.pattern(*root);
    __subgraph_mining(graph, p);
    /// Removing root from the graph
    for (UndiGraph::NodeIterator node = __tree.iso_graph(p).beginNodes(); node != __tree.iso_graph(p).endNodes(); ++node) {
      Instance* u = __tree.iso_map(p, *node).atPos(0);
      Instance* v = __tree.iso_map(p, *node).atPos(1);
      graph.graph().eraseEdge(Edge(graph.id(u), graph.id(v)));
    }
  }
  __sortPatterns();
}

void
GSpan::__sortNodesAndEdges() {
  typedef Bijection<Idx, gspan::LabelData*>::iterator BijIterator;
  for (BijIterator iter = __graph->labels().begin(); iter != __graph->labels().end(); ++iter) {
    try {
      if (__graph->nodes(iter.second()).size() >= __min_freq) {
        __cost.insert(iter.second(), __cost_func(iter.second()->tree_width, __graph->nodes(iter.second()).size()));
        __nodes.push_back(const_cast<gspan::LabelData*>(iter.second()));
      }
    } catch (NotFound&) {
      // It's a label over edges
      if (__isEdgeEligible(*(__graph->edges(iter.second()).begin()))) {
        __cost.insert(iter.second(), __cost_func(iter.second()->tree_width, __graph->edges(iter.second()).size()));
        __edges.push_back(iter.second());
      }
    }
  }
  Bijection<Idx, gspan::LabelData*>* new_labels = new Bijection<Idx, gspan::LabelData*>();
  GSpan::LabelSort my_sort(this);
  std::sort(__nodes.begin(), __nodes.end(), my_sort);
  std::sort(__edges.begin(), __edges.end(), my_sort);
  Size idx = 0;
  for (std::vector<gspan::LabelData*>::iterator iter = __nodes.begin(); iter != __nodes.end(); ++iter) {
    (*iter)->id = ++idx;
    new_labels->insert(idx, *iter);
  }
  for (std::vector<gspan::LabelData*>::iterator iter = __edges.begin(); iter != __edges.end(); ++iter) {
    (*iter)->id = ++idx;
    new_labels->insert(idx, *iter);
    __tree.addRoot(**iter);
  }
  delete __graph->__labels;
  __graph->__labels = new_labels;
}

void
GSpan::__subgraph_mining(gspan::InterfaceGraph& ig, gspan::Pattern& pat) {
  std::vector<gspan::Pattern*> stack;
  stack.push_back(&pat);
  while (not stack.empty()) {
    // Getting next pattern
    gspan::Pattern& p = *(stack.back());
    stack.pop_back();
    if (p.code().codes.size() < __depth_stop) {
      // We need the rightmost path of p
      std::list<NodeId> r_path;
      p.rightmostPath(r_path);
      // Mapping used to count each possible child of p, the position in the vector
      // matches the one in the rightmost path
      std::vector< HashTable<std::string, gspan::DFSTree::EdgeGrowth*>* > count_vector;
      for (size_t i = 0; i < r_path.size(); ++i) {
        count_vector.push_back(new HashTable<std::string, gspan::DFSTree::EdgeGrowth*>());
      }
      // Pointers used in the following for
      HashTable<std::string, gspan::DFSTree::EdgeGrowth*>* edge_count = 0;
      gspan::DFSTree::EdgeGrowth* edge_growth = 0;
      Sequence<Instance*>* seq = 0; Instance* current = 0; Instance* neighbor = 0;
      // Neighbor_id is the neighbor's id in the interface graph and neighbor_node is its id in the rightmost path in the case
      // of a backward edge growth
      NodeId current_id = 0; NodeId neighbor_id = 0; NodeId neighbor_node =0;
      gspan::LabelData* neighbor_label = 0; gspan::EdgeData* edge_data = 0;
      size_t idx;
      // For each subgraph represented by p, we look for a valid edge growth for each instance match of p in its isomorphism graph.
      for (UndiGraph::NodeIterator iso_node = __tree.iso_graph(p).beginNodes(); iso_node != __tree.iso_graph(p).endNodes(); ++iso_node) {
        seq = &(__tree.iso_map(p, *iso_node));
        idx = 0;
        for (std::list<NodeId>::iterator node = r_path.begin(); node != r_path.end(); ++node, ++idx) {
          edge_count = count_vector[idx];
          // Retrieving the equivalent instance in the current match
          current = seq->atPos((Idx) ((*node) - 1));
          current_id = ig.id(current);
          // Checking for edges not in p
          const NodeSet& neighbours = ig.graph().neighbours(current_id);
          for (NodeSet::iterator edge = neighbours.begin();
               edge != neighbours.end(); ++edge) {
            neighbor_id = *edge;
            neighbor = ig.node(neighbor_id).n;
            // We want a forward edge in any case or a backward edge if current is the rightmost vertex
            if ( (not seq->exists(neighbor)) or ((*node) == r_path.back()) ) {
              // Things we need to know: the LabelData data of the neighbour and, if it's a backward edge, its node id in the rightmost path
              edge_data = &(ig.edge(current_id, neighbor_id));
              neighbor_label = (neighbor == edge_data->u)?edge_data->l_u:edge_data->l_v;
              neighbor_node = (seq->exists(neighbor))?seq->pos(neighbor) + 1:0;
              // Adding the edge growth to the edge_growth hashtable
              gspan::DFSTree::EdgeGrowth temp_growth(*node, edge_data->l, neighbor_label, neighbor_node);
              try {
                edge_growth = (*edge_count)[temp_growth.toString()];
                edge_growth->insert(current, neighbor);
              } catch (NotFound&) {
                edge_growth = new gspan::DFSTree::EdgeGrowth(*node, edge_data->l, neighbor_label, neighbor_node);
                edge_growth->insert(current, neighbor);
                edge_count->insert(edge_growth->toString(), edge_growth);
              }
            }
          }
        }
      }
      // Removing any infrequent child
      typedef HashTable<std::string, gspan::DFSTree::EdgeGrowth*>::iterator EdgeCountIter;
      for (size_t node = 0; node < count_vector.size(); ++node) {
        HashTable<std::string, gspan::DFSTree::EdgeGrowth*>* edge_count = count_vector[node];
        for (EdgeCountIter growth = edge_count->begin(); growth != edge_count->end(); ++growth) {
          if ((**growth).count() >= __min_freq) {
            try {
              __tree.growPattern(p, **growth, __min_freq);
            } catch (OperationNotAllowed& e) {
              // The child was not minimal.
            }
          }
          delete *growth;
        }
        delete edge_count;
      }
      // Calling __subgraph_mining over children of p
      const std::list<NodeId>& children = __tree.children(p);
      for (std::list<NodeId>::const_reverse_iterator child = children.rbegin(); child != children.rend(); ++child) {
        stack.push_back(&(__tree.pattern(*child)));
      }
    }
  }
}

void
GSpan::__sortPatterns() {
  // First we put all the patterns in __patterns.
  std::vector<NodeId> stack;
  for (std::list<NodeId>::reverse_iterator root = tree().roots().rbegin(); root != tree().roots().rend(); ++root) {
    stack.push_back(*root);
  }
  NodeId id = 0;
  std::list<NodeId>* children = 0;
  while (not stack.empty()) {
    id = stack.back();
    stack.pop_back();
    __patterns.push_back(&(tree().pattern(id)));
    children = &(tree().children(tree().pattern(id)));
    for (std::list<NodeId>::reverse_iterator child = children->rbegin(); child != children->rend(); ++child) {
      stack.push_back(*child);
    }
  }
  if (__patterns.empty()) {
    GUM_ERROR(NotFound, "no valid pattern found");
  }
  // Second we sort __patterns.
  GSpan::PatternSort my_sort(this);
  std::sort(__patterns.begin(), __patterns.end(), my_sort);
  // Now we need to find all the matches we can, using __patterns.
  // We start by the best Pattern and add it's maximal independent set to __chosen
  GSpan::MatchedInstances* matches = new GSpan::MatchedInstances();
  Set<NodeId>& max_indep_set = tree().max_indep_set(*(__patterns.front()));
  Sequence<Instance*>* match = 0;
  for (Set<NodeId>::iterator node = max_indep_set.begin(); node != max_indep_set.end(); ++node) {
    match = &(tree().iso_map(*(__patterns.front()), *node));
    for (Sequence<Instance*>::iterator i = match->begin(); i != match->end(); ++i) {
      __chosen.insert(*i);
    }
    matches->insert(match);
  }
  __matched_instances.insert(__patterns.front(), matches);
  // Now we see what kind of pattern we can still use
  bool found;
  UndiGraph* iso_graph = 0;
  const NodeSet* neighbours = 0;
  for (std::vector<gspan::Pattern*>::iterator p = __patterns.begin() + 1; p != __patterns.end(); ++p) {
    UndiGraph reduced_iso_graph;
    std::vector<NodeId> degree_list;
    iso_graph = &(tree().iso_graph(**p));
    for (UndiGraph::NodeIterator node = iso_graph->beginNodes(); node != iso_graph->endNodes(); ++node) {
      found = false;
      match = &(tree().iso_map(**p, *node));
      for (Sequence<Instance*>::iterator i = match->begin(); i != match->end(); ++i) {
        if (__chosen.exists(*i)) {
          found = true;
          break;
        }
      }
      if (not found) {
        // We add the pattern to the reduced isomorphism graph to compute the max independent set
        // over the remaining matches
        reduced_iso_graph.insertNode(*node);
        for (UndiGraph::NodeIterator iso = reduced_iso_graph.beginNodes(); iso != reduced_iso_graph.endNodes(); ++iso) {
          if (iso_graph->existsEdge(*node, *iso)) {
            reduced_iso_graph.insertEdge(*node, *iso);
          }
        }
        degree_list.push_back(*node);
      }
    }
    // We create a new set to hold all the chosen matches of p
    matches = new GSpan::MatchedInstances();
    // We can compute the max independent set and the matches belonging to it
    gspan::DFSTree::NeighborDegreeSort my_sort(reduced_iso_graph);
    std::sort(degree_list.begin(), degree_list.end(), my_sort);
    Set<NodeId> removed;
    for (std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node) {
      if (not removed.exists(*node)) {
        // First we update removed to follow the max independent set algorithm
        removed.insert(*node);
        neighbours = &(reduced_iso_graph.neighbours(*node));
        for (NodeSet::const_iterator neighbor = neighbours->begin(); neighbor != neighbours->end(); ++neighbor) {
          removed.insert(*neighbor);
        }
        // Second we update match and matches to keep track of the current match
        match = &(tree().iso_map(**p, *node));
        matches->insert(match);
        for (Sequence<Instance*>::iterator iter = match->begin(); iter != match->end(); ++iter) {
          __chosen.insert(*iter);
        }
      }
    }
    __matched_instances.insert(*p, matches);
  }
}

double
GSpan::patterns_cost() const {
  double cost = 0;
  typedef HashTable<gspan::Pattern*, MatchedInstances*>::const_iterator MatchIter;
  for (MatchIter match = __matched_instances.begin(); match != __matched_instances.end(); ++match) {
    cost += (tree().frequency(*(match.key())) * tree().cost(*(match.key())));
  }
  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node != __graph->graph().endNodes(); ++node) {
    if (not __chosen.exists(__graph->node(*node).n)) {
      cost += __instance_cost(__graph->node(*node).n);
    }
  }
  return cost;
}

double
GSpan::sve_cost() const {
  unsigned long cost = 0;
  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node != __graph->graph().endNodes(); ++node) {
    cost += __instance_cost(__graph->node(*node).n);
  }
  return cost;
}

double
GSpan::__instance_cost(Instance* i) const {
  unsigned cost = 1;
  Set<ClassElement*> inputs;
  for (Set<SlotChain*>::iterator slot_chain = i->type().slotChains().begin(); slot_chain != i->type().slotChains().end(); ++slot_chain) {
    for (Set<Instance*>::iterator instance = i->getInstances((*slot_chain)->id()).begin();
         instance != i->getInstances((*slot_chain)->id()).end(); ++instance) {
      if (not inputs.exists(&((*instance)->get((*slot_chain)->lastElt().id())))) {
        inputs.insert(&((*instance)->get((*slot_chain)->lastElt().id())));
        cost *= (*instance)->get((*slot_chain)->lastElt().id()).type().variable().domainSize();
      }
    }
  }
  for (DAG::NodeIterator node = i->type().dag().beginNodes(); node != i->type().dag().endNodes(); ++node) {
    if (i->type().isOutputNode(i->type().get(*node)) and i->hasRefAttr(*node)) {
      cost *= i->get(*node).type().variable().domainSize();
    }
  }
  return cost;
}

// void
// GSpan::generateBN(size_t number) {
//   for (size_t i = 0; i < number; ++i) {
//     NodeId id = *(__tree.iso_graph(*(__patterns[i])).beginNodes());
//     Sequence<Instance*>& seq = __tree.iso_map(*(__patterns[i]), id);
//     BayesNet<prm_float>* bn = __generateBNTopology(seq);
//     __bn_map.insert(__patterns[i], bn);
//   }
// }
// 
// void
// GSpan::infer(size_t number) {
//   for (size_t i = 0; i < number; ++i) {
//     // First we list the base bn
//     BayesNet<prm_float>& bn = *(__bn_map[__patterns[i]]);
//     VariableElimination<prm_float> ve(bn);
//     ve.makeInference();
//     const std::vector<NodeId>& elim = ve.eliminationOrder();
//     Set<Potential<prm_float>*> pool;
//     for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
//       pool.insert(const_cast<Potential<prm_float>*>(&(bn.cpt(*node))));
//     }
//     Set<Potential<prm_float>*> trash;
//     ve.eliminateNodes(elim, pool, trash);
//   }
// }
// 
// void
// GSpan::__declare_variables(BayesNetFactory<prm_float>& factory, Sequence<Instance*>& seq) {
//   std::string sep = ".";
//   for (Sequence<Instance*>::iterator i = seq.begin(); i != seq.end(); ++i) {
//     std::stringstream i_pos;
//     i_pos << seq.pos(*i);
//     for (DAG::NodeIterator node = (*i)->type().dag().beginNodes(); node != (*i)->type().dag().endNodes(); ++node) {
//       ClassElement& elt = (*i)->get(*node);
//       /// Aggregates are instantiated !
//       switch ((*i)->type().get(elt.id()).elt_type()) {
//         case ClassElement::prm_attribute:
//           {
//             factory.startVariableDeclaration();
//             factory.variableName(i_pos.str() + sep + elt.name());
//             for (Size idx = 0; idx < elt.type()->domainSize(); ++idx) {
//               factory.addModality(elt.type()->label(idx));
//             }
//             factory.endVariableDeclaration();
//             break;
//           }
//         case ClassElement::prm_aggregate:
//           {
//             Aggregate& agg = static_cast<Aggregate&>((*i)->type().get(elt.id()));
//             factory.startVariableDeclaration();
//             factory.variableName(i_pos.str() + sep + elt.name());
//             switch (agg.agg_type()) {
//               case Aggregate::agg_exists:
//                 {
//                   factory.setVariableCPTImplementation(new aggregator::Exists<prm_float>(agg.label()));
//                   break;
//                 }
//               case Aggregate::agg_forall:
//                 {
//                   factory.setVariableCPTImplementation(new aggregator::Forall<prm_float>(agg.label()));
//                   break;
//                 }
//               default: { GUM_ERROR(OperationNotAllowed, "unknonw aggregator"); }
//             }
//             for (Size idx = 0; idx < elt.type()->domainSize(); ++idx) {
//               factory.addModality(elt.type()->label(idx));
//             }
//             factory.endVariableDeclaration();
//             break;
//           }
//         default: { /* Do nothing */ }
//       }
//     }
//   }
// }
// 
// void
// GSpan::__declare_parents(BayesNetFactory<prm_float>& factory, Sequence<Instance*>& seq) {
//   std::string sep = ".";
//   for (Sequence<Instance*>::iterator i = seq.begin(); i != seq.end(); ++i) {
//     std::stringstream i_pos;
//     i_pos << seq.pos(*i);
//     for (DAG::NodeIterator node = (*i)->type().dag().beginNodes(); node != (*i)->type().dag().endNodes(); ++node) {
//       ClassElement& elt = (*i)->get(*node);
//       switch (elt.elt_type()) {
//         case ClassElement::prm_attribute:
//           {
//             factory.startParentsDeclaration(i_pos.str() + sep + elt.name());
//             for (ArcSet::iterator parent = (*i)->type().dag().parents(elt.id()).begin(); parent != (*i)->type().dag().parents(elt.id()).end(); ++parent) {
//               ClassElement& tail = (*i)->get(parent->tail());
//               switch (tail.elt_type()) {
//                 case ClassElement::prm_attribute:
//                   {
//                     factory.addParent(i_pos.str() + sep + tail.name());
//                     break;
//                   }
//                 case ClassElement::prm_slotchain:
//                   {
//                     for (Set<Instance*>::iterator prnt = (*i)->getInstances(tail.id()).begin(); prnt != (*i)->getInstances(tail.id()).end(); ++prnt) {
//                       if (seq.exists(*prnt)) {
//                         std::stringstream prnt_pos;
//                         prnt_pos << seq.pos(*prnt);
//                         factory.addParent(prnt_pos.str() + sep + static_cast<SlotChain&>(tail).lastElt().name());
//                       }
//                     }
//                     break;
//                   }
//                 default: { /* do nothing */ }
//               }
//             }
//             factory.endParentsDeclaration();
//             break;
//           }
//         default: { /* Do nothing */ }
//       }
//     }
//   }
// }
// 
// BayesNet<prm_float>*
// GSpan::__generateBNTopology(Sequence<Instance*>& seq) {
//   BayesNet<prm_float>* bn = new BayesNet<prm_float>();
//   BayesNetFactory<prm_float> factory(bn);
//   // First we declare all the variables
//   __declare_variables(factory, seq);
//   // Second we add parents in the pattern
//   __declare_parents(factory, seq);
//   // The BN contains all the inner and output nodes of the pattern
//   __fill_inner_nodes(*bn, seq);
//   // Now we add the input nodes with a bijection to keep track of them
//   //__fill_input_nodes(*bn, seq);
//   return bn;
// }
// 
// void
// GSpan::__fill_input_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq) {
// //   std::string sep = ".";
// //   for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
// //     std::stringstream iter_pos;
// //     iter_pos << seq.pos(*iter);
// //     for (DAG::NodeIterator node = (*iter)->dag().beginNodes(); node != (*iter)->dag().endNodes(); ++node) {
// //       ClassElement& elt = (*iter)->get(*node);
// //       switch (elt.elt_type()) {
// //         case ClassElement::prm_attribute:
// //           {
// //             if ((*iter)->type().isInputNode(elt.id())) {
// //               CPFBijection<prm_float>::VarBijection* bij = new CPFBijection<prm_float>::VarBijection();
// //               bij->insert(&(bn.variableFromName(iter_pos.str() + sep + elt.name())),
// //                           &(elt.type().variable()));
// //               for (ArcSet::iterator prnt = (*iter)->dag().parents(elt.id()).begin();
// //                    prnt != (*iter)->dag().parents(elt.id()).end(); ++prnt) {
// //                 ClassElement& tail = (*iter)->get(prnt->tail());
// //                 switch (tail.elt_type()) {
// //                   case ClassElement::prm_attribute:
// //                   case ClassElement::prm_aggregate:
// //                     {
// //                       bij->insert(&(bn.variableFromName(iter_pos.str() + sep + tail.name())),
// //                                   &(tail.type().variable()));
// //                       break;
// //                     }
// //                   default: { GUM_ERROR(FatalError, "this is not an inner or output node"); }
// //                 }
// //               }
// //               CPFBijection<prm_float>* cpf = new CPFBijection<prm_float>(elt.cpf(), bij);
// //               delete bn.__probaMap[bn.idFromName(iter_pos.str() + sep + elt.name())];
// //               bn.__probaMap[bn.idFromName(iter_pos.str() + sep + elt.name())] = cpf;
// //             }
// //             break;
// //           }
// //         default: { /* do nothing */ }
// //       }
// //     }
// //   }
// }
// 
// void
// GSpan::__fill_inner_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq) {
//   // std::string sep = ".";
//   // for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
//   //   std::stringstream iter_pos;
//   //   iter_pos << seq.pos(*iter);
//   //   for (DAG::NodeIterator node = (*iter)->type().dag().beginNodes(); node != (*iter)->type().dag().endNodes(); ++node) {
//   //     ClassElement& elt = (*iter)->get(*node);
//   //     switch ((*iter)->type().get(elt.id()).elt_type()) {
//   //       case ClassElement::prm_attribute:
//   //         {
//   //           //if (not (*iter)->type().isInputNode(elt.id())) {
//   //           CPFBijection<prm_float>::VarBijection* bij = new CPFBijection<prm_float>::VarBijection();
//   //           bij->insert(&(bn.variableFromName(iter_pos.str() + sep + elt.name())), &(elt.type().variable()));
//   //           for (ArcSet::iterator prnt = (*iter)->type().dag().parents(elt.id()).begin(); prnt != (*iter)->type().dag().parents(elt.id()).end(); ++prnt) {
//   //             ClassElement& tail = (*iter)->get(prnt->tail());
//   //             switch (tail.elt_type()) {
//   //               case ClassElement::prm_attribute:
//   //               case ClassElement::prm_aggregate:
//   //                 {
//   //                   bij->insert(&(bn.variableFromName(iter_pos.str() + sep + tail.name())),
//   //                               &(tail.type().variable()));
//   //                   break;
//   //                 }
//   //               case ClassElement::prm_slotchain:
//   //                 {
//   //                   SlotChain& sc = static_cast<SlotChain&>(tail);
//   //                   bij->insert(&((*iter)->getInstance(tail.id()).get(sc.lastElt().id()).type().variable()),
//   //                               &((*iter)->getInstance(tail.id()).get(sc.lastElt().id()).type().variable()));
//   //                   break;
//   //                 }
//   //               default: { GUM_ERROR(FatalError, "this is not an inner or output node"); }
//   //             }
//   //           }
//   //           CPFBijection<prm_float>* cpf = new CPFBijection<prm_float>(elt.cpf(), bij);
//   //           delete bn.__probaMap[bn.idFromName(iter_pos.str() + sep + elt.name())];
//   //           bn.__probaMap[bn.idFromName(iter_pos.str() + sep + elt.name())] = cpf;
//   //           //}
//   //           break;
//   //         }
//   //       default: { /* do nothing */ }
//   //     }
//   //   }
//   // }
// }

// void
// GSpan::__printIsoMap(gspan::Pattern& p) {
//   Sequence<Instance*>& seq = __tree.iso_map(p, *(__tree.iso_graph(p).beginNodes()));
//   std::stringstream sBuff;
//   for (Size i = 0; i < seq.size(); ++i) {
//     sBuff << seq.atPos(i)->name() << " ";
//   }
//   std::cerr << std::endl << sBuff.str() << std::endl;
// }

// void
// GSpan::__flushReferences(gspan::Pattern* p, std::ostream& out) const {
//   std::string sep = "_";
//   std::stringstream sBuff;
//   const Sequence<Instance*>& match = **(__matched_instances[p]->begin());
//   // The order used to declare references is unimportant, so lets do it
//   for (Sequence<Instance*>::const_iterator iter = match.begin(); iter != match.end(); ++iter) {
//     std::string prefix = (**iter).name() + sep;
//     for (Set<ReferenceSlot*>::const_iterator ref = (**iter).type().referenceSlots().begin(); ref != (**iter).type().referenceSlots().end(); ++ref) {
//       if ((**ref).isArray()) {
//         Size count = 0;
//         const Set<Instance*>* ref_set = &((**iter).getInstances((**ref).id()));
//         // Checking if more than one instance is not in match
//         for (Set<Instance*>::const_iterator jter = ref_set->begin(); (jter != ref_set->end()) and count < 2; ++jter)
//           if (not match.exists(*jter)) ++count;
//         if (count == 1)     // Not an array
//           sBuff << (**ref).slotType().name() << " " << prefix + (**ref).name() << ";" << std::endl;
//         else if (count > 1) // Is an array
//           sBuff << (**ref).slotType().name() << "[] " << prefix + (**ref).name() << ";" << std::endl;
//       } else {
//         if (not match.exists(const_cast<Instance*>(&((**iter).getInstance((**ref).id())))))
//           sBuff << (**ref).slotType().name() << " " << prefix + (**ref).name() << ";" << std::endl;
//       }
//     }
//   }
//   out << sBuff.str() << std::endl;
// }
// 
// void
// GSpan::__flushVariables(gspan::Pattern* p, std::ostream& out) const {
//   std::string sep = "_";
//   Set<std::string> flushed, canBeFlushed;
//   std::list< std::pair<const Instance*, const Attribute*> > queue;
//   const Sequence<Instance*>& match = **(__matched_instances[p]->begin());
//   for (Sequence<Instance*>::const_iterator iter = match.begin(); iter != match.end(); ++iter) {
//     for (Instance::const_iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
//       if (not flushed.exists((**iter).name() + sep + (**jter).safeName())) {
//         queue.push_front(std::make_pair(*iter, *jter));
//         while (not queue.empty()) {
//           const Instance* inst = queue.front().first;
//           const Attribute* attr = queue.front().second;
//           if (canBeFlushed.exists(inst->name() + sep + attr->safeName())) {
//             __flushAttribute(p, out, queue.front().first, queue.front().second);
//             flushed.insert(inst->name() + sep + attr->safeName());
//             queue.pop_front();
//           } else {
//             const ArcSet* parents = &(inst->type().dag().parents(attr->id()));
//             for (ArcSet::const_iterator arc = parents->begin(); arc != parents->end(); ++arc) {
//               switch (inst->type().get(arc->tail()).elt_type()) {
//                 case ClassElement::prm_attribute:
//                 case ClassElement::prm_aggregate:
//                   {
//                     if (not flushed.exists(inst->name() + sep + inst->get(arc->tail()).safeName()))
//                       queue.push_front(std::make_pair(inst, &(inst->get(arc->tail()))));
//                     break;
//                   }
//                 case ClassElement::prm_slotchain:
//                   {
//                     std::string safe_name = static_cast<const SlotChain&>(inst->type().get(arc->tail())).lastElt().safeName();
//                     const Set<Instance*>* ref_set = &(inst->getInstances(arc->tail()));
//                     for (Set<Instance*>::const_iterator ref = ref_set->begin(); ref != ref_set->end(); ++ref)
//                       if ( (match.exists(*ref)) and (not flushed.exists((**ref).name() + sep + safe_name)) )
//                         queue.push_front(std::make_pair(*ref, &((**ref).get(safe_name))));
//                     break;
//                   }
//                 default: { /* do nothing */ }
//               }
//             }
//             canBeFlushed.insert(inst->name() + sep + attr->safeName());
//           }
//         }
//       }
//     }
//   }
// }
// 
// void
// GSpan::__flushAttribute(gspan::Pattern* p, std::ostream& out,
//                         const Instance* inst, const Attribute* attr) const
// {
//   // First we must check if attr is not a cast descendant
//   if (not inst->type().isCastDescendant(attr->safeName())) {
//     std::string sep = "_";
//     std::stringstream sBuff;
//     // Adding the attributes type and name
//     sBuff << attr->type().name() << " " << attr->name() << " ";
//     const Sequence<Instance*>& match = **(__matched_instances[p]->begin());
//     if (ClassElement::isAttribute(inst->type().get(attr->safeName()))) {
//       // Check if it is a Noisy or
//       if (dynamic_cast<const MultiDimArray<prm_float>*>(attr->cpf().getContent())) {
//         sBuff << "depensdon ";
//         std::vector<std::string> parents;
//         for (ArcSet::const_iterator arc = inst->type().dag().parents(attr->id()).begin(); arc != inst->type().dag().parents(attr->id()).end(); ++attr) {
//           switch (inst->type().get(arc->tail()).elt_type()) {
//             case ClassElement::prm_attribute:
//             case ClassElement::prm_aggregate:
//               {
//                 parents.push_back(inst->type().get(arc->tail()).safeName());
//                 break;
//               }
//             case ClassElement::prm_slotchain:
//               {
//                 const SlotChain& sc = static_cast<const SlotChain&>(inst->type().get(arc->tail()));
//                 break;
//               }
//             default: { /* do nothing */ }
//           }
//         }
//       } else if (dynamic_cast<const MultiDimNoisyORCompound*>(attr->cpf().getContent())) {
// 
//       } else if (dynamic_cast<const MultiDimNoisyORNet*>(attr->cpf().getContent())) {
// 
//       }
//     } else { // its an aggregate
// 
//     }
//   }
//   out << sBuff.str() << std::endl;
// }

} /* namespace prm */
} /* namespace gum */
// ============================================================================
