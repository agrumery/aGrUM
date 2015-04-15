/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
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

      std::ostream &operator<<(std::ostream &out, const LabelData &data) {
        out << data.l;
        return out;
      }

      // LabelData

      INLINE
      LabelData::LabelData() : id(0) { GUM_CONSTRUCTOR(LabelData); }

      INLINE
      LabelData::LabelData(const LabelData &from) : id(from.id), l(from.l) {
        GUM_CONS_CPY(LabelData);
      }

      INLINE
      LabelData::~LabelData() { GUM_DESTRUCTOR(LabelData); }

      INLINE
      bool LabelData::operator==(const LabelData &from) const {
        return (id == from.id) and (l == from.l) and (tree_width == from.tree_width);
      }

      INLINE
      bool LabelData::operator!=(const LabelData &from) const {
        return (id != from.id) and (l != from.l) and (tree_width != from.tree_width);
      }

      template class NodeData<double>;
      template class EdgeData<double>;
      template class InterfaceGraph<double>;
    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
