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
 * @brief Inline implementation of gum::PRMAggregate
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <climits>

#include <agrum/tools/multidim/aggregators/amplitude.h>
#include <agrum/tools/multidim/aggregators/and.h>
#include <agrum/tools/multidim/aggregators/count.h>
#include <agrum/tools/multidim/aggregators/or.h>
#include <agrum/tools/multidim/aggregators/exists.h>
#include <agrum/tools/multidim/aggregators/forall.h>
#include <agrum/tools/multidim/aggregators/max.h>
#include <agrum/tools/multidim/aggregators/median.h>
#include <agrum/tools/multidim/aggregators/min.h>
#include <agrum/tools/multidim/aggregators/sum.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMAggregate< GUM_SCALAR >::PRMAggregate(const std::string& name,
                                             AggregateType      aggType,
                                             const PRMType&     rvType) :
        PRMClassElement< GUM_SCALAR >(name),
        _agg_type_(aggType), _type_(new PRMType(rvType)),
        _label_(std::shared_ptr< Idx >(new Idx(INT_MAX))) {
      GUM_CONSTRUCTOR(PRMAggregate);
      this->safeName_ = PRMObject::LEFT_CAST() + _type_->name() + PRMObject::RIGHT_CAST() + name;
      this->_type_->variable().setName(name);
    }

    template < typename GUM_SCALAR >
    PRMAggregate< GUM_SCALAR >::PRMAggregate(const std::string& name,
                                             AggregateType      aggType,
                                             const PRMType&     rvType,
                                             Idx                label) :
        PRMClassElement< GUM_SCALAR >(name),
        _agg_type_(aggType), _type_(new PRMType(rvType)),
        _label_(std::shared_ptr< Idx >(new Idx(label))) {
      GUM_CONSTRUCTOR(PRMAggregate);
      this->safeName_ = PRMObject::LEFT_CAST() + _type_->name() + PRMObject::RIGHT_CAST() + name;
      this->_type_->variable().setName(name);
    }

    template < typename GUM_SCALAR >
    PRMAggregate< GUM_SCALAR >::~PRMAggregate() {
      GUM_DESTRUCTOR(PRMAggregate);
      delete _type_;
    }

    template < typename GUM_SCALAR >
    PRMAggregate< GUM_SCALAR >::PRMAggregate(const PRMAggregate< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source) {
      GUM_CONS_CPY(PRMAggregate);
      GUM_ERROR(FatalError, "illegal call to gum::PRMAggregate copy constructor.")
    }

    template < typename GUM_SCALAR >
    PRMAggregate< GUM_SCALAR >&
       PRMAggregate< GUM_SCALAR >::operator=(const PRMAggregate< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "illegal call to gum::PRMAggregate copy operator.")
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMAggregate< GUM_SCALAR >::elt_type() const {
      return this->prm_aggregate;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMAggregate< GUM_SCALAR >::AggregateType
       PRMAggregate< GUM_SCALAR >::agg_type() const {
      return _agg_type_;
    }

    template < typename GUM_SCALAR >
    INLINE Idx PRMAggregate< GUM_SCALAR >::label() const {
      if (*_label_ != INT_MAX) return *_label_;
      GUM_ERROR(OperationNotAllowed, "no label defined for this aggregate")
    }

    template < typename GUM_SCALAR >
    INLINE const std::string& PRMAggregate< GUM_SCALAR >::labelValue() const {
      return _label_value_;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMAggregate< GUM_SCALAR >::setLabel(Idx idx) {
      (*_label_) = idx;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMAggregate< GUM_SCALAR >::setLabel(const std::string& value) {
      _label_value_ = value;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMAggregate< GUM_SCALAR >::hasLabel() const {
      return *_label_ != INT_MAX;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMAggregate< GUM_SCALAR >::isDecomposable() const {
      switch (agg_type()) {
        case AggregateType::MIN: {
          return aggregator::Min< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::MAX: {
          return aggregator::Max< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::OR: {
          return aggregator::Or< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::AND: {
          return aggregator::And< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::AMPLITUDE: {
          return aggregator::Amplitude< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::MEDIAN: {
          return aggregator::Median< GUM_SCALAR >().isDecomposable();
        }
        case AggregateType::EXISTS: {
          return aggregator::Exists< GUM_SCALAR >(label()).isDecomposable();
        }
        case AggregateType::FORALL: {
          return aggregator::Forall< GUM_SCALAR >(label()).isDecomposable();
        }
        case AggregateType::COUNT: {
          return aggregator::Count< GUM_SCALAR >(label()).isDecomposable();
        }
        case AggregateType::SUM: {
          return aggregator::Sum< GUM_SCALAR >().isDecomposable();
        }
        default: {
          GUM_ERROR(OperationNotAllowed, "Unknown aggregator.")
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMAggregate< GUM_SCALAR >::type() {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMAggregate< GUM_SCALAR >::type() const {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >& PRMAggregate< GUM_SCALAR >::cpf() {
      GUM_ERROR(OperationNotAllowed, "This is an aggregate.")
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >& PRMAggregate< GUM_SCALAR >::cpf() const {
      GUM_ERROR(OperationNotAllowed, "This is an aggregate.")
    }

    template < typename GUM_SCALAR >
    INLINE MultiDimImplementation< GUM_SCALAR >* PRMAggregate< GUM_SCALAR >::buildImpl() const {
      switch (agg_type()) {
        case AggregateType::MIN: {
          return new aggregator::Min< GUM_SCALAR >();
        }
        case AggregateType::MAX: {
          return new aggregator::Max< GUM_SCALAR >();
        }
        case AggregateType::OR: {
          return new aggregator::Or< GUM_SCALAR >();
        }
        case AggregateType::AND: {
          return new aggregator::And< GUM_SCALAR >();
        }
        case AggregateType::AMPLITUDE: {
          return new aggregator::Amplitude< GUM_SCALAR >();
        }
        case AggregateType::MEDIAN: {
          return new aggregator::Median< GUM_SCALAR >();
        }
        case AggregateType::EXISTS: {
          return new aggregator::Exists< GUM_SCALAR >(label());
        }
        case AggregateType::FORALL: {
          return new aggregator::Forall< GUM_SCALAR >(label());
        }
        case AggregateType::COUNT: {
          return new aggregator::Count< GUM_SCALAR >(label());
        }
        case AggregateType::SUM: {
          return new aggregator::Sum< GUM_SCALAR >();
        }
        default: {
          GUM_ERROR(OperationNotAllowed, "Unknown aggregator.")
        }
      }
      return nullptr;
    }


    // See gum::PRMClassElement<GUM_SCALAR>::addParent_().
    template < typename GUM_SCALAR >
    INLINE void PRMAggregate< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {}

    // See gum::PRMClassElement<GUM_SCALAR>::addChild_().
    template < typename GUM_SCALAR >
    INLINE void PRMAggregate< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMAggregate< GUM_SCALAR >::getCastDescendant() const {
      PRMScalarAttribute< GUM_SCALAR >* cast = 0;

      try {
        cast = new PRMScalarAttribute< GUM_SCALAR >(this->name(), type().superType());
      } catch (NotFound const&) {
        GUM_ERROR(OperationNotAllowed, "this PRMAggregate can not have cast descendant")
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
    INLINE std::shared_ptr< Idx > PRMAggregate< GUM_SCALAR >::sharedLabel() const {
      return _label_;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMAggregate< GUM_SCALAR >::sharedLabel(std::shared_ptr< Idx > label) {
      this->_label_ = label;
    }

  } /* namespace prm */
} /* namespace gum */
