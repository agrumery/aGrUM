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
 * @brief Inline implementation of gum::PRMScalarAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <sstream>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >::PRMScalarAttribute(
       const std::string&                    name,
       const PRMType&                        type,
       MultiDimImplementation< GUM_SCALAR >* impl) :
        PRMAttribute< GUM_SCALAR >(name),
        _type_(new PRMType(type)), _cpf_(new Potential< GUM_SCALAR >(impl)) {
      GUM_CONSTRUCTOR(PRMScalarAttribute);
      _cpf_->add(_type_->variable());

      this->safeName_ = PRMObject::LEFT_CAST() + _type_->name() + PRMObject::RIGHT_CAST() + name;
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >::PRMScalarAttribute(
       const PRMScalarAttribute< GUM_SCALAR >& source) :
        PRMAttribute< GUM_SCALAR >(source),
        _type_(0), _cpf_(0) {
      GUM_CONS_CPY(PRMScalarAttribute);
      GUM_ERROR(FatalError, "Illegal call to the copy constructor of gum::ScalarAttribute")
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >::~PRMScalarAttribute() {
      GUM_DESTRUCTOR(PRMScalarAttribute);
      delete _cpf_;
      delete _type_;
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >*
       PRMScalarAttribute< GUM_SCALAR >::newFactory(const PRMClass< GUM_SCALAR >& c) const {
      auto impl = static_cast< MultiDimImplementation< GUM_SCALAR >* >(
         this->cpf().content()->newFactory());
      return new PRMScalarAttribute< GUM_SCALAR >(this->name(), this->type(), impl);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMScalarAttribute< GUM_SCALAR >::copy(
       Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const {
      auto copy = new PRMScalarAttribute< GUM_SCALAR >(this->name(), this->type());

      if (!bij.existsFirst(&(type().variable()))) {
        bij.insert(&(type().variable()), &(copy->type().variable()));
      }

      delete copy->_cpf_;
      copy->_cpf_ = copyPotential(bij, cpf());

      return copy;
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::copyCpf(
       const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
       const PRMAttribute< GUM_SCALAR >&                                    source) {
      delete _cpf_;
      _cpf_ = new Potential< GUM_SCALAR >();

      for (auto var: source.cpf().variablesSequence()) {
        _cpf_->add(*(bij.second(var)));
      }

      Instantiation inst(*_cpf_), jnst(source.cpf());

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        _cpf_->set(inst, source.cpf().get(jnst));
      }

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(_cpf_->contains(_type_->variable()));
      GUM_ASSERT(!_cpf_->contains(source.type().variable()));
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >&
       PRMScalarAttribute< GUM_SCALAR >::operator=(const PRMScalarAttribute< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "Illegal call to the copy operator of gum::ScalarAttribute")
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMScalarAttribute< GUM_SCALAR >::elt_type() const {
      return this->prm_attribute;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMScalarAttribute< GUM_SCALAR >::type() {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMScalarAttribute< GUM_SCALAR >::type() const {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >& PRMScalarAttribute< GUM_SCALAR >::cpf() const {
      return *_cpf_;
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMScalarAttribute< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        _cpf_->add(elt.type().variable());
      } catch (DuplicateElement const&) {
        GUM_ERROR(DuplicateElement, elt.name() << " as parent of " << this->name())
      } catch (OperationNotAllowed const&) {
        GUM_ERROR(OperationNotAllowed, elt.name() << " of wrong type as parent of " << this->name())
      }
    }

    // See gum::PRMClassElement<GUM_SCALAR>::addChild_().
    template < typename GUM_SCALAR >
    INLINE void
       PRMScalarAttribute< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMScalarAttribute< GUM_SCALAR >::getCastDescendant() const {
      PRMScalarAttribute< GUM_SCALAR >* cast = 0;

      try {
        cast = new PRMScalarAttribute< GUM_SCALAR >(this->name(), type().superType());
      } catch (NotFound const&) {
        GUM_ERROR(OperationNotAllowed, "this ScalarAttribute can not have cast descendant")
      }

      cast->addParent(*this);
      const DiscreteVariable& my_var   = type().variable();
      DiscreteVariable&       cast_var = cast->type().variable();
      Instantiation           inst(cast->cpf());

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        if (type().label_map()[inst.val(my_var)] == inst.val(cast_var)) {
          cast->cpf().set(inst, 1);
        } else {
          cast->cpf().set(inst, 0);
        }
      }

      return cast;
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::setAsCastDescendant(PRMAttribute< GUM_SCALAR >* cast) {
      try {
        type().setSuper(cast->type());
      } catch (OperationNotAllowed const&) {
        GUM_ERROR(OperationNotAllowed, "this ScalarAttribute can not have cast descendant")
      } catch (TypeError const&) {
        std::stringstream msg;
        msg << type().name() << " is not a subtype of " << cast->type().name();
        GUM_ERROR(TypeError, msg.str())
      }
      cast->becomeCastDescendant(type());
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::becomeCastDescendant(PRMType& subtype) {
      delete _cpf_;
      _cpf_ = new Potential< GUM_SCALAR >();
      _cpf_->add(type().variable());
      _cpf_->add(subtype.variable());

      Instantiation inst(*_cpf_);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        auto my_pos = inst.pos(subtype.variable());
        if (subtype.label_map()[my_pos] == inst.pos(type().variable())) {
          _cpf_->set(inst, 1);
        } else {
          _cpf_->set(inst, 0);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::swap(const PRMType& old_type, const PRMType& new_type) {
      if (&(old_type) == _type_) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace attribute own type")
      }
      if (old_type->domainSize() != new_type->domainSize()) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace types with difference domain size")
      }
      if (!_cpf_->contains(old_type.variable())) {
        GUM_ERROR(NotFound, "could not find variable " + old_type.name())
      }

      auto old = _cpf_;

      _cpf_ = new Potential< GUM_SCALAR >();

      for (auto var: old->variablesSequence()) {
        if (var != &(old_type.variable())) {
          _cpf_->add(*var);
        } else {
          _cpf_->add(new_type.variable());
        }
      }

      Instantiation inst(_cpf_), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        _cpf_->set(inst, old->get(jnst));
      }

      delete old;

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(_cpf_->contains(_type_->variable()));
      GUM_ASSERT(_cpf_->contains(new_type.variable()));
      GUM_ASSERT(!_cpf_->contains(old_type.variable()));
    }

    template < typename GUM_SCALAR >
    PRMType* PRMScalarAttribute< GUM_SCALAR >::type_() {
      return _type_;
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::type_(PRMType* t) {
      if (_type_->variable().domainSize() != t->variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace types with difference domain size")
      }
      auto old = _cpf_;

      _cpf_ = new Potential< GUM_SCALAR >();

      for (auto var: old->variablesSequence()) {
        if (var != &(_type_->variable())) {
          _cpf_->add(*var);
        } else {
          _cpf_->add(t->variable());
        }
      }

      Instantiation inst(_cpf_), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        _cpf_->set(inst, old->get(jnst));
      }

      delete old;

      _type_ = t;

      GUM_ASSERT(_cpf_->contains(_type_->variable()));
      GUM_ASSERT(inst.end() && jnst.end());
    }

  } /* namespace prm */
} /* namespace gum */
