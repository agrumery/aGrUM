
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


#include <agrum/graphicalModels/DAGmodel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/graphicalModels/DAGmodel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  DAGmodel::DAGmodel() : __mutableMoralGraph(nullptr), __propertiesMap(nullptr) {
    GUM_CONSTRUCTOR(DAGmodel);
  }

  DAGmodel::DAGmodel(const DAGmodel& from) :
      _dag(from._dag), __mutableMoralGraph(nullptr), __propertiesMap(nullptr) {
    GUM_CONS_CPY(DAGmodel);

    if (from.__propertiesMap) {
      __propertiesMap =
         new HashTable< std::string, std::string >(*(from.__propertiesMap));
    }
  }

  DAGmodel::~DAGmodel() {
    GUM_DESTRUCTOR(DAGmodel);
    // Removing previous properties

    if (__propertiesMap) { delete __propertiesMap; }

    if (__mutableMoralGraph) { delete __mutableMoralGraph; }
  }

  void DAGmodel::__moralGraph() const {
    // @todo : this is a copy of DAG::moralGraph ... we should do better
    __mutableMoralGraph->populateNodes(dag());
    // transform the arcs into edges

    for (const auto arc : arcs())
      __mutableMoralGraph->addEdge(arc.first(), arc.second());

    //}

    // marry the parents
    for (const auto node : nodes()) {
      const auto& par = parents(node);

      for (auto it1 = par.begin(); it1 != par.end(); ++it1) {
        auto it2 = it1;

        for (++it2; it2 != par.end(); ++it2) {
          // will automatically check if this edge already exists
          __mutableMoralGraph->addEdge(*it1, *it2);
        }
      }
    }
  }

  DAGmodel& DAGmodel::operator=(const DAGmodel& source) {
    if (this != &source) {
      if (__propertiesMap) {
        delete __propertiesMap;
        __propertiesMap = nullptr;
      }

      if (__mutableMoralGraph) {
        delete __mutableMoralGraph;
        __mutableMoralGraph = nullptr;
      }

      if (source.__propertiesMap != 0) {
        __propertiesMap =
           new HashTable< std::string, std::string >(*(source.__propertiesMap));
      }

      _dag = source._dag;
    }

    return *this;
  }

  const UndiGraph& DAGmodel::moralGraph(bool clear) const {
    if (clear
        || (__mutableMoralGraph == nullptr)) {   // we have to call _moralGraph
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

  const Sequence< NodeId >& DAGmodel::topologicalOrder(bool clear) const {
    return this->dag().topologicalOrder(clear);
  }

  bool DAGmodel::hasSameStructure(const DAGmodel& other) {
    if (this == &other) return true;

    if (size() != other.size()) return false;

    if (sizeArcs() != other.sizeArcs()) return false;

    for (const auto& nid : nodes()) {
      try {
        other.idFromName(variable(nid).name());
      } catch (NotFound) { return false; }
    }

    for (const auto& arc : arcs()) {
      if (!other.arcs().exists(Arc(other.idFromName(variable(arc.tail()).name()),
                                   other.idFromName(variable(arc.head()).name()))))
        return false;
    }

    return true;
  }
}   // namespace gum
