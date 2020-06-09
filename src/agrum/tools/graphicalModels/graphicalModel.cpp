
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
  GraphicalModel::GraphicalModel() : propertiesMap__(nullptr) {
    GUM_CONSTRUCTOR(GraphicalModel);
  }

  GraphicalModel::GraphicalModel(const GraphicalModel& from) :
      propertiesMap__(nullptr) {
    GUM_CONS_CPY(GraphicalModel);

    if (from.propertiesMap__) {
      propertiesMap__ =
         new HashTable< std::string, std::string >(*(from.propertiesMap__));
    }
  }

  GraphicalModel::~GraphicalModel() {
    GUM_DESTRUCTOR(GraphicalModel);
    if (propertiesMap__) { delete propertiesMap__; }
  }

  GraphicalModel& GraphicalModel::operator=(const GraphicalModel& source) {
    if (this != &source) {
      if (propertiesMap__) {
        delete propertiesMap__;
        propertiesMap__ = nullptr;
      }

      if (source.propertiesMap__ != 0) {
        propertiesMap__ =
           new HashTable< std::string, std::string >(*(source.propertiesMap__));
      }
    }
    return *this;
  }
}   // namespace gum
