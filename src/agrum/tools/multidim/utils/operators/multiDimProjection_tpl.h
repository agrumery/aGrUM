/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief A generic interface to project efficiently a MultiDim table over a
 * subset of its variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

namespace gum {

  /// constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::MultiDimProjection(TABLE< GUM_SCALAR > (
     *proj)(const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&)) :
      proj_(proj) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimProjection);
  }

  /// copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::MultiDimProjection(
     const MultiDimProjection< GUM_SCALAR, TABLE >& from) :
      proj_(from.proj_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimProjection);
  }

  /// destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::~MultiDimProjection() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimProjection);
  }

  /// virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >*
     MultiDimProjection< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimProjection< GUM_SCALAR, TABLE >(*this);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* MultiDimProjection< GUM_SCALAR, TABLE >::execute(
     const TABLE< GUM_SCALAR >&            table,
     const Set< const DiscreteVariable* >& del_vars) const {
    return nbOperations(table, del_vars) != 0.0
       ? new TABLE< GUM_SCALAR >(proj_(table, del_vars))
          : const_cast< TABLE< GUM_SCALAR >* >(&table);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void MultiDimProjection< GUM_SCALAR, TABLE >::execute(
     TABLE< GUM_SCALAR >&                     container,
     const TABLE< GUM_SCALAR >&               table,
     const Set< const DiscreteVariable* >& del_vars) const {
    container = nbOperations(table, del_vars) != 0.0
       ? proj_(table, del_vars) : table;
  }

  /// returns the set of operations to perform as well as the result of the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE std::pair< ScheduleOperation<>*, const IScheduleMultiDim<>* >
     MultiDimProjection< GUM_SCALAR, TABLE >::operations(
        const IScheduleMultiDim<>*            table,
        const Set< const DiscreteVariable* >& del_vars) const {
    auto op = new ScheduleProjection< TABLE< GUM_SCALAR > >(
       dynamic_cast< const ScheduleMultiDim< TABLE< GUM_SCALAR > >&>(*table),
       del_vars, proj_);
    return { op, &op->result() };
  }

  /// add to a given schedule the set of operations needed to perform the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE const IScheduleMultiDim<>*
     MultiDimProjection< GUM_SCALAR, TABLE >::schedule(
                  Schedule<>& schedule,
                  const IScheduleMultiDim<>* table,
                  const Set< const DiscreteVariable* >& del_vars) const {
    const ScheduleMultiDim< TABLE< GUM_SCALAR > >& xtable =
       dynamic_cast< const ScheduleMultiDim< TABLE< GUM_SCALAR > >& > (*table);
    const auto op = schedule.template emplaceProjection(xtable, del_vars, proj_);
    return op->results()[0];
  }


  /// changes the function used for projecting TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  void MultiDimProjection< GUM_SCALAR, TABLE >::setProjectionFunction(TABLE< GUM_SCALAR > (
     *proj)(const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&)) {
    proj_ = proj;
  }

  /// returns the projection function currently used by the projector
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR > (*MultiDimProjection< GUM_SCALAR, TABLE >::projectionFunction())(
     const TABLE< GUM_SCALAR >&,
     const Set< const DiscreteVariable* >&) {
    return proj_;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE double MultiDimProjection< GUM_SCALAR, TABLE >::nbOperations(
     const TABLE< GUM_SCALAR >&            table,
     const Set< const DiscreteVariable* >& del_vars) const {
    bool has_del_vars = false;
    const auto& vars = table.variablesSequence();
    for (const auto var: del_vars) {
      if (vars.exists(var)) {
        has_del_vars = true;
        break;
      }
    }
    return has_del_vars ? double(table.domainSize()) : 0.0;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  double MultiDimProjection< GUM_SCALAR, TABLE >::nbOperations(
     const Sequence< const DiscreteVariable* >& vars,
     const Set< const DiscreteVariable* >&      del_vars) const {
    double res = 1.0;
    bool has_del_vars = false;

    for (const auto var: vars) {
      res *= var->domainSize();
      if (del_vars.exists(var)) has_del_vars = true;
    }

    return has_del_vars ? res : 0.0;
  }

  /// returns the memory consumption used during the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< double, double > MultiDimProjection< GUM_SCALAR, TABLE >::memoryUsage(
     const Sequence< const DiscreteVariable* >& vars,
     const Set< const DiscreteVariable* >&      del_vars) const {
    double res = 1.0;
    bool has_del_vars = false;
    for (const auto var: vars) {
      if (!del_vars.contains(var)) {
        if (std::numeric_limits< double >::max() / var->domainSize() < res) {
          GUM_ERROR(OutOfBounds, "memory usage too high")
        }

        res *= var->domainSize();
      }
      else {
        has_del_vars = true;
      }
    }

    if (has_del_vars)
      res *= sizeof(GUM_SCALAR);
    else
      res = 0.0;
    return {res, res};
  }

  /// returns the memory consumption used during the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE std::pair< double, double > MultiDimProjection< GUM_SCALAR, TABLE >::memoryUsage(
     const TABLE< GUM_SCALAR >&            table,
     const Set< const DiscreteVariable* >& del_vars) const {
    return memoryUsage(table.variablesSequence(), del_vars);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
