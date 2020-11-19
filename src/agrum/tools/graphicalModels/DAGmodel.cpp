/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/tools/graphicalModels/DAGmodel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/DAGmodel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  DAGmodel::DAGmodel() : mutableMoralGraph__(nullptr) {
    GUM_CONSTRUCTOR(DAGmodel);
  }

  DAGmodel::DAGmodel(const DAGmodel& from) :
      dag_(from.dag_), mutableMoralGraph__(nullptr) {
    GUM_CONS_CPY(DAGmodel);
  }

  DAGmodel::~DAGmodel() {
    GUM_DESTRUCTOR(DAGmodel);
    if (mutableMoralGraph__) { delete mutableMoralGraph__; }
  }

  void DAGmodel::moralGraph__() const {
    mutableMoralGraph__->populateNodes(dag());
    // transform the arcs into edges

    for (const auto& arc: arcs())
      mutableMoralGraph__->addEdge(arc.first(), arc.second());

    //}

    // marry the parents
    for (const auto node: nodes()) {
      const auto& par = parents(node);

      for (auto it1 = par.begin(); it1 != par.end(); ++it1) {
        auto it2 = it1;

        for (++it2; it2 != par.end(); ++it2) {
          // will automatically check if this edge already exists
          mutableMoralGraph__->addEdge(*it1, *it2);
        }
      }
    }
  }

  DAGmodel& DAGmodel::operator=(const DAGmodel& source) {
    if (this != &source) {
      GraphicalModel::operator=(source);

      if (mutableMoralGraph__) {
        delete mutableMoralGraph__;
        mutableMoralGraph__ = nullptr;
      }
      dag_ = source.dag_;
    }

    return *this;
  }

  const UndiGraph& DAGmodel::moralGraph(bool clear) const {
    if (clear
        || (mutableMoralGraph__ == nullptr)) {   // we have to call moralGraph_
      if (mutableMoralGraph__ == nullptr) {
        mutableMoralGraph__ = new UndiGraph();
      } else {
        // clear is True ,__mutableMoralGraph exists
        mutableMoralGraph__->clear();
      }

      moralGraph__();
    }

    return *mutableMoralGraph__;
  }

   bool DAGmodel::hasSameStructure(const DAGmodel& other) {
    if (this == &other) return true;

    if (size() != other.size()) return false;

    if (sizeArcs() != other.sizeArcs()) return false;

    for (const auto& nid: nodes()) {
      try {
        other.idFromName(variable(nid).name());
      } catch (NotFound) { return false; }
    }

    for (const auto& arc: arcs()) {
      if (!other.arcs().exists(Arc(other.idFromName(variable(arc.tail()).name()),
                                   other.idFromName(variable(arc.head()).name()))))
        return false;
    }

    return true;
  }
}   // namespace gum
