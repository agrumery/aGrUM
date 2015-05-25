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
/** @file
 * @brief Implementation of DBTransform
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/database/DBTransform.h>

namespace gum {

  namespace learning {

    /// default constructor
    DBTransform::DBTransform() { GUM_CONSTRUCTOR(DBTransform); }

    /// copy constructor
    DBTransform::DBTransform(const DBTransform &) { GUM_CONS_CPY(DBTransform); }

    /// move constructor
    DBTransform::DBTransform(DBTransform &&) { GUM_CONS_MOV(DBTransform); }

    /// destructor
    DBTransform::~DBTransform() { GUM_DESTRUCTOR(DBTransform); }

    /// copy operator
    DBTransform &DBTransform::operator=(const DBTransform &) { return *this; }

    /// move operator
    DBTransform &DBTransform::operator=(DBTransform &&) { return *this; }

  } /* namespace learning */

} /* namespace gum */
