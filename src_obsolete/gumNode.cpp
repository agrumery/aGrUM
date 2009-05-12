/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *   test $Id: $                                                           *
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

/** @file
 * @brief Outlined implementation of the base class of every kind of node
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <iostream>
#include <agrum/gumNode.h>
#include <agrum/gum_utils.h>

#ifdef GUM_NO_INLINE
#include "gumNode.inl"
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// ==============================================================================
/// display the content of the node
// ==============================================================================
std::ostream& operator<< ( std::ostream& stream, const gumNode& node ) {
  return stream << "(" << node.getID() << ",\""<<node.label()<<"\""<<")";
}


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
