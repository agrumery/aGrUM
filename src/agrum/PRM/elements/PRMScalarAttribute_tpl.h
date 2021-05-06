/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
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
        type__(new PRMType(type)), cpf__(new Potential< GUM_SCALAR >(impl)) {
      GUM_CONSTRUCTOR(PRMScalarAttribute);
      cpf__->add(type__->variable());

      this->safeName_ = PRMObject::LEFT_CAST() + type__->name()
                      + PRMObject::RIGHT_CAST() + name;
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >::PRMScalarAttribute(
       const PRMScalarAttribute< GUM_SCALAR >& source) :
        PRMAttribute< GUM_SCALAR >(source),
        type__(0), cpf__(0) {
      GUM_CONS_CPY(PRMScalarAttribute);
      GUM_ERROR(FatalError,
                "Illegal call to the copy constructor of gum::ScalarAttribute");
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >::~PRMScalarAttribute() {
      GUM_DESTRUCTOR(PRMScalarAttribute);
      delete cpf__;
      delete type__;
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMScalarAttribute< GUM_SCALAR >::newFactory(
       const PRMClass< GUM_SCALAR >& c) const {
      auto impl = static_cast< MultiDimImplementation< GUM_SCALAR >* >(
         this->cpf().content()->newFactory());
      return new PRMScalarAttribute< GUM_SCALAR >(this->name(),
                                                  this->type(),
                                                  impl);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMScalarAttribute< GUM_SCALAR >::copy(
       Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const {
      auto copy = new PRMScalarAttribute< GUM_SCALAR >(this->name(), this->type());

      if (!bij.existsFirst(&(type().variable()))) {
        bij.insert(&(type().variable()), &(copy->type().variable()));
      }

      delete copy->cpf__;
      copy->cpf__ = copyPotential(bij, cpf());

      return copy;
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::copyCpf(
       const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
       const PRMAttribute< GUM_SCALAR >& source) {
      delete cpf__;
      cpf__ = new Potential< GUM_SCALAR >();

      for (auto var: source.cpf().variablesSequence()) {
        cpf__->add(*(bij.second(var)));
      }

      Instantiation inst(*cpf__), jnst(source.cpf());

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        cpf__->set(inst, source.cpf().get(jnst));
      }

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(cpf__->contains(type__->variable()));
      GUM_ASSERT(!cpf__->contains(source.type().variable()));
    }

    template < typename GUM_SCALAR >
    PRMScalarAttribute< GUM_SCALAR >& PRMScalarAttribute< GUM_SCALAR >::operator=(
       const PRMScalarAttribute< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError,
                "Illegal call to the copy operator of gum::ScalarAttribute");
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMScalarAttribute< GUM_SCALAR >::elt_type() const {
      return this->prm_attribute;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMScalarAttribute< GUM_SCALAR >::type() {
      return *type__;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMScalarAttribute< GUM_SCALAR >::type() const {
      return *type__;
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >&
                 PRMScalarAttribute< GUM_SCALAR >::cpf() const {
      return *cpf__;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMScalarAttribute< GUM_SCALAR >::addParent(
       const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        cpf__->add(elt.type().variable());
      } catch (DuplicateElement&) {
        GUM_ERROR(DuplicateElement,
                  elt.name() << " as parent of " << this->name());
      } catch (OperationNotAllowed&) {
        GUM_ERROR(OperationNotAllowed,
                  elt.name() << " of wrong type as parent of " << this->name());
      }
    }

    // See gum::PRMClassElement<GUM_SCALAR>::addChild_().
    template < typename GUM_SCALAR >
    INLINE void PRMScalarAttribute< GUM_SCALAR >::addChild(
       const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >*
       PRMScalarAttribute< GUM_SCALAR >::getCastDescendant() const {
      PRMScalarAttribute< GUM_SCALAR >* cast = 0;

      try {
        cast = new PRMScalarAttribute< GUM_SCALAR >(this->name(),
                                                    type().superType());
      } catch (NotFound&) {
        GUM_ERROR(OperationNotAllowed,
                  "this ScalarAttribute can not have cast descendant");
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
    void PRMScalarAttribute< GUM_SCALAR >::setAsCastDescendant(
       PRMAttribute< GUM_SCALAR >* cast) {
      try {
        type().setSuper(cast->type());
      } catch (OperationNotAllowed&) {
        GUM_ERROR(OperationNotAllowed,
                  "this ScalarAttribute can not have cast descendant");
      } catch (TypeError&) {
        std::stringstream msg;
        msg << type().name() << " is not a subtype of " << cast->type().name();
        GUM_ERROR(TypeError, msg.str())
      }
      cast->becomeCastDescendant(type());
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::becomeCastDescendant(PRMType& subtype) {
      delete cpf__;
      cpf__ = new Potential< GUM_SCALAR >();
      cpf__->add(type().variable());
      cpf__->add(subtype.variable());

      Instantiation inst(*cpf__);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        auto my_pos = inst.pos(subtype.variable());
        if (subtype.label_map()[my_pos] == inst.pos(type().variable())) {
          cpf__->set(inst, 1);
        } else {
          cpf__->set(inst, 0);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::swap(const PRMType& old_type,
                                                const PRMType& new_type) {
      if (&(old_type) == type__) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace attribute own type")
      }
      if (old_type->domainSize() != new_type->domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  "Cannot replace types with difference domain size");
      }
      if (!cpf__->contains(old_type.variable())) {
        GUM_ERROR(NotFound, "could not find variable " + old_type.name())
      }

      auto old = cpf__;

      cpf__ = new Potential< GUM_SCALAR >();

      for (auto var: old->variablesSequence()) {
        if (var != &(old_type.variable())) {
          cpf__->add(*var);
        } else {
          cpf__->add(new_type.variable());
        }
      }

      Instantiation inst(cpf__), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        cpf__->set(inst, old->get(jnst));
      }

      delete old;

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(cpf__->contains(type__->variable()));
      GUM_ASSERT(cpf__->contains(new_type.variable()));
      GUM_ASSERT(!cpf__->contains(old_type.variable()));
    }

    template < typename GUM_SCALAR >
    PRMType* PRMScalarAttribute< GUM_SCALAR >::type_() {
      return type__;
    }

    template < typename GUM_SCALAR >
    void PRMScalarAttribute< GUM_SCALAR >::type_(PRMType* t) {
      if (type__->variable().domainSize() != t->variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  "Cannot replace types with difference domain size");
      }
      auto old = cpf__;

      cpf__ = new Potential< GUM_SCALAR >();

      for (auto var: old->variablesSequence()) {
        if (var != &(type__->variable())) {
          cpf__->add(*var);
        } else {
          cpf__->add(t->variable());
        }
      }

      Instantiation inst(cpf__), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        cpf__->set(inst, old->get(jnst));
      }

      delete old;

      type__ = t;

      GUM_ASSERT(cpf__->contains(type__->variable()));
      GUM_ASSERT(inst.end() && jnst.end());
    }

  } /* namespace prm */
} /* namespace gum */
