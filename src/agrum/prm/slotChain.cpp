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
 * @brief Source implementation of gum::SlotChain
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/slotChain.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/slotChain.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {

SlotChain::SlotChain(const std::string& name, Class& start, Class& end,
                     Sequence<ReferenceSlot*>& chain, ClassElement& lastElt):
  ClassElement(name), __start(start), __end(end), __seq(chain), __lastElt(lastElt),
  __isMultiple(false)
{
  GUM_CONSTRUCTOR( SlotChain );
  for (Size i = 0; i < __seq.size(); ++i) {
    if (__seq.atPos(i)->elt_type() == ClassElement::prm_refslot) {
      if (__seq.atPos(i)->isArray()) {
        __isMultiple = true;
        break;
      }
    }
  }
}

SlotChain::~SlotChain()
{
  GUM_DESTRUCTOR( SlotChain );
}

SlotChain::SlotChain(const SlotChain& source):
  ClassElement(source), __start(source.__start), __end(source.__end),
  __seq(source.__seq), __lastElt(source.__lastElt), __isMultiple(source.__isMultiple)
{
  GUM_CONS_CPY( SlotChain );
  GUM_ERROR(FatalError, "Illegal call to gum::SlotChain copy constructor.");
}

SlotChain&
SlotChain::operator=(const SlotChain& source)
{
  GUM_ERROR(FatalError, "Illegal call to gum::SlotChain copy constructor.");
}

// ============================================================================
} /* namespace gum */
// ============================================================================
