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
 * @brief Base class for storing multidimensional tables in scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleStorage.h>

namespace gum {

  namespace ScheduleStorageMethod {

    // storing tables into a Set<Table*>
    template < typename TABLE, typename... CONTAINER_PARAMS >
    INLINE void Execution< TABLE, TABLE*, Set, CONTAINER_PARAMS... >::execute(
       TABLE&                              table,
       Set< TABLE*, CONTAINER_PARAMS... >& container) {
      container.insert(&table);
    }

    // storing tables into a vector<Table>
    template < typename TABLE, typename... CONTAINER_PARAMS >
    INLINE void Execution< TABLE, TABLE, std::vector, CONTAINER_PARAMS... >::execute(
       TABLE&                                     table,
       std::vector< TABLE, CONTAINER_PARAMS... >& container) {
      container.push_back(std::move(table));
    }

  }   // namespace ScheduleStorageMethod


  /// default constructor
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::ScheduleStorage(
     const IScheduleMultiDim&                 table,
     CONTAINER< TABLE, CONTAINER_PARAMS... >& container) :
      ScheduleOperator(ScheduleOperatorType::STORE_MULTIDIM, true, false),
      _container_(&container) {
    // checks that table is a ScheduleMultiDim<T>, where either TABLE=T or
    // TABLE = T*. If this is not the case, then we won't be able to perform the
    // operator, hence we should raise an exception now, i.e., before performing
    // all the computations within the schedule
    try {
      _arg_ = dynamic_cast< ScheduleMultiDim< SCHED_TABLE >* >(
         const_cast< IScheduleMultiDim* >(&table));
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The ScheduleMultiDim cannot be stored in the container "
                "because their types are incompatible.");
    }

    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleStorage);
  }


  /// copy constructor
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::ScheduleStorage(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& from) :
      ScheduleOperator(from),
      _arg_(from._arg_), _container_(from._container_), _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleStorage);
  }


  /// move constructor
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::ScheduleStorage(
     ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >&& from) :
      ScheduleOperator(std::move(from)),
      _arg_(from._arg_), _container_(from._container_), _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleStorage);
  }


  /// virtual copy constructor
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >*
         ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::clone() const {
    return new ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >(*this);
  }


  /// destructor
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::~ScheduleStorage() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleStorage);
  }


  /// copy operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >&
     ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator=(
        const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& from) {
    if (this != &from) {
      _arg_ = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _container_                = from._container_;
      _is_executed_              = from._is_executed_;
      ScheduleOperator::operator=(from);
    }
    return *this;
  }


  /// move operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >&
     ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator=(
        ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >&& from) {
    if (this != &from) {
      _arg_ = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _container_                = from._container_;
      _is_executed_              = from._is_executed_;
      ScheduleOperator::operator=(std::move(from));
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator==(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& op) const {
    return (_container_ == op._container_) && (*_arg_ == *op._arg_);
  }


  /// operator ==
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator==(
     const ScheduleOperator& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& real_op
         = dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator!=(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& op) const {
    return !ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator==(op);
  }


  /// operator !=
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator!=(
     const ScheduleOperator& op) const {
    return !ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::operator==(op);
  }


  /// checks whether two ScheduleOperator have similar parameters
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSimilarArguments(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& op) const {
    return _arg_->hasSameVariables(*op._arg_);
  }


  /// checks whether two ScheduleOperator have similar parameters
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSimilarArguments(
     const ScheduleOperator& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& real_op
         = dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperator have the same parameters
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSameArguments(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& op) const {
    return (_container_ == op._container_) && _arg_->hasSameVariables(*op._arg_)
        && _arg_->hasSameContent(*op._arg_);
  }


  /// checks whether two ScheduleOperator have the same parameters
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSameArguments(
     const ScheduleOperator& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& real_op
         = dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperator perform the same operation
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::isSameOperator(
     const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& op) const {
    return true;
  }


  /// checks whether two ScheduleOperator perform the same operation
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::isSameOperator(
     const ScheduleOperator& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& real_op
         = dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::isSameOperator(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the Store
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE const ScheduleMultiDim< typename std::remove_pointer< TABLE >::type >&
               ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  const Sequence< const IScheduleMultiDim* >&
     ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::args() const {
    return _args_;
  }


  /// returns the sequence of ScheduleMultidim output by the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  const Sequence< const IScheduleMultiDim* >&
     ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::results() const {
    return _results_;
  }


  /// really executes the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::execute() {
    if (_arg_->isAbstract()) {
      GUM_ERROR(NullElement,
                "It is impossible to store an abstract ScheduleMultiDim"
                "into a container.");
    }

    // if the container contains tables, not pointers to tables, we will move
    // directly the table contained within the ScheduleMultiDim into it, else
    // we will allocate a new table and pass this newly allocated tables to
    // the container
    if (std::is_same< SCHED_TABLE, TABLE >::value) {
      // here, the container contains tables, not pointers to tables
      if (_arg_->containsMultiDim()) {
        // here, the ScheduleMultiDim owns the table, so it can pass directly
        // its table to the container
        ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER, CONTAINER_PARAMS... >::
           execute(const_cast< SCHED_TABLE& >(_arg_->multiDim()), *_container_);
      } else {
        // here, the container just contains a reference on the table, hence
        // we should first copy it and this is this copy that will be inserted
        // into the container
        SCHED_TABLE table = _arg_->multiDim();
        ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER, CONTAINER_PARAMS... >::
           execute(table, *_container_);
      }
    } else {
      // here, the container contains pointers to tables. So we should first
      // allocate the table that will be stored into the container
      SCHED_TABLE* table;

      // if the ScheduleMultiDim owns the table, we just have to initialize
      // it by moving the ScheduleMultiDim's table, else we should copy it
      if (_arg_->containsMultiDim()) {
        table = new SCHED_TABLE(std::move(const_cast< SCHED_TABLE& >(_arg_->multiDim())));
      } else {
        table = new SCHED_TABLE(const_cast< SCHED_TABLE& >(_arg_->multiDim()));
      }

      ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER, CONTAINER_PARAMS... >::
         execute(*table, *_container_);
    }

    // be sure that the ScheduleMultiDim becomes abstract
    _arg_->makeAbstract();
    _is_executed_ = true;
  }


  /// indicates whether the operator has been executed
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::isExecuted() const {
    return _is_executed_;
  }


  /// undo a previous execution, if any
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::undo() {
    GUM_ERROR(OperationNotAllowed, "ScheduleStorage cannot be undone.");
  }


  /// modifies the arguments of the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::updateArgs(
     const Sequence< const IScheduleMultiDim* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleStorage::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    ScheduleMultiDim< SCHED_TABLE >* arg;
    try {
      arg = dynamic_cast< ScheduleMultiDim< SCHED_TABLE >* >(
         const_cast< IScheduleMultiDim* >(new_args[0]));
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleStorage::updateArgs does not match what "
                   << "the ScheduleOperator expects");
    }

    // save the new argument
    _arg_ = (ScheduleMultiDim< SCHED_TABLE >*)arg;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_ = false;
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperator */
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE double ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::nbOperations() const {
    return 1.0;
  }


  /// returns the memory consumption used during the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  INLINE std::pair< double, double >
         ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::memoryUsage() const {
    const double size_table = double(_arg_->domainSize()) * _arg_->sizeOfContent() + sizeof(TABLE);
    return {-size_table, -size_table};
  }


  /// displays the content of the operator
  template < typename TABLE,
             template < typename, typename... >
             class CONTAINER,
             typename... CONTAINER_PARAMS >
  std::string ScheduleStorage< TABLE, CONTAINER, CONTAINER_PARAMS... >::toString() const {
    return "store ( " + _arg_->toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
