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
 * @brief Inline implementation of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/gspan/edgeGrowth.h>
#include <agrum/PRM/inference/gspan.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void GSpan< GUM_SCALAR >::discoverPatterns() {
      Timer t;
      _sortNodesAndEdges_();
      gspan::InterfaceGraph< GUM_SCALAR > graph(*_graph_);

      for (auto root = _tree_.roots().begin(); root != _tree_.roots().end(); ++root) {
        if (_tree_.strategy().accept_root(&(_tree_.pattern(*root)))) {
          gspan::Pattern& p = _tree_.pattern(*root);
          _subgraph_mining_(graph, p);

          for (const auto node: _tree_.iso_graph(p).nodes()) {
            PRMInstance< GUM_SCALAR >* u = _tree_.iso_map(p, node).atPos(0);
            PRMInstance< GUM_SCALAR >* v = _tree_.iso_map(p, node).atPos(1);
            graph.graph().eraseEdge(Edge(graph.id(u), graph.id(v)));
          }
        }
      }

      _sortPatterns_();
    }

    template < typename GUM_SCALAR >
    void GSpan< GUM_SCALAR >::_sortNodesAndEdges_() {
      for (auto iter = _graph_->labels().begin(); iter != _graph_->labels().end(); ++iter) {
        try {
          if (_graph_->nodes(iter.second()).size() >= 2) {
            _cost_.insert(
               iter.second(),
               _cost_func_(iter.second()->tree_width, _graph_->nodes(iter.second()).size()));
            _nodes_.push_back(const_cast< gspan::LabelData* >(iter.second()));
          }
        } catch (NotFound const&) {
          // It's a label over edges
          if (_isEdgeEligible_(*(_graph_->edges(iter.second()).begin()))) {
            _cost_.insert(
               iter.second(),
               _cost_func_(iter.second()->tree_width, _graph_->edges(iter.second()).size()));
            _edges_.push_back(iter.second());
          }
        }
      }

      Bijection< Idx, gspan::LabelData* >* new_labels = new Bijection< Idx, gspan::LabelData* >();
      GSpan< GUM_SCALAR >::LabelSort       my_sort(this);
      std::sort(_nodes_.begin(), _nodes_.end(), my_sort);
      std::sort(_edges_.begin(), _edges_.end(), my_sort);
      Size idx = 0;

      for (auto iter = _nodes_.begin(); iter != _nodes_.end(); ++iter) {
        (*iter)->id = ++idx;
        new_labels->insert(idx, *iter);
      }

      for (auto iter = _edges_.begin(); iter != _edges_.end(); ++iter) {
        (*iter)->id = ++idx;
        new_labels->insert(idx, *iter);
        _tree_.addRoot(**iter);
      }

      delete _graph_->_labels_;
      _graph_->_labels_ = new_labels;
    }

    template < typename GUM_SCALAR >
    void GSpan< GUM_SCALAR >::_subgraph_mining_(gspan::InterfaceGraph< GUM_SCALAR >& ig,
                                                gspan::Pattern&                      pat) {
      std::vector< gspan::Pattern* > stack;
      stack.push_back(&pat);
      // Pointers used in the following while
      gspan::Pattern*                                             p           = nullptr;
      HashTable< std::string, gspan::EdgeGrowth< GUM_SCALAR >* >* edge_count  = nullptr;
      gspan::EdgeGrowth< GUM_SCALAR >*                            edge_growth = nullptr;
      Sequence< PRMInstance< GUM_SCALAR >* >*                     seq         = nullptr;
      PRMInstance< GUM_SCALAR >*                                  current     = nullptr;
      PRMInstance< GUM_SCALAR >*                                  neighbor    = nullptr;

      // Neighbor_id is the neighbor's id in the interface graph and
      // neighbor_node
      // is its id in the rightmost path in the case of a backward edge growth
      NodeId            current_id     = 0;
      NodeId            neighbor_node  = 0;
      gspan::LabelData* neighbor_label = 0;

      typename gspan::EdgeData< GUM_SCALAR >* edge_data = nullptr;

      size_t                     idx;
      const std::list< NodeId >* children = 0;

      while (!stack.empty()) {
        // Getting next pattern
        p = stack.back();
        stack.pop_back();

        if (p->code().codes.size() < _depth_stop_) {
          // We need the rightmost path of p
          std::list< NodeId > r_path;
          p->rightmostPath(r_path);
          // Mapping used to count each possible child of p, the position in the
          // vector
          // matches the one in the rightmost path
          std::vector< HashTable< std::string, gspan::EdgeGrowth< GUM_SCALAR >* >* > count_vector;

          for (size_t i = 0; i < r_path.size(); ++i)
            count_vector.push_back(
               new HashTable< std::string, gspan::EdgeGrowth< GUM_SCALAR >* >());

          // For each subgraph represented by p, we look for a valid edge growth
          // for
          // each instance match of p in its isomorphism graph.
          for (const auto iso_node: _tree_.iso_graph(*p).nodes()) {
            seq = &(_tree_.iso_map(*p, iso_node));
            idx = 0;

            for (const auto node: r_path) {
              edge_count = count_vector[idx];
              // Retrieving the equivalent instance in the current match
              current    = seq->atPos((Idx)(node - 1));
              current_id = ig.id(current);
              // Checking for edges not in p

              for (const auto neighbor_id: ig.graph().neighbours(current_id)) {
                neighbor = ig.node(neighbor_id).n;

                // We want a forward edge in any case or a backward edge if
                // current
                // is the rightmost vertex
                if ((!seq->exists(neighbor)) || (node == r_path.back())) {
                  // Things we need to know: the LabelData data of the neighbour
                  // and,
                  // if it's a backward edge, its node id in the rightmost path
                  edge_data      = &(ig.edge(current_id, neighbor_id));
                  neighbor_label = (neighbor == edge_data->u) ? edge_data->l_u : edge_data->l_v;
                  neighbor_node  = (seq->exists(neighbor)) ? seq->pos(neighbor) + 1 : 0;
                  // Adding the edge growth to the edge_growth hashtable
                  gspan::EdgeGrowth< GUM_SCALAR > temp_growth(node,
                                                              edge_data->l,
                                                              neighbor_label,
                                                              neighbor_node);

                  try {
                    edge_growth = (*edge_count)[temp_growth.toString()];
                    edge_growth->insert(current, neighbor);
                  } catch (NotFound const&) {
                    edge_growth = new gspan::EdgeGrowth< GUM_SCALAR >(node,
                                                                      edge_data->l,
                                                                      neighbor_label,
                                                                      neighbor_node);
                    edge_growth->insert(current, neighbor);
                    edge_count->insert(edge_growth->toString(), edge_growth);
                  }
                }
              }
            }
          }

          // Removing any infrequent child
          for (size_t node = 0; node < count_vector.size(); ++node) {
            edge_count = count_vector[node];

            for (const auto& elt: *edge_count) {
              try {
                _tree_.growPattern(*p, *elt.second, 2);
              } catch (OperationNotAllowed const&) {
                // The child was not minimal or was not worth considering
              }

              delete elt.second;
            }

            delete edge_count;
          }

          // Calling  _subgraph_mining_ over children of p
          children = &(_tree_.children(*p));

          for (std::list< NodeId >::const_reverse_iterator child = children->rbegin();
               child != children->rend();
               ++child)
            stack.push_back(&(_tree_.pattern(*child)));
        }
      }
    }

    template < typename GUM_SCALAR >
    void GSpan< GUM_SCALAR >::_sortPatterns_() {
      // First we put all the patterns in  _patterns_.
      std::vector< NodeId > stack;

      for (std::list< NodeId >::reverse_iterator root = tree().roots().rbegin();
           root != tree().roots().rend();
           ++root)
        stack.push_back(*root);

      NodeId               id       = 0;
      std::list< NodeId >* children = nullptr;

      while (!stack.empty()) {
        id = stack.back();
        stack.pop_back();
        _patterns_.push_back(&(tree().pattern(id)));
        children = &(tree().children(tree().pattern(id)));

        for (std::list< NodeId >::reverse_iterator child = children->rbegin();
             child != children->rend();
             ++child)
          stack.push_back(*child);
      }

      if (!_patterns_.empty()) {
        // We sort  _patterns_.
        GSpan< GUM_SCALAR >::PatternSort my_sort(this);
        std::sort(_patterns_.begin(), _patterns_.end(), my_sort);
        // Now we need to find all the matches we can, using  _patterns_.
        // We start by the best Pattern and add it's maximal independent set to
        //  _chosen_
        GSpan< GUM_SCALAR >::MatchedInstances* matches
           = new GSpan< GUM_SCALAR >::MatchedInstances();
        Sequence< PRMInstance< GUM_SCALAR >* >* match = nullptr;

        for (const auto node: tree().max_indep_set(*(_patterns_.front()))) {
          match = &(tree().iso_map(*(_patterns_.front()), node));

          for (const auto i: *match)
            _chosen_.insert(i);

          matches->insert(match);
        }

        _matched_instances_.insert(_patterns_.front(), matches);
        // Now we see what kind of pattern we can still use
        bool       found;
        UndiGraph* iso_graph = nullptr;

        for (auto patt = _patterns_.begin() + 1; patt != _patterns_.end(); ++patt) {
          UndiGraph             reduced_iso_graph;
          std::vector< NodeId > degree_list;
          iso_graph = &(tree().iso_graph(**patt));

          for (const auto node: iso_graph->nodes()) {
            found = false;
            match = &(tree().iso_map(**patt, node));

            for (const auto i: *match)
              if (_chosen_.exists(i)) {
                found = true;
                break;
              }

            if (!found) {
              // We add the pattern to the reduced isomorphism graph to compute
              // the
              // max independent set
              // over the remaining matches
              reduced_iso_graph.addNodeWithId(node);

              for (const auto iso: reduced_iso_graph.nodes())
                if (iso_graph->existsEdge(node, iso)) reduced_iso_graph.addEdge(node, iso);

              degree_list.push_back(node);
            }
          }

          // We create a new set to hold all the chosen matches of patt
          matches = new GSpan< GUM_SCALAR >::MatchedInstances();
          // We can compute the max independent set and the matches belonging to
          // it
          typename gspan::DFSTree< GUM_SCALAR >::NeighborDegreeSort my_sort(reduced_iso_graph);
          std::sort(degree_list.begin(), degree_list.end(), my_sort);
          Set< NodeId > removed;

          for (const auto node: degree_list)
            if (!removed.exists(node)) {
              // First we update removed to follow the max independent set
              // algorithm
              removed.insert(node);

              for (const auto neighbor: reduced_iso_graph.neighbours(node))
                removed.insert(neighbor);

              // Second we update match and matches to keep track of the current
              // match
              match = &(tree().iso_map(**patt, node));
              matches->insert(match);

              for (const auto elt: *match)
                _chosen_.insert(elt);
            }

          _matched_instances_.insert(*patt, matches);
        }

        // // We remove patterns with 0 matches
        std::vector< size_t > trash;

        for (size_t idx = 0; idx < _patterns_.size(); ++idx)
          if (_matched_instances_[_patterns_[idx]]->size() < 2) trash.push_back(idx);

        while (trash.size()) {
          delete _matched_instances_[_patterns_[trash.back()]];
          _matched_instances_.erase(_patterns_[trash.back()]);
          // delete  _patterns_[trash.back()];
          _patterns_[trash.back()] = _patterns_.back();
          _patterns_.pop_back();
          trash.pop_back();
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::GSpan(const PRM< GUM_SCALAR >&             prm,
                                      const PRMSystem< GUM_SCALAR >&       sys,
                                      gspan::SearchStrategy< GUM_SCALAR >* strategy) :
        _graph_(new gspan::InterfaceGraph< GUM_SCALAR >(sys)),
        _tree_(*_graph_, strategy), _depth_stop_(INT_MAX) {
      GUM_CONSTRUCTOR(GSpan);
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::~GSpan() {
      GUM_DESTRUCTOR(GSpan);

      for (const auto& elt: _matched_instances_)
        delete elt.second;

      delete _graph_;
    }

    template < typename GUM_SCALAR >
    INLINE Size GSpan< GUM_SCALAR >::getMaxDFSDepth() const {
      return _depth_stop_;
    }

    template < typename GUM_SCALAR >
    INLINE void GSpan< GUM_SCALAR >::setMaxDFSDepth(Size depth) {
      _depth_stop_ = depth;
    }

    template < typename GUM_SCALAR >
    INLINE gspan::DFSTree< GUM_SCALAR >& GSpan< GUM_SCALAR >::tree() {
      return _tree_;
    }

    template < typename GUM_SCALAR >
    INLINE const gspan::DFSTree< GUM_SCALAR >& GSpan< GUM_SCALAR >::tree() const {
      return _tree_;
    }

    template < typename GUM_SCALAR >
    INLINE Idx GSpan< GUM_SCALAR >::_cost_func_(Size interface_size, Size frequency) {
      return Idx(interface_size * frequency);
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< gspan::Pattern* >& GSpan< GUM_SCALAR >::patterns() {
      return _patterns_;
    }

    template < typename GUM_SCALAR >
    INLINE const std::vector< gspan::Pattern* >& GSpan< GUM_SCALAR >::patterns() const {
      return _patterns_;
    }

    template < typename GUM_SCALAR >
    INLINE typename GSpan< GUM_SCALAR >::MatchedInstances&
       GSpan< GUM_SCALAR >::matches(const gspan::Pattern& p) {
      return *(_matched_instances_[const_cast< gspan::Pattern* >(&p)]);
    }

    template < typename GUM_SCALAR >
    INLINE const typename GSpan< GUM_SCALAR >::MatchedInstances&
       GSpan< GUM_SCALAR >::matches(const gspan::Pattern& p) const {
      return *(_matched_instances_[const_cast< gspan::Pattern* >(&p)]);
    }

    template < typename GUM_SCALAR >
    INLINE gspan::InterfaceGraph< GUM_SCALAR >& GSpan< GUM_SCALAR >::interfaceGraph() {
      return *_graph_;
    }

    template < typename GUM_SCALAR >
    INLINE const gspan::InterfaceGraph< GUM_SCALAR >& GSpan< GUM_SCALAR >::interfaceGraph() const {
      return *_graph_;
    }

    template < typename GUM_SCALAR >
    INLINE bool GSpan< GUM_SCALAR >::_isEdgeEligible_(gspan::EdgeData< GUM_SCALAR >* e) {
      return (_graph_->edges(e->l).size() >= 2) && (_graph_->nodes(e->l_u).size() >= 2)
          && (_graph_->nodes(e->l_v).size() >= 2);
    }

    // LalbeSort

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::LabelSort::LabelSort(GSpan* my_gspan) : gspan(my_gspan) {
      GUM_CONSTRUCTOR(GSpan< GUM_SCALAR >::LabelSort);
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::LabelSort::LabelSort(const LabelSort& source) :
        gspan(source.gspan) {
      GUM_CONS_CPY(GSpan< GUM_SCALAR >::LabelSort);
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::LabelSort::~LabelSort() {
      GUM_DESTRUCTOR(GSpan< GUM_SCALAR >::LabelSort);
    }

    template < typename GUM_SCALAR >
    INLINE bool GSpan< GUM_SCALAR >::LabelSort::operator()(gspan::LabelData* i,
                                                           gspan::LabelData* j) {
      // We want a descending order
      // return gspan-> _cost_[i] > gspan-> _cost_[j];
      return gspan->_tree_.strategy()(i, j);
    }

    // PatternSort

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::PatternSort::PatternSort(GSpan* my_gspan) : gspan(my_gspan) {
      GUM_CONSTRUCTOR(GSpan< GUM_SCALAR >::PatternSort);
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::PatternSort::PatternSort(const PatternSort& source) :
        gspan(source.gspan) {
      GUM_CONS_CPY(GSpan< GUM_SCALAR >::PatternSort);
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >::PatternSort::~PatternSort() {
      GUM_DESTRUCTOR(GSpan< GUM_SCALAR >::PatternSort);
    }

    template < typename GUM_SCALAR >
    INLINE bool GSpan< GUM_SCALAR >::PatternSort::operator()(gspan::Pattern* i, gspan::Pattern* j) {
      // We want a descending order
      return gspan->tree().strategy().operator()(i, j);
    }

  } /* namespace prm */
} /* namespace gum */
