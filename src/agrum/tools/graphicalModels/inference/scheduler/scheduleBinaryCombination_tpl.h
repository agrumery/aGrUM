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
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ScheduleBinaryCombination(
        const ScheduleMultiDim< TABLE1, ALLOC >& table1,
        const ScheduleMultiDim< TABLE2, ALLOC >& table2,
        TABLE_RES (*combine)(const TABLE1&, const TABLE2&),
        const typename ScheduleBinaryCombination< TABLE1,
                                                  TABLE2,
                                                  TABLE_RES,
                                                  ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(ScheduleOperationType::COMBINE_MULTIDIM, false, alloc),
      _arg1_(&table1), _arg2_(&table2), _combine_(combine) {
    // compute the variables of the resulting table
    Sequence< const DiscreteVariable* >        vars  = table1.variablesSequence();
    const Sequence< const DiscreteVariable* >& vars2 = table2.variablesSequence();
    for (const auto var: vars2) {
      if (!vars.exists(var)) { vars.insert(var); }
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    ALLOC< ScheduleMultiDim< TABLE_RES, ALLOC > > allocator(this->get_allocator());
    _result_ = allocator.allocate(1);
    try {
      new ((void*)_result_)
         ScheduleMultiDim< TABLE_RES, ALLOC >(vars, 0, allocator);
    } catch (...) {
      allocator.deallocate(_result_, 1);
      throw;
    }

    // save the args and result into _args_ and _results_
    _args_ << _arg1_ << _arg2_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleBinaryCombination);
  }


  /// copy constructor with a given allocator
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ScheduleBinaryCombination(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& from,
        const typename ScheduleBinaryCombination< TABLE1,
                                                  TABLE2,
                                                  TABLE_RES,
                                                  ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(from, alloc),
      _arg1_(from._arg1_), _arg2_(from._arg2_), _combine_(from._combine_) {
    // copy the result of the from operation
    _result_ = from._result_->clone();

    // save the args and result into _args_ and _results_
    _args_ << _arg1_ << _arg2_;
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_CPY(ScheduleBinaryCombination);
  }


  /// copy constructor
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ScheduleBinaryCombination(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
           from) :
      ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >(
         from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ScheduleBinaryCombination(
        ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&&    from,
        const typename ScheduleBinaryCombination< TABLE1,
                                                  TABLE2,
                                                  TABLE_RES,
                                                  ALLOC >::allocator_type& alloc) :
      ScheduleOperation< ALLOC >(std::move(from), alloc),
      _arg1_(from._arg1_), _arg2_(from._arg2_), _combine_(from._combine_) {
    // move the result of the from operation
    ALLOC< ScheduleMultiDim< TABLE_RES, ALLOC > > allocator(this->get_allocator());
    _result_ = allocator.allocate(1);
    try {
      new ((void*)_result_) ScheduleMultiDim< TABLE_RES, ALLOC >(
         std::move(*(from._result_)), allocator);
    } catch (...) {
      allocator.deallocate(_result_, 1);
      throw;
    }

    // save the args and result into _args_ and _results_
    _args_ = std::move(from._args_);
    _results_ << _result_;

    // for debugging purposes
    GUM_CONS_MOV(ScheduleBinaryCombination);
  }


  /// move constructor
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ScheduleBinaryCombination(
        ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&& from) :
      ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >(
         std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >*
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::clone(
        const typename ScheduleBinaryCombination< TABLE1,
                                                  TABLE2,
                                                  TABLE_RES,
                                                  ALLOC >::allocator_type& alloc)
        const {
    ALLOC< ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC > >
                                                                   allocator(alloc);
    ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >* new_comb =
       allocator.allocate(1);
    try {
      new ((void*)new_comb)
         ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >(*this,
                                                                       alloc);
    } catch (...) {
      allocator.deallocate(new_comb, 1);
      throw;
    }

    return new_comb;
  }


  /// virtual copy constructor
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >*
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     ~ScheduleBinaryCombination() {
    ALLOC< ScheduleMultiDim< TABLE_RES, ALLOC > > allocator(this->get_allocator());
    _result_->~ScheduleMultiDim< TABLE_RES, ALLOC >();
    allocator.deallocate(_result_, 1);

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleBinaryCombination);
  }


  /// copy operator
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator=(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
           from) {
    if (this != &from) {
      // try to copy result (no need to update _results_)
      *_result_ = *(from._result_);

      _arg1_ = from._arg1_;
      _arg2_ = from._arg2_;
      _args_ = from._args_;
      _combine_ = from._combine_;

      ScheduleOperation< ALLOC >::operator=(from);
    }
    return *this;
  }


  /// move operator
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator=(
        ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&& from) {
    if (this != &from) {
      *_result_ = std::move(*(from._result_));

      _arg1_ = from._arg1_;
      _arg2_ = from._arg2_;
      _args_ = std::move(from._args_);
      _combine_ = from._combine_;
      ScheduleOperation< ALLOC >::operator=(std::move(from));
    }
    return *this;
  }


  /// operator ==
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator==(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& op)
        const {
      return (_combine_ == op._combine_) &&
           (*_arg1_ == *op._arg1_) && (*_arg2_ == *op._arg2_);
  }


  /// operator ==
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  bool ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator==(
     const ScheduleOperation< ALLOC >& op) const {
    if (ScheduleOperation< ALLOC >::operator!=(op)) return false;

    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
         real_op = dynamic_cast<
            const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& >(
            op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
            operator==(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// operator !=
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator!=(
        const ScheduleOperation< ALLOC >& op) const {
    return !ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
       operator==(op);
  }


  /// operator !=
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::operator!=(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& op)
        const {
    return !ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
       operator==(op);
  }


  /// checks whether two ScheduleCombination have the same parameters
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::
     hasSameArguments(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& op)
        const {
      return (_arg1_->hasSameVariables(*op._arg1_) &&
              _arg2_->hasSameVariables(*op._arg2_) &&
              _arg1_->hasSameContent(*op._arg1_) &&
              _arg2_->hasSameContent(*op._arg2_));
  }


  /// checks whether two ScheduleCombination have the same parameters
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  bool
     ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::
     hasSameArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
         real_op = dynamic_cast<
            const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& >(
            op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
         hasSameArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleCombination have similar parameters
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::
     hasSimilarArguments(
        const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& op)
        const {
      return (_arg1_->hasSameVariables(*op._arg1_) &&
              _arg2_->hasSameVariables(*op._arg2_));
  }


  /// checks whether two ScheduleCombination have similar parameters
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  bool
     ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::
     hasSimilarArguments(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
         real_op = dynamic_cast<
            const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& >(
            op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
         hasSimilarArguments(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  bool ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::isSameOperation(
       const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& op) const {
    return _combine_ == op._combine_;
  }


  /// checks whether two ScheduleOperation perform the same operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  bool ScheduleBinaryCombination< TABLE1,
                                TABLE2,
                                TABLE_RES,
                                ALLOC >::isSameOperation(
     const ScheduleOperation< ALLOC >& op) const {
    try {
      const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >&
         real_op = dynamic_cast<
            const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >& >(
            op);
      return ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
            isSameOperation(real_op);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the first argument of the combination
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE const ScheduleMultiDim< TABLE1, ALLOC >&
               ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::arg1() const {
    return *_arg1_;
  }


  /// returns the second argument of the combination
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE const ScheduleMultiDim< TABLE2, ALLOC >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::arg2() const {
    return *_arg2_;
  }


  /// returns the sequence of arguments passed to the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::args() const {
    return _args_;
  }


  /// returns the result of the combination
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE const ScheduleMultiDim< TABLE_RES, ALLOC >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::result()
        const {
    return *_result_;
  }


  /// returns the sequence of ScheduleMultidim output by the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE const Sequence< const IScheduleMultiDim< ALLOC >* >&
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::results()
        const {
    return _results_;
  }


  /// modifies the arguments of the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::updateArgs(
     const Sequence< const IScheduleMultiDim< ALLOC >* >& new_args) {
    // check that there are exactly two arguments in new_args and that their
    // types are compatible with TABLE1 and TABLE2
    if (new_args.size() != Size(2)) {
      GUM_ERROR(SizeError,
                "Method ScheduleBinaryCombination::updateArgs expects 2 new "
                   << "arguments, but " << new_args.size() << " were passed.");
    }
    const ScheduleMultiDim< TABLE1, ALLOC >* arg1;
    const ScheduleMultiDim< TABLE2, ALLOC >* arg2;
    try {
      arg1 = dynamic_cast< const ScheduleMultiDim< TABLE1, ALLOC >* >(new_args[0]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the first argument passed to "
                   << "ScheduleBinaryCombination::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }
    try {
      arg2 = dynamic_cast< const ScheduleMultiDim< TABLE2, ALLOC >* >(new_args[1]);
    } catch (std::bad_cast&) {
      GUM_ERROR(TypeError,
                "The type of the second argument passed to "
                   << "ScheduleBinaryCombination::updateArgs does not match what "
                   << "the ScheduleOperation expects");
    }

    // save the new arguments
    _arg1_ = arg1;
    _arg2_ = arg2;
    _args_ = {_arg1_, _arg2_};

    // now the result is obsolete, so make it abstract
    _result_->makeAbstract();
  }


  /// indicates whether the operation has been executed
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE bool
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::isExecuted()
        const {
    return !_result_->isAbstract();
  }


  /// executes the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::execute() {
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
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::undo() {
    _result_->makeAbstract();
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  double
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::nbOperations()
        const {
    return double(_result_->domainSize());
  }


  /// returns the memory consumption used during the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  INLINE std::pair< double, double >
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::memoryUsage()
        const {
    const double domsize =
       double(_result_->domainSize()) * _result_->sizeOfContent();
    return {domsize, domsize};
  }


  /// displays the content of the operation
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  std::string
     ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::toString()
        const {
    return _result_->toString() + " = combine ( " + _arg1_->toString() + " , "
           + _arg2_->toString() + " )";
  }


  /// use a new combination function
  template < typename TABLE1,
             typename TABLE2,
             typename TABLE_RES,
             template < typename >
             class ALLOC >
  void ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >::
     setCombinationFunction(TABLE_RES (*combine)(const TABLE1&, const TABLE2&)) {
    _combine_ = combine;
    _result_->makeAbstract();
  }


}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
