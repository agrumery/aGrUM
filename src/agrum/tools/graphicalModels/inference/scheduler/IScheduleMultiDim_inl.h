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
  INLINE Idx IScheduleMultiDim::_newId_() { return ++_multidim_id_; }


  /// reset the ScheduleMultiDim ID generator to 0
  INLINE void IScheduleMultiDim::resetIdGenerator() { _multidim_id_ = Idx(0); }


  /// constructs a IScheduleMultiDim
  INLINE IScheduleMultiDim::IScheduleMultiDim(const Idx id) :
      id_(id == Idx(0) ? IScheduleMultiDim::_newId_() : id) {
    // make sure that we won't get several IScheduleMultiDim with the same Id
    if (id_ > _multidim_id_) _multidim_id_ = id_;

    // for debugging purposes
    GUM_CONSTRUCTOR(IScheduleMultiDim);
  }


  /// copy constructor
  INLINE IScheduleMultiDim::IScheduleMultiDim(const IScheduleMultiDim& from) : id_(from.id_) {
    // for debugging purposes
    GUM_CONS_CPY(IScheduleMultiDim);
  }


  /// move constructor
  INLINE IScheduleMultiDim::IScheduleMultiDim(IScheduleMultiDim&& from) : id_(from.id_) {
    // for debugging purposes
    GUM_CONS_MOV(IScheduleMultiDim);
  }


  /// destructor
  INLINE IScheduleMultiDim::~IScheduleMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(IScheduleMultiDim);
  }


  /// copy operator
  INLINE IScheduleMultiDim& IScheduleMultiDim::operator=(const IScheduleMultiDim& from) {
    id_ = from.id_;
    return *this;
  }


  /// move operator
  INLINE IScheduleMultiDim& IScheduleMultiDim::operator=(IScheduleMultiDim&& from) {
    id_ = from.id_;
    return *this;
  }


  /// operator ==
  INLINE bool IScheduleMultiDim::operator==(const IScheduleMultiDim& m) const {
    return id_ == m.id_;
  }


  /// operator !=
  INLINE bool IScheduleMultiDim::operator!=(const IScheduleMultiDim& m) const {
    return id_ != m.id_;
  }


  /// returns the id of the ScheduleMultiDim
  INLINE Idx IScheduleMultiDim::id() const { return id_; }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
