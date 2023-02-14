/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Inline implementation of gum::PRMSlotChain<GUM_SCALAR>
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/elements/PRMSlotChain.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMSlotChain< GUM_SCALAR >::PRMSlotChain(
       const std::string&                                name,
       const Sequence< PRMClassElement< GUM_SCALAR >* >& chain) :
        PRMClassElement< GUM_SCALAR >(name),
        _chain_(new Sequence< PRMClassElement< GUM_SCALAR >* >(chain)), _isMultiple_(false) {
      GUM_CONSTRUCTOR(PRMSlotChain);

      if (_chain_->size() < 2) {
        GUM_ERROR(OperationNotAllowed, "chain must contain at least two ClassElement")
      }

      for (Size i = 0; i < _chain_->size() - 1; ++i) {
        if (_chain_->atPos(i)->elt_type() != PRMClassElement< GUM_SCALAR >::prm_refslot) {
          GUM_ERROR(WrongClassElement, "illegal ClassElement in chain")
        } else {
          _isMultiple_
             = _isMultiple_
            || static_cast< PRMReferenceSlot< GUM_SCALAR >* >(_chain_->atPos(i))->isArray();
        }
      }

      _copyLastElt_();

      this->safeName_
         = PRMObject::LEFT_CAST() + lastElt().type().name() + PRMObject::RIGHT_CAST() + name;
      this->lastElt().type().variable().setName(name);
    }

    // Parameters are inverse to prevent unwanted constructors calls (it
    // happened)
    template < typename GUM_SCALAR >
    PRMSlotChain< GUM_SCALAR >::PRMSlotChain(Sequence< PRMClassElement< GUM_SCALAR >* >* chain,
                                             const std::string&                          name) :
        PRMSlotChain(name, *chain) {
      // No need to
      // GUM_CONSTRUCTOR(PRMSlotChain);

      // if ( _chain_->size() < 2) {
      //  GUM_ERROR(OperationNotAllowed,
      //            "chain must containt at least two PRMClassElement")
      //}

      // for (Size i = 0; i <  _chain_->size() - 1; ++i) {
      //  if (not( _chain_->atPos(i)->elt_type() !=
      //          PRMClassElement<GUM_SCALAR>::prm_refslot)) {
      //    GUM_ERROR(WrongClassElement, "illegal PRMClassElement in chain")
      //  } else {
      //     _isMultiple_ =
      //         _isMultiple_ or
      //        static_cast<PRMReferenceSlot<GUM_SCALAR>
      //        *>( _chain_->atPos(i))->isArray();
      //  }
      //}

      // __copyLastElt();
      // this->safeName_ = PRMObject::LEFT_CAST() + lastElt().type().name() +
      // PRMObject::RIGHT_CAST() + name;
    }

    template < typename GUM_SCALAR >
    void PRMSlotChain< GUM_SCALAR >::_copyLastElt_() {
      PRMClassElement< GUM_SCALAR >* new_elt = nullptr;

      switch (_chain_->back()->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          auto old_attr = static_cast< const PRMAttribute< GUM_SCALAR >* >(_chain_->back());

          Bijection< const DiscreteVariable*, const DiscreteVariable* > bij;
          for (auto var: old_attr->cpf().variablesSequence()) {
            if (var != &(old_attr->type().variable())) { bij.insert(var, var); }
          }

          new_elt = old_attr->copy(bij);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
          const PRMAggregate< GUM_SCALAR >* c_agg
             = static_cast< const PRMAggregate< GUM_SCALAR >* >(_chain_->back());
          PRMAggregate< GUM_SCALAR >* agg = new PRMAggregate< GUM_SCALAR >(c_agg->name(),
                                                                           c_agg->agg_type(),
                                                                           c_agg->type(),
                                                                           c_agg->id());
          new_elt                         = agg;
          break;
        }

        default: {
          GUM_ERROR(WrongClassElement, "last element of chain is not valid")
        }
      }

      new_elt->setId(_chain_->back()->id());
      _chain_->setAtPos(_chain_->size() - 1, new_elt);
    }

    template < typename GUM_SCALAR >
    PRMSlotChain< GUM_SCALAR >::~PRMSlotChain() {
      GUM_DESTRUCTOR(PRMSlotChain);
      delete _chain_->back();
      delete _chain_;
    }

    template < typename GUM_SCALAR >
    PRMSlotChain< GUM_SCALAR >::PRMSlotChain(const PRMSlotChain< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source.name()),
        _chain_(new Sequence< PRMClassElement< GUM_SCALAR >* >(source.chain())),
        _isMultiple_(source.isMultiple()) {
      GUM_CONS_CPY(PRMSlotChain);
      _copyLastElt_();
    }

    template < typename GUM_SCALAR >
    PRMSlotChain< GUM_SCALAR >&
       PRMSlotChain< GUM_SCALAR >::operator=(const PRMSlotChain< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "Illegal call to gum::SlotChain<GUM_SCALAR> copy constructor.")
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMSlotChain< GUM_SCALAR >::elt_type() const {
      return this->prm_slotchain;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMSlotChain< GUM_SCALAR >::type() {
      return _chain_->back()->type();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMSlotChain< GUM_SCALAR >::type() const {
      return _chain_->back()->type();
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >& PRMSlotChain< GUM_SCALAR >::cpf() const {
      return _chain_->back()->cpf();
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >& PRMSlotChain< GUM_SCALAR >::end() {
      return static_cast< PRMReferenceSlot< GUM_SCALAR >* >(_chain_->atPos(_chain_->size() - 2))
         ->slotType();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElementContainer< GUM_SCALAR >& PRMSlotChain< GUM_SCALAR >::end() const {
      return static_cast< PRMReferenceSlot< GUM_SCALAR >* >(_chain_->atPos(_chain_->size() - 2))
         ->slotType();
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMSlotChain< GUM_SCALAR >::lastElt() {
      return *(_chain_->back());
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >& PRMSlotChain< GUM_SCALAR >::lastElt() const {
      return *(_chain_->back());
    }

    template < typename GUM_SCALAR >
    INLINE Sequence< PRMClassElement< GUM_SCALAR >* >& PRMSlotChain< GUM_SCALAR >::chain() {
      return *_chain_;
    }

    template < typename GUM_SCALAR >
    INLINE const Sequence< PRMClassElement< GUM_SCALAR >* >&
                 PRMSlotChain< GUM_SCALAR >::chain() const {
      return *_chain_;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMSlotChain< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    INLINE void PRMSlotChain< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    INLINE bool PRMSlotChain< GUM_SCALAR >::isMultiple() const {
      return _isMultiple_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >* PRMSlotChain< GUM_SCALAR >::getCastDescendant() const {
      GUM_ERROR(OperationNotAllowed, "This is a slotchain")
    }

  } /* namespace prm */
} /* namespace gum */
