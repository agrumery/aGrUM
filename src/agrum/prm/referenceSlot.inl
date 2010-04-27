/**************************************************************************
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
 * @brief Inline implementation of gum::ReferenceSlot
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

INLINE
ClassElement::ClassElementType
ReferenceSlot::elt_type() const { return prm_refslot; }

INLINE
ClassElementContainer&
ReferenceSlot::slotType() { return __slotType; }

INLINE
const ClassElementContainer&
ReferenceSlot::slotType() const { return __slotType; }

INLINE
bool
ReferenceSlot::isArray() const { return __isArray; }

INLINE
Type&
ReferenceSlot::type() {
  GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
}

INLINE
const Type&
ReferenceSlot::type() const {
  GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
}

INLINE
Potential<prm_float>&
ReferenceSlot::cpf() {
  GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
}

INLINE
const Potential<prm_float>&
ReferenceSlot::cpf() const {
  GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
}

INLINE
void
ReferenceSlot::addParent(const ClassElement& elt) { }

INLINE
void
ReferenceSlot::addChild(const ClassElement& elt) { }

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
