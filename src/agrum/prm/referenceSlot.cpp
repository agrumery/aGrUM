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
 * @brief Source implementation of gum::ReferenceSlot
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/referenceSlot.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/referenceSlot.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

ReferenceSlot::ReferenceSlot(const std::string& name, Class& type,
                             bool isArray):
  ClassElement(name), __slotType(type), __isArray(isArray)
{
  GUM_CONSTRUCTOR( ReferenceSlot );
}

// Destructor.
ReferenceSlot::~ReferenceSlot()
{
  GUM_DESTRUCTOR( ReferenceSlot );
}

ReferenceSlot::ReferenceSlot(const ReferenceSlot& source):
  ClassElement(source), __slotType(source.__slotType), __isArray(source.__isArray)
{
  GUM_CONS_CPY(ReferenceSlot);
  GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy constructor.");
}

// Copy operator. Raise a FatalError.
ReferenceSlot&
ReferenceSlot::operator=(const ReferenceSlot& from)
{
  GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy operator.");
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
