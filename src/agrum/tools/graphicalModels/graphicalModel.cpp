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


#include <agrum/tools/graphicalModels/graphicalModel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/graphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  GraphicalModel::GraphicalModel() :  _propertiesMap_(nullptr) {
    GUM_CONSTRUCTOR(GraphicalModel);
  }

  GraphicalModel::GraphicalModel(const GraphicalModel& from) :
       _propertiesMap_(nullptr) {
    GUM_CONS_CPY(GraphicalModel);

    if (from. _propertiesMap_) {
       _propertiesMap_
         = new HashTable< std::string, std::string >(*(from. _propertiesMap_));
    }
  }

  GraphicalModel::~GraphicalModel() {
    GUM_DESTRUCTOR(GraphicalModel);
    if ( _propertiesMap_) { delete  _propertiesMap_; }
  }

  GraphicalModel& GraphicalModel::operator=(const GraphicalModel& source) {
    if (this != &source) {
      if ( _propertiesMap_) {
        delete  _propertiesMap_;
         _propertiesMap_ = nullptr;
      }

      if (source. _propertiesMap_ != 0) {
         _propertiesMap_
           = new HashTable< std::string, std::string >(*(source. _propertiesMap_));
      }
    }
    return *this;
  }

  NodeSet GraphicalModel::nodeset(const std::vector< std::string >& names) const {
    NodeSet res;
    for (const auto& name: names) {
      res.insert(idFromName(name));
    }
    return res;
  }

}   // namespace gum
