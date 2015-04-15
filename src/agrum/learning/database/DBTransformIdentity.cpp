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

#include <agrum/learning/database/DBTransformIdentity.h>

namespace gum {

  namespace learning {

    /// default constructor
    DBTransformIdentity::DBTransformIdentity() {
      GUM_CONSTRUCTOR(DBTransformIdentity);
    }

    /// copy constructor
    DBTransformIdentity::DBTransformIdentity(const DBTransformIdentity &from)
        : DBTransform(from) {
      GUM_CONS_CPY(DBTransformIdentity);
    }

    /// move constructor
    DBTransformIdentity::DBTransformIdentity(DBTransformIdentity &&from)
        : DBTransform(from) {
      GUM_CONS_MOV(DBTransformIdentity);
    }

    /// destructor
    DBTransformIdentity::~DBTransformIdentity() {
      GUM_DESTRUCTOR(DBTransformIdentity);
    }

    /// copy operator
    DBTransformIdentity &DBTransformIdentity::
    operator=(const DBTransformIdentity &from) {
      DBTransform::operator=(from);
      return *this;
    }

    /// move operator
    DBTransformIdentity &DBTransformIdentity::operator=(DBTransformIdentity &&from) {
      DBTransform::operator=(std::move(from));
      return *this;
    }

    /// transforms a vector of DBrows
    bool DBTransformIdentity::transform(std::vector<DBRow> &db,
                                        std::vector<std::string> miss) const {
      return true;
    }

  } /* namespace learning */

} /* namespace gum */
