
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
  GraphicalModel::GraphicalModel() : __propertiesMap(nullptr) {
    GUM_CONSTRUCTOR(GraphicalModel);
  }

  GraphicalModel::GraphicalModel(const GraphicalModel& from) :
      __propertiesMap(nullptr) {
    GUM_CONS_CPY(GraphicalModel);

    if (from.__propertiesMap) {
      __propertiesMap =
         new HashTable< std::string, std::string >(*(from.__propertiesMap));
    }
  }

  GraphicalModel::~GraphicalModel() {
    GUM_DESTRUCTOR(GraphicalModel);
    if (__propertiesMap) { delete __propertiesMap; }
  }

  GraphicalModel& GraphicalModel::operator=(const GraphicalModel& source) {
    if (this != &source) {
      if (__propertiesMap) {
        delete __propertiesMap;
        __propertiesMap = nullptr;
      }

      if (source.__propertiesMap != 0) {
        __propertiesMap =
           new HashTable< std::string, std::string >(*(source.__propertiesMap));
      }
    }
    return *this;
  }
}   // namespace gum
