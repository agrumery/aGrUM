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
 * @brief a binary combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleBinaryCombination.h>
#  include <limits>

namespace gum {

  /// default constructor
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::ScheduleBinaryCombination(
     const ScheduleMultiDim< TABLE1 >& table1,
     const ScheduleMultiDim< TABLE2 >& table2,
     TABLE_RES (*combine)(const TABLE1&, const TABLE2&),
     const bool is_result_persistent) :
      ScheduleOperator(ScheduleOperatorType::COMBINE_MULTIDIM, false, is_result_persistent),
      _arg1_(&table1), _arg2_(&table2), _combine_(combine) {
    // compute the variables of the resulting table
    Sequence< const DiscreteVariable* >        vars  = table1.variablesSequence();
    const Sequence< const DiscreteVariable* >& vars2 = table2.variablesSequence();
    for (const auto var: vars2) {
      if (!vars.exists(var)) { vars.insert(var); }
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    _result_ = new ScheduleMultiDim< TABLE_RES >(vars, Idx(0));

    // save the args and result into _args_ and _results_
    _args_ << _arg1_ << _arg2_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleBinaryCombination);
  }


  /// copy constructor
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::ScheduleBinaryCombination(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& from) :
      ScheduleOperator(from),
      _arg1_(from._arg1_), _arg2_(from._arg2_), _combine_(from._combine_) {
    // copy the result of the from operator
    _result_ = from._result_->clone();

    // save the args and result into _args_ and _results_
    _args_ << _arg1_ << _arg2_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleBinaryCombination);
  }


  /// move constructor
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::ScheduleBinaryCombination(
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&& from) :
      ScheduleOperator(std::move(from)),
      _arg1_(from._arg1_), _arg2_(from._arg2_), _result_(from._result_), _combine_(from._combine_) {
    // indicate that from does not contain anything anymore
    from.makeResultsPersistent(true);   // prevent deleting nullptr
    from._result_ = nullptr;

    // save the args and result into _args_ and _results_
    _args_ = std::move(from._args_);
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleBinaryCombination);
  }


  /// virtual copy constructor
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >*
         ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::clone() const {
    return new ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >(*this);
  }


  /// destructor
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::~ScheduleBinaryCombination() {
    // if the result is not persistent, we should remove it
    if (!this->hasPersistentResults()) { delete _result_; }

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleBinaryCombination);
  }


  /// copy operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator=(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& from) {
    if (this != &from) {
      // try to copy result (no need to update _results_)
      *_result_ = *(from._result_);
      ScheduleOperator::operator=(from);

      _arg1_    = from._arg1_;
      _arg2_    = from._arg2_;
      _args_    = from._args_;
      _combine_ = from._combine_;
    }
    return *this;
  }


  /// move operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator=(
        ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&& from) {
    if (this != &from) {
      if (!this->hasPersistentResults()) delete _result_;
      _result_ = from._result_;
      ScheduleOperator::operator=(std::move(from));

      _arg1_    = from._arg1_;
      _arg2_    = from._arg2_;
      _args_    = std::move(from._args_);
      _combine_ = from._combine_;

      from.makeResultsPersistent(true);   // prevent deleting nullptr
      from._result_ = nullptr;
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator==(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& op) const {
    return (_combine_ == op._combine_) && (*_arg1_ == *op._arg1_) && (*_arg2_ == *op._arg2_);
  }


  /// operator ==
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator==(
     const ScheduleOperator& op) const {
    if (ScheduleOperator::operator!=(op)) return false;

    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& real_op
         = dynamic_cast< const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& >(op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator!=(
     const ScheduleOperator& op) const {
    return !ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator==(op);
  }


  /// operator !=
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator!=(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& op) const {
    return !ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::operator==(op);
  }


  /// checks whether two ScheduleCombination have the same parameters
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSameArguments(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& op) const {
    return (_arg1_->hasSameVariables(*op._arg1_) && _arg2_->hasSameVariables(*op._arg2_)
            && _arg1_->hasSameContent(*op._arg1_) && _arg2_->hasSameContent(*op._arg2_));
  }


  /// checks whether two ScheduleCombination have the same parameters
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSameArguments(
     const ScheduleOperator& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& real_op
         = dynamic_cast< const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& >(op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleCombination have similar parameters
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSimilarArguments(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& op) const {
    return (_arg1_->hasSameVariables(*op._arg1_) && _arg2_->hasSameVariables(*op._arg2_));
  }


  /// checks whether two ScheduleCombination have similar parameters
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSimilarArguments(
     const ScheduleOperator& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& real_op
         = dynamic_cast< const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& >(op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperator perform the same set of operations
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::isSameOperator(
     const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& op) const {
    return _combine_ == op._combine_;
  }


  /// checks whether two ScheduleOperator perform the same set of operations
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::isSameOperator(
     const ScheduleOperator& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& real_op
         = dynamic_cast< const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& >(op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::isSameOperator(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the first argument of the combination
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE const ScheduleMultiDim< TABLE1 >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::arg1() const {
    return *_arg1_;
  }


  /// returns the second argument of the combination
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE const ScheduleMultiDim< TABLE2 >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::arg2() const {
    return *_arg2_;
  }


  /// returns the sequence of arguments passed to the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE const Sequence< const IScheduleMultiDim* >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::args() const {
    return _args_;
  }


  /// returns the result of the combination
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE const ScheduleMultiDim< TABLE_RES >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::result() const {
    return *_result_;
  }


  /// returns the sequence of ScheduleMultidim output by the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE const Sequence< const IScheduleMultiDim* >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::results() const {
    return _results_;
  }


  /// modifies the arguments of the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::updateArgs(
     const Sequence< const IScheduleMultiDim* >& new_args) {
    // check that there are exactly two arguments in new_args and that their
    // types are compatible with TABLE1 and TABLE2
    if (new_args.size() != Size(2)) {
      GUM_ERROR(SizeError,
                "Method ScheduleBinaryCombination::updateArgs expects 2 new "
                   << "arguments, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE1 >* arg1;
    const ScheduleMultiDim< TABLE2 >* arg2;
    try {
      arg1 = dynamic_cast< const ScheduleMultiDim< TABLE1 >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the first argument passed to "
                   << "ScheduleBinaryCombination::updateArgs does not match what "
                   << "the ScheduleOperator expects");
    }
    try {
      arg2 = dynamic_cast< const ScheduleMultiDim< TABLE2 >* >(new_args[1]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the second argument passed to "
                   << "ScheduleBinaryCombination::updateArgs does not match what "
                   << "the ScheduleOperator expects");
    }

    // save the new arguments
    _arg1_ = arg1;
    _arg2_ = arg2;
    _args_ = {_arg1_, _arg2_};

    // now the result is obsolete, so make it abstract
    _result_->makeAbstract();
  }


  /// indicates whether the operator has been executed
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::isExecuted() const {
    return !_result_->isAbstract();
  }


  /// executes the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::execute() {
    if (_result_->isAbstract()) {
      // first, get the tables to combine
      const TABLE1& t1 = _arg1_->multiDim();
      const TABLE2& t2 = _arg2_->multiDim();

      // perform the combination and store the result
      TABLE_RES res = _combine_(t1, t2);
      _result_->setMultiDim(std::move(res));
    }
  }


  /// undo a previous execution, if any
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::undo() {
    _result_->makeAbstract();
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperator */
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  double ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::nbOperations() const {
    return double(_result_->domainSize());
  }


  /// returns the memory consumption used during the execution of the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  INLINE std::pair< double, double >
         ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::memoryUsage() const {
    const double domsize
       = double(_result_->domainSize()) * _result_->sizeOfContent() + sizeof(TABLE_RES);
    return {domsize, domsize};
  }


  /// displays the content of the operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  std::string ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::toString() const {
    return _result_->toString() + " = combine ( " + _arg1_->toString() + " , " + _arg2_->toString()
         + " )";
  }


  /// use a new combination function
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >::setCombinationFunction(
     TABLE_RES (*combine)(const TABLE1&, const TABLE2&)) {
    _combine_ = combine;
    _result_->makeAbstract();
  }


}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
