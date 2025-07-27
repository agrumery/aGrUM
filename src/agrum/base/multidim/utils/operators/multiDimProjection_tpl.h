/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief A generic interface to project efficiently a MultiDim table over a
 * subset of its variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

#  include <agrum/agrum.h>

namespace gum {

  /// constructor
  template < class TABLE >
  MultiDimProjection< TABLE >::MultiDimProjection(TABLE (*proj)(const TABLE&,
                                                                const gum::VariableSet&)) :
      proj_(proj) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimProjection);
  }

  /// copy constructor
  template < class TABLE >
  MultiDimProjection< TABLE >::MultiDimProjection(const MultiDimProjection< TABLE >& from) :
      proj_(from.proj_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimProjection);
  }

  /// destructor
  template < class TABLE >
  MultiDimProjection< TABLE >::~MultiDimProjection() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimProjection);
  }

  /// virtual constructor
  template < class TABLE >
  MultiDimProjection< TABLE >* MultiDimProjection< TABLE >::clone() const {
    return new MultiDimProjection< TABLE >(*this);
  }

  /// creates and returns the projection of the table over a subset of its vars
  template < class TABLE >
  INLINE TABLE* MultiDimProjection< TABLE >::execute(const TABLE&            table,
                                                     const gum::VariableSet& del_vars) const {
    // projected constants are equal to the same constant
    if (table.variablesSequence().empty()) return new TABLE(table);
    else return new TABLE(proj_(table, del_vars));
  }

  /// creates and returns the projection of the table over a subset of its vars
  template < class TABLE >
  INLINE void MultiDimProjection< TABLE >::execute(TABLE&                  container,
                                                   const TABLE&            table,
                                                   const gum::VariableSet& del_vars) const {
    if (table.variablesSequence().empty()) container = table;
    else container = proj_(table, del_vars);
  }

  /// returns the set of operations to perform as well as the result of the projection
  template < class TABLE >
  std::pair< ScheduleOperator*, const IScheduleMultiDim* >
      MultiDimProjection< TABLE >::operations(const IScheduleMultiDim* table,
                                              const gum::VariableSet&  del_vars,
                                              const bool               is_result_persistent) const {
    auto op
        = new ScheduleProjection< TABLE >(dynamic_cast< const ScheduleMultiDim< TABLE >& >(*table),
                                          del_vars,
                                          proj_,
                                          is_result_persistent);
    return {op, &op->result()};
  }

  /// add to a given schedule the set of operations needed to perform the projection
  template < class TABLE >
  INLINE const IScheduleMultiDim*
      MultiDimProjection< TABLE >::schedule(Schedule&                schedule,
                                            const IScheduleMultiDim* table,
                                            const gum::VariableSet&  del_vars,
                                            const bool               is_result_persistent) const {
    const auto& xtable = dynamic_cast< const ScheduleMultiDim< TABLE >& >(*table);
    const auto& op     = schedule.emplaceProjection(xtable, del_vars, proj_, is_result_persistent);
    return op.results()[0];
  }

  /// changes the function used for projecting TABLES
  template < class TABLE >
  void MultiDimProjection< TABLE >::setProjectionFunction(TABLE (*proj)(const TABLE&,
                                                                        const gum::VariableSet&)) {
    proj_ = proj;
  }

  /// returns the projection function currently used by the projector
  template < class TABLE >
  INLINE TABLE (*MultiDimProjection< TABLE >::projectionFunction())(const TABLE&,
                                                                    const gum::VariableSet&) {
    return proj_;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < class TABLE >
  INLINE double MultiDimProjection< TABLE >::nbOperations(const TABLE&            table,
                                                          const gum::VariableSet& del_vars) const {
    return double(table.domainSize());
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < class TABLE >
  INLINE double
      MultiDimProjection< TABLE >::nbOperations(const Sequence< const DiscreteVariable* >& vars,
                                                const gum::VariableSet& del_vars) const {
    double res = 1.0;
    for (const auto var: vars) {
      res *= double(var->domainSize());
    }
    return res;
  }

  /// returns the memory consumption used during the projection
  template < class TABLE >
  INLINE std::pair< double, double >
         MultiDimProjection< TABLE >::memoryUsage(const Sequence< const DiscreteVariable* >& vars,
                                               const gum::VariableSet& del_vars) const {
    auto res = double(sizeof(value_type));

    for (const auto var: vars) {
      if (!del_vars.contains(var)) res *= double(var->domainSize());
    }

    res += double(sizeof(TABLE));
    return {res, res};
  }

  /// returns the memory consumption used during the projection
  template < class TABLE >
  INLINE std::pair< double, double >
         MultiDimProjection< TABLE >::memoryUsage(const TABLE&            table,
                                               const gum::VariableSet& del_vars) const {
    return memoryUsage(table.variablesSequence(), del_vars);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
