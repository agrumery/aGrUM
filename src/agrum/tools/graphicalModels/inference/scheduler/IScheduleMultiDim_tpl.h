/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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
 * @brief The Table-agnostic base class of scheduleMultiDim
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <sstream>
#  include <string>

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim.h>


namespace gum {

  /// returns a new distinct ScheduleMultiDim ID
  template < template < typename > class ALLOC >
  INLINE Idx IScheduleMultiDim< ALLOC >::_newId_() {
    return ++ScheduleMultiDimSpace::_multidim_id_;
  }


  /// reset the ScheduleMultiDim ID generator to 0
  template < template < typename > class ALLOC >
  INLINE void IScheduleMultiDim< ALLOC >::resetIdGenerator() {
    ScheduleMultiDimSpace::_multidim_id_ = Idx(0);
  }


  /// return the allocator
  template < template < typename > class ALLOC >
  INLINE typename IScheduleMultiDim< ALLOC >::allocator_type
     IScheduleMultiDim< ALLOC >::get_allocator() const {
    return *this;
  }


  /// constructs a IScheduleMultiDim
  template < template < typename > class ALLOC >
  IScheduleMultiDim< ALLOC >::IScheduleMultiDim(
     const Idx                                                  id,
     const typename IScheduleMultiDim< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      id_(id == Idx(0) ? IScheduleMultiDim< ALLOC >::_newId_() : id) {
    // make sure that we won't get several IScheduleMultiDim with the same Id
    if (id_ > ScheduleMultiDimSpace::_multidim_id_)
      ScheduleMultiDimSpace::_multidim_id_ = id_;

    // for debugging purposes
    GUM_CONSTRUCTOR(IScheduleMultiDim);
  }


  /// copy constructor with a given allocator
  template < template < typename > class ALLOC >
  IScheduleMultiDim< ALLOC >::IScheduleMultiDim(
     const IScheduleMultiDim< ALLOC >&                          from,
     const typename IScheduleMultiDim< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      id_(from.id_) {
    // for debugging purposes
    GUM_CONS_CPY(IScheduleMultiDim);
  }


  /// copy constructor
  template < template < typename > class ALLOC >
  INLINE IScheduleMultiDim< ALLOC >::IScheduleMultiDim(
     const IScheduleMultiDim< ALLOC >& from) :
      IScheduleMultiDim< ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  IScheduleMultiDim< ALLOC >::IScheduleMultiDim(
     IScheduleMultiDim< ALLOC >&&                               from,
     const typename IScheduleMultiDim< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      id_(from.id_) {
    // for debugging purposes
    GUM_CONS_MOV(IScheduleMultiDim);
  }


  /// move constructor
  template < template < typename > class ALLOC >
  INLINE IScheduleMultiDim< ALLOC >::IScheduleMultiDim(
     IScheduleMultiDim< ALLOC >&& from) :
      IScheduleMultiDim< ALLOC >(std::move(from), from.get_allocator()) {}


  /// destructor
  template < template < typename > class ALLOC >
  IScheduleMultiDim< ALLOC >::~IScheduleMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(IScheduleMultiDim);
  }


  /// copy operator
  template < template < typename > class ALLOC >
  INLINE IScheduleMultiDim< ALLOC >& IScheduleMultiDim< ALLOC >::operator=(
     const IScheduleMultiDim< ALLOC >& from) {
    id_ = from.id_;
    return *this;
  }


  /// move operator
  template < template < typename > class ALLOC >
  INLINE IScheduleMultiDim< ALLOC >&
     IScheduleMultiDim< ALLOC >::operator=(IScheduleMultiDim< ALLOC >&& from)
        {
    id_ = from.id_;
    return *this;
  }


  /// operator ==
  template < template < typename > class ALLOC >
  INLINE bool IScheduleMultiDim< ALLOC >::operator==(
     const IScheduleMultiDim< ALLOC >&m) const {
    return id_ == m.id_;
  }


  /// operator !=
  template < template < typename > class ALLOC >
  INLINE bool IScheduleMultiDim< ALLOC >::operator!=(
     const IScheduleMultiDim< ALLOC >&m) const {
    return id_ != m.id_;
  }


  /// returns the id of the ScheduleMultiDim
  template < template < typename > class ALLOC >
  INLINE Idx IScheduleMultiDim< ALLOC >::id() const {
    return id_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
