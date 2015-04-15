/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
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

#include <agrum/graphicalModels/DAGmodel.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphicalModels/DAGmodel.inl>
#endif /* GUM_NO_INLINE */

namespace gum {
  DAGmodel::DAGmodel()
      : __mutableMoralGraph(nullptr), __mutableTopologicalOrder(nullptr),
        __propertiesMap(nullptr) {
    GUM_CONSTRUCTOR(DAGmodel);
  }

  DAGmodel::DAGmodel(const DAGmodel &from)
      : _dag(from._dag), __mutableMoralGraph(nullptr),
        __mutableTopologicalOrder(nullptr), __propertiesMap(nullptr) {
    GUM_CONS_CPY(DAGmodel);

    if (from.__propertiesMap) {
      __propertiesMap =
          new HashTable<std::string, std::string>(*(from.__propertiesMap));
    }
  }

  DAGmodel::~DAGmodel() {
    GUM_DESTRUCTOR(DAGmodel);
    // Removing previous properties

    if (__propertiesMap) {
      delete __propertiesMap;
    }

    if (__mutableMoralGraph) {
      delete __mutableMoralGraph;
    }

    if (__mutableTopologicalOrder) {
      delete __mutableTopologicalOrder;
    }
  }

  void DAGmodel::__topologicalOrder() const {
    DAG dag = this->dag();
    std::vector<NodeId> roots;

    for (const auto node : dag.nodes())
      if (dag.parents(node).empty())
        roots.push_back(node);

    while (roots.size()) {
      __mutableTopologicalOrder->insert(roots.back());
      roots.pop_back();

      while (dag.children(__mutableTopologicalOrder->back()).size()) {
        NodeId child = *(dag.children(__mutableTopologicalOrder->back()).begin());
        dag.eraseArc(Arc(__mutableTopologicalOrder->back(), child));

        if (dag.parents(child).empty())
          roots.push_back(child);
      }
    }

    GUM_ASSERT(dag.sizeArcs() == (gum::Size)(0));
    GUM_ASSERT(__mutableTopologicalOrder->size() == dag.size());
  }

  void DAGmodel::__moralGraph() const {
    __mutableMoralGraph->populateNodes(dag());
    // transform the arcs into edges

    for (const auto arc : arcs())
      __mutableMoralGraph->addEdge(arc.first(), arc.second());

    //}

    // marry the parents
    for (const auto node : nodes()) {
      const NodeSet &parents = dag().parents(node);

      for (auto it1 = parents.begin(); it1 != parents.end(); ++it1) {
        auto it2 = it1;

        for (++it2; it2 != parents.end(); ++it2) {
          // will automatically check if this edge already exists
          __mutableMoralGraph->addEdge(*it1, *it2);
        }
      }
    }
  }

  DAGmodel &DAGmodel::operator=(const DAGmodel &source) {
    if (this != &source) {
      if (__propertiesMap) {
        delete __propertiesMap;
        __propertiesMap = nullptr;
      }

      if (__mutableMoralGraph) {
        delete __mutableMoralGraph;
        __mutableMoralGraph = nullptr;
      }

      if (__mutableTopologicalOrder) {
        delete __mutableTopologicalOrder;
        __mutableTopologicalOrder = nullptr;
      }

      if (source.__propertiesMap != 0) {
        __propertiesMap =
            new HashTable<std::string, std::string>(*(source.__propertiesMap));
      }

      _dag = source._dag;
    }

    return *this;
  }

  const UndiGraph &DAGmodel::moralGraph(bool clear) const {
    if (clear || (__mutableMoralGraph == nullptr)) { // we have to call _moralGraph
      if (__mutableMoralGraph == nullptr) {
        __mutableMoralGraph = new UndiGraph();
      } else {
        // clear is True ,__mutableMoralGraph exists
        __mutableMoralGraph->clear();
      }

      __moralGraph();
    }

    return *__mutableMoralGraph;
  }

  const Sequence<NodeId> &DAGmodel::topologicalOrder(bool clear) const {
    if (clear || (__mutableTopologicalOrder ==
                  nullptr)) { // we have to call _topologicalOrder
      if (__mutableTopologicalOrder == nullptr) {
        __mutableTopologicalOrder = new Sequence<NodeId>();
      } else {
        // clear is True
        __mutableTopologicalOrder->clear();
      }

      __topologicalOrder();
    }

    return *__mutableTopologicalOrder;
  }
}
