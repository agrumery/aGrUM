/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
  DAGmodel::DAGmodel() : _mutableMoralGraph_(nullptr) { GUM_CONSTRUCTOR(DAGmodel); }

  DAGmodel::DAGmodel(const DAGmodel& from) : dag_(from.dag_), _mutableMoralGraph_(nullptr) {
    GUM_CONS_CPY(DAGmodel);
  }

  DAGmodel::~DAGmodel() {
    GUM_DESTRUCTOR(DAGmodel);
    if (_mutableMoralGraph_ != nullptr) { delete _mutableMoralGraph_; }
  }


  DAGmodel& DAGmodel::operator=(const DAGmodel& source) {
    if (this != &source) {
      GraphicalModel::operator=(source);

      if (_mutableMoralGraph_) {
        delete _mutableMoralGraph_;
        _mutableMoralGraph_ = nullptr;
      }
      dag_ = source.dag_;
    }

    return *this;
  }

  const UndiGraph& DAGmodel::moralGraph(bool clear) const {
    if (clear || (_mutableMoralGraph_ == nullptr)) {   // we have to call dag().moralGraph()
      if (_mutableMoralGraph_ == nullptr) {
        _mutableMoralGraph_ = new UndiGraph();
      } else {
        // clear is True , __mutableMoralGraph exists
        _mutableMoralGraph_->clear();
      }
      *_mutableMoralGraph_ = dag().moralGraph();
    }

    return *_mutableMoralGraph_;
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
