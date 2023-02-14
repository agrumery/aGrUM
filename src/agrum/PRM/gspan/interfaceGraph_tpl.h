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
 * @brief Inline implementation of gum::InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {
    namespace gspan {

      // NodeData

      template < typename GUM_SCALAR >
      INLINE NodeData< GUM_SCALAR >::NodeData() : n(0), l(0) {
        GUM_CONSTRUCTOR(NodeData);
      }

      template < typename GUM_SCALAR >
      INLINE NodeData< GUM_SCALAR >::NodeData(const NodeData< GUM_SCALAR >& from) :
          n(from.n), l(from.l) {
        GUM_CONS_CPY(NodeData);
      }

      template < typename GUM_SCALAR >
      INLINE NodeData< GUM_SCALAR >::~NodeData() {
        GUM_DESTRUCTOR(NodeData);
      }

      template < typename GUM_SCALAR >
      INLINE bool NodeData< GUM_SCALAR >::operator==(const NodeData< GUM_SCALAR >& from) const {
        return (n == from.n) && (l == from.l);
      }

      template < typename GUM_SCALAR >
      INLINE bool NodeData< GUM_SCALAR >::operator!=(const NodeData< GUM_SCALAR >& from) const {
        return (n != from.n) && (l != from.l);
      }

      // EdgeData<GUM_SCALAR>

      template < typename GUM_SCALAR >
      INLINE EdgeData< GUM_SCALAR >::EdgeData() : u(0), v(0), l(0) {
        GUM_CONSTRUCTOR(EdgeData);
      }

      template < typename GUM_SCALAR >
      INLINE EdgeData< GUM_SCALAR >::EdgeData(const EdgeData< GUM_SCALAR >& from) :
          u(from.u), v(from.v), l(from.l) {
        GUM_CONS_CPY(EdgeData);
      }

      template < typename GUM_SCALAR >
      INLINE EdgeData< GUM_SCALAR >::~EdgeData() {
        GUM_DESTRUCTOR(EdgeData);
      }

      template < typename GUM_SCALAR >
      INLINE bool EdgeData< GUM_SCALAR >::operator==(const EdgeData< GUM_SCALAR >& from) const {
        return (u == from.u) && (l_u == from.l_u) && (v == from.v) && (l_v == from.l_v)
            && (l == from.l);
      }

      template < typename GUM_SCALAR >
      INLINE bool EdgeData< GUM_SCALAR >::operator!=(const EdgeData< GUM_SCALAR >& from) const {
        return (u != from.u) && (l_u != from.l_u) && (v != from.v) && (l_v != from.l_v)
            && (l != from.l);
      }

      // InterfaceGraph

      template < typename GUM_SCALAR >
      InterfaceGraph< GUM_SCALAR >::InterfaceGraph(const PRMSystem< GUM_SCALAR >& sys) :
          _sys_(&sys), _labels_(new Bijection< Idx, LabelData* >()), _counter_(0),
          _erase_flag_(true) {
        GUM_CONSTRUCTOR(InterfaceGraph);
        HashTable< std::string, LabelData* > label_map;

        // We need to add each instance in  _graph_
        for (auto iter = sys.begin(); iter != sys.end(); ++iter) {
          NodeData< GUM_SCALAR >* node = new NodeData< GUM_SCALAR >();
          node->n                      = iter.val();
          _label_(node, label_map);
          _graph_.addNodeWithId(iter.key());
          _idMap_.insert(node->n, iter.key());
          _nodes_.insert(iter.key(), node);
        }

        NodeData< GUM_SCALAR >* u = nullptr;
        NodeData< GUM_SCALAR >* v = nullptr;

        for (const auto& elt: _nodes_) {
          NodeData< GUM_SCALAR >* data = elt.second;

          for (const auto chain: data->n->type().slotChains()) {
            for (const auto inst: data->n->getInstances(chain->id())) {
              u = (_nodes_[_idMap_[inst]]->l < data->l) ? _nodes_[_idMap_[inst]] : data;
              v = (u != data) ? data : _nodes_[_idMap_[inst]];

              if (!_graph_.existsEdge(_idMap_[u->n], _idMap_[v->n])) {
                EdgeData< GUM_SCALAR >* edge = new EdgeData< GUM_SCALAR >();
                edge->u                      = u->n;
                edge->l_u                    = u->l;
                edge->v                      = v->n;
                edge->l_v                    = v->l;
                _label_(edge, label_map);
                _graph_.addEdge(_idMap_[u->n], _idMap_[v->n]);
                _edges_.insert(Edge(_idMap_[u->n], _idMap_[v->n]), edge);
              }
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      InterfaceGraph< GUM_SCALAR >::InterfaceGraph(const InterfaceGraph< GUM_SCALAR >& source) :
          _sys_(source._sys_), _graph_(source._graph_), _nodes_(source._nodes_),
          _idMap_(source._idMap_), _edges_(source._edges_),
          _labels_(new Bijection< Idx, LabelData* >(*(source._labels_))),
          _nodeMap_(source._nodeMap_), _edgeMap_(source._edgeMap_), _counter_(source._counter_),
          _erase_flag_(false) {
        GUM_CONS_CPY(InterfaceGraph);
      }

      template < typename GUM_SCALAR >
      InterfaceGraph< GUM_SCALAR >::~InterfaceGraph() {
        GUM_DESTRUCTOR(InterfaceGraph);

        if (_erase_flag_) {
          for (const auto& elt: _nodes_)
            delete elt.second;

          for (const auto& elt: _edges_)
            delete elt.second;

          for (const auto& elt: _nodeMap_) {
            delete elt.first;
            delete elt.second;
          }

          for (const auto& elt: _edgeMap_) {
            delete elt.first;
            delete elt.second;
          }
        }

        delete _labels_;
      }

      template < typename GUM_SCALAR >
      InterfaceGraph< GUM_SCALAR >&
         InterfaceGraph< GUM_SCALAR >::operator=(const InterfaceGraph< GUM_SCALAR >& source) {
        GUM_ERROR(FatalError, "not implemented")
      }

      template < typename GUM_SCALAR >
      void InterfaceGraph< GUM_SCALAR >::_label_(NodeData< GUM_SCALAR >*               node,
                                                 HashTable< std::string, LabelData* >& label_map) {
        Size              size = Size(1);
        std::stringstream sBuff;
        sBuff << node->n->type().name();

        // First we search for multiple inputs
        for (const auto chain: node->n->type().slotChains()) {
          if (chain->isMultiple()) {
            sBuff << "-" << node->n->getInstances(chain->id()).size();
            sBuff << chain->name();
            size *= node->n->getInstances(chain->id()).size()
                  * chain->lastElt().type().variable().domainSize();
          } else {
            size *= chain->lastElt().type().variable().domainSize();
          }
        }

        // Second we search for active outputs
        for (const auto nn: node->n->type().containerDag().nodes()) {
          if (node->n->type().isOutputNode(node->n->type().get(nn))) {
            try {
              sBuff << "-" << node->n->getRefAttr(nn).size() << node->n->get(nn).name();
              size *= node->n->get(nn).type().variable().domainSize();
            } catch (NotFound const&) {
              // (nn) is an inactive output node
            }
          }
        }

        // Label is ready
        if (!label_map.exists(sBuff.str())) {
          LabelData* label = new LabelData();
          label_map.insert(sBuff.str(), label);
          label->id         = ++_counter_;
          label->tree_width = size;
          label->l          = sBuff.str();
          _labels_->insert(label->id, label);
          _nodeMap_.insert(label, new Set< NodeData< GUM_SCALAR >* >());
        }

        node->l = label_map[sBuff.str()];
        _nodeMap_[node->l]->insert(node);
      }

      template < typename GUM_SCALAR >
      void InterfaceGraph< GUM_SCALAR >::_label_(EdgeData< GUM_SCALAR >*               edge,
                                                 HashTable< std::string, LabelData* >& label_map) {
        Size              size = Size(1);
        std::stringstream sBuff;
        sBuff << edge->u->type().name() << "-" << edge->v->type().name();

        // First looking for edge->u output nodes in v
        for (const auto chain: edge->u->type().slotChains()) {
          if (edge->u->getInstances(chain->id()).exists(edge->v)) {
            sBuff << "-" << edge->v->type().name() << "." << chain->lastElt().name();
            size *= chain->lastElt().type().variable().domainSize();
          }
        }

        // Second looking for edge->v output nodes in u
        for (const auto chain: edge->v->type().slotChains())
          if (edge->v->getInstances(chain->id()).exists(edge->u)) {
            sBuff << "-" << edge->u->type().name() << "." << chain->lastElt().name();
            size *= chain->lastElt().type().variable().domainSize();
          }

        // Label is ready
        if (!label_map.exists(sBuff.str())) {
          LabelData* label = new LabelData();
          label_map.insert(sBuff.str(), label);
          label->id         = ++_counter_;
          label->l          = sBuff.str();
          label->tree_width = size;
          _labels_->insert(label->id, label);
          _edgeMap_.insert(label, new Set< EdgeData< GUM_SCALAR >* >());
        }

        edge->l = label_map[sBuff.str()];
        _edgeMap_[edge->l]->insert(edge);
      }

      template < typename GUM_SCALAR >
      INLINE UndiGraph& InterfaceGraph< GUM_SCALAR >::graph() {
        return _graph_;
      }

      template < typename GUM_SCALAR >
      INLINE const UndiGraph& InterfaceGraph< GUM_SCALAR >::graph() const {
        return _graph_;
      }

      template < typename GUM_SCALAR >
      INLINE Bijection< Idx, LabelData* >& InterfaceGraph< GUM_SCALAR >::labels() {
        return *_labels_;
      }

      template < typename GUM_SCALAR >
      INLINE const Bijection< Idx, LabelData* >& InterfaceGraph< GUM_SCALAR >::labels() const {
        return *_labels_;
      }

      template < typename GUM_SCALAR >
      INLINE Size InterfaceGraph< GUM_SCALAR >::size(const LabelData* l) const {
        try {
          return _nodeMap_[const_cast< LabelData* >(l)]->size();
        } catch (NotFound const&) { return _edgeMap_[const_cast< LabelData* >(l)]->size(); }
      }

      template < typename GUM_SCALAR >
      INLINE Set< NodeData< GUM_SCALAR >* >&
             InterfaceGraph< GUM_SCALAR >::nodes(const LabelData* l) {
        return *(_nodeMap_[const_cast< LabelData* >(l)]);
      }

      template < typename GUM_SCALAR >
      INLINE const Set< NodeData< GUM_SCALAR >* >&
                   InterfaceGraph< GUM_SCALAR >::nodes(const LabelData* l) const {
        return *(_nodeMap_[const_cast< LabelData* >(l)]);
      }

      template < typename GUM_SCALAR >
      INLINE Set< EdgeData< GUM_SCALAR >* >&
             InterfaceGraph< GUM_SCALAR >::edges(const LabelData* l) {
        return *(_edgeMap_[const_cast< LabelData* >(l)]);
      }

      template < typename GUM_SCALAR >
      INLINE const Set< EdgeData< GUM_SCALAR >* >&
                   InterfaceGraph< GUM_SCALAR >::edges(const LabelData* l) const {
        return *(_edgeMap_[const_cast< LabelData* >(l)]);
      }

      template < typename GUM_SCALAR >
      INLINE LabelData* InterfaceGraph< GUM_SCALAR >::label(Idx id) {
        return _labels_->second(id);
      }

      template < typename GUM_SCALAR >
      INLINE NodeId InterfaceGraph< GUM_SCALAR >::id(const PRMInstance< GUM_SCALAR >& i) const {
        return _idMap_[const_cast< PRMInstance< GUM_SCALAR >* >(&i)];
      }

      template < typename GUM_SCALAR >
      INLINE NodeId InterfaceGraph< GUM_SCALAR >::id(const PRMInstance< GUM_SCALAR >* i) const {
        return _idMap_[const_cast< PRMInstance< GUM_SCALAR >* >(i)];
      }

      template < typename GUM_SCALAR >
      INLINE NodeData< GUM_SCALAR >&
             InterfaceGraph< GUM_SCALAR >::node(const PRMInstance< GUM_SCALAR >* i) {
        return node(id(i));
      }

      template < typename GUM_SCALAR >
      INLINE const NodeData< GUM_SCALAR >&
                   InterfaceGraph< GUM_SCALAR >::node(const PRMInstance< GUM_SCALAR >* i) const {
        return node(id(i));
      }

      template < typename GUM_SCALAR >
      INLINE NodeData< GUM_SCALAR >& InterfaceGraph< GUM_SCALAR >::node(NodeId id) {
        return *(_nodes_[id]);
      }

      template < typename GUM_SCALAR >
      INLINE const NodeData< GUM_SCALAR >& InterfaceGraph< GUM_SCALAR >::node(NodeId id) const {
        return *(_nodes_[id]);
      }

      template < typename GUM_SCALAR >
      INLINE EdgeData< GUM_SCALAR >& InterfaceGraph< GUM_SCALAR >::edge(NodeId u, NodeId v) {
        try {
          return *(_edges_[Edge(u, v)]);
        } catch (NotFound const&) { return *(_edges_[Edge(v, u)]); }
      }

      template < typename GUM_SCALAR >
      INLINE const EdgeData< GUM_SCALAR >& InterfaceGraph< GUM_SCALAR >::edge(NodeId u,
                                                                              NodeId v) const {
        try {
          return *(_edges_[Edge(u, v)]);
        } catch (NotFound const&) { return *(_edges_[Edge(v, u)]); }
      }

      template < typename GUM_SCALAR >
      INLINE std::ostream& operator<<(std::ostream& out, const NodeData< GUM_SCALAR >& data) {
        out << data.n->name() << "(" << data.l->l << ")";
        return out;
      }

      template < typename GUM_SCALAR >
      INLINE std::ostream& operator<<(std::ostream& out, const EdgeData< GUM_SCALAR >& data) {
        out << data.u->name() << " -> " << data.v->name() << "(" << data.l->l << ")";
        return out;
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
