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
  Idx ScheduleMultiDim< GUM_SCALAR >::newId__() {
    static Idx abstract_id__ = 0;
    return ++abstract_id__;
  }

  /// returns a mapping from id to multidimImplementations
  template < typename GUM_SCALAR >
  HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >&
     ScheduleMultiDim< GUM_SCALAR >::id2multidim__() {
    static HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >
       multidims__;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&multidims__);
    }

#  endif /* GUM_DEBUG_MODE */
    return multidims__;
  }

  /// returns the id corresponding to a given multidim
  template < typename GUM_SCALAR >
  HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >&
     ScheduleMultiDim< GUM_SCALAR >::multidim2id__() {
    static HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >
       multidim_ids__;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&multidim_ids__);
    }

#  endif /* GUM_DEBUG_MODE */
    return multidim_ids__;
  }

  /// returns a table indicating how many ScheduleMultiDim have the same id
  template < typename GUM_SCALAR >
  HashTable< Idx, Idx >& ScheduleMultiDim< GUM_SCALAR >::id2refs__() {
    static HashTable< Idx, Idx > ids__;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&ids__);
    }

#  endif /* GUM_DEBUG_MODE */
    return ids__;
  }

  /// returns a table with the variables of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, const Sequence< const DiscreteVariable* >* >&
     ScheduleMultiDim< GUM_SCALAR >::id2vars__() {
    static HashTable< Idx, const Sequence< const DiscreteVariable* >* > vars__;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&vars__);
    }

#  endif /* GUM_DEBUG_MODE */
    return vars__;
  }

  /// returns a table with the domain size of the table corresponding to id
  template < typename GUM_SCALAR >
  HashTable< Idx, Size >& ScheduleMultiDim< GUM_SCALAR >::id2size__() {
    static HashTable< Idx, Size > size__;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static hashtable used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&size__);
    }

#  endif /* GUM_DEBUG_MODE */
    return size__;
  }

  /// constructs a ScheduleMultiDim containing an already built decorator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const MultiDimImplementation< GUM_SCALAR >& multidim) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // check whether the multidimImplementation has already been stored

    if (multidim2id__().exists(&multidim)) {
      // get the id already allocated to the implementation
      id__ = multidim2id__().operator[](&multidim);

      // update the number of references of id__
      HashTable< Idx, Idx >& refs = id2refs__();
      ++refs[id__];
    } else {
      // assign a new id to the implementation
      id__ = newId__();

      // store the multidim into the set of all mappings id<->multidim
      id2multidim__().insert(id__, &multidim);
      multidim2id__().insert(&multidim, id__);

      // indicate that the id is currently used once
      id2refs__().insert(id__, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars
         = new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      id2vars__().insert(id__, vars);

      // store the domain size of the multidim
      id2size__().insert(id__, multidim.domainSize());
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

    if (multidim2id__().exists(impl)) {
      // get the id already allocated to the implementation
      id__ = multidim2id__().operator[](impl);

      // update the number of references of id__
      HashTable< Idx, Idx >& refs = id2refs__();
      ++refs[id__];
    } else {
      // assign a new id to the implementation
      id__ = newId__();

      // store the multidim into the set of all mappings id<->multidim
      id2multidim__().insert(id__, impl);
      multidim2id__().insert(impl, id__);

      // indicate that the id is currently used once
      id2refs__().insert(id__, 1);

      // store the variables of the multidim
      const Sequence< const DiscreteVariable* >* vars
         = new Sequence< const DiscreteVariable* >(multidim.variablesSequence());
      id2vars__().insert(id__, vars);

      // store the domain size of the multidim
      id2size__().insert(id__, multidim.domainSize());
    }
  }

  /// construct an (abstract) ScheduleMultiDim for a decorator yet to be built
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const Sequence< const DiscreteVariable* >& vars) :
      id__(newId__()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);

    // indicate that the id is currently used once
    id2refs__().insert(id__, 1);

    // store the variables of the multidim
    const Sequence< const DiscreteVariable* >* new_vars
       = new Sequence< const DiscreteVariable* >(vars);
    id2vars__().insert(id__, new_vars);

    // compute and store the domain size
    Size size = 1;

    for (const auto var: *new_vars)
      size *= var->domainSize();

    id2size__().insert(id__, size);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::ScheduleMultiDim(
     const ScheduleMultiDim< GUM_SCALAR >& from) :
      id__(from.id__) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleMultiDim);

    // update the number of references of id__
    HashTable< Idx, Idx >& refs = id2refs__();
    ++refs[id__];
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >::~ScheduleMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleMultiDim);

    // get the number of scheduleMultiDims that reference the same id
    HashTable< Idx, Idx >& refs    = id2refs__();
    Idx&                   nb_refs = refs[id__];

    if (nb_refs == 1) {
      // remove the id and the multidimdecorator from the mappings
      if (id2multidim__().exists(id__)) {
        multidim2id__().erase(id2multidim__().operator[](id__));
        id2multidim__().erase(id__);
      }

      refs.erase(id__);

      delete id2vars__().operator[](id__);
      id2vars__().erase(id__);
      id2size__().erase(id__);
    } else
      --nb_refs;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR >& ScheduleMultiDim< GUM_SCALAR >::operator=(
     const ScheduleMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (id__ != from.id__) {
      // remove the old id
      HashTable< Idx, Idx >& refs    = id2refs__();
      Idx&                   nb_refs = refs[id__];

      if (nb_refs == 1) {
        // remove the id and the multidimdecorator from the mappings
        if (id2multidim__().exists(id__)) {
          multidim2id__().erase(id2multidim__().operator[](id__));
          id2multidim__().erase(id__);
        }

        refs.erase(id__);

        delete id2vars__().operator[](id__);
        id2vars__().erase(id__);
        id2size__().erase(id__);
      } else
        --nb_refs;

      id__ = from.id__;

      ++refs[id__];
    }

    return *this;
  }

  /// checks whether two ScheduleMultiDim are related to the same table
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::operator==(
     const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (id__ == m.id__);
  }

  /// checks whether two ScheduleMultiDim are related to different tables
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::operator!=(
     const ScheduleMultiDim< GUM_SCALAR >& m) const {
    return (id__ != m.id__);
  }

  /// returns the multiDimImplementation actually contained in the
  /// ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >&
               ScheduleMultiDim< GUM_SCALAR >::multiDim() const {
    return *(id2multidim__().operator[](id__));
  }

  /// returns whether the ScheduleMultiDim contains a real
  /// multiDimImplementation
  template < typename GUM_SCALAR >
  INLINE bool ScheduleMultiDim< GUM_SCALAR >::isAbstract() const {
    return !id2multidim__().exists(id__);
  }

  /// returns the id of the ScheduleMultiDim
  template < typename GUM_SCALAR >
  INLINE Idx ScheduleMultiDim< GUM_SCALAR >::id() const {
    return id__;
  }

  /// returns the set of variables involved in the multidim
  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               ScheduleMultiDim< GUM_SCALAR >::variablesSequence() const {
    return *(id2vars__().operator[](id__));
  }

  /// returns the domain size of the multidim
  template < typename GUM_SCALAR >
  INLINE Size ScheduleMultiDim< GUM_SCALAR >::domainSize() const {
    return id2size__().operator[](id__);
  }

  /// sets a new multiDimImplementation inside the wrapper
  template < typename GUM_SCALAR >
  void ScheduleMultiDim< GUM_SCALAR >::setMultiDim(
     const MultiDimImplementation< GUM_SCALAR >& m) {
    // check whether the same Multidimimplementation has not already been
    // wrapped with another ID. In this case, we shall throw a DuplicateElement
    // exception
    if (multidim2id__().exists(&m)) {
      if (multidim2id__().operator[](&m) != id__) {
        GUM_ERROR(DuplicateElement,
                  "the multidim table has already been "
                  "wrapped into a ScheduleMultidim with another id");
      }
    } else {
      // here, the multidim has never been stored into a ScheduleMultiDim

      // if the current ScheduleMultiDim already contained a
      // multidimImplementation
      // we should remove it first and, then, add the new one
      if (id2multidim__().exists(id__)) {
        const MultiDimImplementation< GUM_SCALAR >* impl
           = id2multidim__().                       operator[](id__);

        if (impl == &m) return;

        multidim2id__().erase(impl);

        id2multidim__().operator[](id__) = &m;
      } else {
        // store the new multidim
        id2multidim__().insert(id__, &m);
      }

      multidim2id__().insert(&m, id__);

      // update the variables of the scheduleMultiDim
      const Sequence< const DiscreteVariable* >& m_vars = m.variablesSequence();
      Sequence< const DiscreteVariable* >*       vars
         = const_cast< Sequence< const DiscreteVariable* >* >(
            id2vars__().operator[](id__));
      *vars = m_vars;

      // update the size of the wrapper
      id2size__().operator[](id__) = m.domainSize();
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
      str << id2multidim__().operator[](id__);
    } catch (NotFound&) { str << id__; }

    str << ">";

    return str.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
