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
 * @brief Inline implementation of gum::Aggregate
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/aggregate.h>
#include <agrum/PRM/elements/type.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate(const std::string &name, AggregateType aggType,
                                     const Type<GUM_SCALAR> &rvType)
        : ClassElement<GUM_SCALAR>(name), __agg_type(aggType),
          __type(new Type<GUM_SCALAR>(rvType)),
          __label(rvType.variable().domainSize() + 1) {
      GUM_CONSTRUCTOR(Aggregate);
      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate(const std::string &name, AggregateType aggType,
                                     const Type<GUM_SCALAR> &rvType, Idx label)
        : ClassElement<GUM_SCALAR>(name), __agg_type(aggType),
          __type(new Type<GUM_SCALAR>(rvType)), __label(label) {
      GUM_CONSTRUCTOR(Aggregate);
      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR> Aggregate<GUM_SCALAR>::~Aggregate() {
      GUM_DESTRUCTOR(Aggregate);
      delete __type;
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate(const Aggregate<GUM_SCALAR> &source)
        : ClassElement<GUM_SCALAR>(source), __agg_type(source.__agg_type),
          __type(new Type<GUM_SCALAR>(source.type())), __label(source.__label) {
      GUM_CONS_CPY(Aggregate);
      GUM_ERROR(FatalError, "illegal call to gum::Aggregate copy constructor.");
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR> &Aggregate<GUM_SCALAR>::
    operator=(const Aggregate<GUM_SCALAR> &source) {
      GUM_ERROR(FatalError, "illegal call to gum::Aggregate copy operator.");
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    Aggregate<GUM_SCALAR>::elt_type() const {
      return this->prm_aggregate;
    }

    template <typename GUM_SCALAR>
    INLINE typename Aggregate<GUM_SCALAR>::AggregateType
    Aggregate<GUM_SCALAR>::agg_type() const {
      return __agg_type;
    }

    template <typename GUM_SCALAR> INLINE Idx Aggregate<GUM_SCALAR>::label() const {
      return __label;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &Aggregate<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR> &Aggregate<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> &Aggregate<GUM_SCALAR>::cpf() {
      GUM_ERROR(OperationNotAllowed, "This is an aggregate.");
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR> &Aggregate<GUM_SCALAR>::cpf() const {
      GUM_ERROR(OperationNotAllowed, "This is an aggregate.");
    }

    template <typename GUM_SCALAR>
    INLINE MultiDimImplementation<GUM_SCALAR> *
    Aggregate<GUM_SCALAR>::buildImpl() const {
      MultiDimImplementation<GUM_SCALAR> *impl = 0;

      switch (agg_type()) {
        case AggregateType::MIN:
          impl = new aggregator::Min<GUM_SCALAR>();
          break;

        case AggregateType::MAX:
          impl = new aggregator::Max<GUM_SCALAR>();
          break;

        // case AggregateType::COUNT:
        //  if (__label < __type.variable().domainSize()) {
        //    impl = new aggregator::Count<GUM_SCALAR>(__label);
        //  } else {
        //    std::string msg = "This aggregator of type agg_count isn't initialized
        //    "
        //      "properly.";
        //    GUM_ERROR(FatalError, msg);
        //  }
        //  break;
        case AggregateType::EXISTS:
          impl = new aggregator::Exists<GUM_SCALAR>(__label);
          break;

        case AggregateType::FORALL:
          impl = new aggregator::Forall<GUM_SCALAR>(__label);
          break;

        case AggregateType::COUNT:
        case AggregateType::MEAN:
        case AggregateType::OR:
        case AggregateType::AND:
          GUM_ERROR(OperationNotAllowed, "Aggregator not implemented yet.");
          break;

        default:
          GUM_ERROR(OperationNotAllowed, "Unknown aggregator.");
      }

      return impl;
    }

    // See gum::ClassElement<GUM_SCALAR>::_addParent().
    template <typename GUM_SCALAR>
    INLINE void
    Aggregate<GUM_SCALAR>::addParent(const ClassElement<GUM_SCALAR> &elt) {}

    // See gum::ClassElement<GUM_SCALAR>::_addChild().
    template <typename GUM_SCALAR>
    INLINE void
    Aggregate<GUM_SCALAR>::addChild(const ClassElement<GUM_SCALAR> &elt) {}

  } /* namespace prm */
} /* namespace gum */
