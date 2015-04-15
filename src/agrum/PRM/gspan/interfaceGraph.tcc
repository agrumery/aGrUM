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
 * @brief Inline implementation of gum::InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {
    namespace gspan {

      // NodeData

      template <typename GUM_SCALAR>
      INLINE NodeData<GUM_SCALAR>::NodeData()
          : n(0), l(0) {
        GUM_CONSTRUCTOR(NodeData);
      }

      template <typename GUM_SCALAR>
      INLINE NodeData<GUM_SCALAR>::NodeData(const NodeData<GUM_SCALAR> &from)
          : n(from.n), l(from.l) {
        GUM_CONS_CPY(NodeData);
      }

      template <typename GUM_SCALAR> INLINE NodeData<GUM_SCALAR>::~NodeData() {
        GUM_DESTRUCTOR(NodeData);
      }

      template <typename GUM_SCALAR>
      INLINE bool NodeData<GUM_SCALAR>::
      operator==(const NodeData<GUM_SCALAR> &from) const {
        return (n == from.n) and (l == from.l);
      }

      template <typename GUM_SCALAR>
      INLINE bool NodeData<GUM_SCALAR>::
      operator!=(const NodeData<GUM_SCALAR> &from) const {
        return (n != from.n) and (l != from.l);
      }

      // EdgeData<GUM_SCALAR>

      template <typename GUM_SCALAR>
      INLINE EdgeData<GUM_SCALAR>::EdgeData()
          : u(0), v(0), l(0) {
        GUM_CONSTRUCTOR(EdgeData);
      }

      template <typename GUM_SCALAR>
      INLINE EdgeData<GUM_SCALAR>::EdgeData(const EdgeData<GUM_SCALAR> &from)
          : u(from.u), v(from.v), l(from.l) {
        GUM_CONS_CPY(EdgeData);
      }

      template <typename GUM_SCALAR> INLINE EdgeData<GUM_SCALAR>::~EdgeData() {
        GUM_DESTRUCTOR(EdgeData);
      }

      template <typename GUM_SCALAR>
      INLINE bool EdgeData<GUM_SCALAR>::
      operator==(const EdgeData<GUM_SCALAR> &from) const {
        return (u == from.u) and (l_u == from.l_u) and (v == from.v) and
               (l_v == from.l_v) and (l == from.l);
      }

      template <typename GUM_SCALAR>
      INLINE bool EdgeData<GUM_SCALAR>::
      operator!=(const EdgeData<GUM_SCALAR> &from) const {
        return (u != from.u) and (l_u != from.l_u) and (v != from.v) and
               (l_v != from.l_v) and (l != from.l);
      }

      // InterfaceGraph

      template <typename GUM_SCALAR>
      InterfaceGraph<GUM_SCALAR>::InterfaceGraph(const System<GUM_SCALAR> &sys)
          : __sys(&sys), __labels(new Bijection<Idx, LabelData *>()), __counter(0),
            __erase_flag(true) {
        GUM_CONSTRUCTOR(InterfaceGraph);
        HashTable<std::string, LabelData *> label_map;

        // We need to add each instance in __graph
        for (auto iter = sys.begin(); iter != sys.end(); ++iter) {
          NodeData<GUM_SCALAR> *node = new NodeData<GUM_SCALAR>();
          node->n = iter.val();
          __label(node, label_map);
          __graph.addNode(iter.key());
          __idMap.insert(node->n, iter.key());
          __nodes.insert(iter.key(), node);
        }

        NodeData<GUM_SCALAR> *data = nullptr;
        NodeData<GUM_SCALAR> *u = nullptr;
        NodeData<GUM_SCALAR> *v = nullptr;

        for (const auto &elt : __nodes) {
          data = elt.second;

          for (const auto chain : data->n->type().slotChains())
            for (const auto inst : data->n->getInstances(chain->id())) {
              u = (__nodes[__idMap[inst]]->l < data->l) ? __nodes[__idMap[inst]]
                                                        : data;
              v = (u != data) ? data : __nodes[__idMap[inst]];

              if (not __graph.existsEdge(__idMap[u->n], __idMap[v->n])) {
                EdgeData<GUM_SCALAR> *edge = new EdgeData<GUM_SCALAR>();
                edge->u = u->n;
                edge->l_u = u->l;
                edge->v = v->n;
                edge->l_v = v->l;
                __label(edge, label_map);
                __graph.addEdge(__idMap[u->n], __idMap[v->n]);
                __edges.insert(Edge(__idMap[u->n], __idMap[v->n]), edge);
              }
            }
        }
      }

      template <typename GUM_SCALAR>
      InterfaceGraph<GUM_SCALAR>::InterfaceGraph(
          const InterfaceGraph<GUM_SCALAR> &source)
          : __sys(source.__sys), __graph(source.__graph), __nodes(source.__nodes),
            __idMap(source.__idMap), __edges(source.__edges),
            __labels(new Bijection<Idx, LabelData *>(*(source.__labels))),
            __nodeMap(source.__nodeMap), __edgeMap(source.__edgeMap),
            __counter(source.__counter), __erase_flag(false) {
        GUM_CONS_CPY(InterfaceGraph);
      }

      template <typename GUM_SCALAR> InterfaceGraph<GUM_SCALAR>::~InterfaceGraph() {
        GUM_DESTRUCTOR(InterfaceGraph);

        if (__erase_flag) {
          for (const auto &elt : __nodes)
            delete elt.second;

          for (const auto &elt : __edges)
            delete elt.second;

          for (const auto &elt : __nodeMap) {
            delete elt.first;
            delete elt.second;
          }

          for (const auto &elt : __edgeMap) {
            delete elt.first;
            delete elt.second;
          }
        }

        delete __labels;
      }

      template <typename GUM_SCALAR>
      InterfaceGraph<GUM_SCALAR> &InterfaceGraph<GUM_SCALAR>::
      operator=(const InterfaceGraph<GUM_SCALAR> &source) {
        GUM_ERROR(FatalError, "not implemented");
      }

      template <typename GUM_SCALAR>
      void InterfaceGraph<GUM_SCALAR>::__label(
          NodeData<GUM_SCALAR> *node,
          HashTable<std::string, LabelData *> &label_map) {
        unsigned long size = 1;
        std::stringstream sBuff;
        sBuff << node->n->type().name();

        // First we search for multiple inputs
        for (const auto chain : node->n->type().slotChains()) {
          if (chain->isMultiple()) {
            sBuff << "-" << node->n->getInstances(chain->id()).size();
            sBuff << chain->name();
            size *= node->n->getInstances(chain->id()).size() *
                    chain->lastElt().type().variable().domainSize();
          } else {
            size *= chain->lastElt().type().variable().domainSize();
          }
        }

        // Second we search for active outputs
        for (const auto nn : node->n->type().dag().nodes()) {
          if (node->n->type().isOutputNode(node->n->type().get(nn))) {
            try {
              sBuff << "-" << node->n->getRefAttr(nn).size()
                    << node->n->get(nn).name();
              size *= node->n->get(nn).type().variable().domainSize();
            } catch (NotFound &) {
              // (nn) is an inactive output node
            }
          }
        }

        // Label is ready
        if (not label_map.exists(sBuff.str())) {
          LabelData *label = new LabelData();
          label_map.insert(sBuff.str(), label);
          label->id = ++__counter;
          label->tree_width = size;
          label->l = sBuff.str();
          __labels->insert(label->id, label);
          __nodeMap.insert(label, new Set<NodeData<GUM_SCALAR> *>());
        }

        node->l = label_map[sBuff.str()];
        __nodeMap[node->l]->insert(node);
      }

      template <typename GUM_SCALAR>
      void InterfaceGraph<GUM_SCALAR>::__label(
          EdgeData<GUM_SCALAR> *edge,
          HashTable<std::string, LabelData *> &label_map) {
        unsigned long size = 1;
        std::stringstream sBuff;
        sBuff << edge->u->type().name() << "-" << edge->v->type().name();

        // First looking for edge->u output nodes in v
        for (const auto chain : edge->u->type().slotChains()) {
          if (edge->u->getInstances(chain->id()).exists(edge->v)) {
            sBuff << "-" << edge->v->type().name() << "." << chain->lastElt().name();
            size *= chain->lastElt().type().variable().domainSize();
          }
        }

        // Second looking for edge->v output nodes in u
        for (const auto chain : edge->v->type().slotChains())
          if (edge->v->getInstances(chain->id()).exists(edge->u)) {
            sBuff << "-" << edge->u->type().name() << "." << chain->lastElt().name();
            size *= chain->lastElt().type().variable().domainSize();
          }

        // Label is ready
        if (not label_map.exists(sBuff.str())) {
          LabelData *label = new LabelData();
          label_map.insert(sBuff.str(), label);
          label->id = ++__counter;
          label->l = sBuff.str();
          label->tree_width = size;
          __labels->insert(label->id, label);
          __edgeMap.insert(label, new Set<EdgeData<GUM_SCALAR> *>());
        }

        edge->l = label_map[sBuff.str()];
        __edgeMap[edge->l]->insert(edge);
      }

      template <typename GUM_SCALAR>
      INLINE UndiGraph &InterfaceGraph<GUM_SCALAR>::graph() {
        return __graph;
      }

      template <typename GUM_SCALAR>
      INLINE const UndiGraph &InterfaceGraph<GUM_SCALAR>::graph() const {
        return __graph;
      }

      template <typename GUM_SCALAR>
      INLINE Bijection<Idx, LabelData *> &InterfaceGraph<GUM_SCALAR>::labels() {
        return *__labels;
      }

      template <typename GUM_SCALAR>
      INLINE const Bijection<Idx, LabelData *> &
      InterfaceGraph<GUM_SCALAR>::labels() const {
        return *__labels;
      }

      template <typename GUM_SCALAR>
      INLINE Size InterfaceGraph<GUM_SCALAR>::size(const LabelData *l) const {
        try {
          return __nodeMap[const_cast<LabelData *>(l)]->size();
        } catch (NotFound &) {
          return __edgeMap[const_cast<LabelData *>(l)]->size();
        }
      }

      template <typename GUM_SCALAR>
      INLINE Set<NodeData<GUM_SCALAR> *> &
      InterfaceGraph<GUM_SCALAR>::nodes(const LabelData *l) {
        return *(__nodeMap[const_cast<LabelData *>(l)]);
      }

      template <typename GUM_SCALAR>
      INLINE const Set<NodeData<GUM_SCALAR> *> &
      InterfaceGraph<GUM_SCALAR>::nodes(const LabelData *l) const {
        return *(__nodeMap[const_cast<LabelData *>(l)]);
      }

      template <typename GUM_SCALAR>
      INLINE Set<EdgeData<GUM_SCALAR> *> &
      InterfaceGraph<GUM_SCALAR>::edges(const LabelData *l) {
        return *(__edgeMap[const_cast<LabelData *>(l)]);
      }

      template <typename GUM_SCALAR>
      INLINE const Set<EdgeData<GUM_SCALAR> *> &
      InterfaceGraph<GUM_SCALAR>::edges(const LabelData *l) const {
        return *(__edgeMap[const_cast<LabelData *>(l)]);
      }

      template <typename GUM_SCALAR>
      INLINE LabelData *InterfaceGraph<GUM_SCALAR>::label(Idx id) {
        return __labels->second(id);
      }

      template <typename GUM_SCALAR>
      INLINE NodeId
          InterfaceGraph<GUM_SCALAR>::id(const Instance<GUM_SCALAR> &i) const {
        return __idMap[const_cast<Instance<GUM_SCALAR> *>(&i)];
      }

      template <typename GUM_SCALAR>
      INLINE NodeId
          InterfaceGraph<GUM_SCALAR>::id(const Instance<GUM_SCALAR> *i) const {
        return __idMap[const_cast<Instance<GUM_SCALAR> *>(i)];
      }

      template <typename GUM_SCALAR>
      INLINE NodeData<GUM_SCALAR> &
      InterfaceGraph<GUM_SCALAR>::node(const Instance<GUM_SCALAR> *i) {
        return node(id(i));
      }

      template <typename GUM_SCALAR>
      INLINE const NodeData<GUM_SCALAR> &
      InterfaceGraph<GUM_SCALAR>::node(const Instance<GUM_SCALAR> *i) const {
        return node(id(i));
      }

      template <typename GUM_SCALAR>
      INLINE NodeData<GUM_SCALAR> &InterfaceGraph<GUM_SCALAR>::node(NodeId id) {
        return *(__nodes[id]);
      }

      template <typename GUM_SCALAR>
      INLINE const NodeData<GUM_SCALAR> &
      InterfaceGraph<GUM_SCALAR>::node(NodeId id) const {
        return *(__nodes[id]);
      }

      template <typename GUM_SCALAR>
      INLINE EdgeData<GUM_SCALAR> &InterfaceGraph<GUM_SCALAR>::edge(NodeId u,
                                                                    NodeId v) {
        try {
          return *(__edges[Edge(u, v)]);
        } catch (NotFound &) {
          return *(__edges[Edge(v, u)]);
        }
      }

      template <typename GUM_SCALAR>
      INLINE const EdgeData<GUM_SCALAR> &
      InterfaceGraph<GUM_SCALAR>::edge(NodeId u, NodeId v) const {
        try {
          return *(__edges[Edge(u, v)]);
        } catch (NotFound &) {
          return *(__edges[Edge(v, u)]);
        }
      }

      template <typename GUM_SCALAR>
      INLINE std::ostream &operator<<(std::ostream &out,
                                      const NodeData<GUM_SCALAR> &data) {
        out << data.n->name() << "(" << data.l->l << ")";
        return out;
      }

      template <typename GUM_SCALAR>
      INLINE std::ostream &operator<<(std::ostream &out,
                                      const EdgeData<GUM_SCALAR> &data) {
        out << data.u->name() << " -> " << data.v->name() << "(" << data.l->l << ")";
        return out;
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
