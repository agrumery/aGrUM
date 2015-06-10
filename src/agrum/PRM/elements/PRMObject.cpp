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
 * @brief Source implementation of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */

#include <agrum/PRM/elements/PRMObject.h>

#ifdef GUM_NO_INLINE
#include <agrum/PRM/elements/PRMObject.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {
    // ==========================================================================
    // Constructor.
    // @param type The type of this object.
    // ==========================================================================
    PRMObject::PRMObject(const std::string &name) : __name(name) {
      GUM_CONSTRUCTOR(PRMObject);
    }
    // ==========================================================================
    // Copy constructor.
    // ==========================================================================
    PRMObject::PRMObject(const PRMObject &source) : __name(source.__name) {
      GUM_CONS_CPY(PRMObject);
    }
    // ==========================================================================
    // Destructor.
    // ==========================================================================
    PRMObject::~PRMObject() { GUM_DESTRUCTOR(PRMObject); }

    // Copy operator.
    PRMObject &PRMObject::operator=(const PRMObject &source) {
      __name = source.__name;
      return *this;
    }

    // ==========================================================================
    // For printing Type easily.
    // ==========================================================================
    std::ostream &operator<<(std::ostream &out, PRMObject::PRMType obj_type) {
      return (out << PRMObject::enum2str(obj_type));
    }
    // ==========================================================================
  } /* namespace prm */
} /* namespace gum */
