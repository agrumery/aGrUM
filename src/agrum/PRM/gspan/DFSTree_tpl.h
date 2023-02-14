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
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/gspan/DFSTree.h>
#include <agrum/PRM/gspan/edgeGrowth.h>

namespace gum {
  namespace prm {
    namespace gspan {
      template < typename GUM_SCALAR >
      DFSTree< GUM_SCALAR >::~DFSTree() {
        GUM_DESTRUCTOR(DFSTree);

        for (const auto& elt: _data_) {
          delete elt.first;
          delete elt.second;
        }

        delete _strategy_;
      }

      template < typename GUM_SCALAR >
      void DFSTree< GUM_SCALAR >::addRoot(LabelData& label) {
        HashTable< Pattern*, std::pair< Idx, Idx > >                roots;
        HashTable< Pattern*, Sequence< EdgeData< GUM_SCALAR >* >* > roots_edges;

        for (const auto& edge: _graph_->edges(&label)) {
          bool u_first = (edge->l_u->id < edge->l_v->id);
          Idx  u_idx   = (u_first) ? edge->l_u->id : edge->l_v->id;
          Idx  v_idx   = (!u_first) ? edge->l_u->id : edge->l_v->id;

          bool found = false;

          for (const auto& elt: roots)
            if ((elt.second.first == u_idx) && (elt.second.second == v_idx)) {
              roots_edges[elt.first]->insert(edge);
              found = true;
              break;
            }

          /// Then we create a new pattern
          if (!found) {
            Pattern* p = new Pattern();
            roots.insert(p, std::make_pair(u_idx, v_idx));
            roots_edges.insert(p, new Sequence< EdgeData< GUM_SCALAR >* >());
            roots_edges[p]->insert(edge);
            DFSTree< GUM_SCALAR >::PatternData* data = new DFSTree< GUM_SCALAR >::PatternData(p);
            NodeId u = p->addNodeWithLabel((u_first) ? *edge->l_u : *edge->l_v);
            NodeId v = p->addNodeWithLabel((!u_first) ? *edge->l_u : *edge->l_v);
            p->addArc(u, v, label);
            _node_map_.insert(DiGraph::addNode(), p);
            _data_.insert(p, data);
            _roots_.push_back(_node_map_.first(p));
          }
        }

        // This is used to compute the max independent set of p->max_indep_set
        for (const auto& elt: roots_edges) {
          _initialiaze_root_(elt.first, *elt.second);
          strategy().accept_root(elt.first);
          delete elt.second;
        }
      }

      template < typename GUM_SCALAR >
      void
         DFSTree< GUM_SCALAR >::_initialiaze_root_(Pattern*                             p,
                                                   Sequence< EdgeData< GUM_SCALAR >* >& edge_seq) {
        DFSTree< GUM_SCALAR >::PatternData* data = _data_[p];
        std::vector< NodeId >               degree_list;

        for (auto iter = edge_seq.begin(); iter != edge_seq.end(); ++iter) {
          const auto&                             edge = *iter;
          Sequence< PRMInstance< GUM_SCALAR >* >* seq
             = new Sequence< PRMInstance< GUM_SCALAR >* >();

          // Creating the multiset of instances matching p
          bool u_first = (edge->l_u->id < edge->l_v->id);
          seq->insert((u_first) ? edge->u : edge->v);
          seq->insert((!u_first) ? edge->u : edge->v);

          NodeId an_id = data->iso_graph.addNode();
          data->iso_map.insert(an_id, seq);
          degree_list.push_back(an_id);

          // Adding edges between two isomorphisms of p sharing at least one
          // instance
          for (const auto& elt: data->iso_map)
            if (elt.first != an_id)
              for (auto iter = elt.second->begin(); iter != elt.second->end(); ++iter)
                if (seq->exists(*iter)) {
                  data->iso_graph.addEdge(an_id, elt.first);
                  break;
                }
        }

        // Computing p->max_indep_set using a greedy algorithm
        DFSTree< GUM_SCALAR >::NeighborDegreeSort my_operator(data->iso_graph);
        std::sort(degree_list.begin(), degree_list.end(), my_operator);
        Set< NodeId > removed;

        for (const auto node: degree_list) {
          if (!removed.exists(node)) {
            removed.insert(node);

            for (const auto neighbor: data->iso_graph.neighbours(node))
              removed.insert(neighbor);

            data->max_indep_set.insert(node);
          }
        }
      }

      template < typename GUM_SCALAR >
      bool DFSTree< GUM_SCALAR >::_is_new_seq_(
         Sequence< PRMInstance< GUM_SCALAR >* >&                  seq,
         NodeProperty< Sequence< PRMInstance< GUM_SCALAR >* >* >& iso_map) {
        for (const auto& elt: iso_map) {
          bool found = false;

          for (const auto& inst: seq)
            if (!(elt.second->exists(inst))) {
              found = true;
              break;
            }

          if (!found) { return false; }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      void DFSTree< GUM_SCALAR >::_addChild_(Pattern&                  p,
                                             Pattern*                  child,
                                             EdgeGrowth< GUM_SCALAR >& edge_growth) {
        // Adding child to the tree
        NodeId node = DiGraph::addNode();
        _node_map_.insert(node, child);
        // Adding child in p's children list
        std::list< NodeId >& children = _data_[&p]->children;

        if (children.empty()) {
          children.push_back(node);
        } else {
          size_t size = children.size();

          for (std::list< NodeId >::iterator iter = children.begin(); iter != children.end();
               ++iter) {
            if (child->code() < pattern(*iter).code()) {
              children.insert(iter, node);
              break;
            }
          }

          if (size == children.size()) { children.push_back(node); }
        }
      }

      template < typename GUM_SCALAR >
      void DFSTree< GUM_SCALAR >::_checkGrowth_(Pattern&                  p,
                                                Pattern*                  child,
                                                EdgeGrowth< GUM_SCALAR >& edge_growth) {
        NodeId v = edge_growth.v;

        // First we check if the edge is legal
        if (v == 0) { v = child->addNodeWithLabel(*(edge_growth.l_v)); }

        child->addArc(edge_growth.u, v, *(edge_growth.edge));
        // Neighborhood restriction is checked by the Pattern class
        const EdgeCode& edge = child->edgeCode(edge_growth.u, v);

        // Then we check if the edge we added is valid
        if (edge < *(child->code().codes.front())) {
          GUM_ERROR(OperationNotAllowed,
                    "added edge code is lesser than the first "
                    "one in the pattern's DFSCode");
        }

        if (edge.isBackward()) {
          for (auto iter = child->code().codes.begin(); (iter + 1) != child->code().codes.end();
               ++iter) {
            if ((((**iter).i == v) || ((**iter).j == v)) && edge < (**iter)) {
              GUM_ERROR(OperationNotAllowed,
                        "added backward edge is lesser than an existing edge on v");
            }
          }
        }

        // Finally, we check if child is minimal.
        if (!child->isMinimal()) {
          GUM_ERROR(OperationNotAllowed, "the DFSCode for this growth is not minimal")
        }
      }

      template < typename GUM_SCALAR >
      Pattern& DFSTree< GUM_SCALAR >::growPattern(Pattern&                  p,
                                                  EdgeGrowth< GUM_SCALAR >& edge_growth,
                                                  Size                      min_freq) {
        auto* child = new Pattern(p);

        try {
          _checkGrowth_(p, child, edge_growth);
        } catch (OperationNotAllowed const&) {
          delete child;
          throw;
        }

        // Now we need to build the pattern data about child
        auto*                 data = new DFSTree< GUM_SCALAR >::PatternData(child);
        std::vector< NodeId > degree_list;
        NodeProperty< Sequence< PRMInstance< GUM_SCALAR >* >* >& p_iso_map = _data_[&p]->iso_map;
        // typename NodeProperty< std::pair< PRMInstance< GUM_SCALAR >*,
        //                                   PRMInstance< GUM_SCALAR >* > >::iterator_safe match;
        //  Using p information to build child's isomorphism graph
        NodeId id = 0;

        for (const auto& elt: p_iso_map) {
          auto match = edge_growth.matches.begin();

          for (; match != edge_growth.matches.end(); ++match) {
            // Adding the isomorphism in the iso_graph and building the iso_map.
            if (child->code().codes.back()->isForward()) {
              if (elt.second->exists(match.val().first)
                  && !(elt.second->exists(match.val().second))) {
                // Let's see if the new match is already matched
                auto* new_seq = new Sequence< PRMInstance< GUM_SCALAR >* >(*elt.second);
                new_seq->insert(match.val().second);

                if (_is_new_seq_(*new_seq, data->iso_map)) {
                  id = data->iso_graph.addNode();
                  data->iso_map.insert(id, new_seq);
                } else {
                  delete new_seq;
                }

                break;
              }
            } else {
              if (elt.second->exists(match.val().first) && elt.second->exists(match.val().second)) {
                Sequence< PRMInstance< GUM_SCALAR >* >* new_seq
                   = new Sequence< PRMInstance< GUM_SCALAR >* >(*elt.second);

                if (_is_new_seq_(*new_seq, data->iso_map)) {
                  id = data->iso_graph.addNode();
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
            for (const auto node: data->iso_graph.nodes())
              if (node != id)
                for (const auto m: *data->iso_map[id])
                  if (data->iso_map[node]->exists(m)) {
                    data->iso_graph.addEdge(node, id);
                    break;
                  }

            degree_list.push_back(id);
            edge_growth.matches.erase(match.key());
          }
        }

        if (data->iso_graph.size() < min_freq) {
          delete data;
          delete child;
          GUM_ERROR(OperationNotAllowed, "child is not frequent enough")
        }

        // Now we can compute the maximal independent set of child
        DFSTree< GUM_SCALAR >::NeighborDegreeSort my_operator(data->iso_graph);
        std::sort(degree_list.begin(), degree_list.end(), my_operator);
        Set< NodeId > removed;

        for (const auto node: degree_list) {
          if (!removed.exists(node)) {
            removed.insert(node);

            for (const auto neighbor: data->iso_graph.neighbours(node))
              removed.insert(neighbor);

            data->max_indep_set.insert(node);
          }
        }

        _data_.insert(child, data);

        if (!_strategy_->accept_growth(&p, child, edge_growth)) {
          _data_.erase(child);
          delete data;
          delete child;
          GUM_ERROR(OperationNotAllowed, "child is not frequent enough")
        }

        _addChild_(p, child, edge_growth);
        return *child;
      }

      template < typename GUM_SCALAR >
      bool DFSTree< GUM_SCALAR >::_test_equality_(
         HashTable< PRMClassElement< GUM_SCALAR >*, Size >& x,
         HashTable< PRMClassElement< GUM_SCALAR >*, Size >& y) {
        try {
          for (const auto& elt: x)
            if (y[elt.first] != elt.second) return false;
        } catch (NotFound const&) { return false; }

        return true;
      }

      // PatternData
      template < typename GUM_SCALAR >
      DFSTree< GUM_SCALAR >::PatternData::PatternData(const PatternData& from) :
          pattern(from.pattern), children(from.children), iso_graph(from.iso_graph),
          max_indep_set(from.max_indep_set), cost(from.cost), gain(from.gain) {
        GUM_CONS_CPY(DFSTree< GUM_SCALAR >::PatternData);

        for (const auto& elt: from.iso_map)
          iso_map.insert(elt.first, new Sequence< PRMInstance< GUM_SCALAR >* >(*elt.second));
      }

      template < typename GUM_SCALAR >
      DFSTree< GUM_SCALAR >::PatternData::~PatternData() {
        GUM_DESTRUCTOR(DFSTree< GUM_SCALAR >::PatternData);

        for (const auto& elt: iso_map)
          delete elt.second;
      }

      template < typename GUM_SCALAR >
      INLINE DFSTree< GUM_SCALAR >::DFSTree(const InterfaceGraph< GUM_SCALAR >&  graph,
                                            gspan::SearchStrategy< GUM_SCALAR >* strategy) :
          _graph_(&graph),
          _strategy_(strategy) {
        GUM_CONSTRUCTOR(DFSTree);

        if (!_strategy_) _strategy_ = new FrequenceSearch< GUM_SCALAR >(2);

        _strategy_->setTree(this);
      }

      template < typename GUM_SCALAR >
      INLINE std::list< NodeId >& DFSTree< GUM_SCALAR >::roots() {
        return _roots_;
      }

      template < typename GUM_SCALAR >
      INLINE const std::list< NodeId >& DFSTree< GUM_SCALAR >::roots() const {
        return _roots_;
      }

      template < typename GUM_SCALAR >
      INLINE Pattern& DFSTree< GUM_SCALAR >::parent(const Pattern& p) {
        try {
          return *(_node_map_.second(
             *(DiGraph::parents(_node_map_.first(const_cast< Pattern* >(&p))).begin())));
        } catch (NotFound const&) {
          if (_node_map_.existsSecond(const_cast< Pattern* >(&p))) {
            GUM_ERROR(NotFound, "the given pattern is a root node")
          } else {
            GUM_ERROR(NotFound, "pattern not found in this DFSTree")
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE const Pattern& DFSTree< GUM_SCALAR >::parent(const Pattern& p) const {
        try {
          return *(_node_map_.second(
             *(DiGraph::parents(_node_map_.first(const_cast< Pattern* >(&p))).begin())));
        } catch (NotFound const&) {
          if (_node_map_.existsSecond(const_cast< Pattern* >(&p))) {
            GUM_ERROR(NotFound, "the given pattern is a root node")
          } else {
            GUM_ERROR(NotFound, "pattern not found in this DFSTree")
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE std::list< NodeId >& DFSTree< GUM_SCALAR >::children(const Pattern& p) {
        try {
          return _data_[const_cast< Pattern* >(&p)]->children;
        } catch (NotFound const&) { GUM_ERROR(NotFound, "pattern not found in this DFSTree") }
      }

      template < typename GUM_SCALAR >
      INLINE const std::list< NodeId >& DFSTree< GUM_SCALAR >::children(const Pattern& p) const {
        try {
          return _data_[const_cast< Pattern* >(&p)]->children;
        } catch (NotFound const&) { GUM_ERROR(NotFound, "pattern not found in this DFSTree") }
      }

      template < typename GUM_SCALAR >
      INLINE Pattern& DFSTree< GUM_SCALAR >::pattern(NodeId id) {
        try {
          return *(_node_map_.second(id));
        } catch (NotFound const&) { GUM_ERROR(NotFound, "no pattern matching the given id") }
      }

      template < typename GUM_SCALAR >
      INLINE const Pattern& DFSTree< GUM_SCALAR >::pattern(NodeId id) const {
        try {
          return *(_node_map_.second(id));
        } catch (NotFound const&) { GUM_ERROR(NotFound, "no pattern matching the given id") }
      }

      template < typename GUM_SCALAR >
      INLINE UndiGraph& DFSTree< GUM_SCALAR >::iso_graph(const Pattern& p) {
        try {
          return _data_[const_cast< Pattern* >(&p)]->iso_graph;
        } catch (NotFound const&) { GUM_ERROR(NotFound, "pattern not found in this DFSTree") }
      }

      template < typename GUM_SCALAR >
      INLINE Sequence< PRMInstance< GUM_SCALAR >* >&
             DFSTree< GUM_SCALAR >::iso_map(const Pattern& p, NodeId node) {
        try {
          return *(_data_[const_cast< Pattern* >(&p)]->iso_map[node]);
        } catch (NotFound const&) {
          if (_data_.exists(const_cast< Pattern* >(&p))) {
            GUM_ERROR(NotFound, "node not found in Pattern's isomorphism graph")
          } else {
            GUM_ERROR(NotFound, "pattern not found in this DFSTree")
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE Set< NodeId >& DFSTree< GUM_SCALAR >::max_indep_set(const Pattern& p) {
        try {
          return _data_[const_cast< Pattern* >(&p)]->max_indep_set;
        } catch (NotFound const&) { GUM_ERROR(NotFound, "pattern not found in this DFSTree") }
      }

      template < typename GUM_SCALAR >
      INLINE const InterfaceGraph< GUM_SCALAR >& DFSTree< GUM_SCALAR >::graph() const {
        return *_graph_;
      }

      template < typename GUM_SCALAR >
      INLINE std::ostream& operator<<(std::ostream& out, const EdgeGrowth< GUM_SCALAR >& edge) {
        out << edge.u << ", " << *(edge.edge) << ", " << *(edge.l_v) << ", " << edge.v;
        return out;
      }

      template < typename GUM_SCALAR >
      INLINE double DFSTree< GUM_SCALAR >::frequency(const Pattern& p) const {
        return (double)_data_[const_cast< Pattern* >(&p)]->max_indep_set.size();
      }

      template < typename GUM_SCALAR >
      INLINE typename DFSTree< GUM_SCALAR >::PatternData&
         DFSTree< GUM_SCALAR >::data(const Pattern& p) {
        return *(_data_[const_cast< Pattern* >(&p)]);
      }

      template < typename GUM_SCALAR >
      INLINE const typename DFSTree< GUM_SCALAR >::PatternData&
         DFSTree< GUM_SCALAR >::data(const Pattern& p) const {
        return *(_data_[const_cast< Pattern* >(&p)]);
      }

      template < typename GUM_SCALAR >
      INLINE SearchStrategy< GUM_SCALAR >& DFSTree< GUM_SCALAR >::strategy() {
        return *_strategy_;
      }

      template < typename GUM_SCALAR >
      INLINE const SearchStrategy< GUM_SCALAR >& DFSTree< GUM_SCALAR >::strategy() const {
        return *_strategy_;
      }

      // NeighborDegreeSort

      template < typename GUM_SCALAR >
      INLINE DFSTree< GUM_SCALAR >::NeighborDegreeSort::NeighborDegreeSort(UndiGraph& graph) :
          g(graph) {
        GUM_CONSTRUCTOR(DFSTree< GUM_SCALAR >::NeighborDegreeSort);
      }

      template < typename GUM_SCALAR >
      INLINE DFSTree< GUM_SCALAR >::NeighborDegreeSort::NeighborDegreeSort(
         const NeighborDegreeSort& source) :
          g(source.g) {
        GUM_CONS_CPY(DFSTree< GUM_SCALAR >::NeighborDegreeSort);
      }

      template < typename GUM_SCALAR >
      INLINE DFSTree< GUM_SCALAR >::NeighborDegreeSort::~NeighborDegreeSort() {
        GUM_DESTRUCTOR(DFSTree< GUM_SCALAR >::NeighborDegreeSort);
      }

      template < typename GUM_SCALAR >
      INLINE bool DFSTree< GUM_SCALAR >::NeighborDegreeSort::operator()(NodeId i, NodeId j) {
        return g.neighbours(i).size() < g.neighbours(j).size();
      }

      // PatternData

      template < typename GUM_SCALAR >
      INLINE DFSTree< GUM_SCALAR >::PatternData::PatternData(Pattern* p) :
          pattern(p), cost(0), gain(0) {
        GUM_CONSTRUCTOR(DFSTree< GUM_SCALAR >::PatternData);
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
