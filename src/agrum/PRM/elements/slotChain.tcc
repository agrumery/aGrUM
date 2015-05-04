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
 * @brief Inline implementation of gum::SlotChain<GUM_SCALAR>
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/slotChain.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/aggregate.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    SlotChain<GUM_SCALAR>::SlotChain(
        const std::string &name, const Sequence<ClassElement<GUM_SCALAR> *> &chain)
        : ClassElement<GUM_SCALAR>(name),
          __chain(new Sequence<ClassElement<GUM_SCALAR> *>(chain)),
          __isMultiple(false) {
      GUM_CONSTRUCTOR(SlotChain);
      this->_safeName = name;

      if (__chain->size() < 2) {
        GUM_ERROR(OperationNotAllowed,
                  "chain must contain at least two ClassElement");
      }

      for (Size i = 0; i < __chain->size() - 1; ++i) {
        if (__chain->atPos(i)->elt_type() != ClassElement<GUM_SCALAR>::prm_refslot) {
          GUM_ERROR(WrongClassElement, "illegal ClassElement in chain");
        } else {
          __isMultiple =
              __isMultiple or
              static_cast<ReferenceSlot<GUM_SCALAR> *>(__chain->atPos(i))->isArray();
        }
      }

      __copyLastElt();
    }

    // Parameters are inverse to prevent unwanted constructors calls (it happened)
    template <typename GUM_SCALAR>
    SlotChain<GUM_SCALAR>::SlotChain(Sequence<ClassElement<GUM_SCALAR> *> *chain,
                                     const std::string &name)
        : ClassElement<GUM_SCALAR>(name), __chain(chain), __isMultiple(false) {
      GUM_CONSTRUCTOR(SlotChain);
      this->_safeName = name;

      if (__chain->size() < 2) {
        GUM_ERROR(OperationNotAllowed,
                  "chain must containt at least two ClassElement");
      }

      for (Size i = 0; i < __chain->size() - 1; ++i) {
        if (not(__chain->atPos(i)->elt_type() !=
                ClassElement<GUM_SCALAR>::prm_refslot)) {
          GUM_ERROR(WrongClassElement, "illegal ClassElement in chain");
        } else {
          __isMultiple =
              __isMultiple or
              static_cast<ReferenceSlot<GUM_SCALAR> *>(__chain->atPos(i))->isArray();
        }
      }

      __copyLastElt();
    }

    template <typename GUM_SCALAR> void SlotChain<GUM_SCALAR>::__copyLastElt() {
      ClassElement<GUM_SCALAR> *new_elt = 0;

      switch (__chain->back()->elt_type()) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          const Attribute<GUM_SCALAR> *old_attr =
              static_cast<const Attribute<GUM_SCALAR> *>(__chain->back());
          Type<GUM_SCALAR> *t = new Type<GUM_SCALAR>(old_attr->type());
          Bijection<const DiscreteVariable *, const DiscreteVariable *> bij;
          bij.insert(&(old_attr->type().variable()), &(t->variable()));

          for (MultiDimInterface::iterator iter = __chain->back()->cpf().begin();
               iter != __chain->back()->cpf().end(); ++iter)
            if ((*iter) != &(old_attr->type().variable()))
              bij.insert(*iter, *iter);

          Potential<GUM_SCALAR> *p = copyPotential(bij, __chain->back()->cpf());
          new_elt = new Attribute<GUM_SCALAR>(__chain->back()->name(), t, p, true);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate: {
          const Aggregate<GUM_SCALAR> *c_agg =
              static_cast<const Aggregate<GUM_SCALAR> *>(__chain->back());
          Aggregate<GUM_SCALAR> *agg = new Aggregate<GUM_SCALAR>(
              c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->id());
          new_elt = agg;
          break;
        }

        default: {
          GUM_ERROR(WrongClassElement, "last element of chain is not valid");
        }
      }

      new_elt->setId(__chain->back()->id());
      __chain->setAtPos(__chain->size() - 1, new_elt);
    }

    template <typename GUM_SCALAR> SlotChain<GUM_SCALAR>::~SlotChain() {
      GUM_DESTRUCTOR(SlotChain);
      delete __chain->back();
      delete __chain;
    }

    template <typename GUM_SCALAR>
    SlotChain<GUM_SCALAR>::SlotChain(const SlotChain<GUM_SCALAR> &source)
        : ClassElement<GUM_SCALAR>(source.name()),
          __chain(new Sequence<ClassElement<GUM_SCALAR> *>(source.chain())),
          __isMultiple(source.isMultiple()) {
      GUM_CONS_CPY(SlotChain);
      __copyLastElt();
    }

    template <typename GUM_SCALAR>
    SlotChain<GUM_SCALAR> &SlotChain<GUM_SCALAR>::
    operator=(const SlotChain<GUM_SCALAR> &source) {
      GUM_ERROR(FatalError,
                "Illegal call to gum::SlotChain<GUM_SCALAR> copy constructor.");
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    SlotChain<GUM_SCALAR>::elt_type() const {
      return this->prm_slotchain;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &SlotChain<GUM_SCALAR>::type() {
      return __chain->back()->type();
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR> &SlotChain<GUM_SCALAR>::type() const {
      return __chain->back()->type();
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> &SlotChain<GUM_SCALAR>::cpf() {
      return __chain->back()->cpf();
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR> &SlotChain<GUM_SCALAR>::cpf() const {
      return __chain->back()->cpf();
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR> &SlotChain<GUM_SCALAR>::end() {
      return static_cast<ReferenceSlot<GUM_SCALAR> *>(
                 __chain->atPos(__chain->size() - 2))->slotType();
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElementContainer<GUM_SCALAR> &
    SlotChain<GUM_SCALAR>::end() const {
      return static_cast<ReferenceSlot<GUM_SCALAR> *>(
                 __chain->atPos(__chain->size() - 2))->slotType();
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> &SlotChain<GUM_SCALAR>::lastElt() {
      return *(__chain->back());
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &SlotChain<GUM_SCALAR>::lastElt() const {
      return *(__chain->back());
    }

    template <typename GUM_SCALAR>
    INLINE Sequence<ClassElement<GUM_SCALAR> *> &SlotChain<GUM_SCALAR>::chain() {
      return *__chain;
    }

    template <typename GUM_SCALAR>
    INLINE const Sequence<ClassElement<GUM_SCALAR> *> &
    SlotChain<GUM_SCALAR>::chain() const {
      return *__chain;
    }

    template <typename GUM_SCALAR>
    INLINE void
    SlotChain<GUM_SCALAR>::addParent(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE void
    SlotChain<GUM_SCALAR>::addChild(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE bool SlotChain<GUM_SCALAR>::isMultiple() const {
      return __isMultiple;
    }

  } /* namespace prm */
} /* namespace gum */
