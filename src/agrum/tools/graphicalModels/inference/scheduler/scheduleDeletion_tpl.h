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
 * @brief a MultiDim Delete operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeletion.h>

namespace gum {


  /// default constructor
  template < typename TABLE >
  ScheduleDeletion< TABLE >::ScheduleDeletion(const ScheduleMultiDim< TABLE >& table) :
      ScheduleOperation(ScheduleOperationType::DELETE_MULTIDIM, true, false),
      _arg_(const_cast< ScheduleMultiDim< TABLE >* >(&table)) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleDeletion);
  }


  /// copy constructor
  template < typename TABLE >
  ScheduleDeletion< TABLE >::ScheduleDeletion(const ScheduleDeletion< TABLE >& from) :
      ScheduleOperation(from), _arg_(from._arg_), _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleDeletion);
  }


  /// move constructor
  template < typename TABLE >
  ScheduleDeletion< TABLE >::ScheduleDeletion(ScheduleDeletion< TABLE >&& from) :
      ScheduleOperation(std::move(from)), _arg_(from._arg_), _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ = std::move(from._args_);

    // for debugging purposes
    GUM_CONS_MOV(ScheduleDeletion);
  }


  /// virtual copy constructor
  template < typename TABLE >
  INLINE ScheduleDeletion< TABLE >* ScheduleDeletion< TABLE >::clone() const {
    return new ScheduleDeletion< TABLE >(*this);
  }


  /// destructor
  template < typename TABLE >
  ScheduleDeletion< TABLE >::~ScheduleDeletion() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleDeletion);
  }


  /// copy operator
  template < typename TABLE >
  ScheduleDeletion< TABLE >&
     ScheduleDeletion< TABLE >::operator=(const ScheduleDeletion< TABLE >& from) {
    _arg_ = from._arg_;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_              = from._is_executed_;
    ScheduleOperation::operator=(from);
    return *this;
  }


  /// move operator
  template < typename TABLE >
  ScheduleDeletion< TABLE >&
     ScheduleDeletion< TABLE >::operator=(ScheduleDeletion< TABLE >&& from) {
    _arg_                      = from._arg_;
    _args_                     = std::move(from._args_);
    _is_executed_              = from._is_executed_;
    ScheduleOperation::operator=(std::move(from));
    return *this;
  }


  /// operator ==
  template < typename TABLE >
  INLINE bool ScheduleDeletion< TABLE >::operator==(const ScheduleDeletion< TABLE >& op) const {
    return (*_arg_ == *op._arg_);
  }


  /// operator ==
  template < typename TABLE >
  bool ScheduleDeletion< TABLE >::operator==(const ScheduleOperation& op) const {
    try {
      const ScheduleDeletion< TABLE >& real_op
         = dynamic_cast< const ScheduleDeletion< TABLE >& >(op);
      return ScheduleDeletion< TABLE >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE >
  INLINE bool ScheduleDeletion< TABLE >::operator!=(const ScheduleDeletion< TABLE >& op) const {
    return !ScheduleDeletion< TABLE >::operator==(op);
  }


  /// operator !=
  template < typename TABLE >
  INLINE bool ScheduleDeletion< TABLE >::operator!=(const ScheduleOperation& op) const {
    return !ScheduleDeletion< TABLE >::operator==(op);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE >
  INLINE bool
     ScheduleDeletion< TABLE >::hasSimilarArguments(const ScheduleDeletion< TABLE >& op) const {
    return _arg_->hasSameVariables(*op._arg_);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE >
  bool ScheduleDeletion< TABLE >::hasSimilarArguments(const ScheduleOperation& op) const {
    try {
      const ScheduleDeletion< TABLE >& real_op
         = dynamic_cast< const ScheduleDeletion< TABLE >& >(op);
      return ScheduleDeletion< TABLE >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE >
  INLINE bool
     ScheduleDeletion< TABLE >::hasSameArguments(const ScheduleDeletion< TABLE >& op) const {
    return _arg_->hasSameVariables(*op._arg_) && _arg_->hasSameContent(*op._arg_);
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE >
  bool ScheduleDeletion< TABLE >::hasSameArguments(const ScheduleOperation& op) const {
    try {
      const ScheduleDeletion< TABLE >& real_op
         = dynamic_cast< const ScheduleDeletion< TABLE >& >(op);
      return ScheduleDeletion< TABLE >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE >
  INLINE bool
     ScheduleDeletion< TABLE >::isSameOperation(const ScheduleDeletion< TABLE >& op) const {
    return true;
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE >
  bool ScheduleDeletion< TABLE >::isSameOperation(const ScheduleOperation& op) const {
    try {
      const ScheduleDeletion< TABLE >& real_op
         = dynamic_cast< const ScheduleDeletion< TABLE >& >(op);
      return ScheduleDeletion< TABLE >::isSameOperation(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the deletion
  template < typename TABLE >
  INLINE const ScheduleMultiDim< TABLE >& ScheduleDeletion< TABLE >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operation
  template < typename TABLE >
  INLINE const Sequence< const IScheduleMultiDim* >& ScheduleDeletion< TABLE >::args() const {
    return _args_;
  }


  /// returns the result of the combination
  template < typename TABLE >
  INLINE const Sequence< const IScheduleMultiDim* >& ScheduleDeletion< TABLE >::results() const {
    return _results_;
  }


  /// modifies the arguments of the operation
  template < typename TABLE >
  void ScheduleDeletion< TABLE >::updateArgs(const Sequence< const IScheduleMultiDim* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleDeletion::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE >* arg;
    try {
      arg = dynamic_cast< const ScheduleMultiDim< TABLE >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleDeletion::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }

    // save the new argument
    _arg_ = (ScheduleMultiDim< TABLE >*)arg;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_ = false;
  }


  /// indicates whether the operation has been executed
  template < typename TABLE >
  INLINE bool ScheduleDeletion< TABLE >::isExecuted() const {
    return _is_executed_;
  }


  /// executes the operation
  template < typename TABLE >
  INLINE void ScheduleDeletion< TABLE >::execute() {
    _arg_->makeAbstract();
    _is_executed_ = true;
  }


  /// undo a previous execution, if any
  template < typename TABLE >
  void ScheduleDeletion< TABLE >::undo() {
    GUM_ERROR(OperationNotAllowed, "ScheduleDeletion cannot be undone.");
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename TABLE >
  INLINE double ScheduleDeletion< TABLE >::nbOperations() const {
    return 1.0;
  }


  /// returns the memory consumption used during the operation
  template < typename TABLE >
  INLINE std::pair< double, double > ScheduleDeletion< TABLE >::memoryUsage() const {
    const double size_table = double(_arg_->domainSize()) * _arg_->sizeOfContent() + sizeof(TABLE);
    return {-size_table, -size_table};
  }


  /// displays the content of the operation
  template < typename TABLE >
  std::string ScheduleDeletion< TABLE >::toString() const {
    return "delete ( " + _arg_->toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
