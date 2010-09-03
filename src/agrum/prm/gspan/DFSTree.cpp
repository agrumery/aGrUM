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
 * @brief Implementation of the DFSTree class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/gspan/DFSTree.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan/DFSTree.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {
namespace gspan {

void
DFSTree::addRoot(LabelData& label) {
  HashTable<Pattern*, std::pair<Idx, Idx> > roots;
  HashTable<Pattern*, Sequence<EdgeData*>* > roots_edges;
  const Set<EdgeData*>& edges = __graph->edges(&label);
  for (Set<EdgeData*>::const_iterator iter = edges.begin(); iter != edges.end(); ++iter) {
    bool u_first = ((*iter)->l_u->id < (*iter)->l_v->id)?true:false;
    Idx u_idx = (u_first)?(*iter)->l_u->id:(*iter)->l_v->id;
    Idx v_idx = (not u_first)?(*iter)->l_u->id:(*iter)->l_v->id;
    bool found = false;
    for (HashTable<Pattern*, std::pair<Idx, Idx> >::iterator root = roots.begin(); root != roots.end(); ++root) {
      if ( (root->first == u_idx) and (root->second == v_idx) ) {
        roots_edges[root.key()]->insert(*iter);
        found = true;
        break;
      }
    }
    /// Then we create a new pattern
    if (not found) {
      Pattern* p = new Pattern();
      roots.insert(p, std::make_pair(u_idx, v_idx));
      roots_edges.insert(p, new Sequence<EdgeData*>());
      DFSTree::PatternData* data = new DFSTree::PatternData(p);
      NodeId u = p->insertNode((u_first)?*((*iter)->l_u):*((*iter)->l_v));
      NodeId v = p->insertNode((not u_first)?*((*iter)->l_u):*((*iter)->l_v));
      p->insertArc(u, v, label);
      __node_map.insert(DiGraph::insertNode(), p);
      __data.insert(p, data);
      __roots.push_back(__node_map.first(p));
    }
  }
  // This is used to compute the max independent set of p->max_indep_set
  for (HashTable<Pattern*, Sequence<EdgeData*>*>::iterator root = roots_edges.begin(); root != roots_edges.end(); ++root) {
    __initialiaze_root(root.key(), **root);
    delete *root;
  }
}

void
DFSTree::__initialiaze_root(Pattern* p, Sequence<EdgeData*>& edge_seq) {
  DFSTree::PatternData* data = __data[p];
  std::vector<NodeId> degree_list;
  for (Sequence<EdgeData*>::iterator edge = edge_seq.begin(); edge != edge_seq.end(); ++edge) {
    bool u_first = ((*edge)->l_u->id < (*edge)->l_v->id)?true:false;
    Sequence<Instance*>* seq = new Sequence<Instance*>();
    // Creating the multiset of instances matching p
    seq->insert((u_first)?(*edge)->u:(*edge)->v);
    seq->insert((not u_first)?(*edge)->u:(*edge)->v);
    NodeId an_id = data->iso_graph.insertNode();
    data->iso_map.insert(an_id, seq);
    degree_list.push_back(an_id);
    // Adding edges between two isomorphisms of p sharing at least one instance
    typedef Property<Sequence<Instance*>*>::onNodes::iterator IsoIterator;
    for (IsoIterator iso = data->iso_map.begin(); iso != data->iso_map.end(); ++iso) {
      if (iso.key() != an_id) {
        for (Sequence<Instance*>::iterator inst = (*iso)->begin(); inst != (*iso)->end(); ++inst) {
          if (seq->exists(*inst)) {
            data->iso_graph.insertEdge(an_id, iso.key());
            break;
          }
        }
      }
    }
  }
  // Computing p->max_indep_set using a greedy algorithm
  DFSTree::NeighborDegreeSort my_operator(data->iso_graph);
  std::sort(degree_list.begin(), degree_list.end(), my_operator);
  Set<NodeId> removed;
  for (std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node) {
    if (not removed.exists(*node)) {
      removed.insert(*node);
      const NodeSet& neighbours = data->iso_graph.neighbours(*node);
      for (NodeSet::const_iterator neighbor = neighbours.begin();
           neighbor != neighbours.end(); ++neighbor) {
        removed.insert(*neighbor);
      }
      data->max_indep_set.insert(*node);
    }
  }
}

bool
DFSTree::__is_new_seq(Sequence<Instance*>& seq,
                      Property<Sequence<Instance*>*>::onNodes& iso_map)
{
  typedef Property<Sequence<Instance*>*>::onNodes::iterator Iter;
  for (Iter iter = iso_map.begin(); iter != iso_map.end(); ++iter) {
    bool found = false;
    for (Sequence<Instance*>::iterator jter = seq.begin(); jter != seq.end(); ++jter) {
      if (not (*iter)->exists(*jter)) {
        found = true;
        break;
      }
    }
    if (not found) {
      return false;
    }
  }
  return true;
}

Pattern&
DFSTree::growPattern(Pattern& p, EdgeGrowth& edge_growth, Size min_freq) {
  Pattern* child = new Pattern(p);
  try {
    __checkGrowth(p, child, edge_growth);
  } catch (OperationNotAllowed& e) {
    delete child;
    throw e;
  }
  // Now we need to build the pattern data about child
  DFSTree::PatternData* data = new DFSTree::PatternData(child);
  std::vector<NodeId> degree_list;
  typedef Property<Sequence<Instance*>*>::onNodes::iterator IsoMapIter;
  Property<Sequence<Instance*>*>::onNodes& p_iso_map = __data[&p]->iso_map;
  Property< std::pair<Instance*, Instance*> >::onNodes::iterator match;
  // Using p information to build child's isomorphism graph
  NodeId id = 0;
  for (IsoMapIter seq = p_iso_map.begin(); seq != p_iso_map.end(); ++seq) {
    typedef Property< std::pair<Instance*, Instance*> >::onNodes::iterator MatchIter;
    MatchIter match = edge_growth.matches.begin();
    for ( ; match != edge_growth.matches.end(); ++match) {
      /// Adding the isomorphism in the iso_graph and building the iso_map.
      if (child->code().codes.back()->isForward()) {
        if ((*seq)->exists(match->first) and (not (*seq)->exists(match->second))) {
          // Let's see if the new match is already matched
          Sequence<Instance*>* new_seq = new Sequence<Instance*>(**seq);
          new_seq->insert(match->second);
          if (__is_new_seq(*new_seq, data->iso_map)) {
            id = data->iso_graph.insertNode();
            data->iso_map.insert(id, new_seq);
          } else {
            delete new_seq;
          }
          break;
        }
      } else {
        if ((*seq)->exists(match->first) and (*seq)->exists(match->second)) {
          Sequence<Instance*>* new_seq = new Sequence<Instance*>(**seq);
          if (__is_new_seq(*new_seq, data->iso_map)) {
            id = data->iso_graph.insertNode();
            data->iso_map.insert(id, new_seq);
          } else {
            delete new_seq;
          }
          break;
        }
      }
    }
    if (match != edge_growth.matches.end()) {
      // Adding edges in the iso_graph
      for (UndiGraph::NodeIterator node = data->iso_graph.beginNodes(); node != data->iso_graph.endNodes(); ++node) {
        if ((*node) != id) {
          for (Sequence<Instance*>::iterator iter = data->iso_map[id]->begin(); iter != data->iso_map[id]->end(); ++iter) {
            if (data->iso_map[*node]->exists(*iter)) {
              data->iso_graph.insertEdge(*node, id);
              break;
            }
          }
        }
      }
      degree_list.push_back(id);
      edge_growth.matches.erase(match.key());
    }
  }
  if (data->iso_graph.size() < min_freq) {
    delete data;
    delete child;
    GUM_ERROR(OperationNotAllowed, "child is not frequent enough");
  }
  // Now we can compute the maximal independent set of child
  DFSTree::NeighborDegreeSort my_operator(data->iso_graph);
  std::sort(degree_list.begin(), degree_list.end(), my_operator);
  Set<NodeId> removed;
  for (std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node) {
    if (not removed.exists(*node)) {
      removed.insert(*node);
      const NodeSet& neighbours =  data->iso_graph.neighbours(*node);
      for (NodeSet::const_iterator neighbor = neighbours.begin();
           neighbor != neighbours.end(); ++neighbor) {
        removed.insert(*neighbor);
      }
      data->max_indep_set.insert(*node);
    }
  }
  if (data->max_indep_set.size() < min_freq) {
    delete data;
    delete child;
    GUM_ERROR(OperationNotAllowed, "child is not frequent enough");
  }
  __addChild(p, child, edge_growth);
  __data.insert(child, data);
  for (UndiGraph::NodeIterator node = data->iso_graph.beginNodes(); node != data->iso_graph.endNodes(); ++node) {
    __find_sub_pattern(*child, *node);
  }
  return *child;
}

Size
DFSTree::EdgeGrowth::count() {
  return matches.size();
}

double
DFSTree::frequency(const Pattern& p) const {
  return (double) __data[const_cast<Pattern*>(&p)]->max_indep_set.size();
}

double
DFSTree::cost(const Pattern& p) const {
  if (__data[const_cast<Pattern*>(&p)]->cost == 0) {
    Size cost = 1;
    Sequence<Instance*>& seq = **(__data[const_cast<Pattern*>(&p)]->iso_map.begin());
    Sequence<ClassElement*> input_set;
    for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
      for (Set<SlotChain*>::iterator input = (*iter)->type().slotChains().begin(); input != (*iter)->type().slotChains().end(); ++input) {
        for (Set<Instance*>::iterator jter = (*iter)->getInstances((*input)->id()).begin(); jter != (*iter)->getInstances((*input)->id()).end(); ++jter) {
          if ((not seq.exists(*jter)) and (not input_set.exists(&((*jter)->get((*input)->lastElt().safeName())))) ) {
            cost *= (*input)->type().variable().domainSize();
            input_set.insert(&((*jter)->get((*input)->lastElt().safeName())));
          }
        }
      }
      for (Instance::InvRefIterator vec = (**iter).beginInvRef(); vec != (**iter).endInvRef(); ++vec) {
        for (std::vector< std::pair<Instance*, std::string> >::iterator inverse = (**vec).begin(); inverse != (**vec).end(); ++inverse) {
          if (not seq.exists(inverse->first)) {
            cost *= (*iter)->get(vec.key()).type().variable().domainSize();
            break;
          }
        }
        // if ( (*iter)->type().isOutputNode((*iter)->type().get(*node)) and (*iter)->hasRefAttr(*node) ) {
        //   for (Set<Instance::InverseSC*>::iterator inverse = (*iter)->inverseSC(*node).begin(); inverse != (*iter)->inverseSC(*node).end(); ++inverse) {
        //     if (not seq.exists((*inverse)->first)) {
        //       cost *= (*iter)->get(*node).type().variable().domainSize();
        //       break;
        //     }
        //   }
        // }
      }
    }
    __data[const_cast<Pattern*>(&p)]->cost = cost;
  }
  return (double) __data[const_cast<Pattern*>(&p)]->cost;
}

double
DFSTree::gain(const Pattern& p) const {
  if (__data[const_cast<Pattern*>(&p)]->gain == 0) {
    Size gain = 0;
    Sequence<Instance*>& seq = **(__data[const_cast<Pattern*>(&p)]->iso_map.begin());
    for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
      Size current = 1;
      for (Set<SlotChain*>::iterator input = (*iter)->type().slotChains().begin(); input != (*iter)->type().slotChains().end(); ++input) {
        for (Set<Instance*>::iterator jter = (*iter)->getInstances((*input)->id()).begin(); jter != (*iter)->getInstances((*input)->id()).end(); ++jter) {
          current *= (*input)->type().variable().domainSize();
        }
      }
      for (DAG::NodeIterator node = (*iter)->type().dag().beginNodes(); node != (*iter)->type().dag().endNodes(); ++node) {
        if ( (*iter)->type().isOutputNode((**iter).type().get(*node)) and (*iter)->hasRefAttr(*node) ) {
          current *= (*iter)->get(*node).type().variable().domainSize();
        }
      }
      gain += current;
    }
    __data[const_cast<Pattern*>(&p)]->gain = gain;
  }
  return (double) __data[const_cast<Pattern*>(&p)]->gain;
}

INLINE
void
DFSTree::__find_sub_pattern(Pattern& p, NodeId iso_map) {
  // PatternData& data = *(__data[&p]);
  // Sequence<Instance*>& seq = *(data.iso_map[iso_map]);
  // HashTable<ClassElement*, Size>* elt_map = new HashTable<ClassElement*, Size>();
  // ClassElement* elt = 0;
  // for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
  //   for (Set<SlotChain*>::iterator sc = (*iter)->slotChains().begin();
  //        sc != (*iter)->slotChains().end(); ++sc) {
  //     elt = const_cast<ClassElement*>(&((*sc)->lastElt()));
  //     if (not elt_map->exists(elt)) {
  //       elt_map->insert(elt, 0);
  //     }
  //     for (Set<Instance*>::iterator i = (*iter)->getInstances((*sc)->id()).begin();
  //          i != (*iter)->getInstances((*sc)->id()).end(); ++i) {
  //       if (not seq.exists(*i)) {
  //         ++((*elt_map)[elt]);
  //       }
  //     }
  //     if ((*elt_map)[elt] == 0) {
  //       elt_map->erase(elt);
  //     }
  //   }
  // }
  // typedef Sequence< HashTable<ClassElement*, Size>* >::iterator SubPatIter;
  // for (SubPatIter iter = data.sub_patterns.begin(); iter != data.sub_patterns.end(); ++iter) {
  //   if (__test_equality(**iter, *elt_map)) {
  //     data.sub_patterns_map.insert(iso_map, data.sub_patterns.pos(*iter));
  //     ++(data.sub_patterns_count[data.sub_patterns.pos(*iter)]);
  //     break;
  //   }
  // }
  // if (data.sub_patterns_map.exists(iso_map)) {
  //   delete elt_map;
  // } else {
  //   data.sub_patterns_map.insert(iso_map, data.sub_patterns.size());
  //   data.sub_patterns.insert(elt_map);
  //   data.sub_patterns_count.push_back((Size) 1);
  // }
}

INLINE
bool
DFSTree::__test_equality(HashTable<ClassElement*, Size>& x, HashTable<ClassElement*, Size>& y) {
  for (HashTable<ClassElement*, Size>::iterator iter = x.begin(); iter != x.end(); ++iter) {
    try {
      if (y[iter.key()] != (*iter))
        return false;
    } catch (NotFound&) {
      return false;
    }
  }
  return true;
}

// ============================================================================
// PatternData
// ============================================================================

DFSTree::PatternData::PatternData(const PatternData& from):
  pattern(from.pattern), children(from.children), iso_graph(from.iso_graph),
  max_indep_set(from.max_indep_set), cost(from.cost), gain(from.gain)
{
  GUM_CONS_CPY( DFSTree::PatternData );
  typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
  for (Iter iter = from.iso_map.begin(); iter != from.iso_map.end(); ++iter) {
    iso_map.insert(iter.key(), new Sequence<Instance*>(**iter));
  }
}

DFSTree::PatternData::~PatternData() {
  GUM_DESTRUCTOR( DFSTree::PatternData );
  typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
  for (Iter iter = iso_map.begin(); iter != iso_map.end(); ++iter) {
    delete *iter;
  }
  typedef Sequence< HashTable<ClassElement*, Size>* >::iterator SubPatIter;
  for (SubPatIter iter = sub_patterns.begin(); iter != sub_patterns.end(); ++iter) {
    delete *iter;
  }
}


} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
