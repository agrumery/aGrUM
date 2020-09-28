
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
 * @brief a MultiDim Delete operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

// to ease ide parser
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeleteMultiDim.h>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::ScheduleDeleteMultiDim(
     const ScheduleMultiDim< GUM_SCALAR >& table) :
      ScheduleOperation< GUM_SCALAR >(
         ScheduleOperation< GUM_SCALAR >::Type::DELETE_MULTIDIM),
      table__(table), args__(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleDeleteMultiDim);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::ScheduleDeleteMultiDim(
     const ScheduleDeleteMultiDim< GUM_SCALAR >& from) :
      ScheduleOperation< GUM_SCALAR >(from),
      table__(from.table__), args__(0) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleDeleteMultiDim);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >*
     ScheduleDeleteMultiDim< GUM_SCALAR >::newFactory() const {
    return new ScheduleDeleteMultiDim< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >::~ScheduleDeleteMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleDeleteMultiDim);

    if (args__) delete args__;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleDeleteMultiDim< GUM_SCALAR >&
     ScheduleDeleteMultiDim< GUM_SCALAR >::operator=(
        const ScheduleDeleteMultiDim< GUM_SCALAR >& from) {
    // avoid self assignment
    if (&from != this) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      table__ = from.table__;

      if (args__) {
        args__->clear();
        args__->insert(&table__);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  bool ScheduleDeleteMultiDim< GUM_SCALAR >::operator==(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleDeleteMultiDim< GUM_SCALAR >& real_op =
       static_cast< const ScheduleDeleteMultiDim< GUM_SCALAR >& >(op);
    return table__ == real_op.table__;
  }

  /// operator !=
  template < typename GUM_SCALAR >
  bool ScheduleDeleteMultiDim< GUM_SCALAR >::operator!=(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return true;

    const ScheduleDeleteMultiDim< GUM_SCALAR >& real_op =
       static_cast< const ScheduleDeleteMultiDim< GUM_SCALAR >& >(op);
    return table__ != real_op.table__;
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleDeleteMultiDim< GUM_SCALAR >::execute() {
    const MultiDimImplementation< GUM_SCALAR >& multidim = table__.multiDim();
    ScheduleMultiDim< GUM_SCALAR >::multidim2id__().erase(&multidim);
    ScheduleMultiDim< GUM_SCALAR >::id2multidim__().erase(table__.id());
    delete &multidim;
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float ScheduleDeleteMultiDim< GUM_SCALAR >::nbOperations() const {
    return 1.0f;
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  INLINE std::pair< long, long >
         ScheduleDeleteMultiDim< GUM_SCALAR >::memoryUsage() const {
    long size_table = long(table__.domainSize());

    if (size_table < 0) {
      GUM_ERROR(OutOfBounds, "memory usage out of long int range");
    }

    return std::pair< long, long >(-size_table, -size_table);
  }

  /// returns the multidims to be deleted
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleDeleteMultiDim< GUM_SCALAR >::multiDimArgs() const {
    if (!args__) {
      args__ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      args__->insert(&table__);
    }

    return *args__;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleDeleteMultiDim< GUM_SCALAR >::multiDimResults() const {
    static Sequence< const ScheduleMultiDim< GUM_SCALAR >* > empty_seq;
#  ifdef GUM_DEBUG_MODE
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static sequence used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __debug__::inc_deletion__(
         "Sequence", __FILE__, __LINE__, "destructor of", (void*)&empty_seq);
      __debug__::inc_deletion__("SequenceImplementation",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
      __debug__::inc_deletion__(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)&empty_seq);
      __debug__::inc_deletion__("SequenceIteratorSafe",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
      __debug__::inc_deletion__("SequenceIteratorSafe",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&empty_seq);
    }

#  endif /* GUM_DEBUG_MODE */
    return empty_seq;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleDeleteMultiDim< GUM_SCALAR >::toString() const {
    return "delete ( " + table__.toString() + " )";
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
