
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


/**
 * @file
 * @brief Implementation of gum::InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/gspan/interfaceGraph.h>

namespace gum {
  namespace prm {
    namespace gspan {

      std::ostream& operator<<(std::ostream& out, const LabelData& data) {
        out << data.l;
        return out;
      }

      // LabelData

      LabelData::LabelData() : id(0) { GUM_CONSTRUCTOR(LabelData); }

      LabelData::LabelData(const LabelData& from) : id(from.id), l(from.l) {
        GUM_CONS_CPY(LabelData);
      }

      LabelData::~LabelData() { GUM_DESTRUCTOR(LabelData); }

      bool LabelData::operator==(const LabelData& from) const {
        return (id == from.id) && (l == from.l) && (tree_width == from.tree_width);
      }

      bool LabelData::operator!=(const LabelData& from) const {
        return (id != from.id) && (l != from.l) && (tree_width != from.tree_width);
      }


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      template class NodeData< double >;
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      template class EdgeData< double >;
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      template class InterfaceGraph< double >;
#    endif
#  endif
#endif

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
