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
    template < typename TABLE,
               template < typename > class ALLOC,
               typename... CONTAINER_PARAMS >
    INLINE void Execution< TABLE, TABLE*, Set, ALLOC, CONTAINER_PARAMS... >::
       execute(TABLE& table,
               Set< TABLE*, CONTAINER_PARAMS... >& container) {
      container.insert(&table);
    }

    // storing tables into a vector<Table>
    template < typename TABLE,
               template < typename > class ALLOC,
               typename... CONTAINER_PARAMS >
    INLINE void Execution<TABLE, TABLE, std::vector, ALLOC, CONTAINER_PARAMS... >::
       execute(TABLE& table,
               std::vector< TABLE, CONTAINER_PARAMS... >& container) {
      container.push_back(std::move(table));
    }

  }



  /// default constructor
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::ScheduleStorage(
     const IScheduleMultiDim< ALLOC >& table,
     CONTAINER<TABLE, CONTAINER_PARAMS...>&  container,
     const typename ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(ScheduleOperationType::STORE_MULTIDIM, true, false, alloc),
      _container_(&container) {
    // checks that table is a ScheduleMultiDim<T,ALLOC>, where either TABLE=T or
    // TABLE = T*. If this is not the case, then we won't be able to perform the
    // operation, hence we should raise an exception now, i.e., before performing
    // all the computations within the schedule
    try {
      _arg_ = dynamic_cast<ScheduleMultiDim< SCHED_TABLE, ALLOC >*>
         (const_cast<IScheduleMultiDim< ALLOC >*> (&table));
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


  /// copy constructor with a given allocator
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::ScheduleStorage(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&                          from,
     const typename ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(from, alloc),
      _arg_(from._arg_),
      _container_(from._container_),
      _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleStorage);
  }


  /// copy constructor
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::
     ScheduleStorage(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& from) :
      ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::ScheduleStorage(
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&&                               from,
     const typename ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(std::move(from), alloc),
      _arg_(from._arg_),
      _container_(from._container_),
      _is_executed_(from._is_executed_) {
    // save the arg into _args_ (no need to update _results_)
    _args_ << _arg_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleStorage);
  }


  /// move constructor
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::
     ScheduleStorage(
        ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&& from) :
      ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >(std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >*
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::clone(
        const allocator_type& alloc) const {
    ALLOC< ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... > >
       allocator(alloc);
    ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >*
       new_store = allocator.allocate(1);
    try {
      new ((void*)new_store)
         ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_store, 1);
      throw;
    }

    return new_store;
  }


  /// virtual copy constructor
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >*
         ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::~ScheduleStorage() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleStorage);
  }


  /// copy operator
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator=(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& from) {
    if (this != &from) {
      _arg_ = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _container_ = from._container_;
      _is_executed_ = from._is_executed_;
      ScheduleOperation< ALLOC >::operator=(from);
    }
    return *this;
  }


  /// move operator
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator=(
        ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >&& from) {
    if (this != &from) {
      _arg_ = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _container_ = from._container_;
      _is_executed_ = from._is_executed_;
      ScheduleOperation< ALLOC >::operator=(std::move(from));
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator==(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& op) const {
      return (_container_ == op._container_) && (*_arg_ == *op._arg_);
  }


  /// operator ==
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator==(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& real_op =
         dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator!=(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& op) const {
    return !ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator==(op);
  }


  /// operator !=
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator!=(
     const ScheduleOperation< ALLOC >& op) const {
    return !ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::operator==(op);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSimilarArguments(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& op) const {
    return _arg_->hasSameVariables(*op._arg_);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSimilarArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& real_op =
         dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSameArguments(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& op) const {
    return (_container_ == op._container_) &&
           _arg_->hasSameVariables(*op._arg_) &&
           _arg_->hasSameContent(*op._arg_);
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSameArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& real_op =
         dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::isSameOperation(
     const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& op) const {
    return true;
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  bool ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::isSameOperation(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& real_op =
         dynamic_cast< const ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >& >(op);
      return ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::isSameOperation(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the Store
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE const ScheduleMultiDim< typename std::remove_pointer<TABLE>::type, ALLOC >&
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  const Sequence< const IScheduleMultiDim< ALLOC >* >&
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::args() const {
    return _args_;
  }


  /// returns the sequence of ScheduleMultidim output by the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  const Sequence< const IScheduleMultiDim< ALLOC >* >&
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::results()
        const {
    return _results_;
  }


  /// really executes the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::execute() {
    if (_arg_->isAbstract()) {
      GUM_ERROR(NullElement,
                "It is impossible to store an abstract ScheduleMultiDim"
                "into a container.");
    }

    // if the container contains tables, not pointers to tables, we will move
    // directly the table contained within the ScheduleMultiDim into it, else
    // we will allocate a new table and pass this newly allocated tables to
    // the container
    if (std::is_same<SCHED_TABLE, TABLE>::value) {
      // here, the container contains tables, not pointers to tables
      if (_arg_->containsMultiDim()) {
        // here, the ScheduleMultiDim owns the table, so it can pass directly
        // its table to the container
        ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER, ALLOC,
                                          CONTAINER_PARAMS... >::
           execute(const_cast<SCHED_TABLE&>(_arg_->multiDim()), *_container_);
      }
      else {
        // here, the container just contains a reference on the table, hence
        // we should first copy it and this is this copy that will be inserted
        // into the container
        SCHED_TABLE table = _arg_->multiDim();
        ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER, ALLOC,
                                          CONTAINER_PARAMS... >::
           execute(table, *_container_);
      }
    }
    else {
      // here, the container contains pointers to tables. So we should first
      // allocate the table that will be stored into the container
      ALLOC< SCHED_TABLE > allocator = this->get_allocator();
      SCHED_TABLE*         table = allocator.allocate(1);

      // if the ScheduleMultiDim owns the table, we just have to initialize
      // it by moving the ScheduleMultiDim's table, else we should copy it
      if (_arg_->containsMultiDim()) {
        new ((void*)table)
           SCHED_TABLE(std::move(const_cast<SCHED_TABLE&>(_arg_->multiDim())));
      }
      else {
        new ((void*)table)
           SCHED_TABLE(const_cast<SCHED_TABLE&>(_arg_->multiDim()));
      }

      ScheduleStorageMethod::Execution< SCHED_TABLE, TABLE, CONTAINER,
                                        ALLOC, CONTAINER_PARAMS... >::
              execute(*table, *_container_);
    }

    // be sure that the ScheduleMultiDim becomes abstract
    _arg_->makeAbstract();
    _is_executed_ = true;
  }


  /// indicates whether the operation has been executed
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE bool
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::isExecuted() const {
    return _is_executed_;
  }


  /// undo a previous execution, if any
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::undo() {
    GUM_ERROR(OperationNotAllowed, "ScheduleStorage cannot be undone.");
  }


  /// modifies the arguments of the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  void ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::updateArgs(
     const Sequence< const IScheduleMultiDim< ALLOC >* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleStorage::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    ScheduleMultiDim< SCHED_TABLE, ALLOC >* arg;
    try {
      arg = dynamic_cast< ScheduleMultiDim< SCHED_TABLE, ALLOC >* >
         (const_cast< IScheduleMultiDim< ALLOC >* >(new_args[0]));
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleStorage::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }

    // save the new argument
    _arg_ = (ScheduleMultiDim< SCHED_TABLE, ALLOC >*) arg;
    _args_.clear();
    _args_ << _arg_;
    _is_executed_ = false;
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE double ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::nbOperations() const {
    return 1.0;
  }


  /// returns the memory consumption used during the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  INLINE std::pair< double, double >
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::memoryUsage() const {
    const double size_table =
       double(_arg_->domainSize()) * _arg_->sizeOfContent()
       + sizeof(TABLE);
    return {-size_table, -size_table};
  }


  /// displays the content of the operation
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             template < typename > class ALLOC,
             typename... CONTAINER_PARAMS >
  std::string
     ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >::toString() const {
    return "store ( " + _arg_->toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
