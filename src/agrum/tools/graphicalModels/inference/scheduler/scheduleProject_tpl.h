/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::ScheduleProject(
     const ScheduleMultiDim< GUM_SCALAR >& table,
     const Set< const DiscreteVariable* >& del_vars,
     MultiDimImplementation< GUM_SCALAR >* (*project)(const MultiDimImplementation< GUM_SCALAR >&,
                                                      const Set< const DiscreteVariable* >&)) :
      ScheduleOperation< GUM_SCALAR >(ScheduleOperation< GUM_SCALAR >::Type::PROJECT_MULTIDIM),
      _table_(table), _del_vars_(del_vars), _args_(0), _results_(0), _project_(project) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProject);

    // compute the variables that shall belong to the result of the projection
    Sequence< const DiscreteVariable* > vars = _table_.variablesSequence();

    for (const auto var: del_vars)
      vars.erase(var);

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    _result_ = new ScheduleMultiDim< GUM_SCALAR >(vars);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::ScheduleProject(const ScheduleProject< GUM_SCALAR >& f) :
      ScheduleOperation< GUM_SCALAR >(f), _table_(f._table_), _del_vars_(f._del_vars_),
      _result_(new ScheduleMultiDim< GUM_SCALAR >(*(f._result_))), _args_(0), _results_(0),
      _project_(f._project_) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleProject);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >* ScheduleProject< GUM_SCALAR >::newFactory() const {
    return new ScheduleProject< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::~ScheduleProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleProject);
    delete _result_;

    if (_args_) delete _args_;

    if (_results_) delete _results_;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >&
     ScheduleProject< GUM_SCALAR >::operator=(const ScheduleProject< GUM_SCALAR >& from) {
    // avoid self assignment
    if (this != &from) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      _table_                                  = from._table_;
      _del_vars_                               = from._del_vars_;
      *_result_                                = *(from._result_);
      _project_                                = from._project_;

      // update  _args_ and  _results_ if they were already created
      if (_args_) {
        _args_->clear();
        _args_->insert(&_table_);
      }

      if (_results_) {
        _results_->clear();
        _results_->insert(_result_);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  bool ScheduleProject< GUM_SCALAR >::operator==(const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleProject< GUM_SCALAR >& real_op
       = static_cast< const ScheduleProject< GUM_SCALAR >& >(op);
    return ((_table_ == real_op._table_) && (_del_vars_ == real_op._del_vars_)
            && (_project_ == real_op._project_));
  }

  /// operator !=
  template < typename GUM_SCALAR >
  bool ScheduleProject< GUM_SCALAR >::operator!=(const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return true;

    const ScheduleProject< GUM_SCALAR >& real_op
       = static_cast< const ScheduleProject< GUM_SCALAR >& >(op);
    return ((_table_ != real_op._table_) || (_del_vars_ != real_op._del_vars_)
            || (_project_ != real_op._project_));
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleProject< GUM_SCALAR >::execute() {
    if (_result_->isAbstract()) {
      const MultiDimImplementation< GUM_SCALAR >& t   = _table_.multiDim();
      MultiDimImplementation< GUM_SCALAR >*       res = _project_(t, _del_vars_);
      _result_->setMultiDim(*res);
    }
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float ScheduleProject< GUM_SCALAR >::nbOperations() const {
    return float(_table_.domainSize());
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  std::pair< long, long > ScheduleProject< GUM_SCALAR >::memoryUsage() const {
    long                                       size = 1;
    const Sequence< const DiscreteVariable* >& seq  = _table_.variablesSequence();

    for (const auto var: seq)
      if (!_del_vars_.contains(var)) {
        if (std::numeric_limits< long >::max() / (long)var->domainSize() < size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range")
        }

        size *= long(var->domainSize());
      }

    return std::pair< long, long >(size, size);
  }

  /// returns the scheduleMultidim resulting from the execution of the operation
  template < typename GUM_SCALAR >
  INLINE const ScheduleMultiDim< GUM_SCALAR >& ScheduleProject< GUM_SCALAR >::result() const {
    return *_result_;
  }

  /// returns the set of multidims passed in argument to the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleProject< GUM_SCALAR >::multiDimArgs() const {
    if (!_args_) {
      _args_ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      _args_->insert(&_table_);
    }

    return *_args_;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleProject< GUM_SCALAR >::multiDimResults() const {
    if (!_results_) {
      _results_ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      _results_->insert(_result_);
    }

    return *_results_;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleProject< GUM_SCALAR >::toString() const {
    return _result_->toString() + " = project ( " + _table_.toString() + " , "
         + _del_vars_.toString() + " )";
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
