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
 * @brief Inline implementation of gum::Attribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/type.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR>::Attribute(const std::string &name,
                                     const Type<GUM_SCALAR> &type,
                                     MultiDimImplementation<GUM_SCALAR> *impl)
        : ClassElement<GUM_SCALAR>(name), __type(new Type<GUM_SCALAR>(type)),
          __cpf(new Potential<GUM_SCALAR>(impl)), __delete_type(true) {
      GUM_CONSTRUCTOR(Attribute);
      __cpf->add(**__type);
      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR>::Attribute(const std::string &name, Type<GUM_SCALAR> *type,
                                     Potential<GUM_SCALAR> *cpf, bool delete_type)
        : ClassElement<GUM_SCALAR>(name), __type(type), __cpf(cpf),
          __delete_type(delete_type) {
      GUM_CONSTRUCTOR(Attribute);

      if (not __cpf->variablesSequence().exists(&(type->variable())))
        GUM_ERROR(
            OperationNotAllowed,
            "the given Potential does not contain the type of this Attribute.");

      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR>::Attribute(const Attribute<GUM_SCALAR> &source)
        : ClassElement<GUM_SCALAR>(source),
          __type(new Type<GUM_SCALAR>(source.type())), __cpf(0) {
      GUM_CONS_CPY(Attribute);
      GUM_ERROR(FatalError,
                "Illegal call to the copy constructor of gum::Attribute");
    }

    template <typename GUM_SCALAR> Attribute<GUM_SCALAR>::~Attribute() {
      GUM_DESTRUCTOR(Attribute);
      delete __cpf;

      if (__delete_type) {
        delete __type;
      }
    }

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR> &Attribute<GUM_SCALAR>::
    operator=(const Attribute<GUM_SCALAR> &from) {
      GUM_ERROR(FatalError, "Illegal call to the copy operator of gum::Attribute");
    }

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR> *Attribute<GUM_SCALAR>::getCastDescendant() const {
      Attribute<GUM_SCALAR> *cast = 0;

      try {
        cast = new Attribute<GUM_SCALAR>(this->name(), type().super());
      } catch (NotFound &) {
        GUM_ERROR(OperationNotAllowed,
                  "this Attribute can not have cast descendant");
      }

      cast->addParent(*this);
      const DiscreteVariable &my_var = type().variable();
      DiscreteVariable &cast_var = cast->type().variable();
      Instantiation inst(cast->cpf());

      for (inst.setFirst(); not inst.end(); inst.inc()) {
        if (type().label_map()[inst.val(my_var)] == inst.val(cast_var)) {
          cast->cpf().set(inst, 1);
        } else {
          cast->cpf().set(inst, 0);
        }
      }

      return cast;
    }

    template <typename GUM_SCALAR>
    void Attribute<GUM_SCALAR>::setAsCastDescendant(Attribute<GUM_SCALAR> *cast) {
      try {
        type().setSuper(cast->type());
      } catch (OperationNotAllowed &) {
        GUM_ERROR(OperationNotAllowed,
                  "this Attribute can not have cast descendant");
      } catch (WrongType &) {
        GUM_ERROR(WrongType, "the cast descendant Type<GUM_SCALAR> is not a direct "
                             "subtype of this Attribute super Type<GUM_SCALAR>");
      }

      Potential<GUM_SCALAR> *cpf = new Potential<GUM_SCALAR>();
      cpf->add(cast->type().variable());
      cpf->add(type().variable());
      DiscreteVariable &my_var = type().variable();
      DiscreteVariable &cast_var = cast->type().variable();
      Instantiation inst(*cpf);

      for (inst.setFirst(); not inst.end(); inst.inc()) {
        if (type().label_map()[inst.pos(my_var)] == inst.pos(cast_var))
          cpf->set(inst, 1);
        else
          cpf->set(inst, 0);
      }

      delete cast->__cpf;
      cast->__cpf = cpf;
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    Attribute<GUM_SCALAR>::elt_type() const {
      return this->prm_attribute;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &Attribute<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR> &Attribute<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> &Attribute<GUM_SCALAR>::cpf() {
      return *__cpf;
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR> &Attribute<GUM_SCALAR>::cpf() const {
      return *__cpf;
    }

    template <typename GUM_SCALAR>
    INLINE void
    Attribute<GUM_SCALAR>::addParent(const ClassElement<GUM_SCALAR> &elt) {
      try {
        __cpf->add(elt.type().variable());
      } catch (DuplicateElement &) {
        std::stringstream msg;
        msg << ": " << elt.name() << " as parent of " << this->name();
        GUM_ERROR(DuplicateElement, msg.str());
      } catch (OperationNotAllowed &) {
        std::stringstream msg;
        msg << ": " << elt.name() << " of wrong type as parent of " << this->name();
        GUM_ERROR(OperationNotAllowed, msg.str());
      }
    }

    // See gum::ClassElement<GUM_SCALAR>::_addChild().
    template <typename GUM_SCALAR>
    INLINE void
    Attribute<GUM_SCALAR>::addChild(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE FuncAttribute<GUM_SCALAR>::FuncAttribute(
        const std::string &name, const Type<GUM_SCALAR> &type,
        MultiDimImplementation<GUM_SCALAR> *impl)
        : Attribute<GUM_SCALAR>(name, type, impl) {
      GUM_CONSTRUCTOR(FuncAttribute);
    }

    template <typename GUM_SCALAR>
    INLINE FuncAttribute<GUM_SCALAR>::FuncAttribute(const std::string &name,
                                                    Type<GUM_SCALAR> *type,
                                                    Potential<GUM_SCALAR> *cpf,
                                                    bool delete_type)
        : Attribute<GUM_SCALAR>(name, type, cpf, delete_type) {
      GUM_CONSTRUCTOR(FuncAttribute);
    }

    template <typename GUM_SCALAR>
    INLINE FuncAttribute<GUM_SCALAR>::~FuncAttribute() {
      GUM_DESTRUCTOR(FuncAttribute);
    }

    template <typename GUM_SCALAR>
    INLINE void
    FuncAttribute<GUM_SCALAR>::addParent(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE void
    FuncAttribute<GUM_SCALAR>::addChild(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE
    FuncAttribute<GUM_SCALAR>::FuncAttribute(const FuncAttribute<GUM_SCALAR> &source)
        : Attribute<GUM_SCALAR>(source) {
      GUM_CONS_CPY(FuncAttribute);
      GUM_ERROR(FatalError, "illegal call to copy constructor");
    }

    template <typename GUM_SCALAR>
    INLINE FuncAttribute<GUM_SCALAR> &FuncAttribute<GUM_SCALAR>::
    operator=(const FuncAttribute<GUM_SCALAR> &from) {
      GUM_ERROR(FatalError, "illegal call to copy operator");
    }

  } /* namespace prm */
} /* namespace gum */
