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
 * @brief A generic class to project efficiently a ScheduleMultiDim over a subset
 * of its variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleProjection<GUM_SCALAR>::ScheduleProjection() {
    /// for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProjection);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleProjection<GUM_SCALAR>::ScheduleProjection(
      const ScheduleProjection<GUM_SCALAR> &from) {
    /// for debugging purposes
    GUM_CONS_CPY(ScheduleProjection);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleProjection<GUM_SCALAR>::~ScheduleProjection() {
    /// for debugging purposes
    GUM_DESTRUCTOR(ScheduleProjection);
  }

  /// virtual constructor
  template <typename GUM_SCALAR>
  ScheduleProjection<GUM_SCALAR> *
  ScheduleProjection<GUM_SCALAR>::newFactory() const {
    return new ScheduleProjection<GUM_SCALAR>(*this);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template <typename GUM_SCALAR>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleProjection<GUM_SCALAR>::project(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(table);
    return project(sched_table, del_vars, schedule);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleProjection<GUM_SCALAR>::project(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(*(table.content()));
    return project(sched_table, del_vars, schedule);
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template <typename GUM_SCALAR>
  INLINE float ScheduleProjection<GUM_SCALAR>::nbOperations(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(table);
    return nbOperations(sched_table, del_vars, schedule);
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE float ScheduleProjection<GUM_SCALAR>::nbOperations(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(*(table.content()));
    return nbOperations(sched_table, del_vars, schedule);
  }

  /// returns the memory consumption used during the projection
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long> ScheduleProjection<GUM_SCALAR>::memoryUsage(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(table);
    return memoryUsage(sched_table, del_vars, schedule);
  }

  /// returns the memory consumption used during the projection
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE std::pair<long, long> ScheduleProjection<GUM_SCALAR>::memoryUsage(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    ScheduleMultiDim<GUM_SCALAR> sched_table(*(table.content()));
    return memoryUsage(sched_table, del_vars, schedule);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
