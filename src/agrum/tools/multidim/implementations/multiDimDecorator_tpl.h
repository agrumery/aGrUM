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
 * @brief Implementation for MultiDimDecorator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <mutex>

// include the operators that will be used by the decorators
#include <agrum/tools/multidim/utils/operators/completeProjections4MultiDim.h>
#include <agrum/tools/multidim/utils/operators/operators4MultiDim.h>
#include <agrum/tools/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/tools/multidim/utils/partialInstantiation4MultiDim.h>

#include <agrum/tools/multidim/implementations/multiDimDecorator.h>

namespace gum {
  // instrumental and non-API function
  template < typename GUM_SCALAR >
  void _initPotentialOperators__() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      // register the operators that will be used by the decorator
      Operators4MultiDimInitialize< GUM_SCALAR > op;
      op.init();

      // register the projectors that will be used by the decorator
      Projections4MultiDimInitialize< GUM_SCALAR > proj;
      proj.init();

      // register the projectors that will be used by the decorator
      CompleteProjections4MultiDimInitialize< GUM_SCALAR > comp_proj;
      comp_proj.init();

      // register the partial instantiators that will be used by the decorator
      PartialInstantiation4MultiDimInitialize< GUM_SCALAR > inst;
      inst.init();
    });
  }

  // constructors
  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >::MultiDimDecorator(
     MultiDimImplementation< GUM_SCALAR >* aContent,
     GUM_SCALAR                            empty_value) :
      content_(aContent),
      empty_value_(empty_value) {
    _initPotentialOperators__< GUM_SCALAR >();
    GUM_CONSTRUCTOR(MultiDimDecorator);
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >::MultiDimDecorator(
     const MultiDimDecorator< GUM_SCALAR >& from) :
      MultiDimContainer< GUM_SCALAR >(from) {
    GUM_CONS_CPY(MultiDimDecorator);
    _initPotentialOperators__< GUM_SCALAR >();
    empty_value_ = from.empty_value_;
    content()->copy(from.content());
  }


  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >&
     MultiDimDecorator< GUM_SCALAR >::operator=(MultiDimDecorator< GUM_SCALAR >&& from) {
    MultiDimContainer< GUM_SCALAR >::operator=(
       std::forward< MultiDimContainer< GUM_SCALAR > >(from));
    GUM_OP_MOV(MultiDimDecorator);

    if (this != &from) {
      if (content_ != nullptr) delete (content_);   // should be the case
      empty_value_  = from.empty_value_;
      content_      = from.content_;
      from.content_ = nullptr;
    }

    return *this;
  }


  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >::MultiDimDecorator(
     MultiDimDecorator< GUM_SCALAR >&& from) noexcept :
      MultiDimContainer< GUM_SCALAR >(std::forward< MultiDimContainer< GUM_SCALAR > >(from)) {
    GUM_CONS_MOV(MultiDimDecorator);

    empty_value_  = from.empty_value_;
    content_      = from.content_;
    from.content_ = nullptr;
  }


  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >& MultiDimDecorator< GUM_SCALAR >::operator=(
     const MultiDimDecorator< GUM_SCALAR >& from) noexcept {
    GUM_OP_CPY(MultiDimDecorator);
    _initPotentialOperators__< GUM_SCALAR >();
    MultiDimContainer< GUM_SCALAR >::operator=(from);
    empty_value_ = from.empty_value_;
    if (content_ == nullptr)
      content_ = static_cast< MultiDimImplementation< GUM_SCALAR >* >(from.content()->newFactory());
    MultiDimDecorator< GUM_SCALAR >::content()->copy(*from.content());
    return *this;
  }


  // destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimDecorator< GUM_SCALAR >::~MultiDimDecorator() {
    if (content_ != nullptr) { delete (content_); }

    GUM_DESTRUCTOR(MultiDimDecorator);
  }

  // return a data, given a Instantiation - final method

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR& MultiDimDecorator< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "_get in the implementation !")
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimDecorator< GUM_SCALAR >::get(const Instantiation& i) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      return empty_value_;
    } else {
      return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->get(i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::set(const Instantiation& i,
                                                   const GUM_SCALAR&    value) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->nbrDim() == 0) {
      empty_value_ = value;
    } else {
      static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->set(i, value);
    }
  }

  // get the size of domains - final method

  template < typename GUM_SCALAR >
  INLINE Size MultiDimDecorator< GUM_SCALAR >::domainSize() const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->domainSize();
  }

  // add a new var to the sequence of vars - final method

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::add(const DiscreteVariable& v) {
    if (v.domainSize() < 1) {
      GUM_ERROR(InvalidArgument, "Empty variable " << v << " cannot be added in a Potential")
    }
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->add(v);
  }

  // listen to change in each recorded Instantiation. final method

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::changeNotification(const Instantiation&          i,
                                                                  const DiscreteVariable* const var,
                                                                  Idx oldval,
                                                                  Idx newval) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->changeNotification(i,
                                                                                  var,
                                                                                  oldval,
                                                                                  newval);
  }

  // listen to an assignment of a value in a Instantiation

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::setChangeNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->setChangeNotification(i);
  }

  // listen to setFirst in each recorded Instantiation. final method.

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::setFirstNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->setFirstNotification(i);
  }

  // listen to setLast in each recorded Instantiation. final method.

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::setLastNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->setLastNotification(i);
  }

  // listen to increment in each recorded Instantiation. final method.

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::setIncNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->setIncNotification(i);
  }

  // listen to increment in each recorded Instantiation. final method.

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::setDecNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->setDecNotification(i);
  }

  // add a Instantiation as a slave of this

  template < typename GUM_SCALAR >
  INLINE bool MultiDimDecorator< GUM_SCALAR >::registerSlave(Instantiation& i) {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->registerSlave(i);
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::erase(const DiscreteVariable& var) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->erase(var);
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::erase(const std::string& name) {
    erase(variable(name));
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& MultiDimDecorator< GUM_SCALAR >::variable(Idx i) const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->variable(i);
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
     MultiDimDecorator< GUM_SCALAR >::variable(const std::string& name) const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->variable(name);
  }

  template < typename GUM_SCALAR >
  INLINE Idx MultiDimDecorator< GUM_SCALAR >::pos(const DiscreteVariable& var) const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->pos(var);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimDecorator< GUM_SCALAR >::contains(const DiscreteVariable& var) const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->contains(var);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimDecorator< GUM_SCALAR >::empty() const {
    if (content_ == nullptr) return true;
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty();
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimDecorator< GUM_SCALAR >::unregisterSlave(Instantiation& i) {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->unregisterSlave(i);
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::fill(const GUM_SCALAR& d) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      empty_value_ = d;
    } else {
      static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->fill(d);
    }
  }

  // notification modification on vars to all Instantiation listeners.

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::notifyChange() const {
    /*( (MultiDimContainer<GUM_SCALAR> *) content_)->notifyChange();*/
    GUM_ERROR(OperationNotAllowed, "Not implemented yet")
  }

  // give a const ref to the sequence of DiscreteVariable*. final method.

  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               MultiDimDecorator< GUM_SCALAR >::variablesSequence() const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->variablesSequence();
  }

  // get the nbr of vars in the sequence. final method

  template < typename GUM_SCALAR >
  INLINE Idx MultiDimDecorator< GUM_SCALAR >::nbrDim() const {
    return static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->nbrDim();
  }

  template < typename GUM_SCALAR >
  void MultiDimDecorator< GUM_SCALAR >::populate(const std::vector< GUM_SCALAR >& v) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      if (v.size() == 1) {
        empty_value_ = v[0];
      } else {
        GUM_ERROR(SizeError, "Size do not match in populate")
      }
    } else {
      content_->populate(v);
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimDecorator< GUM_SCALAR >::apply(std::function< GUM_SCALAR(GUM_SCALAR) > f) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      empty_value_ = f(empty_value_);
    } else {
      content_->apply(f);
    }
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR
     MultiDimDecorator< GUM_SCALAR >::reduce(std::function< GUM_SCALAR(GUM_SCALAR, GUM_SCALAR) > f,
                                             GUM_SCALAR base) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      return base;
    } else {
      return content_->reduce(f, base);
    }
  }

  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >* MultiDimDecorator< GUM_SCALAR >::content() {
    return content_;
  }

  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               MultiDimDecorator< GUM_SCALAR >::content() const {
    return content_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::beginMultipleChanges() {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->beginMultipleChanges();
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::endMultipleChanges() {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->endMultipleChanges();
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::endMultipleChanges(const GUM_SCALAR& x) {
    static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->endMultipleChanges(x);
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::swapContent_(
     MultiDimImplementation< GUM_SCALAR >* aContent) const {
    if (aContent != nullptr) {
      // TODO : frees all slave instantiations
      // TODO : control the dimensions ?
      MultiDimImplementation< GUM_SCALAR >* tmp = content_;
      content_                                  = aContent;
      // registers all instantiations
      delete (tmp);
    }
  }


  // string representation of internal data about i in this.
  template < typename GUM_SCALAR >
  INLINE std::string MultiDimDecorator< GUM_SCALAR >::toString(const Instantiation* i) const {
    return content_->toString(i);
  }


  template < typename GUM_SCALAR >
  INLINE void MultiDimDecorator< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                        const DiscreteVariable* y) {
    this->content()->replace(*x, *y);
  }

  template < typename GUM_SCALAR >
  INLINE std::string MultiDimDecorator< GUM_SCALAR >::toString() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(content_)->empty()) {
      std::stringstream ss;
      ss << "<> :: " << empty_value_;
      return ss.str();
    } else {
      return content_->toString();
    }
  }

  //@todo force GUM_SCALAR to be double-castable (to be able to use fabs,etc.)
} /* namespace gum */
