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


#include <agrum/tools/graphicalModels/graphicalModel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/graphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  GraphicalModel::GraphicalModel() { GUM_CONSTRUCTOR(GraphicalModel) }

  GraphicalModel::GraphicalModel(const GraphicalModel& from) { GUM_CONS_CPY(GraphicalModel); }

  GraphicalModel::~GraphicalModel() { GUM_DESTRUCTOR(GraphicalModel); }

  GraphicalModel& GraphicalModel::operator=(const GraphicalModel& source) {
    if (this != &source) { _propertiesMap_ = source._propertiesMap_; }
    return *this;
  }

  NodeSet GraphicalModel::nodeset(const std::vector< std::string >& names) const {
    NodeSet res;
    for (const auto& name: names) {
      res.insert(idFromName(name));
    }
    return res;
  }

  void
      GraphicalModel::spaceCplxToStream(std::stringstream& s, double dSize, int dim, Size usedMem) {
    if (dSize > 6) s << "domainSize: 10^" << dSize;
    else s << "domainSize: " << std::round(std::pow(10.0, dSize));

    s << ", dim: " << dim << ", mem: ";

    if (const Size go = usedMem / (1024 * 1024 * 1024); go > 0) s << go << "Go ";
    if (const Size mo = (usedMem / (1024 * 1024)) % 1024; mo > 0) s << mo << "Mo ";
    if (const Size ko = (usedMem / 1024) % 1024; ko > 0) s << ko << "Ko ";
    s << usedMem % 1024 << "o";
  }
}   // namespace gum
