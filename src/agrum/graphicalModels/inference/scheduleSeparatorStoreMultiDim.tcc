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
 * @brief an operator used by scheduled inferences to store tables into separators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <sstream>
#include <agrum/config.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::ScheduleSeparatorStoreMultiDim(
      const ScheduleMultiDim<GUM_SCALAR> &table,
      ArcProperty<Set<const MultiDimImplementation<GUM_SCALAR> *>> &separator_tables,
      Arc separator)
      : ScheduleOperation<GUM_SCALAR>(
            ScheduleOperation<GUM_SCALAR>::Type::SEPARATOR_STORE_MULTIDIM),
        __table(table), __tableSet(&separator_tables), __separator(separator),
        __args(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleSeparatorStoreMultiDim);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::ScheduleSeparatorStoreMultiDim(
      const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &from)
      : ScheduleOperation<GUM_SCALAR>(from), __table(from.__table),
        __tableSet(from.__tableSet), __separator(from.__separator), __args(0) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleSeparatorStoreMultiDim);
  }

  /// virtual copy constructor: creates a clone of the operation
  template <typename GUM_SCALAR>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR> *
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::newFactory() const {
    return new ScheduleSeparatorStoreMultiDim<GUM_SCALAR>(*this);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::~ScheduleSeparatorStoreMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleSeparatorStoreMultiDim);

    if (__args)
      delete __args;
  }

  /// copy operator
  template <typename GUM_SCALAR>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &
      ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::
      operator=(const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &from) {
    // avoid self assignment
    if (&from != this) {
      ScheduleOperation<GUM_SCALAR>::operator=(from);
      __table = from.__table;
      __tableSet = from.__tableSet;
      __separator = from.__separator;

      if (__args) {
        __args->clear();
        __args->insert(&__table);
      }
    }

    return *this;
  }

  /// operator ==
  template <typename GUM_SCALAR>
  bool ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::
  operator==(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return false;

    const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &real_op =
        static_cast<const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &>(op);
    return ((__table == real_op.__table) && (__tableSet == real_op.__tableSet) &&
            (__separator == real_op.__separator));
  }

  /// operator !=
  template <typename GUM_SCALAR>
  bool ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::
  operator!=(const ScheduleOperation<GUM_SCALAR> &op) const {
    if (this->type() != op.type())
      return true;

    const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &real_op =
        static_cast<const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &>(op);
    return ((__table != real_op.__table) || (__tableSet != real_op.__tableSet) ||
            (__separator != real_op.__separator));
  }

  /// executes the operation
  template <typename GUM_SCALAR>
  void ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::execute() {
    const MultiDimImplementation<GUM_SCALAR> &multidim = __table.multiDim();

    if (!__tableSet->exists(__separator)) {
      __tableSet->insert(__separator,
                         Set<const MultiDimImplementation<GUM_SCALAR> *>());
    }

    __tableSet->operator[](__separator).insert(&multidim);
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename GUM_SCALAR>
  INLINE float ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::nbOperations() const {
    return 1.0f;
  }

  /// returns the memory consumption used during the operation
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long>
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::memoryUsage() const {
    return std::pair<long, long>(0, 0);
  }

  /// returns the multidim to be stored
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::multiDimArgs() const {
    if (!__args) {
      __args = new Sequence<const ScheduleMultiDim<GUM_SCALAR> *>;
      __args->insert(&__table);
    }

    return *__args;
  }

  /// returns the set of multidims that should be the result of the operation
  template <typename GUM_SCALAR>
  INLINE const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
  ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::multiDimResults() const {
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
    }

#endif /* NDEBUG */
    return empty_seq;
  }

  /// displays the content of the operation
  template <typename GUM_SCALAR>
  std::string ScheduleSeparatorStoreMultiDim<GUM_SCALAR>::toString() const {
    std::stringstream s;
    s << "store ( " << __table.toString() << ", separator " << __separator << " )";
    return s.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
