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
 * @brief Inline implementation of gum::PRMFormAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <iostream>

#include <agrum/tools/core/math/formula.h>

#include <agrum/PRM/elements/PRMScalarAttribute.h>
#include <agrum/PRM/elements/PRMType.h>

// to ease IDE parser
#include <agrum/PRM/elements/PRMFormAttribute.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >::PRMFormAttribute(
       const PRMClass< GUM_SCALAR >&          c,
       const std::string&                     name,
       const PRMType&                         type,
       MultiDimImplementation< std::string >* impl) :
        PRMAttribute< GUM_SCALAR >(name),
        type__(new PRMType(type)), cpf__(0), formulas__(impl), class__(&c) {
      GUM_CONSTRUCTOR(PRMFormAttribute);
      formulas__->add(type__->variable());
      this->safeName_ = PRMObject::LEFT_CAST() + type__->name()
                      + PRMObject::RIGHT_CAST() + name;
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >::~PRMFormAttribute() {
      GUM_DESTRUCTOR(PRMFormAttribute);
      delete type__;
      delete cpf__;
      delete formulas__;
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMFormAttribute< GUM_SCALAR >::newFactory(
       const PRMClass< GUM_SCALAR >& c) const {
      auto impl = static_cast< MultiDimImplementation< std::string >* >(
         this->formulas__->newFactory());
      return new PRMFormAttribute< GUM_SCALAR >(c,
                                                this->name(),
                                                this->type(),
                                                impl);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMFormAttribute< GUM_SCALAR >::copy(
       Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const {
      auto copy = new PRMFormAttribute< GUM_SCALAR >(*class__,
                                                     this->name(),
                                                     this->type());
      for (auto var: formulas__->variablesSequence()) {
        if (var != &(type__->variable())) { copy->formulas__->add(*var); }
      }

      Instantiation inst(*(copy->formulas__)), jnst(*formulas__);
      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        copy->formulas__->set(inst, formulas__->get(jnst));
      }

      GUM_ASSERT(copy->formulas__->contains(copy->type__->variable()));
      return copy;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::copyCpf(
       const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
       const PRMAttribute< GUM_SCALAR >& source) {
      delete formulas__;
      formulas__ = new MultiDimArray< std::string >();

      for (const auto& var: source.cpf().variablesSequence()) {
        formulas__->add(*(bij.second(var)));
      }

      if (dynamic_cast< const PRMFormAttribute< GUM_SCALAR >* >(&source)) {
        const auto& src
           = static_cast< const PRMFormAttribute< GUM_SCALAR >& >(source);

        Instantiation inst(formulas__), jnst(src.formulas__);

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          formulas__->set(inst, src.formulas__->get(jnst));
        }

        GUM_ASSERT(inst.end() && jnst.end());

      } else {
        Instantiation inst(formulas__), jnst(source.cpf());

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          auto val = std::to_string(source.cpf().get(jnst));
          formulas__->set(inst, val);
        }

        GUM_ASSERT(inst.end() && jnst.end());
      }

      if (cpf__) {
        delete cpf__;
        cpf__ = 0;
      }

      GUM_ASSERT(formulas__->contains(type__->variable()));
      GUM_ASSERT(!formulas__->contains(source.type().variable()));
    }

    template < typename GUM_SCALAR >
    typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMFormAttribute< GUM_SCALAR >::elt_type() const {
      return this->prm_attribute;
    }

    template < typename GUM_SCALAR >
    PRMType& PRMFormAttribute< GUM_SCALAR >::type() {
      return *type__;
    }

    template < typename GUM_SCALAR >
    const PRMType& PRMFormAttribute< GUM_SCALAR >::type() const {
      return *type__;
    }

    template < typename GUM_SCALAR >
    const Potential< GUM_SCALAR >& PRMFormAttribute< GUM_SCALAR >::cpf() const {
      if (cpf__ == 0) { fillCpf__(); }
      return *cpf__;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::addParent(
       const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        if (cpf__) {
          delete cpf__;
          cpf__ = 0;
        }
        formulas__->add(elt.type().variable());
      } catch (DuplicateElement&) {
        GUM_ERROR(DuplicateElement,
                  elt.name() << " as parent of " << this->name());
      } catch (OperationNotAllowed&) {
        GUM_ERROR(OperationNotAllowed,
                  elt.name() << " of wrong type as parent of " << this->name(););
      }

      GUM_ASSERT(formulas__->contains(type__->variable()));
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::addChild(
       const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >*
       PRMFormAttribute< GUM_SCALAR >::getCastDescendant() const {
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

      GUM_ASSERT(formulas__->contains(type__->variable()));
      return cast;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::setAsCastDescendant(
       PRMAttribute< GUM_SCALAR >* cast) {
      try {
        type().setSuper(cast->type());
      } catch (OperationNotAllowed&) {
        GUM_ERROR(OperationNotAllowed,
                  "this ScalarAttribute can not have cast descendant");
      } catch (TypeError&) {
        std::stringstream msg;
        msg << type().name() << " is not a subtype of " << cast->type().name();
        GUM_ERROR(TypeError, msg.str());
      }

      cast->becomeCastDescendant(type());
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::becomeCastDescendant(PRMType& subtype) {
      delete formulas__;

      formulas__ = new MultiDimArray< std::string >();
      formulas__->add(type().variable());
      formulas__->add(subtype.variable());

      Instantiation inst(formulas__);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        auto my_pos = inst.pos(subtype.variable());
        if (subtype.label_map()[my_pos] == inst.pos(type().variable())) {
          formulas__->set(inst, "1");
        } else {
          formulas__->set(inst, "0");
        }
      }

      if (cpf__) {
        delete cpf__;
        cpf__ = nullptr;
      }
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >::PRMFormAttribute(
       const PRMFormAttribute& source) :
        PRMAttribute< GUM_SCALAR >(source.name()) {
      GUM_CONS_CPY(PRMFormAttribute);
      GUM_ERROR(OperationNotAllowed, "Cannot copy FormAttribute");
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >& PRMFormAttribute< GUM_SCALAR >::operator=(
       const PRMFormAttribute< GUM_SCALAR >& source) {
      GUM_ERROR(OperationNotAllowed, "Cannot copy FormAttribute");
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::fillCpf__() const {
      try {
        if (cpf__) { delete cpf__; }

        cpf__ = new Potential< GUM_SCALAR >();

        for (auto var: formulas__->variablesSequence()) {
          cpf__->add(*var);
        }

        auto params = class__->scope();

        Instantiation inst(formulas__);
        Instantiation jnst(cpf__);

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          // With CPT defined using rules, empty values can appear
          auto val = formulas__->get(inst);
          if (val == "") { val = "0.0"; }

          Formula f(val);

          for (auto item: params) {
            f.variables().insert(item.first, item.second->value());
          }

          cpf__->set(jnst, (GUM_SCALAR)f.result());
        }

        GUM_ASSERT(inst.end() && jnst.end());

      } catch (Exception&) { GUM_ERROR(NotFound, "undefined value in cpt"); }
      GUM_ASSERT(formulas__->contains(type__->variable()));
    }

    template < typename GUM_SCALAR >
    MultiDimImplementation< std::string >&
       PRMFormAttribute< GUM_SCALAR >::formulas() {
      if (cpf__) {
        delete cpf__;
        cpf__ = 0;
      }
      return *formulas__;
    }

    template < typename GUM_SCALAR >
    const MultiDimImplementation< std::string >&
       PRMFormAttribute< GUM_SCALAR >::formulas() const {
      return *formulas__;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::swap(const PRMType& old_type,
                                              const PRMType& new_type) {
      if (&(old_type) == type__) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace attribute own type");
      }
      if (old_type->domainSize() != new_type->domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  "Cannot replace types with difference domain size");
      }
      if (!formulas__->contains(old_type.variable())) {
        GUM_ERROR(NotFound, "could not find variable " + old_type.name());
      }

      auto old = formulas__;

      formulas__ = new MultiDimArray< std::string >();

      for (auto var: old->variablesSequence()) {
        if (var != &(old_type.variable())) {
          formulas__->add(*var);
        } else {
          formulas__->add(new_type.variable());
        }
      }

      Instantiation inst(formulas__), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        formulas__->set(inst, old->get(jnst));
      }

      delete old;

      if (cpf__) {
        delete cpf__;
        cpf__ = 0;
      }

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(formulas__->contains(type__->variable()));
      GUM_ASSERT(!formulas__->contains(new_type.variable()));
      GUM_ASSERT(formulas__->contains(new_type.variable()));
    }

    template < typename GUM_SCALAR >
    PRMType* PRMFormAttribute< GUM_SCALAR >::type_() {
      return type__;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::type_(PRMType* t) {
      if (type__->variable().domainSize() != t->variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  "Cannot replace types with difference domain size");
      }
      auto old = formulas__;

      formulas__ = new MultiDimArray< std::string >();

      for (auto var: old->variablesSequence()) {
        if (var != &(type__->variable())) {
          formulas__->add(*var);
        } else {
          formulas__->add(t->variable());
        }
      }

      Instantiation inst(formulas__), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
           inst.inc(), jnst.inc()) {
        formulas__->set(inst, old->get(jnst));
      }

      delete old;

      type__ = t;

      if (cpf__) {
        delete cpf__;
        cpf__ = 0;
      }

      GUM_ASSERT(formulas__->contains(type__->variable()));
      GUM_ASSERT(inst.end() && jnst.end());
    }

  } /* namespace prm */
} /* namespace gum */
