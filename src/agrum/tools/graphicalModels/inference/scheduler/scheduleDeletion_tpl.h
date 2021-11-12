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
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >::ScheduleDeletion(
     const ScheduleMultiDim< TABLE, ALLOC >&                          table,
     const typename ScheduleDeletion< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(ScheduleOperationType::DELETE_MULTIDIM, true, alloc),
      _arg_(const_cast< ScheduleMultiDim< TABLE, ALLOC >* >(&table)) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleDeletion);
  }


  /// copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >::ScheduleDeletion(
     const ScheduleDeletion< TABLE, ALLOC >&                          from,
     const typename ScheduleDeletion< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(from, alloc),
      _arg_(from._arg_),
      _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleDeletion);
  }


  /// copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleDeletion< TABLE, ALLOC >::ScheduleDeletion(
     const ScheduleDeletion< TABLE, ALLOC >& from) :
      ScheduleDeletion< TABLE, ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >::ScheduleDeletion(
     ScheduleDeletion< TABLE, ALLOC >&&                               from,
     const typename ScheduleDeletion< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(std::move(from), alloc),
      _arg_(from._arg_),
      _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ = std::move(from._args_);

    // for debugging purposes
    GUM_CONS_MOV(ScheduleDeletion);
  }


  /// move constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleDeletion< TABLE, ALLOC >::ScheduleDeletion(
     ScheduleDeletion< TABLE, ALLOC >&& from) :
      ScheduleDeletion< TABLE, ALLOC >(std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >* ScheduleDeletion< TABLE, ALLOC >::clone(
     const typename ScheduleDeletion< TABLE, ALLOC >::allocator_type& alloc)
     const {
    ALLOC< ScheduleDeletion< TABLE, ALLOC > > allocator(alloc);
    ScheduleDeletion< TABLE, ALLOC >* new_del = allocator.allocate(1);
    try {
      new ((void*)new_del) ScheduleDeletion< TABLE, ALLOC >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_del, 1);
      throw;
    }

    return new_del;
  }


  /// virtual copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleDeletion< TABLE, ALLOC >*
         ScheduleDeletion< TABLE, ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >::~ScheduleDeletion() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleDeletion);
  }


  /// copy operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >& ScheduleDeletion< TABLE, ALLOC >::operator=(
     const ScheduleDeletion< TABLE, ALLOC >& from) {
    _arg_ = from._arg_;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_ = from._is_executed_;
    ScheduleOperation< ALLOC >::operator=(from);
    return *this;
  }


  /// move operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleDeletion< TABLE, ALLOC >& ScheduleDeletion< TABLE, ALLOC >::operator=(
     ScheduleDeletion< TABLE, ALLOC >&& from) {
    _arg_ = from._arg_;
    _args_ = std::move(from._args_);
    _is_executed_ = from._is_executed_;
    ScheduleOperation< ALLOC >::operator=(std::move(from));
    return *this;
  }


  /// operator ==
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::operator==(
     const ScheduleDeletion< TABLE, ALLOC >& op) const {
      return (*_arg_ == *op._arg_);
  }


  /// operator ==
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleDeletion< TABLE, ALLOC >::operator==(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleDeletion< TABLE, ALLOC >& real_op =
         dynamic_cast< const ScheduleDeletion< TABLE, ALLOC >& >(op);
      return ScheduleDeletion< TABLE, ALLOC >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::operator!=(
     const ScheduleDeletion< TABLE, ALLOC >& op) const {
    return !ScheduleDeletion< TABLE, ALLOC >::operator==(op);
  }


  /// operator !=
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::operator!=(
     const ScheduleOperation< ALLOC >& op) const {
    return !ScheduleDeletion< TABLE, ALLOC >::operator==(op);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::hasSimilarArguments(
     const ScheduleDeletion< TABLE, ALLOC >& op) const {
    return _arg_->hasSameVariables(*op._arg_);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleDeletion< TABLE, ALLOC >::hasSimilarArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleDeletion< TABLE, ALLOC >& real_op =
         dynamic_cast< const ScheduleDeletion< TABLE, ALLOC >& >(op);
      return ScheduleDeletion< TABLE, ALLOC >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::hasSameArguments(
     const ScheduleDeletion< TABLE, ALLOC >& op) const {
    return _arg_->hasSameVariables(*op._arg_) &&
           _arg_->hasSameContent(*op._arg_);
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleDeletion< TABLE, ALLOC >::hasSameArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleDeletion< TABLE, ALLOC >& real_op =
         dynamic_cast< const ScheduleDeletion< TABLE, ALLOC >& >(op);
      return ScheduleDeletion< TABLE, ALLOC >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::isSameOperation(
     const ScheduleDeletion< TABLE, ALLOC >& op) const {
    return true;
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleDeletion< TABLE, ALLOC >::isSameOperation(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleDeletion< TABLE, ALLOC >& real_op =
         dynamic_cast< const ScheduleDeletion< TABLE, ALLOC >& >(op);
      return ScheduleDeletion< TABLE, ALLOC >::isSameOperation(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the deletion
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const ScheduleMultiDim< TABLE, ALLOC >&
               ScheduleDeletion< TABLE, ALLOC >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
               ScheduleDeletion< TABLE, ALLOC >::args() const {
    return _args_;
  }


  /// returns the result of the combination
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
               ScheduleDeletion< TABLE, ALLOC >::results() const {
    return _results_;
  }


  /// modifies the arguments of the operation
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleDeletion< TABLE, ALLOC >::updateArgs(
     const Sequence< const IScheduleMultiDim< ALLOC >* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleDeletion::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE, ALLOC >* arg;
    try {
      arg = dynamic_cast< const ScheduleMultiDim< TABLE, ALLOC >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleDeletion::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }

    // save the new argument
    _arg_ = (ScheduleMultiDim< TABLE, ALLOC >*)arg;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_ = false;
  }


  /// indicates whether the operation has been executed
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleDeletion< TABLE, ALLOC >::isExecuted() const {
    return _is_executed_;
  }


  /// executes the operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE void ScheduleDeletion< TABLE, ALLOC >::execute() {
    _arg_->makeAbstract();
    _is_executed_ = true;
  }


  /// undo a previous execution, if any
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleDeletion< TABLE, ALLOC >::undo() {
    GUM_ERROR(OperationNotAllowed, "ScheduleDeletion cannot be undone.");
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename TABLE, template < typename > class ALLOC >
  INLINE double ScheduleDeletion< TABLE, ALLOC >::nbOperations() const {
    return 1.0;
  }


  /// returns the memory consumption used during the operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE std::pair< double, double >
         ScheduleDeletion< TABLE, ALLOC >::memoryUsage() const {
    const double size_table = double(_arg_->domainSize());
    return std::pair< double, double >(-size_table, -size_table);
  }


  /// displays the content of the operation
  template < typename TABLE, template < typename > class ALLOC >
  std::string ScheduleDeletion< TABLE, ALLOC >::toString() const {
    return "delete ( " + _arg_->toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
