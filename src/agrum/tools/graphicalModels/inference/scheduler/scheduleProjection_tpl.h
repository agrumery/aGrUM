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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection.h>

namespace gum {

  /// default constructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >::ScheduleProjection(
     const ScheduleMultiDim< TABLE, ALLOC >& table,
     const Set< const DiscreteVariable* >&   del_vars,
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&),
     const typename ScheduleProjection< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(ScheduleOperationType::PROJECT_MULTIDIM, false, alloc),
      _arg_(&table), _del_vars_(del_vars), _project_(project) {
    // compute the variables that shall belong to the result of the projection
    Sequence< const DiscreteVariable* > vars = table.variablesSequence();
    for (const auto var: del_vars) {
      if (vars.exists(var)) vars.erase(var);
    }

    // create the scheduleMultiDim that should result from the projection
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > allocator(this->get_allocator());
    _result_ = allocator.allocate(1);
    try {
      // if table is a constant, just copy it
      if (table.variablesSequence().empty() && !table.isAbstract())
        new ((void*)_result_)
           ScheduleMultiDim< TABLE, ALLOC >(table.multiDim(), true, Idx(0), allocator);
      else
        new ((void*)_result_) ScheduleMultiDim< TABLE, ALLOC >(vars, Idx(0), allocator);
    } catch (...) {
      allocator.deallocate(_result_, 1);
      throw;
    }

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProjection);
  }


  /// copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >::ScheduleProjection(
     const ScheduleProjection< TABLE, ALLOC >&                          from,
     const typename ScheduleProjection< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(from, alloc),
      _arg_(from._arg_), _del_vars_(from._del_vars_), _project_(from._project_) {
    // copy the result of the from operation
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > allocator(this->get_allocator());
    _result_ = allocator.allocate(1);
    try {
      new ((void*)_result_) ScheduleMultiDim< TABLE, ALLOC >(*(from._result_), allocator);
    } catch (...) {
      allocator.deallocate(_result_, 1);
      throw;
    }

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleProjection);
  }


  /// copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleProjection< TABLE, ALLOC >::ScheduleProjection(
     const ScheduleProjection< TABLE, ALLOC >& from) :
      ScheduleProjection< TABLE, ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >::ScheduleProjection(
     ScheduleProjection< TABLE, ALLOC >&&                               from,
     const typename ScheduleProjection< TABLE, ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(std::move(from), alloc),
      _arg_(from._arg_), _del_vars_(std::move(from._del_vars_)), _project_(from._project_) {
    // move the result of the from operation
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > allocator(this->get_allocator());
    _result_ = allocator.allocate(1);
    try {
      new ((void*)_result_)
         ScheduleMultiDim< TABLE, ALLOC >(std::move(*(from._result_)), allocator);
    } catch (...) {
      allocator.deallocate(_result_, 1);
      throw;
    }

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleProjection);
  }


  /// move constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleProjection< TABLE, ALLOC >::ScheduleProjection(
     ScheduleProjection< TABLE, ALLOC >&& from) :
      ScheduleProjection< TABLE, ALLOC >(std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >* ScheduleProjection< TABLE, ALLOC >::clone(
     const typename ScheduleProjection< TABLE, ALLOC >::allocator_type& alloc) const {
    ALLOC< ScheduleProjection< TABLE, ALLOC > > allocator(alloc);
    ScheduleProjection< TABLE, ALLOC >*         new_proj = allocator.allocate(1);
    try {
      new ((void*)new_proj) ScheduleProjection< TABLE, ALLOC >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_proj, 1);
      throw;
    }

    return new_proj;
  }


  /// virtual copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  INLINE ScheduleProjection< TABLE, ALLOC >* ScheduleProjection< TABLE, ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >::~ScheduleProjection() {
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > allocator(this->get_allocator());
    _result_->~ScheduleMultiDim< TABLE, ALLOC >();
    allocator.deallocate(_result_, 1);

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleProjection);
  }


  /// copy operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >&
     ScheduleProjection< TABLE, ALLOC >::operator=(const ScheduleProjection< TABLE, ALLOC >& from) {
    // avoid self assignment
    if (this != &from) {
      // copy the set of variables to delete in a temporary variable, just
      // in case something goes wrong below
      const Set< const DiscreteVariable* > new_del_vars = from._del_vars_;

      // copy in a temporary variable, in case something goes wrong
      const ScheduleMultiDim< TABLE, ALLOC > new_result = *from._result_;
      *_result_                                         = std::move(new_result);

      _del_vars_ = std::move(new_del_vars);
      _arg_      = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _project_                           = from._project_;
      ScheduleOperation< ALLOC >::operator=(from);
    }
    return *this;
  }


  /// move operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleProjection< TABLE, ALLOC >&
     ScheduleProjection< TABLE, ALLOC >::operator=(ScheduleProjection< TABLE, ALLOC >&& from) {
    // avoid self assignment
    if (this != &from) {
      *_result_  = std::move(*(from._result_));
      _del_vars_ = std::move(from._del_vars_);
      _arg_      = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _project_                           = from._project_;
      ScheduleOperation< ALLOC >::operator=(std::move(from));
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleProjection< TABLE, ALLOC >::operator==(
     const ScheduleProjection< TABLE, ALLOC >& op) const {
    return (_project_ == op._project_) && (*_arg_ == *op._arg_) && (_del_vars_ == op._del_vars_);
  }


  /// operator ==
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleProjection< TABLE, ALLOC >::operator==(const ScheduleOperation< ALLOC >& op) const {
    if (ScheduleOperation< ALLOC >::operator!=(op)) return false;

    try {
      const ScheduleProjection< TABLE, ALLOC >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE, ALLOC >& >(op);
      return ScheduleProjection< TABLE, ALLOC >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool
     ScheduleProjection< TABLE, ALLOC >::operator!=(const ScheduleOperation< ALLOC >& op) const {
    return !ScheduleProjection< TABLE, ALLOC >::operator==(op);
  }


  /// operator !=
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleProjection< TABLE, ALLOC >::operator!=(
     const ScheduleProjection< TABLE, ALLOC >& op) const {
    return !ScheduleProjection< TABLE, ALLOC >::operator==(op);
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleProjection< TABLE, ALLOC >::hasSimilarArguments(
     const ScheduleProjection< TABLE, ALLOC >& op) const {
    return (_arg_->hasSameVariables(*op._arg_) && (_del_vars_ == op._del_vars_));
  }


  /// checks whether two ScheduleOperation have similar parameters
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleProjection< TABLE, ALLOC >::hasSimilarArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleProjection< TABLE, ALLOC >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE, ALLOC >& >(op);
      return ScheduleProjection< TABLE, ALLOC >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleProjection< TABLE, ALLOC >::hasSameArguments(
     const ScheduleProjection< TABLE, ALLOC >& op) const {
    return (_arg_->hasSameVariables(*op._arg_) && _arg_->hasSameContent(*op._arg_)
            && (_del_vars_ == op._del_vars_));
  }


  /// checks whether two ScheduleOperation have the same parameters
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleProjection< TABLE, ALLOC >::hasSameArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleProjection< TABLE, ALLOC >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE, ALLOC >& >(op);
      return ScheduleProjection< TABLE, ALLOC >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleProjection< TABLE, ALLOC >::isSameOperation(
     const ScheduleProjection< TABLE, ALLOC >& op) const {
    return _project_ == op._project_;
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleProjection< TABLE, ALLOC >::isSameOperation(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleProjection< TABLE, ALLOC >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE, ALLOC >& >(op);
      return ScheduleProjection< TABLE, ALLOC >::isSameOperation(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the projection
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const ScheduleMultiDim< TABLE, ALLOC >& ScheduleProjection< TABLE, ALLOC >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operation
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
               ScheduleProjection< TABLE, ALLOC >::args() const {
    return _args_;
  }


  /// returns the result of the projection
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const ScheduleMultiDim< TABLE, ALLOC >&
               ScheduleProjection< TABLE, ALLOC >::result() const {
    return *_result_;
  }


  /// returns the result of the projection
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
               ScheduleProjection< TABLE, ALLOC >::results() const {
    return _results_;
  }


  /// modifies the arguments of the operation
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleProjection< TABLE, ALLOC >::updateArgs(
     const Sequence< const IScheduleMultiDim< ALLOC >* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleProjection::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE, ALLOC >* new_table;
    try {
      new_table = dynamic_cast< const ScheduleMultiDim< TABLE, ALLOC >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleProjection::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }

    // if the new table is a constant, just copy it
    if (new_table->variablesSequence().empty() && !new_table->isAbstract()) {
      *_result_
         = std::move(ScheduleMultiDim< TABLE, ALLOC >(new_table->multiDim(), true, _result_->id()));
    } else {
      // get the variables remaining after the projection
      Sequence< const DiscreteVariable* > vars = new_table->variablesSequence();
      for (const auto var: _del_vars_) {
        if (vars.exists(var)) vars.erase(var);
      }

      *_result_ = std::move(ScheduleMultiDim< TABLE, ALLOC >(vars, _result_->id()));
    }

    _arg_ = new_table;
    _args_.clear();
    _args_ << _arg_;
  }


  /// indicates whether the operation has been executed
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleProjection< TABLE, ALLOC >::isExecuted() const {
    return !_result_->isAbstract();
  }


  /// executes the operation
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleProjection< TABLE, ALLOC >::execute() {
    if (_result_->isAbstract()) {
      const TABLE& tab = _arg_->multiDim();
      if (_arg_->domainSize() > 1) {
        TABLE res = _project_(tab, _del_vars_);
        _result_->setMultiDim(std::move(res));
      } else {
        _result_->setMultiDim(tab, true);
      }
    }
  }


  /// undo a previous execution, if any
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleProjection< TABLE, ALLOC >::undo() {
    _result_->makeAbstract();
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename TABLE, template < typename > class ALLOC >
  INLINE double ScheduleProjection< TABLE, ALLOC >::nbOperations() const {
    return double(_arg_->domainSize());
  }


  /// returns the memory consumption used during the operation
  template < typename TABLE, template < typename > class ALLOC >
  std::pair< double, double > ScheduleProjection< TABLE, ALLOC >::memoryUsage() const {
    const double domsize
       = double(_result_->domainSize()) * _result_->sizeOfContent() + sizeof(TABLE);
    return {domsize, domsize};
  }


  /// displays the content of the operation
  template < typename TABLE, template < typename > class ALLOC >
  std::string ScheduleProjection< TABLE, ALLOC >::toString() const {
    return _result_->toString() + " = project ( " + _arg_->toString() + " , "
         + _del_vars_.toString() + " )";
  }


  /// use a new projection function
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleProjection< TABLE, ALLOC >::setProjectionFunction(
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&)) {
    _project_ = project;
    _result_->makeAbstract();
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
