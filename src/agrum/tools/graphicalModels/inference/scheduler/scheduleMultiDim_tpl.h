/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief a MultiDimImplementation Wrapper used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <sstream>
#  include <string>

#  include <agrum/agrum.h>

#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>

namespace gum {

  /// returns a new distinct ID for each abtract scheduleMultiDim
  template < typename GUM_SCALAR >
  Idx ScheduleMultiDim< GUM_SCALAR >::_newId_() {
    static Idx _abstract_id_ = 0;
    return ++_abstract_id_;
  }

  /// returns a mapping from id to multidimImplementations
  template < typename GUM_SCALAR >
  HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >&
     ScheduleMultiDim< GUM_SCALAR >::_id2multidim_() {
    static HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* > _multidims_;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&_multidims_);
    }

#  endif /* GUM_DEBUG_MODE */
    return _multidims_;
  }

  /// returns the id corresponding to a given multidim
  template < typename GUM_SCALAR >
  HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >&
     ScheduleMultiDim< GUM_SCALAR >::_multidim2id_() {
    static HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx > _multidim_ids_;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&_multidim_ids_);
    }

#  endif /* GUM_DEBUG_MODE */
    return _multidim_ids_;
  }

  /// returns a table indicating how many ScheduleMultiDim have the same id
  template < typename GUM_SCALAR >
  HashTable< Idx, Idx >& ScheduleMultiDim< GUM_SCALAR >::_id2refs_() {
    static HashTable< Idx, Idx > _ids_;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("HashTable", __FILE__, __LINE__, "destructor of", (void*)&_ids_);
    }

#  endif /* GUM_DEBUG_MODE */
    return _ids_;
  }

  /// returns a table with the variables of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, const Sequence< const DiscreteVariable* >* >&
     ScheduleMultiDim< GUM_SCALAR >::_id2vars_() {
    static HashTable< Idx, const Sequence< const DiscreteVariable* >* > _vars_;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("HashTable", __FILE__, __LINE__, "destructor of", (void*)&_vars_);
    }

#  endif /* GUM_DEBUG_MODE */
    return _vars_;
  }

  /// returns a table with the domain size of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, Size >& ScheduleMultiDim< GUM_SCALAR >::_id2size_() {
    static HashTable< Idx, Size > _size_;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("HashTable", __FILE__, __LINE__, "destructor of", (void*)&_size_);
    }

#  endif /* GUM_DEBUG_MODE */
    return _size_;
  }

  /// constructs a ScheduleMultiDim containing an already built decorator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const MultiDimImplementation< GUM_SCALAR >& multidim) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // check whether the multidimImplementation has already been stored

    if (_multidim2id_().exists(&multidim)) {
      // get the id already allocated to the implementation
      _id_ = _multidim2id_().operator[](&multidim);

      // update the number of references of  _id_
      HashTable< Idx, Idx >& refs = _id2refs_();
      ++refs[_id_];
    } else {
      // assign a new id to the implementation
      _id_ = _newId_();

      // store the multidim into the set of all mappings id<->multidim
      _id2multidim_().insert(_id_, &multidim);
      _multidim2id_().insert(&multidim, _id_);

      // indicate that the id is currently used once
      _id2refs_().insert(_id_, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars
         = new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      _id2vars_().insert(_id_, vars);

      // store the domain size of the multidim
      _id2size_().insert(_id_, multidim.domainSize());
    }
  }

  /// constructs a ScheduleMultiDim containing an already built decorator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const MultiDimDecorator< GUM_SCALAR >& multidim) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    const MultiDimImplementation< GUM_SCALAR >* impl = multidim.content();

    // check whether the multidimImplementation has already been stored

    if (_multidim2id_().exists(impl)) {
      // get the id already allocated to the implementation
      _id_ = _multidim2id_().operator[](impl);

      // update the number of references of  _id_
      HashTable< Idx, Idx >& refs = _id2refs_();
      ++refs[_id_];
    } else {
      // assign a new id to the implementation
      _id_ = _newId_();

      // store the multidim into the set of all mappings id<->multidim
      _id2multidim_().insert(_id_, impl);
      _multidim2id_().insert(impl, _id_);

      // indicate that the id is currently used once
      _id2refs_().insert(_id_, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars
         = new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      _id2vars_().insert(_id_, vars);

      // store the domain size of the multidim
      _id2size_().insert(_id_, multidim.domainSize());
    }
  }

  /// construct an (abstract) ScheduleMultiDim for a decorator yet to be built
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const Sequence< const DiscreteVariable* >& vars) :
      _id_(_newId_()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // indicate that the id is currently used once
    _id2refs_().insert(_id_, 1);

    // store the variables of the multidim
    const Sequence< const DiscreteVariable* >* new_vars
       = new Sequence< const DiscreteVariable* >(vars);
    _id2vars_().insert(_id_, new_vars);

    // compute and store the domain size
    Size size = 1;

    for (const auto var: *new_vars)
      size *= var->domainSize();

    _id2size_().insert(_id_, size);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(const ScheduleMultiDim< GUM_SCALAR >& from) :
      _id_(from._id_) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleMultiDim);

    // update the number of references of  _id_
    HashTable< Idx, Idx >& refs = _id2refs_();
    ++refs[_id_];
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::~ScheduleMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleMultiDim);

    // get the number of scheduleMultiDims that reference the same id
    HashTable< Idx, Idx >& refs    = _id2refs_();
    Idx&                   nb_refs = refs[_id_];

    if (nb_refs == 1) {
      // remove the id and the multidimdecorator from the mappings
      if (_id2multidim_().exists(_id_)) {
        _multidim2id_().erase(_id2multidim_().operator[](_id_));
        _id2multidim_().erase(_id_);
      }

      refs.erase(_id_);

      delete _id2vars_().operator[](_id_);
      _id2vars_().erase(_id_);
      _id2size_().erase(_id_);
    } else
      --nb_refs;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >&
     ScheduleMultiDim< GUM_SCALAR >::operator=(const ScheduleMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (_id_ != from._id_) {
      // remove the old id
      HashTable< Idx, Idx >& refs    = _id2refs_();
      Idx&                   nb_refs = refs[_id_];

      if (nb_refs == 1) {
        // remove the id and the multidimdecorator from the mappings
        if (_id2multidim_().exists(_id_)) {
          _multidim2id_().erase(_id2multidim_().operator[](_id_));
          _id2multidim_().erase(_id_);
        }

        refs.erase(_id_);

        delete _id2vars_().operator[](_id_);
        _id2vars_().erase(_id_);
        _id2size_().erase(_id_);
      } else
        --nb_refs;

      _id_ = from._id_;

      ++refs[_id_];
    }

    return *this;
  }

  /// checks whether two ScheduleMultiDim are related to the same table
  template < typename GUM_SCALAR >
  INLINE bool
     ScheduleMultiDim< GUM_SCALAR >::operator==(const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (_id_ == m._id_);
  }

  /// checks whether two ScheduleMultiDim are related to different tables
  template < typename GUM_SCALAR >
  INLINE bool
     ScheduleMultiDim< GUM_SCALAR >::operator!=(const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (_id_ != m._id_);
  }

  /// returns the multiDimImplementation actually contained in the
  /// ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >&
               ScheduleMultiDim< GUM_SCALAR >::multiDim() const {
    return *(_id2multidim_().operator[](_id_));
  }

  /// returns whether the ScheduleMultiDim contains a real
  /// multiDimImplementation
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::isAbstract() const {
    return !_id2multidim_().exists(_id_);
  }

  /// returns the id of the ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE Idx ScheduleMultiDim< GUM_SCALAR >::id() const {
    return _id_;
  }

  /// returns the set of variables involved in the multidim
  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               ScheduleMultiDim< GUM_SCALAR >::variablesSequence() const {
    return *(_id2vars_().operator[](_id_));
  }

  /// returns the domain size of the multidim
  template < typename GUM_SCALAR >
  INLINE Size ScheduleMultiDim< GUM_SCALAR >::domainSize() const {
    return _id2size_().operator[](_id_);
  }

  /// sets a new multiDimImplementation inside the wrapper
  template < typename GUM_SCALAR >
  void ScheduleMultiDim< GUM_SCALAR >::setMultiDim(const MultiDimImplementation< GUM_SCALAR >& m) {
    // check whether the same Multidimimplementation has not already been
    // wrapped with another ID. In this case, we shall throw a DuplicateElement
    // exception
    if (_multidim2id_().exists(&m)) {
      if (_multidim2id_().operator[](&m) != _id_) {
        GUM_ERROR(DuplicateElement,
                  "the multidim table has already been "
                  "wrapped into a ScheduleMultidim with another id");
      }
    } else {
      // here, the multidim has never been stored into a ScheduleMultiDim

      // if the current ScheduleMultiDim already contained a
      // multidimImplementation
      // we should remove it first and, then, add the new one
      if (_id2multidim_().exists(_id_)) {
        const MultiDimImplementation< GUM_SCALAR >* impl = _id2multidim_().operator[](_id_);

        if (impl == &m) return;

        _multidim2id_().erase(impl);

        _id2multidim_().operator[](_id_) = &m;
      } else {
        // store the new multidim
        _id2multidim_().insert(_id_, &m);
      }

      _multidim2id_().insert(&m, _id_);

      // update the variables of the scheduleMultiDim
      const Sequence< const DiscreteVariable* >& m_vars = m.variablesSequence();
      Sequence< const DiscreteVariable* >*       vars
         = const_cast< Sequence< const DiscreteVariable* >* >(_id2vars_().operator[](_id_));
      *vars = m_vars;

      // update the size of the wrapper
      _id2size_().operator[](_id_) = m.domainSize();
    }
  }

  /// sets a new multiDimImplementation inside the wrapper
  template < typename GUM_SCALAR >
  INLINE void
     ScheduleMultiDim< GUM_SCALAR >::setMultiDim(const MultiDimDecorator< GUM_SCALAR >& m) {
    setMultiDim(*(m.content()));
  }

  /// displays the content of the multidim
  template < typename GUM_SCALAR >
  std::string ScheduleMultiDim< GUM_SCALAR >::toString() const {
    std::stringstream str;
    str << "<";

    // get the pointer of the multidim if it exists

    try {
      str << _id2multidim_().operator[](_id_);
    } catch (NotFound&) { str << _id_; }

    str << ">";

    return str.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
