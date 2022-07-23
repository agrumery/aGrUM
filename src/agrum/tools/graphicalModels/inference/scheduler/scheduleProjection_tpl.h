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
  template < typename TABLE >
  ScheduleProjection< TABLE >::ScheduleProjection(
     const ScheduleMultiDim< TABLE >&      table,
     const Set< const DiscreteVariable* >& del_vars,
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&),
     const bool is_result_persistent) :
      ScheduleOperator(ScheduleOperatorType::PROJECT_MULTIDIM, false, is_result_persistent),
      _arg_(&table), _del_vars_(del_vars), _project_(project) {
    // compute the variables that shall belong to the result of the projection
    Sequence< const DiscreteVariable* > vars = table.variablesSequence();
    for (const auto var: del_vars) {
      if (vars.exists(var)) vars.erase(var);
    }

    // create the scheduleMultiDim that should result from the projection
    // if table is a constant, just copy it
    if (table.variablesSequence().empty() && !table.isAbstract())
      _result_ = new ScheduleMultiDim< TABLE >(table.multiDim(), true, Idx(0));
    else _result_ = new ScheduleMultiDim< TABLE >(vars, Idx(0));

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProjection);
  }


  /// copy constructor
  template < typename TABLE >
  ScheduleProjection< TABLE >::ScheduleProjection(const ScheduleProjection< TABLE >& from) :
      ScheduleOperator(from), _arg_(from._arg_), _del_vars_(from._del_vars_),
      _project_(from._project_) {
    // copy the result of the from operator
    _result_ = new ScheduleMultiDim< TABLE >(*(from._result_));

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleProjection);
  }


  /// move constructor
  template < typename TABLE >
  ScheduleProjection< TABLE >::ScheduleProjection(ScheduleProjection< TABLE >&& from) :
      ScheduleOperator(std::move(from)), _arg_(from._arg_), _result_(from._result_),
      _del_vars_(std::move(from._del_vars_)), _project_(from._project_) {
    // indicate that from does not contain anything anymore
    from.makeResultsPersistent(true);   // prevent deleting nullptr
    from._result_ = nullptr;

    // save the args and result into _args_ and _results_
    _args_ << _arg_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleProjection);
  }


  /// virtual copy constructor
  template < typename TABLE >
  INLINE ScheduleProjection< TABLE >* ScheduleProjection< TABLE >::clone() const {
    return new ScheduleProjection< TABLE >(*this);
  }


  /// destructor
  template < typename TABLE >
  ScheduleProjection< TABLE >::~ScheduleProjection() {
    if (!this->hasPersistentResults()) { delete _result_; }

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleProjection);
  }


  /// copy operator
  template < typename TABLE >
  ScheduleProjection< TABLE >&
     ScheduleProjection< TABLE >::operator=(const ScheduleProjection< TABLE >& from) {
    // avoid self assignment
    if (this != &from) {
      // copy the set of variables to delete in a temporary variable, just
      // in case something goes wrong below
      const Set< const DiscreteVariable* > new_del_vars = from._del_vars_;

      // try to copy result (no need to update _results_)
      *_result_ = *(from._result_);
      ScheduleOperator::operator=(from);

      _del_vars_ = std::move(new_del_vars);
      _arg_      = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _project_ = from._project_;
    }
    return *this;
  }


  /// move operator
  template < typename TABLE >
  ScheduleProjection< TABLE >&
     ScheduleProjection< TABLE >::operator=(ScheduleProjection< TABLE >&& from) {
    // avoid self assignment
    if (this != &from) {
      if (!this->hasPersistentResults()) delete _result_;
      _result_ = from._result_;
      ScheduleOperator::operator=(std::move(from));

      _del_vars_ = std::move(from._del_vars_);
      _arg_      = from._arg_;
      _args_.clear();
      _args_ << _arg_;
      _project_ = from._project_;

      from.makeResultsPersistent(true);   // prevent deleting nullptr
      from._result_ = nullptr;
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE >
  INLINE bool ScheduleProjection< TABLE >::operator==(const ScheduleProjection< TABLE >& op) const {
    return (_project_ == op._project_) && (*_arg_ == *op._arg_) && (_del_vars_ == op._del_vars_);
  }


  /// operator ==
  template < typename TABLE >
  bool ScheduleProjection< TABLE >::operator==(const ScheduleOperator& op) const {
    if (ScheduleOperator::operator!=(op)) return false;

    try {
      const ScheduleProjection< TABLE >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE >& >(op);
      return ScheduleProjection< TABLE >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE >
  INLINE bool ScheduleProjection< TABLE >::operator!=(const ScheduleOperator& op) const {
    return !ScheduleProjection< TABLE >::operator==(op);
  }


  /// operator !=
  template < typename TABLE >
  INLINE bool ScheduleProjection< TABLE >::operator!=(const ScheduleProjection< TABLE >& op) const {
    return !ScheduleProjection< TABLE >::operator==(op);
  }


  /// checks whether two ScheduleOperator have similar parameters
  template < typename TABLE >
  INLINE bool
     ScheduleProjection< TABLE >::hasSimilarArguments(const ScheduleProjection< TABLE >& op) const {
    return (_arg_->hasSameVariables(*op._arg_) && (_del_vars_ == op._del_vars_));
  }


  /// checks whether two ScheduleOperator have similar parameters
  template < typename TABLE >
  bool ScheduleProjection< TABLE >::hasSimilarArguments(const ScheduleOperator& op) const {
    try {
      const ScheduleProjection< TABLE >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE >& >(op);
      return ScheduleProjection< TABLE >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperator have the same parameters
  template < typename TABLE >
  INLINE bool
     ScheduleProjection< TABLE >::hasSameArguments(const ScheduleProjection< TABLE >& op) const {
    return (_arg_->hasSameVariables(*op._arg_) && _arg_->hasSameContent(*op._arg_)
            && (_del_vars_ == op._del_vars_));
  }


  /// checks whether two ScheduleOperator have the same parameters
  template < typename TABLE >
  bool ScheduleProjection< TABLE >::hasSameArguments(const ScheduleOperator& op) const {
    try {
      const ScheduleProjection< TABLE >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE >& >(op);
      return ScheduleProjection< TABLE >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperator perform the same operation
  template < typename TABLE >
  INLINE bool
     ScheduleProjection< TABLE >::isSameOperator(const ScheduleProjection< TABLE >& op) const {
    return _project_ == op._project_;
  }


  /// checks whether two ScheduleOperator perform the same operation
  template < typename TABLE >
  bool ScheduleProjection< TABLE >::isSameOperator(const ScheduleOperator& op) const {
    try {
      const ScheduleProjection< TABLE >& real_op
         = dynamic_cast< const ScheduleProjection< TABLE >& >(op);
      return ScheduleProjection< TABLE >::isSameOperator(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the argument of the projection
  template < typename TABLE >
  INLINE const ScheduleMultiDim< TABLE >& ScheduleProjection< TABLE >::arg() const {
    return *_arg_;
  }


  /// returns the sequence of arguments passed to the operator
  template < typename TABLE >
  INLINE const Sequence< const IScheduleMultiDim* >& ScheduleProjection< TABLE >::args() const {
    return _args_;
  }


  /// returns the result of the projection
  template < typename TABLE >
  INLINE const ScheduleMultiDim< TABLE >& ScheduleProjection< TABLE >::result() const {
    return *_result_;
  }


  /// returns the result of the projection
  template < typename TABLE >
  INLINE const Sequence< const IScheduleMultiDim* >& ScheduleProjection< TABLE >::results() const {
    return _results_;
  }


  /// modifies the arguments of the operator
  template < typename TABLE >
  void
     ScheduleProjection< TABLE >::updateArgs(const Sequence< const IScheduleMultiDim* >& new_args) {
    // check that there is exactly one argument in new_args and that its type
    // is compatible with TABLE
    if (new_args.size() != Size(1)) {
      GUM_ERROR(SizeError,
                "Method ScheduleProjection::updateArgs expects 1 new "
                   << "argument, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE >* new_table;
    try {
      new_table = dynamic_cast< const ScheduleMultiDim< TABLE >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the argument passed to "
                   << "ScheduleProjection::updateArgs does not match what "
                   << "the ScheduleOperator expects");
    }

    // if the new table is a constant, just copy it
    if (new_table->variablesSequence().empty() && !new_table->isAbstract()) {
      *_result_ = std::move(ScheduleMultiDim< TABLE >(new_table->multiDim(), true, _result_->id()));
    } else {
      // get the variables remaining after the projection
      Sequence< const DiscreteVariable* > vars = new_table->variablesSequence();
      for (const auto var: _del_vars_) {
        if (vars.exists(var)) vars.erase(var);
      }

      *_result_ = std::move(ScheduleMultiDim< TABLE >(vars, _result_->id()));
    }

    _arg_ = new_table;
    _args_.clear();
    _args_ << _arg_;
  }


  /// indicates whether the operator has been executed
  template < typename TABLE >
  bool ScheduleProjection< TABLE >::isExecuted() const {
    return !_result_->isAbstract();
  }


  /// executes the operator
  template < typename TABLE >
  void ScheduleProjection< TABLE >::execute() {
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
  template < typename TABLE >
  void ScheduleProjection< TABLE >::undo() {
    _result_->makeAbstract();
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperator */
  template < typename TABLE >
  INLINE double ScheduleProjection< TABLE >::nbOperations() const {
    return double(_arg_->domainSize());
  }


  /// returns the memory consumption used during the operator
  template < typename TABLE >
  std::pair< double, double > ScheduleProjection< TABLE >::memoryUsage() const {
    const double domsize
       = double(_result_->domainSize()) * _result_->sizeOfContent() + sizeof(TABLE);
    return {domsize, domsize};
  }


  /// displays the content of the operator
  template < typename TABLE >
  std::string ScheduleProjection< TABLE >::toString() const {
    return _result_->toString() + " = project ( " + _arg_->toString() + " , "
         + _del_vars_.toString() + " )";
  }


  /// use a new projection function
  template < typename TABLE >
  void ScheduleProjection< TABLE >::setProjectionFunction(
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&)) {
    _project_ = project;
    _result_->makeAbstract();
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
