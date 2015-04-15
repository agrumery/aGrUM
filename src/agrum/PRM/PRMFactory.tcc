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
/** @file
 * @brief Inline implementation of PRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/PRMFactory.h>

namespace gum {

  namespace prm {
    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addLabel(const std::string &l,
                                          std::string extends) {
      if (extends == "") {
        Type<GUM_SCALAR> *t = static_cast<Type<GUM_SCALAR> *>(
            __checkStack(1, PRMObject::PRMType::TYPE));
        LabelizedVariable *var = dynamic_cast<LabelizedVariable *>(t->__var);

        if (not var) {
          GUM_ERROR(FatalError,
                    "the current type's variable is not a LabelizedVariable.");
        } else if (t->__super) {
          GUM_ERROR(OperationNotAllowed, "current type is a subtype.");
        }

        try {
          var->addLabel(l);
        } catch (DuplicateElement &) {
          GUM_ERROR(DuplicateElement, "a label with the same value already exists");
        }
      } else {
        Type<GUM_SCALAR> *t = static_cast<Type<GUM_SCALAR> *>(
            __checkStack(1, PRMObject::PRMType::TYPE));
        LabelizedVariable *var = dynamic_cast<LabelizedVariable *>(t->__var);

        if (not var) {
          GUM_ERROR(FatalError,
                    "the current type's variable is not a LabelizedVariable.");
        } else if (not t->__super) {
          GUM_ERROR(OperationNotAllowed, "current type is not a subtype.");
        }

        bool found = false;

        for (Idx i = 0; i < t->__super->__var->domainSize(); ++i) {
          if (t->__super->__var->label(i) == extends) {
            try {
              var->addLabel(l);
            } catch (DuplicateElement &) {
              GUM_ERROR(DuplicateElement,
                        "a label with the same value already exists");
            }

            t->__label_map->push_back(i);

            found = true;
            break;
          }
        }

        if (not found) {
          GUM_ERROR(NotFound, "inexistent label in super type.");
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::startClass(const std::string &name,
                                            const std::string &extends,
                                            const Set<std::string> *implements) {
      std::string real_name = __addPrefix(name);
      __checkDuplicateName(real_name);
      Class<GUM_SCALAR> *c = 0;
      Class<GUM_SCALAR> *mother = 0;
      Set<Interface<GUM_SCALAR> *> impl;

      if (implements != 0) {
        for (const auto &imp : *implements)
          impl.insert(__retrieveInterface(imp));
      }

      if (extends != "") {
        mother = __retrieveClass(extends);
      }

      if ((extends == "") and impl.empty()) {
        c = new Class<GUM_SCALAR>(real_name);
      } else if ((extends != "") and impl.empty()) {
        c = new Class<GUM_SCALAR>(real_name, *mother);
      } else if ((extends == "") and (not impl.empty())) {
        c = new Class<GUM_SCALAR>(real_name, impl);
      } else if ((extends != "") and (not impl.empty())) {
        c = new Class<GUM_SCALAR>(real_name, *mother, impl);
      }

      __prm->__classMap.insert(c->name(), c);

      __prm->__classes.insert(c);
      __stack.push_back(c);
    }

    template <typename GUM_SCALAR> void PRMFactory<GUM_SCALAR>::endClass() {
      Class<GUM_SCALAR> *c = static_cast<Class<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::CLASS));
      std::string name;
      std::stringstream msg;
      msg << "class " << c->name() << " does not respect interface ";

      try {
        for (const auto &i : c->implements()) {
          try {
            for (const auto node : i->dag().nodes()) {
              name = i->get(node).name();

              switch (i->get(node).elt_type()) {
                case ClassElement<GUM_SCALAR>::prm_aggregate:
                case ClassElement<GUM_SCALAR>::prm_attribute: {
                  if ((c->get(name).elt_type() ==
                       ClassElement<GUM_SCALAR>::prm_attribute) or
                      (c->get(name).elt_type() ==
                       ClassElement<GUM_SCALAR>::prm_aggregate)) {
                    if (not c->get(name).type().isSubTypeOf(i->get(name).type())) {
                      GUM_ERROR(TypeError, msg.str() + i->name());
                    }
                  } else {
                    GUM_ERROR(TypeError, msg.str() + i->name());
                  }

                  break;
                }

                case ClassElement<GUM_SCALAR>::prm_refslot: {
                  if (c->get(name).elt_type() ==
                      ClassElement<GUM_SCALAR>::prm_refslot) {
                    const ReferenceSlot<GUM_SCALAR> &ref_i =
                        static_cast<const ReferenceSlot<GUM_SCALAR> &>(i->get(name));
                    const ReferenceSlot<GUM_SCALAR> &ref_this =
                        static_cast<const ReferenceSlot<GUM_SCALAR> &>(c->get(name));

                    if (not ref_this.slotType().isSubTypeOf(ref_i.slotType())) {
                      GUM_ERROR(TypeError, msg.str() + i->name());
                    }
                  } else {
                    GUM_ERROR(TypeError, msg.str() + i->name());
                  }

                  break;
                }

                case ClassElement<GUM_SCALAR>::prm_slotchain: {
                  // Nothing to check: they are automatically inherited
                  break;
                }

                default: {
                  std::string msg =
                      "unexpected ClassElement<GUM_SCALAR> in interface ";
                  GUM_ERROR(FatalError, msg + i->name());
                }
              }
            }
          } catch (NotFound &) {
            GUM_ERROR(TypeError, msg.str() + i->name());
          }
        }
      } catch (NotFound &) {
        // this Class<GUM_SCALAR> does not implement any Interface<GUM_SCALAR>
      }

      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::startInterface(const std::string &name,
                                                const std::string &extends) {
      std::string real_name = __addPrefix(name);
      __checkDuplicateName(real_name);
      Interface<GUM_SCALAR> *i = 0;
      Interface<GUM_SCALAR> *super = 0;

      if (extends != "") {
        super = __retrieveInterface(extends);
        // try {
        //   super = __prm->__interfaceMap[extends];
        // } catch (NotFound&) {
        //   try {
        //     super = __prm->__interfaceMap[__addPrefix(extends)];
        //   } catch (NotFound&) {
        //     std::string msg = "unknown interface: ";
        //     GUM_ERROR(NotFound, msg + extends);
        //   }
        // }
      }

      if (super != 0) {
        i = new Interface<GUM_SCALAR>(real_name, *super);
      } else {
        i = new Interface<GUM_SCALAR>(real_name);
      }

      __prm->__interfaceMap.insert(i->name(), i);

      __prm->__interfaces.insert(i);
      __stack.push_back(i);
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addAttribute(Attribute<GUM_SCALAR> *attr) {
      Class<GUM_SCALAR> *c = static_cast<Class<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::CLASS));
      c->add(attr);
      Size count = 0;
      const Sequence<const DiscreteVariable *> &vars =
          attr->cpf().variablesSequence();

      for (const auto node : c->dag().nodes()) {
        try {
          if (vars.exists(&(c->get(node).type().variable()))) {
            ++count;

            if (&(attr->type().variable()) != &(c->get(node).type().variable())) {
              c->addArc(c->get(node).safeName(), attr->safeName());
            }
          }
        } catch (OperationNotAllowed &) {
        }
      }

      if (count != attr->cpf().variablesSequence().size()) {
        GUM_ERROR(NotFound, "unable to found all parents of this attribute");
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addParent(const std::string &name) {
      // Retrieving pointers
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));
      ClassElementContainer<GUM_SCALAR> *c = __checkStackContainter(2);

      try {
        ClassElement<GUM_SCALAR> &elt = c->get(name);

        switch (elt.elt_type()) {
          case ClassElement<GUM_SCALAR>::prm_refslot: {
            GUM_ERROR(OperationNotAllowed,
                      "can not add a reference slot as a parent of an attribute");
            break;
          }

          case ClassElement<GUM_SCALAR>::prm_slotchain: {
            if (static_cast<SlotChain<GUM_SCALAR> &>(elt).isMultiple()) {
              GUM_ERROR(OperationNotAllowed,
                        "can not add a multiple slot chain to an attribute");
            }

            c->addArc(name, a->name());

            break;
          }

          case ClassElement<GUM_SCALAR>::prm_attribute:
          case ClassElement<GUM_SCALAR>::prm_aggregate: {
            c->addArc(name, a->name());
            break;
          }

          default: { GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR>"); }
        }
      } catch (NotFound &) {
        // Check if name is a slot chain
        SlotChain<GUM_SCALAR> *sc = __buildSlotChain(c, name);

        if (sc == 0) {
          std::string msg = "found no ClassElement<GUM_SCALAR> with the given name ";
          GUM_ERROR(NotFound, msg + name);
        } else if (not sc->isMultiple()) {
          c->add(sc);
          c->addArc(sc->name(), a->name());
        } else {
          delete sc;
          GUM_ERROR(OperationNotAllowed,
                    "Impossible to add a multiple reference slot as"
                    " direct parent of an Attribute<GUM_SCALAR>.");
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::setRawCPFByFloatLines(const std::vector<float> &array) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));
      __checkStack(2, PRMObject::PRMType::CLASS);

      if (a->cpf().domainSize() != array.size())
        GUM_ERROR(OperationNotAllowed, "illegal CPF size");

      std::vector<GUM_SCALAR> array2(array.begin(), array.end());
      a->cpf().fillWith(array2);
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::setRawCPFByLines(const std::vector<GUM_SCALAR> &array) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));
      __checkStack(2, PRMObject::PRMType::CLASS);

      if (a->cpf().domainSize() != array.size())
        GUM_ERROR(OperationNotAllowed, "illegal CPF size");

      a->cpf().fillWith(array);
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::setRawCPFByFloatColumns(
        const std::vector<float> &array) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));

      if (a->cpf().domainSize() != array.size()) {
        std::stringstream s;
        s << "illegal CPF size, found " << array.size() << " and expected "
          << a->cpf().domainSize();
        GUM_ERROR(OperationNotAllowed, s.str());
      }

      std::vector<GUM_SCALAR> array2(array.begin(), array.end());
      setRawCPFByColumns(array2);
    }
    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::setRawCPFByColumns(
        const std::vector<GUM_SCALAR> &array) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));

      if (a->cpf().domainSize() != array.size()) {
        std::stringstream s;
        s << "illegal CPF size, found " << array.size() << " and expected "
          << a->cpf().domainSize();
        GUM_ERROR(OperationNotAllowed, s.str());
      }

      if (a->cpf().nbrDim() == 1) {
        setRawCPFByLines(array);
      } else {
        std::vector<Size> pos(a->cpf().nbrDim(), 0);
        Instantiation inst(a->cpf());
        inst.setFirst();

        for (const auto &elt : array) {
          a->cpf().set(inst, elt);

          for (size_t idx = pos.size(); idx > 0; --idx) {
            bool stop = true;
            ++(pos[idx - 1]);

            if (pos[idx - 1] ==
                a->cpf().variablesSequence().atPos(idx - 1)->domainSize()) {
              pos[idx - 1] = 0;
              stop = false;
            }

            inst.chgVal(a->cpf().variablesSequence().atPos(idx - 1), pos[idx - 1]);

            if (stop) {
              break;
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::setCPFByFloatRule(
        const std::vector<std::string> &parents, const std::vector<float> &values) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));

      if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of parents");
      }

      if (values.size() != a->type().variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of values");
      }

      std::vector<GUM_SCALAR> values2(values.begin(), values.end());
      setCPFByRule(parents, values2);
    }
    template <typename GUM_SCALAR>
    void
    PRMFactory<GUM_SCALAR>::setCPFByRule(const std::vector<std::string> &parents,
                                         const std::vector<GUM_SCALAR> &values) {
      Attribute<GUM_SCALAR> *a = static_cast<Attribute<GUM_SCALAR> *>(
          __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute));

      if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of parents");
      }

      if (values.size() != a->type().variable().domainSize()) {
        GUM_ERROR(OperationNotAllowed, "wrong number of values");
      }

      Instantiation inst(a->cpf());

      // jnst holds parents with a specific value (not "*")
      // knst holds parents without a specific value ("*")
      Instantiation jnst, knst;
      const DiscreteVariable *var = 0;
      // not_used Size pos = 0;
      bool found = false;

      for (size_t i = 0; i < parents.size(); ++i) {
        var = a->cpf().variablesSequence().atPos(1 + i);

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

          if (not found) {
            std::string msg = "could not find label ";
            GUM_ERROR(NotFound, msg + parents[i]);
          }
        }
      }

      inst.setVals(jnst);

      for (Size i = 0; i < a->type()->domainSize(); ++i) {
        inst.chgVal(a->type().variable(), i);

        for (inst.setFirstIn(knst); not inst.end(); inst.incIn(knst)) {
          a->cpf().set(inst, values[i]);
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addParameter(const std::string &type,
                                              const std::string &name,
                                              std::string value) {
      Class<GUM_SCALAR> *c = static_cast<Class<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::CLASS));

      if (value == "") {
        MultiDimSparse<GUM_SCALAR> *impl = new MultiDimSparse<GUM_SCALAR>(
            std::numeric_limits<GUM_SCALAR>::signaling_NaN());
        Attribute<GUM_SCALAR> *a =
            new Attribute<GUM_SCALAR>(name, *__retrieveType(type), impl);
        c->addParameter(a, false);
      } else {
        MultiDimSparse<GUM_SCALAR> *impl = new MultiDimSparse<GUM_SCALAR>(0);
        Attribute<GUM_SCALAR> *a =
            new Attribute<GUM_SCALAR>(name, *__retrieveType(type), impl);
        Instantiation inst(a->cpf());
        bool found = false;

        for (inst.setFirst(); not inst.end(); inst.inc()) {
          if (a->type()->label(inst.pos(*(a->type()))) == value) {
            a->cpf().set(inst, 1);
            found = true;
            break;
          }
        }

        if (not found) {
          GUM_ERROR(NotFound, "illegal default value.");
        }

        c->addParameter(a, true);
      }
    }

    template <typename GUM_SCALAR>
    void
    PRMFactory<GUM_SCALAR>::addAggregator(const std::string &name,
                                          const std::string &agg_type,
                                          const std::vector<std::string> &chains,
                                          const std::vector<std::string> &params) {
      Class<GUM_SCALAR> *c = static_cast<Class<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::CLASS));
      // Checking call legality

      if (chains.size() == 0) {
        GUM_ERROR(OperationNotAllowed,
                  "an Aggregate<GUM_SCALAR> requires at least one parent");
      }

      // Retrieving the parents of the aggregate
      std::vector<ClassElement<GUM_SCALAR> *> inputs;

      // This helps knowing if the aggregate has parents outside the current class
      // (see below)
      bool hasSC = __retrieveInputs(c, chains, inputs);

      // Checking that all inputs shares the same Type<GUM_SCALAR> (trivial if
      // inputs.size() == 1)
      if (inputs.size() > 1) {
        for (auto iter = inputs.begin() + 1; iter != inputs.end(); ++iter) {
          if ((**(iter - 1)).type() != (**iter).type()) {
            GUM_ERROR(WrongType, "found different types");
          }
        }
      }

      // Different treatments for different types of aggregate.
      Aggregate<GUM_SCALAR> *agg = 0;

      switch (Aggregate<GUM_SCALAR>::str2enum(agg_type)) {
        case Aggregate<GUM_SCALAR>::AggregateType::OR:
        case Aggregate<GUM_SCALAR>::AggregateType::AND: {
          if (inputs.front()->type() != *(__retrieveType("boolean"))) {
            GUM_ERROR(WrongType, "expected booleans");
          }
        }

        case Aggregate<GUM_SCALAR>::AggregateType::MIN:
        case Aggregate<GUM_SCALAR>::AggregateType::MAX: {
          if (params.size() != 0) {
            GUM_ERROR(OperationNotAllowed, "invalid number of paramaters");
          }

          agg = new Aggregate<GUM_SCALAR>(name,
                                          Aggregate<GUM_SCALAR>::str2enum(agg_type),
                                          inputs.front()->type());

          break;
        }

        case Aggregate<GUM_SCALAR>::AggregateType::EXISTS:
        case Aggregate<GUM_SCALAR>::AggregateType::FORALL: {
          if (params.size() != 1) {
            GUM_ERROR(OperationNotAllowed, "invalid number of parameters");
          }

          Idx label_idx = 0;

          while (label_idx < inputs.front()->type()->domainSize()) {
            if (inputs.front()->type()->label(label_idx) == params.front()) {
              break;
            }

            ++label_idx;
          }

          if (label_idx == inputs.front()->type()->domainSize()) {
            GUM_ERROR(NotFound, "could not find label");
          }

          // Creating and adding the Aggregate<GUM_SCALAR>
          agg = new Aggregate<GUM_SCALAR>(name,
                                          Aggregate<GUM_SCALAR>::str2enum(agg_type),
                                          *(__retrieveType("boolean")), label_idx);

          break;
        }

        default: { GUM_ERROR(FatalError, "Unknown aggregator."); }
      }

      std::string safe_name = agg->safeName();

      try {
        if (hasSC) {
          try {
            c->add(agg);
          } catch (DuplicateElement &) {
            c->overload(agg);
          }
        } else {
          // Inner aggregators can be directly used has attributes
          Attribute<GUM_SCALAR> *attr =
              new Attribute<GUM_SCALAR>(agg->name(), agg->type(), agg->buildImpl());

          try {
            c->add(attr);
          } catch (DuplicateElement &) {
            c->overload(attr);
          }

          delete agg;
        }
      } catch (DuplicateElement &e) {
        delete agg;
        throw;
      }

      for (const auto &elt : inputs) {
        c->addArc(elt->safeName(), safe_name);
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addReferenceSlot(const std::string &type,
                                                  const std::string &name,
                                                  bool isArray) {
      ClassElementContainer<GUM_SCALAR> *owner = __checkStackContainter(1);
      ClassElementContainer<GUM_SCALAR> *slotType = 0;

      try {
        slotType = __retrieveClass(type);
      } catch (NotFound &) {
        try {
          slotType = __retrieveInterface(type);
        } catch (NotFound &) {
          GUM_ERROR(NotFound, "unknown ReferenceSlot<GUM_SCALAR> slot type");
        }
      }

      ReferenceSlot<GUM_SCALAR> *ref =
          new ReferenceSlot<GUM_SCALAR>(name, *slotType, isArray);

      try {
        owner->add(ref);
      } catch (DuplicateElement &e) {
        owner->overload(ref);
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addArray(const std::string &type,
                                          const std::string &name, Size size) {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));
      Class<GUM_SCALAR> *c = __retrieveClass(type);
      Instance<GUM_SCALAR> *inst = 0;

      try {
        model->addArray(name, *c);

        for (Size i = 0; i < size; ++i) {
          std::stringstream elt_name;
          elt_name << name << "[" << i << "]";
          inst = new Instance<GUM_SCALAR>(elt_name.str(), *c);
          model->add(name, inst);
        }
      } catch (TypeError &e) {
        delete inst;
        throw;
      } catch (NotFound &e) {
        delete inst;
        throw;
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::incArray(const std::string &l_i,
                                          const std::string &r_i) {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));

      if (model->isArray(l_i)) {
        if (model->isInstance(r_i)) {
          model->add(l_i, model->get(r_i));
        } else {
          GUM_ERROR(NotFound, "right value is not an instance");
        }
      } else {
        GUM_ERROR(NotFound, "left value is no an array");
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::setReferenceSlot(const std::string &l_i,
                                                  const std::string &l_ref,
                                                  const std::string &r_i) {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));
      std::vector<Instance<GUM_SCALAR> *> lefts;
      std::vector<Instance<GUM_SCALAR> *> rights;

      if (model->isInstance(l_i)) {
        lefts.push_back(&(model->get(l_i)));
      } else if (model->isArray(l_i)) {
        for (const auto &elt : model->getArray(l_i))
          lefts.push_back(elt);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array");
      }

      if (model->isInstance(r_i)) {
        rights.push_back(&(model->get(r_i)));
      } else if (model->isArray(r_i)) {
        for (const auto &elt : model->getArray(r_i))
          rights.push_back(elt);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array");
      }

      for (const auto l : lefts) {
        for (const auto r : rights) {
          if ((*l).type().get(l_ref).elt_type() ==
              ClassElement<GUM_SCALAR>::prm_refslot) {
            (*l).add((*l).type().get(l_ref).id(), *r);
          } else {
            GUM_ERROR(NotFound, "unfound reference slot");
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::setParameter(const std::string &instance,
                                              const std::string &param,
                                              const std::string &value) {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));
      Instance<GUM_SCALAR> *i = 0;

      try {
        i = &(model->get(instance));
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "instance not found in current system");
      }

      Attribute<GUM_SCALAR> *a = 0;

      try {
        a = &(i->get(param));

        if (not i->type().isParameter(*a)) {
          GUM_ERROR(OperationNotAllowed, "given attribute is not a parameter");
        }
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no such parameter in the given instance");
      }

      Size label = a->type()->domainSize();

      for (Size idx = 0; idx < a->type()->domainSize(); ++idx) {
        if (value == a->type()->label(idx)) {
          label = idx;
          break;
        }
      }

      if (label == a->type()->domainSize()) {
        GUM_ERROR(NotFound, "no such label in the given parameter");
      }

      Potential<GUM_SCALAR> pot;

      pot.add(a->type().variable());
      pot.fill((GUM_SCALAR)0);
      Instantiation inst(pot);
      inst.chgVal(a->type().variable(), label);
      pot.set(inst, (GUM_SCALAR)1);
      i->setParameterValue(a->safeName(), pot);
    }

    template <typename GUM_SCALAR>
    SlotChain<GUM_SCALAR> *PRMFactory<GUM_SCALAR>::__buildSlotChain(
        ClassElementContainer<GUM_SCALAR> *start, const std::string &name) {
      std::vector<std::string> v;
      decomposePath(name, v);
      ClassElementContainer<GUM_SCALAR> *current = start;
      ReferenceSlot<GUM_SCALAR> *ref = 0;
      Sequence<ClassElement<GUM_SCALAR> *> elts;

      for (size_t i = 0; i < v.size(); ++i) {
        try {
          switch (current->get(v[i]).elt_type()) {
            case ClassElement<GUM_SCALAR>::prm_refslot:
              ref = &(static_cast<ReferenceSlot<GUM_SCALAR> &>(current->get(v[i])));
              elts.insert(ref);
              current = &(/*const_cast<ClassElementContainer<GUM_SCALAR>&>*/ (
                  ref->slotType()));
              break;

            case ClassElement<GUM_SCALAR>::prm_aggregate:
            case ClassElement<GUM_SCALAR>::prm_attribute:

              if (i == v.size() - 1) {
                elts.insert(&(current->get(v[i])));
                break;
              } else {
                return 0;
              }

            default: { return 0; }
          }
        } catch (NotFound &) {
          return 0;
        }
      }

      GUM_ASSERT(v.size() == elts.size());

      current->setOutputNode(*(elts.back()), true);

      return new SlotChain<GUM_SCALAR>(name, elts);
    }

    template <typename GUM_SCALAR>
    bool PRMFactory<GUM_SCALAR>::__retrieveInputs(
        Class<GUM_SCALAR> *c, const std::vector<std::string> &chains,
        std::vector<ClassElement<GUM_SCALAR> *> &inputs) {
      bool retVal = false;

      for (size_t i = 0; i < chains.size(); ++i) {
        try {
          inputs.push_back(&(c->get(chains[i])));
          retVal = retVal or ClassElement<GUM_SCALAR>::isSlotChain(*(inputs.back()));
        } catch (NotFound &) {
          inputs.push_back(__buildSlotChain(c, chains[i]));
          retVal = true;

          if (inputs.back()) {
            c->add(inputs.back());
          } else {
            GUM_ERROR(NotFound, "unknown slot chain");
          }
        }
      }

      Type<GUM_SCALAR> *t = __retrieveCommonType(inputs);

      std::vector<std::pair<ClassElement<GUM_SCALAR> *, ClassElement<GUM_SCALAR> *>>
          toAdd;

      for (const auto &elt : inputs) {
        if ((*elt).type() != (*t)) {
          if (ClassElement<GUM_SCALAR>::isSlotChain(*elt)) {
            SlotChain<GUM_SCALAR> *sc = static_cast<SlotChain<GUM_SCALAR> *>(elt);
            std::stringstream name;

            for (Size idx = 0; idx < sc->chain().size() - 1; ++idx) {
              name << sc->chain().atPos(idx)->name() << ".";
            }

            name << ".(" << t->name() << ")" << sc->lastElt().name();

            try {
              toAdd.push_back(std::make_pair(elt, &(c->get(name.str()))));
            } catch (NotFound &) {
              toAdd.push_back(std::make_pair(elt, __buildSlotChain(c, name.str())));
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

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR> *PRMFactory<GUM_SCALAR>::__retrieveCommonType(
        const std::vector<ClassElement<GUM_SCALAR> *> &elts) {
      const Type<GUM_SCALAR> *current = 0;
      HashTable<std::string, Size> counters;
      // Finding all types and super types

      for (const auto &elt : elts) {
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
              current = &(current->super());
            } else {
              current = 0;
            }
          }
        } catch (OperationNotAllowed &) {
          GUM_ERROR(WrongClassElement,
                    "found a ClassElement<GUM_SCALAR> without a type");
        }
      }

      // We need to find the most specialized (i.e. max depth) common type
      current = 0;

      int max_depth = -1;

      int current_depth = 0;

      for (const auto &elt : counters) {
        if ((elt.second) == elts.size()) {
          current_depth = __typeDepth(__retrieveType(elt.first));

          if (current_depth > max_depth) {
            max_depth = current_depth;
            current = __retrieveType(elt.first);
          }
        }
      }

      if (current) {
        return const_cast<Type<GUM_SCALAR> *>(current);
      }

      GUM_ERROR(NotFound, "could not find a common type");
    }

    template <typename GUM_SCALAR>
    void PRMFactory<GUM_SCALAR>::addNoisyOrCompound(
        const std::string &name, const std::vector<std::string> &chains,
        const std::vector<float> &numbers, float leak,
        const std::vector<std::string> &labels) {
      if (currentType() != PRMObject::PRMType::CLASS) {
        GUM_ERROR(gum::FactoryInvalidState, "invalid state to add a noisy-or");
      }

      Class<GUM_SCALAR> *c =
          dynamic_cast<gum::prm::Class<GUM_SCALAR> *>(getCurrent());

      std::vector<ClassElement<GUM_SCALAR> *> parents;

      for (const auto &elt : chains)
        parents.push_back(&(c->get(elt)));

      Type<GUM_SCALAR> *common_type = __retrieveCommonType(parents);

      for (size_t idx = 0; idx < parents.size(); ++idx) {
        if (parents[idx]->type() != (*common_type)) {
          ClassElement<GUM_SCALAR> *parent = parents[idx];
          // Either safe_name is an non existing slot chain or an existing cast
          // descendant
          std::string safe_name = parent->cast(*common_type);

          if (not c->exists(safe_name)) {
            if (ClassElement<GUM_SCALAR>::isSlotChain(*parent)) {
              parents[idx] = __buildSlotChain(c, safe_name);
              c->add(parents[idx]);
            } else {
              GUM_ERROR(NotFound, "unable to find parent");
            }
          } else {
            parents[idx] = &(c->get(safe_name));
          }
        }
      }

      if (numbers.size() == 1) {
        gum::prm::Attribute<GUM_SCALAR> *attr = new gum::prm::Attribute<GUM_SCALAR>(
            name, retrieveType("boolean"),
            new gum::MultiDimNoisyORCompound<GUM_SCALAR>(leak, numbers.front()));
        addAttribute(attr);
      } else if (numbers.size() == parents.size()) {
        gum::MultiDimNoisyORCompound<GUM_SCALAR> *noisy =
            new gum::MultiDimNoisyORCompound<GUM_SCALAR>(leak);
        gum::prm::FuncAttribute<GUM_SCALAR> *attr =
            new gum::prm::FuncAttribute<GUM_SCALAR>(name, retrieveType("boolean"),
                                                    noisy);

        for (size_t idx = 0; idx < numbers.size(); ++idx) {
          noisy->causalWeight(parents[idx]->type().variable(), numbers[idx]);
        }

        addAttribute(attr);
      } else {
        GUM_ERROR(OperationNotAllowed, "invalid parameters for a noisy or");
      }

      if (not labels.empty()) {
        GUM_ERROR(OperationNotAllowed, "labels definitions not handle for noisy-or");
      }
    }

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR> *
    PRMFactory<GUM_SCALAR>::__retrieveType(const std::string &name) const {
      try {
        return __prm->__typeMap[name];
      } catch (NotFound &) {
        try {
          return __prm->__typeMap[__addPrefix(name)];
        } catch (NotFound &) {
          // Looking for the type using all declared namespaces
          std::string prefix;
          std::string dot = ".";

          for (const auto &ns : __namespaces) {
            if (__prm->__typeMap.exists(ns + dot + name)) {
              if (prefix != "") {
                GUM_ERROR(NotFound, "Type name '"
                                        << name
                                        << "' is ambiguous. Specify full name");
              }

              prefix = ns;
            }
          }

          return __prm->__typeMap[prefix + dot + name];
        }
      }

      return 0;
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR> *
    PRMFactory<GUM_SCALAR>::__retrieveClass(const std::string &name) const {
      try {
        return __prm->__classMap[name];
      } catch (NotFound &) {
        try {
          return __prm->__classMap[__addPrefix(name)];
        } catch (NotFound &) {
          // Looking for the class using all declared namespaces
          std::string prefix;
          std::string dot = ".";

          for (const auto &ns : __namespaces) {
            if (__prm->__classMap.exists(ns + dot + name)) {
              if (prefix != "") {
                GUM_ERROR(NotFound, "Class name '"
                                        << name
                                        << "' is ambiguous. Specify full name");
              }

              prefix = ns;
            }
          }

          return __prm->__classMap[prefix + dot + name];
        }
      }

      // Just for compilation warnings
      return 0;
    }

    template <typename GUM_SCALAR>
    Interface<GUM_SCALAR> *
    PRMFactory<GUM_SCALAR>::__retrieveInterface(const std::string &name) const {
      try {
        return __prm->__interfaceMap[name];
      } catch (NotFound &) {
        try {
          return __prm->__interfaceMap[__addPrefix(name)];
        } catch (NotFound &) {
          // Looking for the interface using all declared namespaces
          std::string prefix;
          std::string dot = ".";

          for (const auto &ns : __namespaces) {
            if (__prm->__interfaceMap.exists(ns + dot + name)) {
              if (prefix != "") {
                GUM_ERROR(NotFound, "Class name '"
                                        << name
                                        << "' is ambiguous. Specify full name");
              }

              prefix = ns;
            }
          }

          return __prm->__interfaceMap[prefix + dot + name];
        }
      }

      // Just for compilation warnings
      return 0;
    }

    template <typename GUM_SCALAR> INLINE PRMFactory<GUM_SCALAR>::PRMFactory() {
      GUM_CONSTRUCTOR(PRMFactory);
      __prm = new PRM<GUM_SCALAR>();
    }

    template <typename GUM_SCALAR> INLINE PRMFactory<GUM_SCALAR>::~PRMFactory() {
      GUM_DESTRUCTOR(PRMFactory);
    }

    template <typename GUM_SCALAR>
    INLINE PRM<GUM_SCALAR> *PRMFactory<GUM_SCALAR>::prm() const {
      return __prm;
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::PRMType PRMFactory<GUM_SCALAR>::currentType() const {
      if (__stack.size() == 0) {
        GUM_ERROR(NotFound, "no object being built");
      }

      return __stack.back()->obj_type();
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject *PRMFactory<GUM_SCALAR>::getCurrent() {
      if (__stack.size() == 0) {
        GUM_ERROR(NotFound, "no object being built");
      }

      return __stack.back();
    }

    template <typename GUM_SCALAR>
    INLINE const PRMObject *PRMFactory<GUM_SCALAR>::getCurrent() const {
      if (__stack.size() == 0) {
        GUM_ERROR(NotFound, "no object being built");
      }

      return __stack.back();
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject *PRMFactory<GUM_SCALAR>::closeCurrent() {
      if (__stack.size() > 0) {
        PRMObject *obj = __stack.back();
        __stack.pop_back();
        return obj;
      } else {
        return 0;
      }
    }

    template <typename GUM_SCALAR>
    INLINE std::string PRMFactory<GUM_SCALAR>::currentPackage() const {
      return (__packages.empty()) ? "" : __packages.back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startDiscreteType(const std::string &name,
                                                          std::string super) {
      if (super == "") {
        std::string real_name = __addPrefix(name);
        __checkDuplicateName(real_name);
        Type<GUM_SCALAR> *t =
            new Type<GUM_SCALAR>(LabelizedVariable(real_name, "", 0));
        __stack.push_back(t);
      } else {
        std::string real_name = __addPrefix(name);
        __checkDuplicateName(real_name);
        Type<GUM_SCALAR> *t =
            new Type<GUM_SCALAR>(LabelizedVariable(real_name, "", 0));
        t->__super = __retrieveType(super);
        t->__label_map = new std::vector<Idx>();
        __stack.push_back(t);
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endDiscreteType() {
      Type<GUM_SCALAR> *t =
          static_cast<Type<GUM_SCALAR> *>(__checkStack(1, PRMObject::PRMType::TYPE));

      if (not t->__isValid()) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid subtype");
      } else if (t->variable().domainSize() < 2) {
        GUM_ERROR(OperationNotAllowed, "current type is not a valid discrete type");
      }

      __prm->__typeMap.insert(t->name(), t);

      __prm->__types.insert(t);
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endInterface() {
      __checkStack(1, PRMObject::PRMType::INTERFACE);
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addAttribute(const std::string &type,
                                                     const std::string &name) {
      __checkStack(1, PRMObject::PRMType::INTERFACE);
      startAttribute(type, name);
      endAttribute();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startAttribute(const std::string &type,
                                                       const std::string &name) {
      ClassElementContainer<GUM_SCALAR> *c = __checkStackContainter(1);
      Attribute<GUM_SCALAR> *a =
          new Attribute<GUM_SCALAR>(name, *__retrieveType(type));
      std::string dot = ".";

      try {
        c->add(a);
      } catch (DuplicateElement &) {
        c->overload(a);
      }

      __stack.push_back(a);
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endAttribute() {
      __checkStack(1, ClassElement<GUM_SCALAR>::prm_attribute);
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startSystem(const std::string &name) {
      __checkDuplicateName(__addPrefix(name));
      System<GUM_SCALAR> *model = new System<GUM_SCALAR>(__addPrefix(name));
      __stack.push_back(model);
    }

    template <typename GUM_SCALAR> INLINE void PRMFactory<GUM_SCALAR>::endSystem() {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));
      __stack.pop_back();
      model->instantiate();
      __prm->__systemMap.insert(model->name(), model);
      __prm->__systems.insert(model);
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addInstance(const std::string &type,
                                                    const std::string &name) {
      System<GUM_SCALAR> *model = static_cast<System<GUM_SCALAR> *>(
          __checkStack(1, PRMObject::PRMType::SYSTEM));
      Class<GUM_SCALAR> *c = __retrieveClass(type);
      Instance<GUM_SCALAR> *inst = new Instance<GUM_SCALAR>(name, *c);

      try {
        model->add(inst);
      } catch (OperationNotAllowed &e) {
        delete inst;
        throw;
      }
    }

    template <typename GUM_SCALAR>
    INLINE std::string
    PRMFactory<GUM_SCALAR>::__addPrefix(const std::string &str) const {
      if (not __packages.empty()) {
        std::string full_name = __packages.back();
        full_name.append(".");
        full_name.append(str);
        return full_name;
      } else {
        return str;
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::__checkDuplicateName(const std::string &name) {
      if (__prm->__classMap.exists(name) || __prm->__typeMap.exists(name) ||
          __prm->__interfaceMap.exists(name) || __prm->__systemMap.exists(name)) {
        std::stringstream msg;
        msg << "\"" << name << "\" is already used.";
        GUM_ERROR(DuplicateElement, msg.str());
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject *
    PRMFactory<GUM_SCALAR>::__checkStack(Idx i, PRMObject::PRMType obj_type) {
      // Don't forget that Idx are unsigned int
      if (__stack.size() - i > __stack.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      PRMObject *obj = __stack[__stack.size() - i];

      if (obj->obj_type() != obj_type) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      return obj;
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR> *
    PRMFactory<GUM_SCALAR>::__checkStackContainter(Idx i) {
      // Don't forget that Idx are unsigned int
      if (__stack.size() - i > __stack.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      PRMObject *obj = __stack[__stack.size() - i];

      if ((obj->obj_type() == PRMObject::PRMType::CLASS) ||
          (obj->obj_type() == PRMObject::PRMType::INTERFACE)) {
        return static_cast<ClassElementContainer<GUM_SCALAR> *>(obj);
      } else {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> *PRMFactory<GUM_SCALAR>::__checkStack(
        Idx i, typename ClassElement<GUM_SCALAR>::ClassElementType elt_type) {
      // Don't forget that Idx are unsigned int
      if (__stack.size() - i > __stack.size()) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      ClassElement<GUM_SCALAR> *obj =
          dynamic_cast<ClassElement<GUM_SCALAR> *>(__stack[__stack.size() - i]);

      if (obj == 0) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      if (obj->elt_type() != elt_type) {
        GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
      }

      return obj;
    }

    template <typename GUM_SCALAR>
    INLINE int PRMFactory<GUM_SCALAR>::__typeDepth(const Type<GUM_SCALAR> *t) {
      int depth = 0;
      const Type<GUM_SCALAR> *current = t;

      while (current->isSubType()) {
        ++depth;
        current = &(current->super());
      }

      return depth;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::pushPackage(const std::string &name) {
      __packages.push_back(name);
      __namespaces.insert(name);
    }

    template <typename GUM_SCALAR>
    INLINE std::string PRMFactory<GUM_SCALAR>::popPackage() {
      if (not __packages.empty()) {
        std::string s = __packages.back();
        __packages.pop_back();
        return s;
      }

      return "";
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setReferenceSlot(const std::string &l_i,
                                                         const std::string &r_i) {
      size_t pos = l_i.find_last_of('.');

      if (pos != std::string::npos) {
        std::string l_ref = l_i.substr(pos + 1, std::string::npos);
        setReferenceSlot(l_i.substr(0, pos), l_ref, r_i);
      } else {
        GUM_ERROR(NotFound, "left value does not name an instance or an array");
      }
    }

    template <typename GUM_SCALAR>
    INLINE Class<GUM_SCALAR> &
    PRMFactory<GUM_SCALAR>::retrieveClass(const std::string &name) {
      return *__retrieveClass(name);
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &
    PRMFactory<GUM_SCALAR>::retrieveType(const std::string &name) {
      return *__retrieveType(name);
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &PRMFactory<GUM_SCALAR>::retrieveCommonType(
        const std::vector<ClassElement<GUM_SCALAR> *> &elts) {
      return *(__retrieveCommonType(elts));
    }

    template <typename GUM_SCALAR>
    bool PRMFactory<GUM_SCALAR>::isClassOrInterface(const std::string &type) const {
      std::string dot = ".";
      return prm()->isClass(type) or prm()->isClass(currentPackage() + dot + type) or
             prm()->isInterface(type) or
             prm()->isInterface(currentPackage() + dot + type);
    }

    template <typename GUM_SCALAR>
    bool
    PRMFactory<GUM_SCALAR>::isArrayInCurrentSystem(const std::string &name) const {
      const System<GUM_SCALAR> *system =
          static_cast<const System<GUM_SCALAR> *>(getCurrent());
      return (system && system->isArray(name));
    }
  } /* namespace prm */
} /* namespace gum */
