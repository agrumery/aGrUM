
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <sstream>
#  include <string>

#  include <agrum/agrum.h>

#  include <agrum/tools/graphicalModels/inference/scheduleMultiDim.h>

namespace gum {

  /// returns a new distinct ID for each abtract scheduleMultiDim
  template < typename GUM_SCALAR >
  Idx ScheduleMultiDim< GUM_SCALAR >::__newId() {
    static Idx __abstract_id = 0;
    return ++__abstract_id;
  }

  /// returns a mapping from id to multidimImplementations
  template < typename GUM_SCALAR >
  HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >&
     ScheduleMultiDim< GUM_SCALAR >::__id2multidim() {
    static HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >
       __multidims;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&__multidims);
    }

#  endif /* GUM_DEBUG_MODE */
    return __multidims;
  }

  /// returns the id corresponding to a given multidim
  template < typename GUM_SCALAR >
  HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >&
     ScheduleMultiDim< GUM_SCALAR >::__multidim2id() {
    static HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >
       __multidim_ids;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&__multidim_ids);
    }

#  endif /* GUM_DEBUG_MODE */
    return __multidim_ids;
  }

  /// returns a table indicating how many ScheduleMultiDim have the same id
  template < typename GUM_SCALAR >
  HashTable< Idx, Idx >& ScheduleMultiDim< GUM_SCALAR >::__id2refs() {
    static HashTable< Idx, Idx > __ids;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&__ids);
    }

#  endif /* GUM_DEBUG_MODE */
    return __ids;
  }

  /// returns a table with the variables of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, const Sequence< const DiscreteVariable* >* >&
     ScheduleMultiDim< GUM_SCALAR >::__id2vars() {
    static HashTable< Idx, const Sequence< const DiscreteVariable* >* > __vars;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&__vars);
    }

#  endif /* GUM_DEBUG_MODE */
    return __vars;
  }

  /// returns a table with the domain size of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, Size >& ScheduleMultiDim< GUM_SCALAR >::__id2size() {
    static HashTable< Idx, Size > __size;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&__size);
    }

#  endif /* GUM_DEBUG_MODE */
    return __size;
  }

  /// constructs a ScheduleMultiDim containing an already built decorator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const MultiDimImplementation< GUM_SCALAR >& multidim) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // check whether the multidimImplementation has already been stored

    if (__multidim2id().exists(&multidim)) {
      // get the id already allocated to the implementation
      __id = __multidim2id().operator[](&multidim);

      // update the number of references of __id
      HashTable< Idx, Idx >& refs = __id2refs();
      ++refs[__id];
    } else {
      // assign a new id to the implementation
      __id = __newId();

      // store the multidim into the set of all mappings id<->multidim
      __id2multidim().insert(__id, &multidim);
      __multidim2id().insert(&multidim, __id);

      // indicate that the id is currently used once
      __id2refs().insert(__id, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars =
         new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      __id2vars().insert(__id, vars);

      // store the domain size of the multidim
      __id2size().insert(__id, multidim.domainSize());
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

    if (__multidim2id().exists(impl)) {
      // get the id already allocated to the implementation
      __id = __multidim2id().operator[](impl);

      // update the number of references of __id
      HashTable< Idx, Idx >& refs = __id2refs();
      ++refs[__id];
    } else {
      // assign a new id to the implementation
      __id = __newId();

      // store the multidim into the set of all mappings id<->multidim
      __id2multidim().insert(__id, impl);
      __multidim2id().insert(impl, __id);

      // indicate that the id is currently used once
      __id2refs().insert(__id, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars =
         new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      __id2vars().insert(__id, vars);

      // store the domain size of the multidim
      __id2size().insert(__id, multidim.domainSize());
    }
  }

  /// construct an (abstract) ScheduleMultiDim for a decorator yet to be built
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const Sequence< const DiscreteVariable* >& vars) :
      __id(__newId()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // indicate that the id is currently used once
    __id2refs().insert(__id, 1);

    // store the variables of the multidim
    const Sequence< const DiscreteVariable* >* new_vars =
       new Sequence< const DiscreteVariable* >(vars);
    __id2vars().insert(__id, new_vars);

    // compute and store the domain size
    Size size = 1;

    for (const auto var: *new_vars)
      size *= var->domainSize();

    __id2size().insert(__id, size);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const ScheduleMultiDim< GUM_SCALAR >& from) :
      __id(from.__id) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleMultiDim);

    // update the number of references of __id
    HashTable< Idx, Idx >& refs = __id2refs();
    ++refs[__id];
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::~ScheduleMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleMultiDim);

    // get the number of scheduleMultiDims that reference the same id
    HashTable< Idx, Idx >& refs = __id2refs();
    Idx&                   nb_refs = refs[__id];

    if (nb_refs == 1) {
      // remove the id and the multidimdecorator from the mappings
      if (__id2multidim().exists(__id)) {
        __multidim2id().erase(__id2multidim().operator[](__id));
        __id2multidim().erase(__id);
      }

      refs.erase(__id);

      delete __id2vars().operator[](__id);
      __id2vars().erase(__id);
      __id2size().erase(__id);
    } else
      --nb_refs;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >& ScheduleMultiDim< GUM_SCALAR >::operator=(
     const ScheduleMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (__id != from.__id) {
      // remove the old id
      HashTable< Idx, Idx >& refs = __id2refs();
      Idx&                   nb_refs = refs[__id];

      if (nb_refs == 1) {
        // remove the id and the multidimdecorator from the mappings
        if (__id2multidim().exists(__id)) {
          __multidim2id().erase(__id2multidim().operator[](__id));
          __id2multidim().erase(__id);
        }

        refs.erase(__id);

        delete __id2vars().operator[](__id);
        __id2vars().erase(__id);
        __id2size().erase(__id);
      } else
        --nb_refs;

      __id = from.__id;

      ++refs[__id];
    }

    return *this;
  }

  /// checks whether two ScheduleMultiDim are related to the same table
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::operator==(
     const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (__id == m.__id);
  }

  /// checks whether two ScheduleMultiDim are related to different tables
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::operator!=(
     const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (__id != m.__id);
  }

  /// returns the multiDimImplementation actually contained in the
  /// ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >&
               ScheduleMultiDim< GUM_SCALAR >::multiDim() const {
    return *(__id2multidim().operator[](__id));
  }

  /// returns whether the ScheduleMultiDim contains a real
  /// multiDimImplementation
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::isAbstract() const {
    return !__id2multidim().exists(__id);
  }

  /// returns the id of the ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE Idx ScheduleMultiDim< GUM_SCALAR >::id() const {
    return __id;
  }

  /// returns the set of variables involved in the multidim
  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               ScheduleMultiDim< GUM_SCALAR >::variablesSequence() const {
    return *(__id2vars().operator[](__id));
  }

  /// returns the domain size of the multidim
  template < typename GUM_SCALAR >
  INLINE Size ScheduleMultiDim< GUM_SCALAR >::domainSize() const {
    return __id2size().operator[](__id);
  }

  /// sets a new multiDimImplementation inside the wrapper
  template < typename GUM_SCALAR >
  void ScheduleMultiDim< GUM_SCALAR >::setMultiDim(
     const MultiDimImplementation< GUM_SCALAR >& m) {
    // check whether the same Multidimimplementation has not already been
    // wrapped with another ID. In this case, we shall throw a DuplicateElement
    // exception
    if (__multidim2id().exists(&m)) {
      if (__multidim2id().operator[](&m) != __id) {
        GUM_ERROR(DuplicateElement,
                  "the multidim table has already been "
                  "wrapped into a ScheduleMultidim with another id");
      }
    } else {
      // here, the multidim has never been stored into a ScheduleMultiDim

      // if the current ScheduleMultiDim already contained a
      // multidimImplementation
      // we should remove it first and, then, add the new one
      if (__id2multidim().exists(__id)) {
        const MultiDimImplementation< GUM_SCALAR >* impl =
           __id2multidim().                         operator[](__id);

        if (impl == &m) return;

        __multidim2id().erase(impl);

        __id2multidim().operator[](__id) = &m;
      } else {
        // store the new multidim
        __id2multidim().insert(__id, &m);
      }

      __multidim2id().insert(&m, __id);

      // update the variables of the scheduleMultiDim
      const Sequence< const DiscreteVariable* >& m_vars = m.variablesSequence();
      Sequence< const DiscreteVariable* >*       vars =
         const_cast< Sequence< const DiscreteVariable* >* >(
            __id2vars().operator[](__id));
      *vars = m_vars;

      // update the size of the wrapper
      __id2size().operator[](__id) = m.domainSize();
    }
  }

  /// sets a new multiDimImplementation inside the wrapper
  template < typename GUM_SCALAR >
  INLINE void ScheduleMultiDim< GUM_SCALAR >::setMultiDim(
     const MultiDimDecorator< GUM_SCALAR >& m) {
    setMultiDim(*(m.content()));
  }

  /// displays the content of the multidim
  template < typename GUM_SCALAR >
  std::string ScheduleMultiDim< GUM_SCALAR >::toString() const {
    std::stringstream str;
    str << "<";

    // get the pointer of the multidim if it exists

    try {
      str << __id2multidim().operator[](__id);
    } catch (NotFound&) { str << __id; }

    str << ">";

    return str.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */