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
 * @brief a MultiDim Delete operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>
#include <agrum/config.h>

// to ease ide parser
#include <agrum/graphicalModels/inference/scheduleDeleteMultiDim.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleDeleteMultiDim<GUM_SCALAR>::ScheduleDeleteMultiDim(
      const ScheduleMultiDim<GUM_SCALAR> &table)
      : ScheduleOperation<GUM_SCALAR>(
            ScheduleOperation<GUM_SCALAR>::Type::DELETE_MULTIDIM),
        __table(table), __args(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleDeleteMultiDim);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleDeleteMultiDim<GUM_SCALAR>::ScheduleDeleteMultiDim(
      const ScheduleDeleteMultiDim<GUM_SCALAR> &from)
      : ScheduleOperation<GUM_SCALAR>(from), __table(from.__table), __args(0) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleDeleteMultiDim);
  }

  /// virtual copy constructor: creates a clone of the operation
  template <typename GUM_SCALAR>
  ScheduleDeleteMultiDim<GUM_SCALAR> *
  ScheduleDeleteMultiDim<GUM_SCALAR>::newFactory() const {
    return new ScheduleDeleteMultiDim<GUM_SCALAR>(*this);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleDeleteMultiDim<GUM_SCALAR>::~ScheduleDeleteMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleDeleteMultiDim);

    if (__args)
      delete __args;
  }

  /// copy operator
  template <typename GUM_SCALAR>
  ScheduleDeleteMultiDim<GUM_SCALAR> &ScheduleDeleteMultiDim<GUM_SCALAR>::
  operator=(const ScheduleDeleteMultiDim<GUM_SCALAR> &from) {
    // avoid self assignment
    if (&from != this) {
      ScheduleOperation<GUM_SCALAR>::operator=(from);
      __table = from.__table;

      if (__args) {
        __args->clear();
        __args->insert(&__table);
      }
    }

    return *this;
  }

  /// operator ==
  template <typename GUM_SCALAR>
  bool ScheduleDeleteMultiDim<GUM_SCALAR>::
  operator==(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return false;

    const ScheduleDeleteMultiDim<GUM_SCALAR> &real_op =
        static_cast<const ScheduleDeleteMultiDim<GUM_SCALAR> &>(op);
    return __table == real_op.__table;
  }

  /// operator !=
  template <typename GUM_SCALAR>
  bool ScheduleDeleteMultiDim<GUM_SCALAR>::
  operator!=(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return true;

    const ScheduleDeleteMultiDim<GUM_SCALAR> &real_op =
        static_cast<const ScheduleDeleteMultiDim<GUM_SCALAR> &>(op);
    return __table != real_op.__table;
  }

  /// executes the operation
  template <typename GUM_SCALAR> void ScheduleDeleteMultiDim<GUM_SCALAR>::execute() {
    const MultiDimImplementation<GUM_SCALAR> &multidim = __table.multiDim();
    ScheduleMultiDim<GUM_SCALAR>::__multidim2id().erase(&multidim);
    ScheduleMultiDim<GUM_SCALAR>::__id2multidim().erase(__table.id());
    delete &multidim;
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename GUM_SCALAR>
  INLINE float ScheduleDeleteMultiDim<GUM_SCALAR>::nbOperations() const {
    return 1.0f;
  }

  /// returns the memory consumption used during the operation
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long>
  ScheduleDeleteMultiDim<GUM_SCALAR>::memoryUsage() const {
    long size_table = __table.domainSize();

    if (size_table < 0) {
      GUM_ERROR(OutOfBounds, "memory usage out of long int range");
    }

    return std::pair<long, long>(-size_table, -size_table);
  }

  /// returns the multidims to be deleted
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleDeleteMultiDim<GUM_SCALAR>::multiDimArgs() const {
    if (!__args) {
      __args = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __args->insert(&__table);
    }

    return *__args;
  }

  /// returns the set of multidims that should be the result of the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleDeleteMultiDim<GUM_SCALAR>::multiDimResults() const {
    static Sequence<const ScheduleMultiDim<GUM_SCALAR> *> empty_seq;
#ifndef NDEBUG
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static sequence used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::__inc_deletion("Sequence", __FILE__, __LINE__, "destructor of",
                                (void *)&empty_seq);
      __debug__::__inc_deletion("SequenceImplementation", __FILE__, __LINE__,
                                "destructor of", (void *)&empty_seq);
      __debug__::__inc_deletion("HashTable", __FILE__, __LINE__, "destructor of",
                                (void *)&empty_seq);
      __debug__::__inc_deletion("SequenceIteratorSafe", __FILE__, __LINE__,
                                "destructor of", (void *)&empty_seq);
      __debug__::__inc_deletion("SequenceIteratorSafe", __FILE__, __LINE__,
                                "destructor of", (void *)&empty_seq);
    }

#endif /* NDEBUG */
    return empty_seq;
  }

  /// displays the content of the operation
  template <typename GUM_SCALAR>
  std::string ScheduleDeleteMultiDim<GUM_SCALAR>::toString() const {
    return "delete ( " + __table.toString() + " )";
  }

} /* namespace */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
