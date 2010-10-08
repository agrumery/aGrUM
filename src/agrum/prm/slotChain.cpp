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
namespace prm {

SlotChain::SlotChain(const std::string& name, const Sequence< ClassElement* >& chain):
  ClassElement(name), __chain(new Sequence<ClassElement*>(chain)), __isMultiple(false)
{
  GUM_CONSTRUCTOR( SlotChain );
  _safeName = name;
  if (__chain->size() < 2) {
    GUM_ERROR(OperationNotAllowed, "chain must contain at least two ClassElement");
  }
  for (Size i = 0; i < __chain->size() - 1; ++i) {
    if (__chain->atPos(i)->elt_type() != ClassElement::prm_refslot) {
      GUM_ERROR(WrongClassElement, "illegal ClassElement in chain");
    } else {
      __isMultiple = __isMultiple or static_cast<ReferenceSlot*>(__chain->atPos(i))->isArray();
    }
  }
  __copyLastElt();
}

// Parameters are inverse to prevent unwanted constructors calls (it happened)
SlotChain::SlotChain(Sequence<ClassElement*>* chain, const std::string& name):
  ClassElement(name), __chain(chain), __isMultiple(false)
{
  GUM_CONSTRUCTOR( SlotChain );
  _safeName = name;
  if (__chain->size() < 2) {
    GUM_ERROR(OperationNotAllowed, "chain must containt at least two ClassElement");
  }
  for (Size i = 0; i < __chain->size() - 1; ++i) {
    if (not (__chain->atPos(i)->elt_type() != ClassElement::prm_refslot) ) {
      GUM_ERROR(WrongClassElement, "illegal ClassElement in chain");
    } else {
      __isMultiple = __isMultiple or static_cast<ReferenceSlot*>(__chain->atPos(i))->isArray();
    }
  }
  __copyLastElt();
}

void
SlotChain::__copyLastElt() {
  ClassElement* new_elt = 0;
  switch (__chain->back()->elt_type()) {
    case prm_attribute:
      {
        const Attribute* old_attr = static_cast<const Attribute*>(__chain->back());
        Type* t = new Type(old_attr->type());
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
        bij.insert(&(old_attr->type().variable()), &(t->variable()));
        for (MultiDimInterface::iterator iter = __chain->back()->cpf().begin(); iter != __chain->back()->cpf().end(); ++iter)
          if ((*iter) != &(old_attr->type().variable()))
            bij.insert(*iter, *iter);
        Potential<prm_float>* p = copyPotential(bij, __chain->back()->cpf());
        new_elt = new Attribute(__chain->back()->name(), t, p, true);
        break;
      }
    case prm_aggregate:
      {
        const Aggregate* c_agg = static_cast<const Aggregate*>(__chain->back());
        Aggregate* agg = new Aggregate(c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->id());
        new_elt = agg;
        break;
      }
    default:
      {
        GUM_ERROR(WrongClassElement, "last element of chain is not valid");
      }
  }
  new_elt->setId(__chain->back()->id());
  __chain->setAtPos(__chain->size() - 1, new_elt);
}

SlotChain::~SlotChain()
{
  GUM_DESTRUCTOR( SlotChain );
  delete __chain->back();
  delete __chain;
}

SlotChain::SlotChain(const SlotChain& source):
  ClassElement(source.name()), __chain(new Sequence<ClassElement*>(source.chain())),
  __isMultiple(source.isMultiple())
{
  GUM_CONS_CPY( SlotChain );
  __copyLastElt();
}

SlotChain&
SlotChain::operator=(const SlotChain& source)
{
  GUM_ERROR(FatalError, "Illegal call to gum::SlotChain copy constructor.");
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
