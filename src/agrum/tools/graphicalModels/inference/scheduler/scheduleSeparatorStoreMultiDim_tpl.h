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
 * @brief an operator used by scheduled inferences to store tables into
 *separators
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <sstream>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::ScheduleSeparatorStoreMultiDim(
     const ScheduleMultiDim< GUM_SCALAR >&                              table,
     ArcProperty< Set< const MultiDimImplementation< GUM_SCALAR >* > >& separator_tables,
     Arc                                                                separator) :
      ScheduleOperation< GUM_SCALAR >(
         ScheduleOperation< GUM_SCALAR >::Type::SEPARATOR_STORE_MULTIDIM),
      _table_(table), _tableSet_(&separator_tables), _separator_(separator), _args_(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleSeparatorStoreMultiDim);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::ScheduleSeparatorStoreMultiDim(
     const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& from) :
      ScheduleOperation< GUM_SCALAR >(from),
      _table_(from._table_), _tableSet_(from._tableSet_), _separator_(from._separator_), _args_(0) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleSeparatorStoreMultiDim);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleSeparatorStoreMultiDim< GUM_SCALAR >*
     ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::newFactory() const {
    return new ScheduleSeparatorStoreMultiDim< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::~ScheduleSeparatorStoreMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleSeparatorStoreMultiDim);

    if (_args_) delete _args_;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleSeparatorStoreMultiDim< GUM_SCALAR >&
     ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::operator=(
        const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (&from != this) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      _table_                                  = from._table_;
      _tableSet_                               = from._tableSet_;
      _separator_                              = from._separator_;

      if (_args_) {
        _args_->clear();
        _args_->insert(&_table_);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  bool ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::operator==(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& real_op
       = static_cast< const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& >(op);
    return ((_table_ == real_op._table_) && (_tableSet_ == real_op._tableSet_)
            && (_separator_ == real_op._separator_));
  }

  /// operator !=
  template < typename GUM_SCALAR >
  bool ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::operator!=(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return true;

    const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& real_op
       = static_cast< const ScheduleSeparatorStoreMultiDim< GUM_SCALAR >& >(op);
    return ((_table_ != real_op._table_) || (_tableSet_ != real_op._tableSet_)
            || (_separator_ != real_op._separator_));
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::execute() {
    const MultiDimImplementation< GUM_SCALAR >& multidim = _table_.multiDim();

    if (!_tableSet_->exists(_separator_)) {
      _tableSet_->insert(_separator_, Set< const MultiDimImplementation< GUM_SCALAR >* >());
    }

    _tableSet_->operator[](_separator_).insert(&multidim);
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::nbOperations() const {
    return 1.0f;
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  INLINE std::pair< long, long > ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::memoryUsage() const {
    return std::pair< long, long >(0, 0);
  }

  /// returns the multidim to be stored
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::multiDimArgs() const {
    if (!_args_) {
      _args_ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      _args_->insert(&_table_);
    }

    return *_args_;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::multiDimResults() const {
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
    }

#  endif /* [s] */
    return empty_seq;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleSeparatorStoreMultiDim< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << "store ( " << _table_.toString() << ", separator " << _separator_ << " )";
    return s.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
