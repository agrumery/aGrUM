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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>
#include <agrum/config.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleProject<GUM_SCALAR>::ScheduleProject(
      const ScheduleMultiDim<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      MultiDimImplementation<GUM_SCALAR> *(*project)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const Set<const DiscreteVariable *> &))
      : ScheduleOperation<GUM_SCALAR>(
            ScheduleOperation<GUM_SCALAR>::Type::PROJECT_MULTIDIM),
        __table(table), __del_vars(del_vars), __args(0), __results(0),
        __project(project) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProject);

    // compute the variables that shall belong to the result of the projection
    Sequence<const DiscreteVariable *> vars = __table.variablesSequence();

    for (const auto var : del_vars)
      vars.erase(var);

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    __result = new ScheduleMultiDim<GUM_SCALAR>(vars);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleProject<GUM_SCALAR>::ScheduleProject(const ScheduleProject<GUM_SCALAR> &f)
      : ScheduleOperation<GUM_SCALAR>(f), __table(f.__table),
        __del_vars(f.__del_vars),
        __result(new ScheduleMultiDim<GUM_SCALAR>(*(f.__result))), __args(0),
        __results(0), __project(f.__project) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleProject);
  }

  /// virtual copy constructor: creates a clone of the operation
  template <typename GUM_SCALAR>
  ScheduleProject<GUM_SCALAR> *ScheduleProject<GUM_SCALAR>::newFactory() const {
    return new ScheduleProject<GUM_SCALAR>(*this);
  }

  /// destructor
  template <typename GUM_SCALAR> ScheduleProject<GUM_SCALAR>::~ScheduleProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleProject);
    delete __result;

    if (__args)
      delete __args;

    if (__results)
      delete __results;
  }

  /// copy operator
  template <typename GUM_SCALAR>
  ScheduleProject<GUM_SCALAR> &ScheduleProject<GUM_SCALAR>::
  operator=(const ScheduleProject<GUM_SCALAR> &from) {
    // avoid self assignment
    if (this != &from) {
      ScheduleOperation<GUM_SCALAR>::operator=(from);
      __table = from.__table;
      __del_vars = from.__del_vars;
      *__result = *(from.__result);
      __project = from.__project;

      // update __args and __results if they were already created
      if (__args) {
        __args->clear();
        __args->insert(&__table);
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
  bool ScheduleProject<GUM_SCALAR>::
  operator==(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return false;

    const ScheduleProject<GUM_SCALAR> &real_op =
        static_cast<const ScheduleProject<GUM_SCALAR> &>(op);
    return ((__table == real_op.__table) && (__del_vars == real_op.__del_vars) &&
            (__project == real_op.__project));
  }

  /// operator !=
  template <typename GUM_SCALAR>
  bool ScheduleProject<GUM_SCALAR>::
  operator!=(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return true;

    const ScheduleProject<GUM_SCALAR> &real_op =
        static_cast<const ScheduleProject<GUM_SCALAR> &>(op);
    return ((__table != real_op.__table) || (__del_vars != real_op.__del_vars) ||
            (__project != real_op.__project));
  }

  /// executes the operation
  template <typename GUM_SCALAR> void ScheduleProject<GUM_SCALAR>::execute() {
    if (__result->isAbstract()) {
      const MultiDimImplementation<GUM_SCALAR> &t = __table.multiDim();
      MultiDimImplementation<GUM_SCALAR> *res = __project(t, __del_vars);
      __result->setMultiDim(*res);
    }
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename GUM_SCALAR>
  INLINE float ScheduleProject<GUM_SCALAR>::nbOperations() const {
    return __table.domainSize();
  }

  /// returns the memory consumption used during the operation
  template <typename GUM_SCALAR>
  std::pair<long, long> ScheduleProject<GUM_SCALAR>::memoryUsage() const {
    long size = 1;
    const Sequence<const DiscreteVariable *> &seq = __table.variablesSequence();

    for (const auto var : seq)
      if (!__del_vars.contains(var)) {
        if (std::numeric_limits<long>::max() / (long)var->domainSize() < size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        size *= var->domainSize();
      }

    return std::pair<long, long>(size, size);
  }

  /// returns the scheduleMultidim resulting from the execution of the operation
  template <typename GUM_SCALAR>
  INLINE const ScheduleMultiDim<GUM_SCALAR> &
  ScheduleProject<GUM_SCALAR>::result() const {
    return *__result;
  }

  /// returns the set of multidims passed in argument to the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleProject<GUM_SCALAR>::multiDimArgs() const {
    if (!__args) {
      __args = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __args->insert(&__table);
    }

    return *__args;
  }

  /// returns the set of multidims that should be the result of the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleProject<GUM_SCALAR>::multiDimResults() const {
    if (!__results) {
      __results = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __results->insert(__result);
    }

    return *__results;
  }

  /// displays the content of the operation
  template <typename GUM_SCALAR>
  std::string ScheduleProject<GUM_SCALAR>::toString() const {
    return __result->toString() + " = project ( " + __table.toString() + " , " +
           __del_vars.toString() + " )";
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
