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
 * @brief Inline implementation of gum::PRMFormAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
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
    PRMFormAttribute< GUM_SCALAR >::PRMFormAttribute(const PRMClass< GUM_SCALAR >&          c,
                                                     const std::string&                     name,
                                                     const PRMType&                         type,
                                                     MultiDimImplementation< std::string >* impl) :
        PRMAttribute< GUM_SCALAR >(name),
        _type_(new PRMType(type)), _cpf_(0), _formulas_(impl), _class_(&c) {
      GUM_CONSTRUCTOR(PRMFormAttribute);
      _formulas_->add(_type_->variable());
      this->safeName_ = PRMObject::LEFT_CAST() + _type_->name() + PRMObject::RIGHT_CAST() + name;
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >::~PRMFormAttribute() {
      GUM_DESTRUCTOR(PRMFormAttribute);
      delete _type_;
      delete _cpf_;
      delete _formulas_;
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >*
       PRMFormAttribute< GUM_SCALAR >::newFactory(const PRMClass< GUM_SCALAR >& c) const {
      auto impl
         = static_cast< MultiDimImplementation< std::string >* >(this->_formulas_->newFactory());
      return new PRMFormAttribute< GUM_SCALAR >(c, this->name(), this->type(), impl);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMFormAttribute< GUM_SCALAR >::copy(
       Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const {
      auto copy = new PRMFormAttribute< GUM_SCALAR >(*_class_, this->name(), this->type());
      for (auto var: _formulas_->variablesSequence()) {
        if (var != &(_type_->variable())) { copy->_formulas_->add(*var); }
      }

      Instantiation inst(*(copy->_formulas_)), jnst(*_formulas_);
      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        copy->_formulas_->set(inst, _formulas_->get(jnst));
      }

      GUM_ASSERT(copy->_formulas_->contains(copy->_type_->variable()));
      return copy;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::copyCpf(
       const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
       const PRMAttribute< GUM_SCALAR >&                                    source) {
      delete _formulas_;
      _formulas_ = new MultiDimArray< std::string >();

      for (const auto& var: source.cpf().variablesSequence()) {
        _formulas_->add(*(bij.second(var)));
      }

      if (dynamic_cast< const PRMFormAttribute< GUM_SCALAR >* >(&source)) {
        const auto& src = static_cast< const PRMFormAttribute< GUM_SCALAR >& >(source);

        Instantiation inst(_formulas_), jnst(src._formulas_);

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          _formulas_->set(inst, src._formulas_->get(jnst));
        }

        GUM_ASSERT(inst.end() && jnst.end());

      } else {
        Instantiation inst(_formulas_), jnst(source.cpf());

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          auto val = std::to_string(source.cpf().get(jnst));
          _formulas_->set(inst, val);
        }

        GUM_ASSERT(inst.end() && jnst.end());
      }

      if (_cpf_) {
        delete _cpf_;
        _cpf_ = 0;
      }

      GUM_ASSERT(_formulas_->contains(_type_->variable()));
      GUM_ASSERT(!_formulas_->contains(source.type().variable()));
    }

    template < typename GUM_SCALAR >
    typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMFormAttribute< GUM_SCALAR >::elt_type() const {
      return this->prm_attribute;
    }

    template < typename GUM_SCALAR >
    PRMType& PRMFormAttribute< GUM_SCALAR >::type() {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    const PRMType& PRMFormAttribute< GUM_SCALAR >::type() const {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    const Potential< GUM_SCALAR >& PRMFormAttribute< GUM_SCALAR >::cpf() const {
      if (_cpf_ == 0) { _fillCpf_(); }
      return *_cpf_;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        if (_cpf_) {
          delete _cpf_;
          _cpf_ = 0;
        }
        _formulas_->add(elt.type().variable());
      } catch (DuplicateElement const&) {
        GUM_ERROR(DuplicateElement, elt.name() << " as parent of " << this->name())
      } catch (OperationNotAllowed const&) {
        GUM_ERROR(OperationNotAllowed,
                  elt.name() << " of wrong type as parent of " << this->name();)
      }

      GUM_ASSERT(_formulas_->contains(_type_->variable()));
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMFormAttribute< GUM_SCALAR >::getCastDescendant() const {
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

      GUM_ASSERT(_formulas_->contains(_type_->variable()));
      return cast;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::setAsCastDescendant(PRMAttribute< GUM_SCALAR >* cast) {
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
    void PRMFormAttribute< GUM_SCALAR >::becomeCastDescendant(PRMType& subtype) {
      delete _formulas_;

      _formulas_ = new MultiDimArray< std::string >();
      _formulas_->add(type().variable());
      _formulas_->add(subtype.variable());

      Instantiation inst(_formulas_);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        auto my_pos = inst.pos(subtype.variable());
        if (subtype.label_map()[my_pos] == inst.pos(type().variable())) {
          _formulas_->set(inst, "1");
        } else {
          _formulas_->set(inst, "0");
        }
      }

      if (_cpf_) {
        delete _cpf_;
        _cpf_ = nullptr;
      }
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >::PRMFormAttribute(const PRMFormAttribute& source) :
        PRMAttribute< GUM_SCALAR >(source.name()) {
      GUM_CONS_CPY(PRMFormAttribute);
      GUM_ERROR(OperationNotAllowed, "Cannot copy FormAttribute")
    }

    template < typename GUM_SCALAR >
    PRMFormAttribute< GUM_SCALAR >&
       PRMFormAttribute< GUM_SCALAR >::operator=(const PRMFormAttribute< GUM_SCALAR >& source) {
      GUM_ERROR(OperationNotAllowed, "Cannot copy FormAttribute")
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::_fillCpf_() const {
      try {
        if (_cpf_) { delete _cpf_; }

        _cpf_ = new Potential< GUM_SCALAR >();

        for (auto var: _formulas_->variablesSequence()) {
          _cpf_->add(*var);
        }

        auto params = _class_->scope();

        Instantiation inst(_formulas_);
        Instantiation jnst(_cpf_);

        for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
             inst.inc(), jnst.inc()) {
          // With CPT defined using rules, empty values can appear
          auto val = _formulas_->get(inst);
          if (val.empty()) { val = "0.0"; }

          Formula f(val);

          for (auto item: params) {
            f.variables().insert(item.first, item.second->value());
          }

          _cpf_->set(jnst, (GUM_SCALAR)f.result());
        }

        GUM_ASSERT(inst.end() && jnst.end());

      } catch (Exception const&) { GUM_ERROR(NotFound, "undefined value in cpt") }
      GUM_ASSERT(_formulas_->contains(_type_->variable()))
    }

    template < typename GUM_SCALAR >
    MultiDimImplementation< std::string >& PRMFormAttribute< GUM_SCALAR >::formulas() {
      if (_cpf_) {
        delete _cpf_;
        _cpf_ = 0;
      }
      return *_formulas_;
    }

    template < typename GUM_SCALAR >
    const MultiDimImplementation< std::string >& PRMFormAttribute< GUM_SCALAR >::formulas() const {
      return *_formulas_;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::swap(const PRMType& old_type, const PRMType& new_type) {
      if (&(old_type) == _type_) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace attribute own type")
      }
      if (old_type->domainSize() != new_type->domainSize()) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace types with difference domain size")
      }
      if (!_formulas_->contains(old_type.variable())) {
        GUM_ERROR(NotFound, "could not find variable " + old_type.name())
      }

      auto old = _formulas_;

      _formulas_ = new MultiDimArray< std::string >();

      for (auto var: old->variablesSequence()) {
        if (var != &(old_type.variable())) {
          _formulas_->add(*var);
        } else {
          _formulas_->add(new_type.variable());
        }
      }

      Instantiation inst(_formulas_), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        _formulas_->set(inst, old->get(jnst));
      }

      delete old;

      if (_cpf_) {
        delete _cpf_;
        _cpf_ = 0;
      }

      GUM_ASSERT(inst.end() && jnst.end());
      GUM_ASSERT(_formulas_->contains(_type_->variable()));
      GUM_ASSERT(!_formulas_->contains(new_type.variable()));
      GUM_ASSERT(_formulas_->contains(new_type.variable()));
    }

    template < typename GUM_SCALAR >
    PRMType* PRMFormAttribute< GUM_SCALAR >::type_() {
      return _type_;
    }

    template < typename GUM_SCALAR >
    void PRMFormAttribute< GUM_SCALAR >::type_(PRMType* t) {
      if (_type_->variable().domainSize() != t->variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "Cannot replace types with difference domain size")
      }
      auto old = _formulas_;

      _formulas_ = new MultiDimArray< std::string >();

      for (auto var: old->variablesSequence()) {
        if (var != &(_type_->variable())) {
          _formulas_->add(*var);
        } else {
          _formulas_->add(t->variable());
        }
      }

      Instantiation inst(_formulas_), jnst(old);

      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        _formulas_->set(inst, old->get(jnst));
      }

      delete old;

      _type_ = t;

      if (_cpf_) {
        delete _cpf_;
        _cpf_ = 0;
      }

      GUM_ASSERT(_formulas_->contains(_type_->variable()));
      GUM_ASSERT(inst.end() && jnst.end());
    }

  } /* namespace prm */
} /* namespace gum */
