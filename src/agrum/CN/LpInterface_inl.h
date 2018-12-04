/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <agrum/CN/LpInterface.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  INLINE Size
         HashFunc< credal::lp::LpCol >::castToSize(const credal::lp::LpCol& key) {
    return Size(key.id());
  }

  INLINE Size HashFunc< credal::lp::LpCol >::
              operator()(const credal::lp::LpCol& key) const {
    return (castToSize(key) * gum::HashFuncConst::gold) & this->_hash_mask;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
