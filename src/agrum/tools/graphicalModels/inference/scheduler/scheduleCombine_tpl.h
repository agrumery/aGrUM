
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief a Combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleCombine.h>
#  include <limits>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleCombine< GUM_SCALAR >::ScheduleCombine(
     const ScheduleMultiDim< GUM_SCALAR >& table1,
     const ScheduleMultiDim< GUM_SCALAR >& table2,
     MultiDimImplementation< GUM_SCALAR >* (*combine)(
        const MultiDimImplementation< GUM_SCALAR >&,
        const MultiDimImplementation< GUM_SCALAR >&)) :
      ScheduleOperation< GUM_SCALAR >(
         ScheduleOperation< GUM_SCALAR >::Type::COMBINE_MULTIDIM),
      table1__(table1), table2__(table2), args__(0), results__(0),
      combine__(combine) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleCombine);

    // compute the variables of the resulting table
    Sequence< const DiscreteVariable* >        vars = table1__.variablesSequence();
    const Sequence< const DiscreteVariable* >& vars2 =
       table2__.variablesSequence();

    for (typename Sequence< const DiscreteVariable* >::const_iterator_safe iter =
            vars2.beginSafe();
         iter != vars2.endSafe();
         ++iter) {
      if (!vars.exists(*iter)) { vars.insert(*iter); }
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    result__ = new ScheduleMultiDim< GUM_SCALAR >(vars);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleCombine< GUM_SCALAR >::ScheduleCombine(
     const ScheduleCombine< GUM_SCALAR >& from) :
      ScheduleOperation< GUM_SCALAR >(from),
      table1__(from.table1__), table2__(from.table2__),
      result__(new ScheduleMultiDim< GUM_SCALAR >(*(from.result__))), args__(0),
      results__(0), combine__(from.combine__) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleCombine);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleCombine< GUM_SCALAR >*
     ScheduleCombine< GUM_SCALAR >::newFactory() const {
    return new ScheduleCombine< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleCombine< GUM_SCALAR >::~ScheduleCombine() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleCombine);
    delete result__;

    if (args__) delete args__;

    if (results__) delete results__;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleCombine< GUM_SCALAR >& ScheduleCombine< GUM_SCALAR >::operator=(
     const ScheduleCombine< GUM_SCALAR >& from) {
    // avoid self assignment
    if (this != &from) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      table1__ = from.table1__;
      table2__ = from.table2__;
      *result__ = *(from.result__);
      combine__ = from.combine__;

      // update args__ and results__ if they were already created
      if (args__) {
        args__->clear();
        args__->insert(&table1__);
        args__->insert(&table2__);
      }

      if (results__) {
        results__->clear();
        results__->insert(result__);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  INLINE bool ScheduleCombine< GUM_SCALAR >::operator==(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleCombine< GUM_SCALAR >& real_op =
       static_cast< const ScheduleCombine< GUM_SCALAR >& >(op);
    return ((((table1__ == real_op.table1__) && (table2__ == real_op.table2__))
             || ((table1__ == real_op.table2__) && (table2__ == real_op.table1__)))
            && (combine__ == real_op.combine__));
  }

  /// operator !=
  template < typename GUM_SCALAR >
  INLINE bool ScheduleCombine< GUM_SCALAR >::operator!=(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    return !operator==(op);
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleCombine< GUM_SCALAR >::execute() {
    if (result__->isAbstract()) {
      // first, get the multidims to combine
      const MultiDimImplementation< GUM_SCALAR >& t1 = table1__.multiDim();
      const MultiDimImplementation< GUM_SCALAR >& t2 = table2__.multiDim();

      // perform the combination and store the result
      MultiDimImplementation< GUM_SCALAR >* res = combine__(t1, t2);
      result__->setMultiDim(*res);
    }
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  float ScheduleCombine< GUM_SCALAR >::nbOperations() const {
    const Sequence< const DiscreteVariable* >& seq1 = table1__.variablesSequence();
    const Sequence< const DiscreteVariable* >& seq2 = table2__.variablesSequence();

    if (seq1.empty() && seq2.empty()) return 0.0f;

    float size = 1;

    for (const auto var: seq1)
      size *= var->domainSize();

    for (const auto var: seq2)
      if (!seq1.exists(var)) size *= var->domainSize();

    return size;
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  std::pair< long, long > ScheduleCombine< GUM_SCALAR >::memoryUsage() const {
    const Sequence< const DiscreteVariable* >& seq1 = table1__.variablesSequence();
    const Sequence< const DiscreteVariable* >& seq2 = table2__.variablesSequence();

    if (seq1.empty() && seq2.empty()) return std::pair< long, long >(0, 0);

    long size = 1;

    for (const auto var: seq1) {
      if (std::numeric_limits< long >::max() / (long)var->domainSize() < size) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range");
      }

      size *= long(var->domainSize());
    }

    for (const auto var: seq2)
      if (!seq1.exists(var)) {
        if (std::numeric_limits< long >::max() / (long)var->domainSize() < size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        size *= long(var->domainSize());
      }

    return std::pair< long, long >(size, size);
  }

  /// returns the set of multidims passed in argument to the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleCombine< GUM_SCALAR >::multiDimArgs() const {
    if (!args__) {
      args__ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      args__->insert(&table1__);
      args__->insert(&table2__);
    }

    return *args__;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleCombine< GUM_SCALAR >::multiDimResults() const {
    if (!results__) {
      results__ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      results__->insert(result__);
    }

    return *results__;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleCombine< GUM_SCALAR >::toString() const {
    return result__->toString() + " = combine ( " + table1__.toString() + " , "
           + table2__.toString() + " )";
  }

  /// returns the scheduleMultidim resulting from the execution of the operation
  template < typename GUM_SCALAR >
  INLINE const ScheduleMultiDim< GUM_SCALAR >&
               ScheduleCombine< GUM_SCALAR >::result() const {
    return *result__;
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
