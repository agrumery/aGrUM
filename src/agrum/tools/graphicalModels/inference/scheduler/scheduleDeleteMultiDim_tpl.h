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
 * @brief a MultiDim Delete operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

// to ease ide parser
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeleteMultiDim.h>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::ScheduleDeleteMultiDim(
     const ScheduleMultiDim< GUM_SCALAR >& table) :
      ScheduleOperation< GUM_SCALAR >(ScheduleOperation< GUM_SCALAR >::Type::DELETE_MULTIDIM),
      _table_(table), _args_(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleDeleteMultiDim);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::ScheduleDeleteMultiDim(
     const ScheduleDeleteMultiDim< GUM_SCALAR >& from) :
      ScheduleOperation< GUM_SCALAR >(from),
      _table_(from._table_), _args_(0) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleDeleteMultiDim);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >* ScheduleDeleteMultiDim< GUM_SCALAR >::newFactory() const {
    return new ScheduleDeleteMultiDim< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::~ScheduleDeleteMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleDeleteMultiDim);

    if (_args_) delete _args_;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >& ScheduleDeleteMultiDim< GUM_SCALAR >::operator=(
     const ScheduleDeleteMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (&from != this) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      _table_                                  = from._table_;

      if (_args_) {
        _args_->clear();
        _args_->insert(&_table_);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  bool ScheduleDeleteMultiDim< GUM_SCALAR >::operator==(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleDeleteMultiDim< GUM_SCALAR >& real_op
       = static_cast< const ScheduleDeleteMultiDim< GUM_SCALAR >& >(op);
    return _table_ == real_op._table_;
  }

  /// operator !=
  template < typename GUM_SCALAR >
  bool ScheduleDeleteMultiDim< GUM_SCALAR >::operator!=(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return true;

    const ScheduleDeleteMultiDim< GUM_SCALAR >& real_op
       = static_cast< const ScheduleDeleteMultiDim< GUM_SCALAR >& >(op);
    return _table_ != real_op._table_;
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleDeleteMultiDim< GUM_SCALAR >::execute() {
    const MultiDimImplementation< GUM_SCALAR >& multidim = _table_.multiDim();
    ScheduleMultiDim< GUM_SCALAR >::_multidim2id_().erase(&multidim);
    ScheduleMultiDim< GUM_SCALAR >::_id2multidim_().erase(_table_.id());
    delete &multidim;
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float ScheduleDeleteMultiDim< GUM_SCALAR >::nbOperations() const {
    return 1.0f;
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  INLINE std::pair< long, long > ScheduleDeleteMultiDim< GUM_SCALAR >::memoryUsage() const {
    long size_table = long(_table_.domainSize());

    if (size_table < 0) { GUM_ERROR(OutOfBounds, "memory usage out of long int range") }

    return std::pair< long, long >(-size_table, -size_table);
  }

  /// returns the multidims to be deleted
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleDeleteMultiDim< GUM_SCALAR >::multiDimArgs() const {
    if (!_args_) {
      _args_ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      _args_->insert(&_table_);
    }

    return *_args_;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleDeleteMultiDim< GUM_SCALAR >::multiDimResults() const {
    static Sequence< const ScheduleMultiDim< GUM_SCALAR >* > empty_seq;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static sequence used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::_inc_deletion_("Sequence", __FILE__, __LINE__, "destructor of", (void*)&empty_seq);
      __debug__::_inc_deletion_("SequenceImplementation",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
      __debug__::_inc_deletion_("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
      __debug__::_inc_deletion_("SequenceIteratorSafe",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
      __debug__::_inc_deletion_("SequenceIteratorSafe",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
    }

#  endif /* GUM_DEBUG_MODE */
    return empty_seq;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleDeleteMultiDim< GUM_SCALAR >::toString() const {
    return "delete ( " + _table_.toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
