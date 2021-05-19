/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/tools/graphicalModels/UGmodel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/UGmodel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  UGmodel::UGmodel() {
    GUM_CONSTRUCTOR(UGmodel);
    ;
  }

  UGmodel::UGmodel(const UGmodel& from) : graph_(from.graph_) {
    GUM_CONS_CPY(UGmodel);
  }

  UGmodel::~UGmodel() {
    GUM_DESTRUCTOR(UGmodel);
    ;
  }

  UGmodel& UGmodel::operator=(const UGmodel& source) {
    if (this != &source) {
      GraphicalModel::operator=(source);
      graph_                  = source.graph_;
    }

    return *this;
  }

  bool UGmodel::hasSameStructure(const UGmodel& other) {
    if (this == &other) return true;

    if (size() != other.size()) return false;

    if (sizeEdges() != other.sizeEdges()) return false;

    for (const auto& nid: nodes()) {
      try {
        other.idFromName(variable(nid).name());
      } catch (NotFound) { return false; }
    }

    for (const auto& edge: edges()) {
      if (!other.edges().exists(
             Edge(other.idFromName(variable(edge.first()).name()),
                  other.idFromName(variable(edge.second()).name()))))
        return false;
    }

    return true;
  }
}   // namespace gum
