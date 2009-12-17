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
 * @brief Inline implementation of gum::SlotChain
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {

INLINE
ClassElement::ClassElementType
SlotChain::elt_type() const { return prm_slotchain; }

INLINE
Type&
SlotChain::type() { return __lastElt.type(); }

INLINE
const Type&
SlotChain::type() const { return __lastElt.type(); }

INLINE
Potential<prm_float>&
SlotChain::cpf() { return __lastElt.cpf(); }

INLINE
const Potential<prm_float>&
SlotChain::cpf() const { return __lastElt.cpf(); }

INLINE
Class&
SlotChain::start() { return __start; }

INLINE
const Class&
SlotChain::start() const { return __start; }

INLINE
Class&
SlotChain::end() { return __end; }

INLINE
const Class&
SlotChain::end() const { return __end; }

INLINE
ClassElement&
SlotChain::lastElt() {return __lastElt; }

INLINE
const ClassElement&
SlotChain::lastElt() const {return __lastElt; }

INLINE
const Sequence<ReferenceSlot*>&
SlotChain::chain() const { return __seq; }

INLINE
void
SlotChain::addParent(const ClassElement& elt) { }

INLINE
void
SlotChain::addChild(const ClassElement& elt) { }

INLINE
bool
SlotChain::isMultiple() const { return __isMultiple; }

// ============================================================================
} /* namespace gum */
// ============================================================================
