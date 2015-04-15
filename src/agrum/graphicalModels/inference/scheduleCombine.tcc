/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief a Combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>
#include <agrum/config.h>
#include <agrum/graphicalModels/inference/scheduleCombine.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleCombine<GUM_SCALAR>::ScheduleCombine(
      const ScheduleMultiDim<GUM_SCALAR> &table1,
      const ScheduleMultiDim<GUM_SCALAR> &table2,
      MultiDimImplementation<GUM_SCALAR> *(*combine)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const MultiDimImplementation<GUM_SCALAR> &))
      : ScheduleOperation<GUM_SCALAR>(
            ScheduleOperation<GUM_SCALAR>::Type::COMBINE_MULTIDIM),
        __table1(table1), __table2(table2), __args(0), __results(0),
        __combine(combine) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleCombine);

    // compute the variables of the resulting table
    Sequence<const DiscreteVariable *> vars = __table1.variablesSequence();
    const Sequence<const DiscreteVariable *> &vars2 = __table2.variablesSequence();

    for (typename Sequence<const DiscreteVariable *>::const_iterator_safe iter =
             vars2.beginSafe();
         iter != vars2.endSafe(); ++iter) {
      if (!vars.exists(*iter)) {
        vars.insert(*iter);
      }
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    __result = new ScheduleMultiDim<GUM_SCALAR>(vars);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleCombine<GUM_SCALAR>::ScheduleCombine(
      const ScheduleCombine<GUM_SCALAR> &from)
      : ScheduleOperation<GUM_SCALAR>(from), __table1(from.__table1),
        __table2(from.__table2),
        __result(new ScheduleMultiDim<GUM_SCALAR>(*(from.__result))), __args(0),
        __results(0), __combine(from.__combine) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleCombine);
  }

  /// virtual copy constructor: creates a clone of the operation
  template <typename GUM_SCALAR>
  ScheduleCombine<GUM_SCALAR> *ScheduleCombine<GUM_SCALAR>::newFactory() const {
    return new ScheduleCombine<GUM_SCALAR>(*this);
  }

  /// destructor
  template <typename GUM_SCALAR> ScheduleCombine<GUM_SCALAR>::~ScheduleCombine() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleCombine);
    delete __result;

    if (__args)
      delete __args;

    if (__results)
      delete __results;
  }

  /// copy operator
  template <typename GUM_SCALAR>
  ScheduleCombine<GUM_SCALAR> &ScheduleCombine<GUM_SCALAR>::
  operator=(const ScheduleCombine<GUM_SCALAR> &from) {
    // avoid self assignment
    if (this != &from) {
      ScheduleOperation<GUM_SCALAR>::operator=(from);
      __table1 = from.__table1;
      __table2 = from.__table2;
      *__result = *(from.__result);
      __combine = from.__combine;

      // update __args and __results if they were already created
      if (__args) {
        __args->clear();
        __args->insert(&__table1);
        __args->insert(&__table2);
      }

      if (__results) {
        __results->clear();
        __results->insert(__result);
      }
    }

    return *this;
  }

  /// operator ==
  template <typename GUM_SCALAR>
  INLINE bool ScheduleCombine<GUM_SCALAR>::
  operator==(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return false;

    const ScheduleCombine<GUM_SCALAR> &real_op =
        static_cast<const ScheduleCombine<GUM_SCALAR> &>(op);
    return ((((__table1 == real_op.__table1) && (__table2 == real_op.__table2)) ||
             ((__table1 == real_op.__table2) && (__table2 == real_op.__table1))) &&
            (__combine == real_op.__combine));
  }

  /// operator !=
  template <typename GUM_SCALAR>
  INLINE bool ScheduleCombine<GUM_SCALAR>::
  operator!=(const ScheduleOperation<GUM_SCALAR> &op) const {
    return !operator==(op);
  }

  /// executes the operation
  template <typename GUM_SCALAR> void ScheduleCombine<GUM_SCALAR>::execute() {
    if (__result->isAbstract()) {
      // first, get the multidims to combine
      const MultiDimImplementation<GUM_SCALAR> &t1 = __table1.multiDim();
      const MultiDimImplementation<GUM_SCALAR> &t2 = __table2.multiDim();

      // perform the combination and store the result
      MultiDimImplementation<GUM_SCALAR> *res = __combine(t1, t2);
      __result->setMultiDim(*res);
    }
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename GUM_SCALAR>
  float ScheduleCombine<GUM_SCALAR>::nbOperations() const {
    const Sequence<const DiscreteVariable *> &seq1 = __table1.variablesSequence();
    const Sequence<const DiscreteVariable *> &seq2 = __table2.variablesSequence();

    if (seq1.empty() && seq2.empty())
      return 0.0f;

    float size = 1;

    for (const auto var : seq1)
      size *= var->domainSize();

    for (const auto var : seq2)
      if (!seq1.exists(var))
        size *= var->domainSize();

    return size;
  }

  /// returns the memory consumption used during the operation
  template <typename GUM_SCALAR>
  std::pair<long, long> ScheduleCombine<GUM_SCALAR>::memoryUsage() const {
    const Sequence<const DiscreteVariable *> &seq1 = __table1.variablesSequence();
    const Sequence<const DiscreteVariable *> &seq2 = __table2.variablesSequence();

    if (seq1.empty() && seq2.empty())
      return std::pair<long, long>(0, 0);

    long size = 1;

    for (const auto var : seq1) {
      if (std::numeric_limits<long>::max() / (long)var->domainSize() < size) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range");
      }

      size *= var->domainSize();
    }

    for (const auto var : seq2)
      if (!seq1.exists(var)) {
        if (std::numeric_limits<long>::max() / (long)var->domainSize() < size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        size *= var->domainSize();
      }

    return std::pair<long, long>(size, size);
  }

  /// returns the set of multidims passed in argument to the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleCombine<GUM_SCALAR>::multiDimArgs() const {
    if (!__args) {
      __args = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __args->insert(&__table1);
      __args->insert(&__table2);
    }

    return *__args;
  }

  /// returns the set of multidims that should be the result of the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleCombine<GUM_SCALAR>::multiDimResults() const {
    if (!__results) {
      __results = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __results->insert(__result);
    }

    return *__results;
  }

  /// displays the content of the operation
  template <typename GUM_SCALAR>
  std::string ScheduleCombine<GUM_SCALAR>::toString() const {
    return __result->toString() + " = combine ( " + __table1.toString() + " , " +
           __table2.toString() + " )";
  }

  /// returns the scheduleMultidim resulting from the execution of the operation
  template <typename GUM_SCALAR>
  INLINE const ScheduleMultiDim<GUM_SCALAR> &
  ScheduleCombine<GUM_SCALAR>::result() const {
    return *__result;
  }

} /* namespace */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
