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
 * @brief Template implementations for the GTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/tupleHashFunc.h>
// =========================================================================


namespace gum {

  // Returns the value of a key as a Size.
  Size HashFunc< std::tuple< unsigned int, unsigned int, unsigned int > >::
     castToSize(
        const std::tuple< unsigned int, unsigned int, unsigned int >& key) {
    return HashFuncConst::pi * Size(std::get< 0 >(key)) * Size(2)
           * Size(std::get< 1 >(key)) * Size(3) * Size(std::get< 2 >(key));
  }

  // the hash function for tuple (unsigned int, unsigned int, unsigned int)
  Size HashFunc< std::tuple< unsigned int, unsigned int, unsigned int > >::
       operator()(
        const std::tuple< unsigned int, unsigned int, unsigned int >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->_hash_mask;
  }

}   // namespace gum
