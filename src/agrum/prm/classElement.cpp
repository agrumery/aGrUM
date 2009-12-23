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
 * @brief Source implementation of gum::ClassElement
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/classElement.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/classElement.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

ClassElement::ClassElement(const std::string& name):
  PRMObject(name) {
  GUM_CONSTRUCTOR( ClassElement );
}

ClassElement::ClassElement(const ClassElement& source):
  PRMObject(source.name()), __id(source.id()) {
  GUM_CONS_CPY( ClassElement);
}

ClassElement::~ClassElement() {
  GUM_DESTRUCTOR( ClassElement );
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
