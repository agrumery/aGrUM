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


/** @file
 * @brief Inline implementation of PRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/PRMFactory.h>

#include <iostream>
#include <sstream>

#include <agrum/tools/core/math/formula.h>

#include <agrum/tools/variables/allDiscreteVariables.h>

#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMFuncAttribute.h>

namespace gum {

  namespace prm {

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startClass(const std::string&        name,
                                                     const std::string&        extends,
                                                     const Set< std::string >* implements,
                                                     bool                      delayInheritance) {
      std::string real_name = _addPrefix_(name);
      if (_prm_->_classMap_.exists(real_name) || _prm_->_interfaceMap_.exists(real_name)) {
        GUM_ERROR(DuplicateElement, "'" << real_name << "' is already used.")
      }
      PRMClass< GUM_SCALAR >*            c      = nullptr;
      PRMClass< GUM_SCALAR >*            mother = nullptr;
      Set< PRMInterface< GUM_SCALAR >* > impl;

      if (implements != 0) {
        for (const auto& imp: *implements) {
          impl.insert(_retrieveInterface_(imp));
        }
      }

      if (extends != "") { mother = _retrieveClass_(extends); }

      if ((extends.empty()) && impl.empty()) {
        c = new PRMClass< GUM_SCALAR >(real_name);
      } else if ((extends != "") && impl.empty()) {
        c = new PRMClass< GUM_SCALAR >(real_name, *mother, delayInheritance);
      } else if ((extends.empty()) && (!impl.empty())) {
        c = new PRMClass< GUM_SCALAR >(real_name, impl, delayInheritance);
      } else if ((extends != "") && (!impl.empty())) {
        c = new PRMClass< GUM_SCALAR >(real_name, *mother, impl, delayInheritance);
      }

      _prm_->_classMap_.insert(c->name(), c);
      _prm_->_classes_.insert(c);
      _stack_.push_back(c);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::continueClass(const std::string& name) {
      std::string real_name = _addPrefix_(name);
      if (!(_prm_->_classMap_.exists(real_name))) {
        std::stringstream msg;
        msg << "'" << real_name << "' not found";
        GUM_ERROR(NotFound, msg.str())
      }
      _stack_.push_back(&(_prm_->getClass(real_name)));
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endClass(bool checkImplementations) {
      PRMClass< GUM_SCALAR >* c
         = static_cast< PRMClass< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::CLASS));

      if (checkImplementations) { _checkInterfaceImplementation_(c); }

      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::_checkInterfaceImplementation_(PRMClass< GUM_SCALAR >* c) {
      try {
        for (const auto& i: c->implements()) {
          try {
            for (const auto& node: i->containerDag().nodes()) {
              std::string name = i->get(node).name();

              switch (i->get(node).elt_type()) {
                case PRMClassElement< GUM_SCALAR >::prm_aggregate:
                case PRMClassElement< GUM_SCALAR >::prm_attribute: {
                  if ((c->get(name).elt_type() == PRMClassElement< GUM_SCALAR >::prm_attribute)
                      || (c->get(name).elt_type()
                          == PRMClassElement< GUM_SCALAR >::prm_aggregate)) {
                    if (!c->get(name).type().isSubTypeOf(i->get(name).type())) {
                      std::stringstream msg;
                      msg << "class " << c->name() << " does not respect interface ";
                      GUM_ERROR(PRMTypeError, msg.str() + i->name())
                    }
                  } else {
                    std::stringstream msg;
                    msg << "class " << c->name() << " does not respect interface ";
                    GUM_ERROR(PRMTypeError, msg.str() + i->name())
                  }

                  break;
                }

                case PRMClassElement< GUM_SCALAR >::prm_refslot: {
                  if (c->get(name).elt_type() == PRMClassElement< GUM_SCALAR >::prm_refslot) {
                    const PRMReferenceSlot< GUM_SCALAR >& ref_i
                       = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(i->get(name));
                    const PRMReferenceSlot< GUM_SCALAR >& ref_this
                       = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(c->get(name));

                    if (!ref_this.slotType().isSubTypeOf(ref_i.slotType())) {
                      std::stringstream msg;
                      msg << "class " << c->name() << " does not respect interface ";
                      GUM_ERROR(PRMTypeError, msg.str() + i->name())
                    }
                  } else {
                    std::stringstream msg;
                    msg << "class " << c->name() << " does not respect interface ";
                    GUM_ERROR(PRMTypeError, msg.str() + i->name())
                  }

                  break;
                }

                case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
                  // Nothing to check: they are automatically inherited
                  break;
                }

                default: {
                  std::string msg = "unexpected ClassElement<GUM_SCALAR> in interface ";
                  GUM_ERROR(FatalError, msg + i->name())
                }
              }
            }
          } catch (NotFound const&) {
            std::stringstream msg;
            msg << "class " << c->name() << " does not respect interface ";
            GUM_ERROR(PRMTypeError, msg.str() + i->name())
          }
        }
      } catch (NotFound const&) {
        // this Class<GUM_SCALAR> does not implement any
        // PRMInterface<GUM_SCALAR>
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startInterface(const std::string& name,
                                                         const std::string& extends,
                                                         bool               delayInheritance) {
      std::string real_name = _addPrefix_(name);
      if (_prm_->_classMap_.exists(real_name) || _prm_->_interfaceMap_.exists(real_name)) {
        GUM_ERROR(DuplicateElement, "'" << real_name << "' is already used.")
      }
      PRMInterface< GUM_SCALAR >* i     = nullptr;
      PRMInterface< GUM_SCALAR >* super = nullptr;

      if (extends != "") { super = _retrieveInterface_(extends); }

      if (super != nullptr) {
        i = new PRMInterface< GUM_SCALAR >(real_name, *super, delayInheritance);
      } else {
        i = new PRMInterface< GUM_SCALAR >(real_name);
      }

      _prm_->_interfaceMap_.insert(i->name(), i);
      _prm_->_interfaces_.insert(i);
      _stack_.push_back(i);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::continueInterface(const std::string& name) {
      std::string real_name = _addPrefix_(name);
      if (!_prm_->_interfaceMap_.exists(real_name)) {
        GUM_ERROR(DuplicateElement, "'" << real_name << "' not found.")
      }

      PRMInterface< GUM_SCALAR >* i = _retrieveInterface_(real_name);
      _stack_.push_back(i);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addAttribute(PRMAttribute< GUM_SCALAR >* attr) {
      PRMClass< GUM_SCALAR >* c
         = static_cast< PRMClass< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::CLASS));
      c->add(attr);
      Size                                       count = 0;
      const Sequence< const DiscreteVariable* >& vars  = attr->cpf().variablesSequence();

      for (const auto& node: c->containerDag().nodes()) {
        try {
          if (vars.exists(&(c->get(node).type().variable()))) {
            ++count;

            if (&(attr->type().variable()) != &(c->get(node).type().variable())) {
              c->addArc(c->get(node).safeName(), attr->safeName());
            }
          }
        } catch (OperationNotAllowed const&) {}
      }

      if (count != attr->cpf().variablesSequence().size()) {
        GUM_ERROR(NotFound, "unable to found all parents of this attribute")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::_addParent_(PRMClassElementContainer< GUM_SCALAR >* c,
                                                      PRMAttribute< GUM_SCALAR >*             a,
                                                      const std::string& name) {
      try {
        PRMClassElement< GUM_SCALAR >& elt = c->get(name);

        switch (elt.elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_refslot: {
            GUM_ERROR(OperationNotAllowed,
                      "can not add a reference slot as a parent of an attribute")
            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
            if (static_cast< PRMSlotChain< GUM_SCALAR >& >(elt).isMultiple()) {
              GUM_ERROR(OperationNotAllowed, "can not add a multiple slot chain to an attribute")
            }

            c->addArc(name, a->name());

            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_attribute:
          case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
            c->addArc(name, a->name());
            break;
          }

          default: {
            GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR>")
          }
        }
      } catch (NotFound const&) {
        // Check if name is a slot chain
        PRMSlotChain< GUM_SCALAR >* sc = _buildSlotChain_(c, name);

        if (sc == nullptr) {
          std::string msg = "found no ClassElement<GUM_SCALAR> with the given name ";
          GUM_ERROR(NotFound, msg + name)
        } else if (!sc->isMultiple()) {
          c->add(sc);
          c->addArc(sc->name(), a->name());
        } else {
          delete sc;
          GUM_ERROR(OperationNotAllowed,
                    "Impossible to add a multiple reference slot as"
                    " direct parent of an PRMAttribute<GUM_SCALAR>.");
        }
      }
    }


    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addParent(const std::string& name) {
      PRMClassElementContainer< GUM_SCALAR >* c = _checkStackContainter_(2);
      try {
        // Retrieving pointers
        PRMAttribute< GUM_SCALAR >* a = static_cast< PRMAttribute< GUM_SCALAR >* >(
           _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));
        _addParent_(c, a, name);
      } catch (FactoryInvalidState const&) {
        auto agg = static_cast< PRMAggregate< GUM_SCALAR >* >(
           _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_aggregate));
        _addParent_(static_cast< PRMClass< GUM_SCALAR >* >(c), agg, name);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setRawCPFByFloatLines(const std::vector< float >& array) {
      PRMAttribute< GUM_SCALAR >* a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));
      _checkStack_(2, PRMObject::prm_type::CLASS);

      if (a->cpf().domainSize() != array.size()) GUM_ERROR(OperationNotAllowed, "illegal CPF size")

      std::vector< GUM_SCALAR > array2(array.begin(), array.end());
      a->cpf().fillWith(array2);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setRawCPFByLines(const std::vector< GUM_SCALAR >& array) {
      auto elt = _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute);
      auto a   = static_cast< PRMAttribute< GUM_SCALAR >* >(elt);
      _checkStack_(2, PRMObject::prm_type::CLASS);

      if (a->cpf().domainSize() != array.size()) {
        GUM_ERROR(OperationNotAllowed, "illegal CPF size")
      }

      a->cpf().fillWith(array);
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::setRawCPFByFloatColumns(const std::vector< float >& array) {
      PRMAttribute< GUM_SCALAR >* a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if (a->cpf().domainSize() != array.size()) {
        GUM_ERROR(OperationNotAllowed, "illegal CPF size")
      }

      std::vector< GUM_SCALAR > array2(array.begin(), array.end());
      setRawCPFByColumns(array2);
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::setRawCPFByColumns(const std::vector< GUM_SCALAR >& array) {
      PRMAttribute< GUM_SCALAR >* a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if (a->cpf().domainSize() != array.size()) {
        GUM_ERROR(OperationNotAllowed, "illegal CPF size")
      }

      if (a->cpf().nbrDim() == 1) {
        setRawCPFByLines(array);

      } else {
        Instantiation inst(a->cpf());
        Instantiation jnst;
        for (auto idx = inst.variablesSequence().rbegin(); idx != inst.variablesSequence().rend();
             --idx) {
          jnst.add(**idx);
        }

        jnst.setFirst();
        auto idx = (std::size_t)0;
        while ((!jnst.end()) && idx < array.size()) {
          inst.setVals(jnst);
          a->cpf().set(inst, array[idx]);
          jnst.inc();
          ++idx;
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::setCPFByFloatRule(const std::vector< std::string >& parents,
                                                   const std::vector< float >&       values) {
      auto a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of parents")
      }

      if (values.size() != a->type().variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of values")
      }

      std::vector< GUM_SCALAR > values2(values.begin(), values.end());
      setCPFByRule(parents, values2);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setCPFByRule(const std::vector< std::string >& parents,
                                                       const std::vector< GUM_SCALAR >&  values) {
      auto a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of parents")
      }

      if (values.size() != a->type().variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of values")
      }

      if (dynamic_cast< PRMFormAttribute< GUM_SCALAR >* >(a)) {
        auto form = static_cast< PRMFormAttribute< GUM_SCALAR >* >(a);
        // jnst holds parents with a specific value (not "*")
        // knst holds parents without a specific value ("*")
        Instantiation           jnst, knst;
        const DiscreteVariable* var = 0;
        // not_used Size pos = 0;
        bool found = false;

        for (Idx i = 0; i < parents.size(); ++i) {
          var = form->formulas().variablesSequence().atPos(1 + i);

          if (parents[i] == "*") {
            knst.add(*var);
          } else {
            jnst.add(*var);
            // not_used pos = 0;
            found = false;

            for (Size j = 0; j < var->domainSize(); ++j) {
              if (var->label(j) == parents[i]) {
                jnst.chgVal(*var, j);
                found = true;
                break;
              }
            }

            if (!found) {
              std::string msg = "could not find label ";
              GUM_ERROR(NotFound, msg + parents[i])
            }
          }
        }

        Instantiation inst(form->formulas());
        inst.setVals(jnst);

        for (Size i = 0; i < form->type()->domainSize(); ++i) {
          inst.chgVal(form->type().variable(), i);

          for (inst.setFirstIn(knst); !inst.end(); inst.incIn(knst)) {
            form->formulas().set(inst, std::to_string(values[i]));
          }
        }

      } else {
        GUM_ERROR(OperationNotAllowed, "invalide attribute type")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setCPFByRule(const std::vector< std::string >& parents,
                                                       const std::vector< std::string >& values) {
      auto a = static_cast< PRMAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of parents")
      }

      if (values.size() != a->type().variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of values")
      }

      if (dynamic_cast< PRMFormAttribute< GUM_SCALAR >* >(a)) {
        auto form = static_cast< PRMFormAttribute< GUM_SCALAR >* >(a);
        // jnst holds parents with a specific value (not "*")
        // knst holds parents without a specific value ("*")
        Instantiation           jnst, knst;
        const DiscreteVariable* var = 0;
        // not_used Size pos = 0;
        bool found = false;

        for (Idx i = 0; i < parents.size(); ++i) {
          var = form->formulas().variablesSequence().atPos(1 + i);

          if (parents[i] == "*") {
            knst.add(*var);
          } else {
            jnst.add(*var);
            // not_used pos = 0;
            found = false;

            for (Size j = 0; j < var->domainSize(); ++j) {
              if (var->label(j) == parents[i]) {
                jnst.chgVal(*var, j);
                found = true;
                break;
              }
            }

            if (!found) {
              std::string msg = "could not find label ";
              GUM_ERROR(NotFound, msg + parents[i])
            }
          }
        }

        Instantiation inst(form->formulas());
        inst.setVals(jnst);

        for (Size i = 0; i < form->type()->domainSize(); ++i) {
          inst.chgVal(form->type().variable(), i);

          for (inst.setFirstIn(knst); !inst.end(); inst.incIn(knst)) {
            form->formulas().set(inst, values[i]);
          }
        }

      } else {
        GUM_ERROR(OperationNotAllowed, "invalide attribute type")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addParameter(const std::string& type,
                                                       const std::string& name,
                                                       double             value) {
      auto c = static_cast< PRMClass< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::CLASS));

      PRMParameter< GUM_SCALAR >* p = nullptr;
      if (type == "int") {
        p = new PRMParameter< GUM_SCALAR >(name,
                                           PRMParameter< GUM_SCALAR >::ParameterType::INT,
                                           (GUM_SCALAR)value);
      } else if (type == "real") {
        p = new PRMParameter< GUM_SCALAR >(name,
                                           PRMParameter< GUM_SCALAR >::ParameterType::REAL,
                                           (GUM_SCALAR)value);
      }

      try {
        c->add(p);
      } catch (DuplicateElement const&) { c->overload(p); }
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::startAggregator(const std::string&                name,
                                                 const std::string&                agg_type,
                                                 const std::string&                rv_type,
                                                 const std::vector< std::string >& params) {
      PRMClass< GUM_SCALAR >* c
         = static_cast< PRMClass< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::CLASS));

      auto agg = new PRMAggregate< GUM_SCALAR >(name,
                                                PRMAggregate< GUM_SCALAR >::str2enum(agg_type),
                                                *_retrieveType_(rv_type));

      try {
        c->add(agg);
      } catch (DuplicateElement const&) { c->overload(agg); }

      switch (agg->agg_type()) {
        case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT:
        case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS:
        case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL: {
          if (params.size() != 1) {
            GUM_ERROR(OperationNotAllowed, "aggregate requires a parameter")
          }
          agg->setLabel(params.front());
          break;
        }
        default: {
          // Nothing to do
        }
      }
      _stack_.push_back(agg);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::continueAggregator(const std::string& name) {
      PRMClassElementContainer< GUM_SCALAR >* c = _checkStackContainter_(1);

      if (!c->exists(name)) GUM_ERROR(NotFound, "Element " << name << "not found")

      auto& agg = c->get(name);
      if (!PRMClassElement< GUM_SCALAR >::isAggregate(agg))
        GUM_ERROR(OperationNotAllowed, "Element " << name << " not an aggregate")

      _stack_.push_back(&agg);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::_addParent_(PRMClass< GUM_SCALAR >*     c,
                                                      PRMAggregate< GUM_SCALAR >* agg,
                                                      const std::string&          name) {
      auto chains = std::vector< std::string >{name};
      auto inputs = std::vector< PRMClassElement< GUM_SCALAR >* >();
      _retrieveInputs_(c, chains, inputs);

      switch (agg->agg_type()) {
        case PRMAggregate< GUM_SCALAR >::AggregateType::OR:
        case PRMAggregate< GUM_SCALAR >::AggregateType::AND: {
          if (inputs.front()->type() != *(_retrieveType_("boolean"))) {
            GUM_ERROR(TypeError, "expected booleans")
          }

          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT:
        case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS:
        case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL: {
          if (!agg->hasLabel()) {
            auto param     = agg->labelValue();
            Idx  label_idx = 0;

            while (label_idx < inputs.front()->type()->domainSize()) {
              if (inputs.front()->type()->label(label_idx) == param) { break; }

              ++label_idx;
            }

            if (label_idx == inputs.front()->type()->domainSize()) {
              GUM_ERROR(NotFound, "could not find label")
            }

            agg->setLabel(label_idx);
          }

          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::SUM:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MEDIAN:
        case PRMAggregate< GUM_SCALAR >::AggregateType::AMPLITUDE:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MIN:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MAX: {
          break;
        }

        default: {
          GUM_ERROR(FatalError, "Unknown aggregator.")
        }
      }

      c->addArc(inputs.front()->safeName(), agg->safeName());
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endAggregator() {
      _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_aggregate);
      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addAggregator(const std::string&                name,
                                                        const std::string&                agg_type,
                                                        const std::vector< std::string >& chains,
                                                        const std::vector< std::string >& params,
                                                        std::string                       type) {
      PRMClass< GUM_SCALAR >* c
         = static_cast< PRMClass< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::CLASS));
      // Checking call legality

      if (chains.size() == 0) {
        GUM_ERROR(OperationNotAllowed, "a PRMAggregate<GUM_SCALAR> requires at least one parent")
      }

      // Retrieving the parents of the aggregate
      std::vector< PRMClassElement< GUM_SCALAR >* > inputs;

      // This helps knowing if the aggregate has parents outside the current
      // class
      // (see below)
      bool hasSC = _retrieveInputs_(c, chains, inputs);

      // Checking that all inputs shares the same PRMType (trivial
      // if
      // inputs.size() == 1)
      if (inputs.size() > 1) {
        for (auto iter = inputs.begin() + 1; iter != inputs.end(); ++iter) {
          if ((**(iter - 1)).type() != (**iter).type()) {
            GUM_ERROR(TypeError, "found different types")
          }
        }
      }

      // Different treatments for different types of aggregate.
      PRMAggregate< GUM_SCALAR >* agg = nullptr;

      switch (PRMAggregate< GUM_SCALAR >::str2enum(agg_type)) {
        case PRMAggregate< GUM_SCALAR >::AggregateType::OR:
        case PRMAggregate< GUM_SCALAR >::AggregateType::AND: {
          if (inputs.front()->type() != *(_retrieveType_("boolean"))) {
            GUM_ERROR(TypeError, "expected booleans")
          }
          if (params.size() != 0) { GUM_ERROR(OperationNotAllowed, "invalid number of paramaters") }

          agg = new PRMAggregate< GUM_SCALAR >(name,
                                               PRMAggregate< GUM_SCALAR >::str2enum(agg_type),
                                               inputs.front()->type());

          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS:
        case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL: {
          if (params.size() != 1) { GUM_ERROR(OperationNotAllowed, "invalid number of parameters") }

          Idx label_idx = 0;

          while (label_idx < inputs.front()->type()->domainSize()) {
            if (inputs.front()->type()->label(label_idx) == params.front()) { break; }

            ++label_idx;
          }

          if (label_idx == inputs.front()->type()->domainSize()) {
            GUM_ERROR(NotFound, "could not find label")
          }

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate< GUM_SCALAR >(name,
                                               PRMAggregate< GUM_SCALAR >::str2enum(agg_type),
                                               *(_retrieveType_("boolean")),
                                               label_idx);
          agg->label();

          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::SUM:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MEDIAN:
        case PRMAggregate< GUM_SCALAR >::AggregateType::AMPLITUDE:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MIN:
        case PRMAggregate< GUM_SCALAR >::AggregateType::MAX: {
          if (params.size() != 0) { GUM_ERROR(OperationNotAllowed, "invalid number of parameters") }

          auto output_type = _retrieveType_(type);

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate< GUM_SCALAR >(name,
                                               PRMAggregate< GUM_SCALAR >::str2enum(agg_type),
                                               *output_type);

          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT: {
          if (params.size() != 1) { GUM_ERROR(OperationNotAllowed, "invalid number of parameters") }

          Idx label_idx = 0;

          while (label_idx < inputs.front()->type()->domainSize()) {
            if (inputs.front()->type()->label(label_idx) == params.front()) { break; }

            ++label_idx;
          }

          if (label_idx == inputs.front()->type()->domainSize()) {
            GUM_ERROR(NotFound, "could not find label")
          }

          auto output_type = _retrieveType_(type);

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate< GUM_SCALAR >(name,
                                               PRMAggregate< GUM_SCALAR >::str2enum(agg_type),
                                               *output_type,
                                               label_idx);

          break;
        }

        default: {
          GUM_ERROR(FatalError, "Unknown aggregator.")
        }
      }

      std::string safe_name = agg->safeName();

      try {
        if (hasSC) {
          try {
            c->add(agg);
          } catch (DuplicateElement const&) { c->overload(agg); }
        } else {
          // Inner aggregators can be directly used as attributes
          auto attr
             = new PRMScalarAttribute< GUM_SCALAR >(agg->name(), agg->type(), agg->buildImpl());

          try {
            c->add(attr);
          } catch (DuplicateElement const&) { c->overload(attr); }

          delete agg;
        }
      } catch (DuplicateElement const&) {
        delete agg;
        throw;
      }

      for (const auto& elt: inputs) {
        c->addArc(elt->safeName(), safe_name);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addReferenceSlot(const std::string& type,
                                                           const std::string& name,
                                                           bool               isArray) {
      PRMClassElementContainer< GUM_SCALAR >* owner    = _checkStackContainter_(1);
      PRMClassElementContainer< GUM_SCALAR >* slotType = 0;

      try {
        slotType = _retrieveClass_(type);
      } catch (NotFound const&) {
        try {
          slotType = _retrieveInterface_(type);
        } catch (NotFound const&) {
          GUM_ERROR(NotFound, "unknown ReferenceSlot<GUM_SCALAR> slot type")
        }
      }

      PRMReferenceSlot< GUM_SCALAR >* ref
         = new PRMReferenceSlot< GUM_SCALAR >(name, *slotType, isArray);

      try {
        owner->add(ref);
      } catch (DuplicateElement const&) { owner->overload(ref); }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addArray(const std::string& type,
                                                   const std::string& name,
                                                   Size               size) {
      PRMSystem< GUM_SCALAR >* model
         = static_cast< PRMSystem< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::SYSTEM));
      PRMClass< GUM_SCALAR >*    c    = _retrieveClass_(type);
      PRMInstance< GUM_SCALAR >* inst = 0;

      try {
        model->addArray(name, *c);

        for (Size i = 0; i < size; ++i) {
          std::stringstream elt_name;
          elt_name << name << "[" << i << "]";
          inst = new PRMInstance< GUM_SCALAR >(elt_name.str(), *c);
          model->add(name, inst);
        }
      } catch (PRMTypeError const&) {
        delete inst;
        throw;
      } catch (NotFound const&) {
        delete inst;
        throw;
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::incArray(const std::string& l_i, const std::string& r_i) {
      PRMSystem< GUM_SCALAR >* model
         = static_cast< PRMSystem< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::SYSTEM));

      if (model->isArray(l_i)) {
        if (model->isInstance(r_i)) {
          model->add(l_i, model->get(r_i));
        } else {
          GUM_ERROR(NotFound, "right value is not an instance")
        }
      } else {
        GUM_ERROR(NotFound, "left value is no an array")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setReferenceSlot(const std::string& l_i,
                                                           const std::string& l_ref,
                                                           const std::string& r_i) {
      auto model
         = static_cast< PRMSystem< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::SYSTEM));
      std::vector< PRMInstance< GUM_SCALAR >* > lefts;
      std::vector< PRMInstance< GUM_SCALAR >* > rights;

      if (model->isInstance(l_i)) {
        lefts.push_back(&(model->get(l_i)));
      } else if (model->isArray(l_i)) {
        for (const auto& elt: model->getArray(l_i))
          lefts.push_back(elt);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array")
      }

      if (model->isInstance(r_i)) {
        rights.push_back(&(model->get(r_i)));
      } else if (model->isArray(r_i)) {
        for (const auto& elt: model->getArray(r_i))
          rights.push_back(elt);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array")
      }

      for (const auto l: lefts) {
        for (const auto r: rights) {
          auto& elt = l->type().get(l_ref);
          if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(elt)) {
            l->add(elt.id(), *r);

          } else {
            GUM_ERROR(NotFound, "unfound reference slot")
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMSlotChain< GUM_SCALAR >*
           PRMFactory< GUM_SCALAR >::_buildSlotChain_(PRMClassElementContainer< GUM_SCALAR >* start,
                                                  const std::string&                      name) {
      std::vector< std::string > v;
      decomposePath(name, v);
      PRMClassElementContainer< GUM_SCALAR >*    current = start;
      PRMReferenceSlot< GUM_SCALAR >*            ref     = nullptr;
      Sequence< PRMClassElement< GUM_SCALAR >* > elts;

      for (size_t i = 0; i < v.size(); ++i) {
        try {
          switch (current->get(v[i]).elt_type()) {
            case PRMClassElement< GUM_SCALAR >::prm_refslot:
              ref = &(static_cast< PRMReferenceSlot< GUM_SCALAR >& >(current->get(v[i])));
              elts.insert(ref);
              current = &(/*const_cast<PRMClassElementContainer<GUM_SCALAR>&>*/ (ref->slotType()));
              break;

            case PRMClassElement< GUM_SCALAR >::prm_aggregate:
            case PRMClassElement< GUM_SCALAR >::prm_attribute:

              if (i == v.size() - 1) {
                elts.insert(&(current->get(v[i])));
                break;
              } else {
                return nullptr;
              }

            default: {
              return nullptr;
            }
          }
        } catch (NotFound const&) { return nullptr; }
      }

      GUM_ASSERT(v.size() == elts.size());

      current->setOutputNode(*(elts.back()), true);

      return new PRMSlotChain< GUM_SCALAR >(name, elts);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMFactory< GUM_SCALAR >::_retrieveInputs_(
       PRMClass< GUM_SCALAR >*                        c,
       const std::vector< std::string >&              chains,
       std::vector< PRMClassElement< GUM_SCALAR >* >& inputs) {
      bool retVal = false;

      for (size_t i = 0; i < chains.size(); ++i) {
        try {
          inputs.push_back(&(c->get(chains[i])));
          retVal = retVal || PRMClassElement< GUM_SCALAR >::isSlotChain(*(inputs.back()));
        } catch (NotFound const&) {
          inputs.push_back(_buildSlotChain_(c, chains[i]));
          retVal = true;

          if (inputs.back()) {
            c->add(inputs.back());
          } else {
            GUM_ERROR(NotFound, "unknown slot chain")
          }
        }
      }

      PRMType* t = _retrieveCommonType_(inputs);

      std::vector< std::pair< PRMClassElement< GUM_SCALAR >*, PRMClassElement< GUM_SCALAR >* > >
         toAdd;

      for (const auto& elt: inputs) {
        if ((*elt).type() != (*t)) {
          if (PRMClassElement< GUM_SCALAR >::isSlotChain(*elt)) {
            PRMSlotChain< GUM_SCALAR >* sc = static_cast< PRMSlotChain< GUM_SCALAR >* >(elt);
            std::stringstream           name;

            for (Size idx = 0; idx < sc->chain().size() - 1; ++idx) {
              name << sc->chain().atPos(idx)->name() << ".";
            }

            name << ".(" << t->name() << ")" << sc->lastElt().name();

            try {
              toAdd.push_back(std::make_pair(elt, &(c->get(name.str()))));
            } catch (NotFound const&) {
              toAdd.push_back(std::make_pair(elt, _buildSlotChain_(c, name.str())));
            }
          } else {
            std::stringstream name;
            name << "(" << t->name() << ")" << elt->name();
            toAdd.push_back(std::make_pair(elt, &(c->get(name.str()))));
          }
        }
      }

      return retVal;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType* PRMFactory< GUM_SCALAR >::_retrieveCommonType_(
       const std::vector< PRMClassElement< GUM_SCALAR >* >& elts) {
      const PRMType*                 current = nullptr;
      HashTable< std::string, Size > counters;
      // Finding all types and super types

      for (const auto& elt: elts) {
        try {
          current = &((*elt).type());

          while (current != 0) {
            // Filling counters
            if (counters.exists(current->name())) {
              ++(counters[current->name()]);
            } else {
              counters.insert(current->name(), 1);
            }

            // Loop guard
            if (current->isSubType()) {
              current = &(current->superType());
            } else {
              current = nullptr;
            }
          }
        } catch (OperationNotAllowed const&) {
          GUM_ERROR(WrongClassElement, "found a ClassElement<GUM_SCALAR> without a type")
        }
      }

      // We need to find the most specialized (i.e. max depth) common type
      current = nullptr;

      int max_depth = -1;

      int current_depth = 0;

      for (const auto& elt: counters) {
        if ((elt.second) == elts.size()) {
          current_depth = _typeDepth_(_retrieveType_(elt.first));

          if (current_depth > max_depth) {
            max_depth = current_depth;
            current   = _retrieveType_(elt.first);
          }
        }
      }

      if (current) { return const_cast< PRMType* >(current); }

      GUM_ERROR(NotFound, "could not find a common type")
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::addNoisyOrCompound(const std::string&                name,
                                                    const std::vector< std::string >& chains,
                                                    const std::vector< float >&       numbers,
                                                    float                             leak,
                                                    const std::vector< std::string >& labels) {
      if (currentType() != PRMObject::prm_type::CLASS) {
        GUM_ERROR(gum::FactoryInvalidState, "invalid state to add a noisy-or")
      }

      PRMClass< GUM_SCALAR >* c = dynamic_cast< gum::prm::PRMClass< GUM_SCALAR >* >(getCurrent());

      std::vector< PRMClassElement< GUM_SCALAR >* > parents;

      for (const auto& elt: chains)
        parents.push_back(&(c->get(elt)));

      PRMType* common_type = _retrieveCommonType_(parents);

      for (size_t idx = 0; idx < parents.size(); ++idx) {
        if (parents[idx]->type() != (*common_type)) {
          PRMClassElement< GUM_SCALAR >* parent = parents[idx];
          // Either safe_name is an non existing slot chain or an existing cast
          // descendant
          std::string safe_name = parent->cast(*common_type);

          if (!c->exists(safe_name)) {
            if (PRMClassElement< GUM_SCALAR >::isSlotChain(*parent)) {
              parents[idx] = _buildSlotChain_(c, safe_name);
              c->add(parents[idx]);
            } else {
              GUM_ERROR(NotFound, "unable to find parent")
            }
          } else {
            parents[idx] = &(c->get(safe_name));
          }
        }
      }

      if (numbers.size() == 1) {
        auto impl = new gum::MultiDimNoisyORCompound< GUM_SCALAR >(leak, numbers.front());
        auto attr = new PRMScalarAttribute< GUM_SCALAR >(name, retrieveType("boolean"), impl);
        addAttribute(attr);
      } else if (numbers.size() == parents.size()) {
        gum::MultiDimNoisyORCompound< GUM_SCALAR >* noisy
           = new gum::MultiDimNoisyORCompound< GUM_SCALAR >(leak);
        gum::prm::PRMFuncAttribute< GUM_SCALAR >* attr
           = new gum::prm::PRMFuncAttribute< GUM_SCALAR >(name, retrieveType("boolean"), noisy);

        for (size_t idx = 0; idx < numbers.size(); ++idx) {
          noisy->causalWeight(parents[idx]->type().variable(), numbers[idx]);
        }

        addAttribute(attr);
      } else {
        GUM_ERROR(OperationNotAllowed, "invalid parameters for a noisy or")
      }

      if (!labels.empty()) {
        GUM_ERROR(OperationNotAllowed, "labels definitions not handle for noisy-or")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMType* PRMFactory< GUM_SCALAR >::_retrieveType_(const std::string& name) const {
      PRMType*    type = nullptr;
      std::string full_name;

      // Looking for the type using its name
      if (_prm_->_typeMap_.exists(name)) {
        type      = _prm_->_typeMap_[name];
        full_name = name;
      }

      // Looking for the type in current package
      std::string prefixed = _addPrefix_(name);
      if (_prm_->_typeMap_.exists(prefixed)) {
        if (type == 0) {
          type      = _prm_->_typeMap_[prefixed];
          full_name = prefixed;
        } else if (full_name != prefixed) {
          GUM_ERROR(DuplicateElement, "Type name '" << name << "' is ambiguous: specify full name.")
        }
      }

      // Looking for the type relatively to current package
      std::string relatif_ns = currentPackage();
      size_t      last_dot   = relatif_ns.find_last_of('.');
      if (last_dot != std::string::npos) {
        relatif_ns = relatif_ns.substr(0, last_dot) + '.' + name;
        if (_prm_->_typeMap_.exists(relatif_ns)) {
          if (type == 0) {
            type      = _prm_->_typeMap_[relatif_ns];
            full_name = relatif_ns;
          } else if (full_name != relatif_ns) {
            GUM_ERROR(DuplicateElement,
                      "Type name '" << name << "' is ambiguous: specify full name.");
          }
        }
      }


      // Looking for the type using all declared namespaces
      if (!_namespaces_.empty()) {
        auto ns_list = _namespaces_.back();
        for (gum::Size i = 0; i < ns_list->size(); ++i) {
          std::string ns      = (*ns_list)[i];
          std::string ns_name = ns + "." + name;
          if (_prm_->_typeMap_.exists(ns_name)) {
            if (type == 0) {
              type      = _prm_->_typeMap_[ns_name];
              full_name = ns_name;
            } else if (full_name != ns_name) {
              GUM_ERROR(DuplicateElement,
                        "Type name '" << name << "' is ambiguous: specify full name.");
            }
          }
        }
      }

      if (type == 0) { GUM_ERROR(NotFound, "Type '" << name << "' not found, check imports.") }

      return type;
    }

    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >*
       PRMFactory< GUM_SCALAR >::_retrieveClass_(const std::string& name) const {
      PRMClass< GUM_SCALAR >* a_class = nullptr;
      std::string             full_name;

      // Looking for the type using its name
      if (_prm_->_classMap_.exists(name)) {
        a_class   = _prm_->_classMap_[name];
        full_name = name;
      }

      // Looking for the type using current package
      std::string prefixed = _addPrefix_(name);
      if (_prm_->_classMap_.exists(prefixed)) {
        if (a_class == nullptr) {
          a_class   = _prm_->_classMap_[prefixed];
          full_name = prefixed;
        } else if (full_name != prefixed) {
          GUM_ERROR(DuplicateElement,
                    "Class name '" << name << "' is ambiguous: specify full name.");
        }
      }

      // Looking for the class using all declared namespaces
      if (!_namespaces_.empty()) {
        auto ns_list = _namespaces_.back();
        for (gum::Size i = 0; i < ns_list->size(); ++i) {
          std::string ns      = (*ns_list)[i];
          std::string ns_name = ns + "." + name;
          if (_prm_->_classMap_.exists(ns_name)) {
            if (a_class == 0) {
              a_class   = _prm_->_classMap_[ns_name];
              full_name = ns_name;
            } else if (full_name != ns_name) {
              GUM_ERROR(DuplicateElement,
                        "Class name '" << name << "' is ambiguous: specify full name.");
            }
          }
        }
      }

      if (a_class == 0) { GUM_ERROR(NotFound, "Class '" << name << "' not found, check imports.") }

      return a_class;
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >*
       PRMFactory< GUM_SCALAR >::_retrieveInterface_(const std::string& name) const {
      PRMInterface< GUM_SCALAR >* interface = nullptr;
      std::string                 full_name;

      // Looking for the type using its name
      if (_prm_->_interfaceMap_.exists(name)) {
        interface = _prm_->_interfaceMap_[name];
        full_name = name;
      }

      // Looking for the type using current package
      std::string prefixed = _addPrefix_(name);
      if (_prm_->_interfaceMap_.exists(prefixed)) {
        if (interface == nullptr) {
          interface = _prm_->_interfaceMap_[prefixed];
          full_name = prefixed;
        } else if (full_name != prefixed) {
          GUM_ERROR(DuplicateElement,
                    "Interface name '" << name << "' is ambiguous: specify full name.");
        }
      }

      // Looking for the interf using all declared namespaces
      if (!_namespaces_.empty()) {
        auto ns_list = _namespaces_.back();
        // for( const auto & ns : *( _namespaces_.top()) ) {
        for (gum::Size i = 0; i < ns_list->size(); ++i) {
          std::string ns      = (*ns_list)[i];
          std::string ns_name = ns + "." + name;

          if (_prm_->_interfaceMap_.exists(ns_name)) {
            if (interface == nullptr) {
              interface = _prm_->_interfaceMap_[ns_name];
              full_name = ns_name;
            } else if (full_name != ns_name) {
              GUM_ERROR(DuplicateElement,
                        "Interface name '" << name << "' is ambiguous: specify full name.");
            }
          }
        }
      }

      if (interface == nullptr) {
        GUM_ERROR(NotFound, "Interface '" << name << "' not found, check imports.")
      }

      return interface;
    }

    template < typename GUM_SCALAR >
    INLINE PRMFactory< GUM_SCALAR >::PRMFactory() {
      GUM_CONSTRUCTOR(PRMFactory);
      _prm_ = new PRM< GUM_SCALAR >();
    }

    template < typename GUM_SCALAR >
    INLINE PRMFactory< GUM_SCALAR >::PRMFactory(PRM< GUM_SCALAR >* prm) :
        IPRMFactory(), _prm_(prm) {
      GUM_CONSTRUCTOR(PRMFactory);
    }

    template < typename GUM_SCALAR >
    INLINE PRMFactory< GUM_SCALAR >::~PRMFactory() {
      GUM_DESTRUCTOR(PRMFactory);
      while (!_namespaces_.empty()) {
        auto ns = _namespaces_.back();
        _namespaces_.pop_back();
        delete ns;
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRM< GUM_SCALAR >* PRMFactory< GUM_SCALAR >::prm() const {
      return _prm_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMFactory< GUM_SCALAR >::currentType() const {
      if (_stack_.size() == 0) { GUM_ERROR(NotFound, "no object being built") }

      return _stack_.back()->obj_type();
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject* PRMFactory< GUM_SCALAR >::getCurrent() {
      if (_stack_.size() == 0) { GUM_ERROR(NotFound, "no object being built") }

      return _stack_.back();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMObject* PRMFactory< GUM_SCALAR >::getCurrent() const {
      if (_stack_.size() == 0) { GUM_ERROR(NotFound, "no object being built") }

      return _stack_.back();
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject* PRMFactory< GUM_SCALAR >::closeCurrent() {
      if (_stack_.size() > 0) {
        PRMObject* obj = _stack_.back();
        _stack_.pop_back();
        return obj;
      } else {
        return 0;
      }
    }

    template < typename GUM_SCALAR >
    INLINE std::string PRMFactory< GUM_SCALAR >::currentPackage() const {
      return (_packages_.empty()) ? "" : _packages_.back();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startDiscreteType(const std::string& name,
                                                            std::string        super) {
      std::string real_name = _addPrefix_(name);
      if (_prm_->_typeMap_.exists(real_name)) {
        GUM_ERROR(DuplicateElement, "'" << real_name << "' is already used.")
      }
      if (super.empty()) {
        auto t = new PRMType(LabelizedVariable(real_name, "", 0));
        _stack_.push_back(t);
      } else {
        auto t         = new PRMType(LabelizedVariable(real_name, "", 0));
        t->_superType_ = _retrieveType_(super);
        t->_label_map_ = new std::vector< Idx >();
        _stack_.push_back(t);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addLabel(const std::string& l, std::string extends) {
      if (extends.empty()) {
        PRMType*           t = static_cast< PRMType* >(_checkStack_(1, PRMObject::prm_type::TYPE));
        LabelizedVariable* var = dynamic_cast< LabelizedVariable* >(t->_var_);

        if (!var) {
          GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.")
        } else if (t->_superType_) {
          GUM_ERROR(OperationNotAllowed, "current type is a subtype.")
        }

        try {
          var->addLabel(l);
        } catch (DuplicateElement const&) {
          GUM_ERROR(DuplicateElement, "a label '" << l << "' already exists")
        }
      } else {
        PRMType*           t = static_cast< PRMType* >(_checkStack_(1, PRMObject::prm_type::TYPE));
        LabelizedVariable* var = dynamic_cast< LabelizedVariable* >(t->_var_);

        if (!var) {
          GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.")
        } else if (!t->_superType_) {
          GUM_ERROR(OperationNotAllowed, "current type is not a subtype.")
        }

        bool found = false;

        for (Idx i = 0; i < t->_superType_->_var_->domainSize(); ++i) {
          if (t->_superType_->_var_->label(i) == extends) {
            try {
              var->addLabel(l);
            } catch (DuplicateElement const&) {
              GUM_ERROR(DuplicateElement, "a label '" << l << "' already exists")
            }

            t->_label_map_->push_back(i);

            found = true;
            break;
          }
        }

        if (!found) { GUM_ERROR(NotFound, "inexistent label in super type.") }
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endDiscreteType() {
      PRMType* t = static_cast< PRMType* >(_checkStack_(1, PRMObject::prm_type::TYPE));

      if (!t->_isValid_()) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid subtype")
      } else if (t->variable().domainSize() < 2) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid discrete type")
      }

      _prm_->_typeMap_.insert(t->name(), t);

      _prm_->_types_.insert(t);
      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startDiscretizedType(const std::string& name) {
      std::string real_name = _addPrefix_(name);
      if (_prm_->_typeMap_.exists(real_name)) {
        GUM_ERROR(DuplicateElement, "'" << real_name << "' is already used.")
      }
      auto var = DiscretizedVariable< double >(real_name, "");
      auto t   = new PRMType(var);
      _stack_.push_back(t);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addTick(double tick) {
      PRMType* t = static_cast< PRMType* >(_checkStack_(1, PRMObject::prm_type::TYPE));
      DiscretizedVariable< double >* var = dynamic_cast< DiscretizedVariable< double >* >(t->_var_);

      if (!var) { GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.") }

      try {
        var->addTick(tick);
      } catch (DefaultInLabel const&) {
        GUM_ERROR(OperationNotAllowed, "tick already in used for this variable")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endDiscretizedType() {
      PRMType* t = static_cast< PRMType* >(_checkStack_(1, PRMObject::prm_type::TYPE));

      if (t->variable().domainSize() < 2) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid discrete type")
      }

      _prm_->_typeMap_.insert(t->name(), t);

      _prm_->_types_.insert(t);
      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::addRangeType(const std::string& name, long minVal, long maxVal) {
      std::string real_name = _addPrefix_(name);
      if (_prm_->_typeMap_.exists(real_name)) {
        std::stringstream msg;
        msg << "\"" << real_name << "' is already used.";
        GUM_ERROR(DuplicateElement, msg.str())
      }

      auto var = RangeVariable(real_name, "", minVal, maxVal);
      auto t   = new PRMType(var);

      if (t->variable().domainSize() < 2) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid discrete type")
      }

      _prm_->_typeMap_.insert(t->name(), t);
      _prm_->_types_.insert(t);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endInterface() {
      _checkStack_(1, PRMObject::prm_type::PRM_INTERFACE);
      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addAttribute(const std::string& type,
                                                       const std::string& name) {
      _checkStack_(1, PRMObject::prm_type::PRM_INTERFACE);
      startAttribute(type, name);
      endAttribute();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startAttribute(const std::string& type,
                                                         const std::string& name,
                                                         bool               scalar_attr) {
      PRMClassElementContainer< GUM_SCALAR >* c = _checkStackContainter_(1);
      PRMAttribute< GUM_SCALAR >*             a = nullptr;

      if (PRMObject::isClass(*c) && (!scalar_attr)) {
        a = new PRMFormAttribute< GUM_SCALAR >(static_cast< PRMClass< GUM_SCALAR >& >(*c),
                                               name,
                                               *_retrieveType_(type));

      } else {
        a = new PRMScalarAttribute< GUM_SCALAR >(name, *_retrieveType_(type));
      }

      std::string dot = ".";

      try {
        try {
          c->add(a);
        } catch (DuplicateElement const&) { c->overload(a); }
      } catch (Exception const&) {
        if (a != nullptr && (!c->exists(a->id()))) { delete a; }
      }

      _stack_.push_back(a);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::continueAttribute(const std::string& name) {
      PRMClassElementContainer< GUM_SCALAR >* c = _checkStackContainter_(1);

      if (!c->exists(name)) GUM_ERROR(NotFound, "Attribute " << name << "not found")

      auto& a = c->get(name);

      if (!PRMClassElement< GUM_SCALAR >::isAttribute(a))
        GUM_ERROR(OperationNotAllowed, "Element " << name << " not an attribute")

      _stack_.push_back(&a);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endAttribute() {
      _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute);
      _stack_.pop_back();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::startSystem(const std::string& name) {
      if (_prm_->_systemMap_.exists(name)) {
        GUM_ERROR(DuplicateElement, "'" << name << "' is already used.")
      }
      PRMSystem< GUM_SCALAR >* model = new PRMSystem< GUM_SCALAR >(_addPrefix_(name));
      _stack_.push_back(model);
      _prm_->_systemMap_.insert(model->name(), model);
      _prm_->_systems_.insert(model);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::endSystem() {
      try {
        PRMSystem< GUM_SCALAR >* model
           = static_cast< PRMSystem< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::SYSTEM));
        _stack_.pop_back();
        model->instantiate();
      } catch (Exception const&) { GUM_ERROR(FatalError, "could not create system") }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addInstance(const std::string& type,
                                                      const std::string& name) {
      auto c = _retrieveClass_(type);

      // If class contains parameters, calls the proper addIsntance method
      if (c->parameters().size() > 0) {
        HashTable< std::string, double > params;
        addInstance(type, name, params);

      } else {
        _addInstance_(c, name);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::addInstance(const std::string&                      type,
                                             const std::string&                      name,
                                             const HashTable< std::string, double >& params) {
      auto c = _retrieveClass_(type);

      if (c->parameters().empty()) {
        if (params.empty()) {
          _addInstance_(c, name);
        } else {
          GUM_ERROR(OperationNotAllowed, "Class " + type + " does not have parameters")
        }

      } else {
        auto my_params = params;
        // Adding all parameters to my_params
        for (const auto& p: c->parameters()) {
          if (!my_params.exists(p->name())) { my_params.insert(p->name(), p->value()); }
        }

        // Building sub class name using my_params
        std::stringstream sBuff;
        sBuff << c->name() << "<";

        for (const auto& p: my_params) {
          sBuff << p.first << "=" << p.second << ",";
        }

        // Removing last , and adding closing >
        std::string sub_c = sBuff.str().substr(0, sBuff.str().size() - 1) + ">";

        // Adding class in current package
        try {
          auto pck_cpy = _packages_;
          _packages_.clear();

          startClass(sub_c, c->name());

          // Update inherited parameters
          for (auto p: my_params) {
            auto type = static_cast< PRMParameter< GUM_SCALAR >& >(c->get(p.first)).valueType();
            if (type == PRMParameter< GUM_SCALAR >::ParameterType::INT) {
              addParameter("int", p.first, p.second);

            } else {
              addParameter("real", p.first, p.second);
            }
          }

          endClass();

          _packages_ = pck_cpy;

        } catch (DuplicateElement const&) {
          // Sub Class already exists in this system
        }
        c = _retrieveClass_(sub_c);
        _addInstance_(c, name);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::_addInstance_(PRMClass< GUM_SCALAR >* type,
                                                        const std::string&      name) {
      PRMInstance< GUM_SCALAR >* i = nullptr;
      try {
        auto s
           = static_cast< PRMSystem< GUM_SCALAR >* >(_checkStack_(1, PRMObject::prm_type::SYSTEM));
        i = new PRMInstance< GUM_SCALAR >(name, *type);
        s->add(i);

      } catch (OperationNotAllowed const&) {
        if (i) { delete i; }
        throw;
      }
    }

    template < typename GUM_SCALAR >
    INLINE std::string PRMFactory< GUM_SCALAR >::_addPrefix_(const std::string& str) const {
      if (!_packages_.empty()) {
        std::string full_name = _packages_.back();
        full_name.append(".");
        full_name.append(str);
        return full_name;
      } else {
        return str;
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject* PRMFactory< GUM_SCALAR >::_checkStack_(Idx i, PRMObject::prm_type obj_type) {
      // Don't forget that Idx are unsigned int
      if (_stack_.size() - i > _stack_.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }

      PRMObject* obj = _stack_[_stack_.size() - i];

      if (obj->obj_type() != obj_type) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }

      return obj;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >*
           PRMFactory< GUM_SCALAR >::_checkStackContainter_(Idx i) {
      // Don't forget that Idx are unsigned int
      if (_stack_.size() - i > _stack_.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }

      PRMObject* obj = _stack_[_stack_.size() - i];

      if ((obj->obj_type() == PRMObject::prm_type::CLASS)
          || (obj->obj_type() == PRMObject::prm_type::PRM_INTERFACE)) {
        return static_cast< PRMClassElementContainer< GUM_SCALAR >* >(obj);
      } else {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >* PRMFactory< GUM_SCALAR >::_checkStack_(
       Idx                                                      i,
       typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type) {
      // Don't forget that Idx are unsigned int
      if (_stack_.size() - i > _stack_.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }

      PRMClassElement< GUM_SCALAR >* obj
         = dynamic_cast< PRMClassElement< GUM_SCALAR >* >(_stack_[_stack_.size() - i]);

      if (obj == 0) { GUM_ERROR(FactoryInvalidState, "illegal sequence of calls") }

      if (obj->elt_type() != elt_type) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls")
      }

      return obj;
    }

    template < typename GUM_SCALAR >
    INLINE int PRMFactory< GUM_SCALAR >::_typeDepth_(const PRMType* t) {
      int            depth   = 0;
      const PRMType* current = t;

      while (current->isSubType()) {
        ++depth;
        current = &(current->superType());
      }

      return depth;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::pushPackage(const std::string& name) {
      _packages_.push_back(name);
      _namespaces_.push_back(new List< std::string >());
    }

    template < typename GUM_SCALAR >
    INLINE std::string PRMFactory< GUM_SCALAR >::popPackage() {
      std::string plop = currentPackage();

      if (!_packages_.empty()) {
        std::string s = _packages_.back();
        _packages_.pop_back();

        if (_namespaces_.size() > 0) {
          delete _namespaces_.back();
          _namespaces_.pop_back();
        }
        return s;
      }

      return plop;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::addImport(const std::string& name) {
      if (name.size() == 0) { GUM_ERROR(OperationNotAllowed, "illegal import name") }
      if (_namespaces_.empty()) { _namespaces_.push_back(new List< std::string >()); }
      _namespaces_.back()->push_back(name);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFactory< GUM_SCALAR >::setReferenceSlot(const std::string& l_i,
                                                           const std::string& r_i) {
      size_t pos = l_i.find_last_of('.');

      if (pos != std::string::npos) {
        std::string l_ref = l_i.substr(pos + 1, std::string::npos);
        setReferenceSlot(l_i.substr(0, pos), l_ref, r_i);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >&
           PRMFactory< GUM_SCALAR >::retrieveClass(const std::string& name) {
      return *_retrieveClass_(name);
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMFactory< GUM_SCALAR >::retrieveType(const std::string& name) {
      return *_retrieveType_(name);
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMFactory< GUM_SCALAR >::retrieveCommonType(
       const std::vector< PRMClassElement< GUM_SCALAR >* >& elts) {
      return *(_retrieveCommonType_(elts));
    }


    template < typename GUM_SCALAR >
    INLINE bool PRMFactory< GUM_SCALAR >::isClassOrInterface(const std::string& type) const {
      try {
        _retrieveClass_(type);
        return true;

      } catch (NotFound const&) {
      } catch (DuplicateElement const&) {}

      try {
        _retrieveInterface_(type);
        return true;

      } catch (NotFound const&) {
      } catch (DuplicateElement const&) {}

      return false;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMFactory< GUM_SCALAR >::isArrayInCurrentSystem(const std::string& name) const {
      const PRMSystem< GUM_SCALAR >* system
         = static_cast< const PRMSystem< GUM_SCALAR >* >(getCurrent());
      return (system && system->isArray(name));
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::setRawCPFByColumns(const std::vector< std::string >& array) {
      _checkStack_(2, PRMObject::prm_type::CLASS);

      auto a = static_cast< PRMFormAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if (a->formulas().domainSize() != array.size()) {
        GUM_ERROR(OperationNotAllowed, "illegal CPF size")
      }

      if (a->formulas().nbrDim() == 1) {
        setRawCPFByLines(array);

      } else {
        Instantiation inst(a->formulas());
        Instantiation jnst;
        for (auto idx = inst.variablesSequence().rbegin(); idx != inst.variablesSequence().rend();
             --idx) {
          jnst.add(**idx);
        }

        jnst.setFirst();
        auto idx = (std::size_t)0;
        while ((!jnst.end()) && idx < array.size()) {
          inst.setVals(jnst);
          a->formulas().set(inst, array[idx]);
          jnst.inc();
          ++idx;
        }

        // Generate cpf by calling it
        a->cpf();
      }
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMFactory< GUM_SCALAR >::setRawCPFByLines(const std::vector< std::string >& array) {
      _checkStack_(2, PRMObject::prm_type::CLASS);

      auto a = static_cast< PRMFormAttribute< GUM_SCALAR >* >(
         _checkStack_(1, PRMClassElement< GUM_SCALAR >::prm_attribute));

      if (a->formulas().domainSize() != array.size()) {
        GUM_ERROR(OperationNotAllowed, "illegal CPF size")
      }

      a->formulas().populate(array);

      /// Generate CPF
      a->cpf();
    }

  } /* namespace prm */
} /* namespace gum */
