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

#include <limits>
#include <agrum/config.h>
#include <agrum/graphicalModels/inference/scheduleProject.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleProjectionBasic<GUM_SCALAR>::ScheduleProjectionBasic(
      MultiDimImplementation<GUM_SCALAR> *(*proj)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const Set<const DiscreteVariable *> &))
      : ScheduleProjection<GUM_SCALAR>(), _proj(proj) {
    /// for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProjectionBasic);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleProjectionBasic<GUM_SCALAR>::ScheduleProjectionBasic(
      const ScheduleProjectionBasic<GUM_SCALAR> &from)
      : ScheduleProjection<GUM_SCALAR>(from), _proj(from._proj) {
    /// for debugging purposes
    GUM_CONS_CPY(ScheduleProjectionBasic);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleProjectionBasic<GUM_SCALAR>::~ScheduleProjectionBasic() {
    /// for debugging purposes
    GUM_DESTRUCTOR(ScheduleProjectionBasic);
  }

  /// virtual constructor
  template <typename GUM_SCALAR>
  ScheduleProjectionBasic<GUM_SCALAR> *
  ScheduleProjectionBasic<GUM_SCALAR>::newFactory() const {
    return new ScheduleProjectionBasic<GUM_SCALAR>(*this);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template <typename GUM_SCALAR>
  ScheduleMultiDim<GUM_SCALAR> ScheduleProjectionBasic<GUM_SCALAR>::project(
      const ScheduleMultiDim<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      Schedule<GUM_SCALAR> &schedule) {
    // create the projection that will be performed later on
    ScheduleProject<GUM_SCALAR> proj(table, del_vars, _proj);
    schedule.insert(proj);

    return proj.result();
  }

  /// creates and returns the projection of the table over a subset of its vars
  template <typename GUM_SCALAR>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleProjectionBasic<GUM_SCALAR>::project(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::project(table, del_vars, schedule);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleProjectionBasic<GUM_SCALAR>::project(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::project(table, del_vars, schedule);
  }

  /// changes the function used for projecting tables
  template <typename GUM_SCALAR>
  void ScheduleProjectionBasic<GUM_SCALAR>::setProjectFunction(
      MultiDimImplementation<GUM_SCALAR> *(*proj)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const Set<const DiscreteVariable *> &)) {
    _proj = proj;
  }

  /// returns the projection function currently used by the projector
  template <typename GUM_SCALAR>
  INLINE MultiDimImplementation<GUM_SCALAR> *(*ScheduleProjectionBasic<
      GUM_SCALAR>::projectFunction())(const MultiDimImplementation<GUM_SCALAR> &,
                                      const Set<const DiscreteVariable *> &) {
    return _proj;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template <typename GUM_SCALAR>
  float ScheduleProjectionBasic<GUM_SCALAR>::nbOperations(
      const ScheduleMultiDim<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    const Sequence<const DiscreteVariable *> &vars = table.variablesSequence();

    float res = 1.0f;

    for (const auto var : vars)
      res *= var->domainSize();

    return res;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template <typename GUM_SCALAR>
  INLINE float ScheduleProjectionBasic<GUM_SCALAR>::nbOperations(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::nbOperations(table, del_vars, schedule);
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE float ScheduleProjectionBasic<GUM_SCALAR>::nbOperations(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::nbOperations(table, del_vars, schedule);
  }

  /// returns the memory consumption used during the projection
  template <typename GUM_SCALAR>
  std::pair<long, long> ScheduleProjectionBasic<GUM_SCALAR>::memoryUsage(
      const ScheduleMultiDim<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    const Sequence<const DiscreteVariable *> &vars = table.variablesSequence();
    long res = 1;

    for (const auto var : vars)
      if (!del_vars.contains(var)) {
        if (std::numeric_limits<long>::max() / (long)var->domainSize() < res) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        res *= var->domainSize();
      }

    return std::pair<long, long>(res, res);
  }

  /// returns the memory consumption used during the projection
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long> ScheduleProjectionBasic<GUM_SCALAR>::memoryUsage(
      const MultiDimImplementation<GUM_SCALAR> &table,
      const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::memoryUsage(table, del_vars, schedule);
  }

  /// returns the memory consumption used during the projection
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE std::pair<long, long> ScheduleProjectionBasic<GUM_SCALAR>::memoryUsage(
      const TABLE<GUM_SCALAR> &table, const Set<const DiscreteVariable *> &del_vars,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleProjection<GUM_SCALAR>::memoryUsage(table, del_vars, schedule);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
