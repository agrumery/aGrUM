
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief A generic class to combine efficiently several ScheduleMultiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// constructor
  template < typename GUM_SCALAR >
  ScheduleCombination< GUM_SCALAR >::ScheduleCombination() {
    /// for debugging purposes
    GUM_CONSTRUCTOR(ScheduleCombination);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleCombination< GUM_SCALAR >::ScheduleCombination(
     const ScheduleCombination< GUM_SCALAR >& from) {
    /// for debugging purposes
    GUM_CONS_CPY(ScheduleCombination);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleCombination< GUM_SCALAR >::~ScheduleCombination() {
    /// for debugging purposes
    GUM_DESTRUCTOR(ScheduleCombination);
  }

  // adds to a given schedule the operations necessary to perform a combination
  template < typename GUM_SCALAR >
  ScheduleMultiDim< GUM_SCALAR > ScheduleCombination< GUM_SCALAR >::combine(
     const Set< const MultiDimImplementation< GUM_SCALAR >* >& set,
     Schedule< GUM_SCALAR >&                                   schedule) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto pot : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*pot));

    // perform the combination
    const ScheduleMultiDim< GUM_SCALAR >& res = combine(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

  // adds to a given schedule the operations necessary to perform a combination
  template < typename GUM_SCALAR >
  template < template < typename > class TABLE >
  ScheduleMultiDim< GUM_SCALAR > ScheduleCombination< GUM_SCALAR >::combine(
     const Set< const TABLE< GUM_SCALAR >* >& set,
     Schedule< GUM_SCALAR >&                  schedule) {
    // first wrap the TABLES into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto tab : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*tab->content()));

    // perform the combination
    const ScheduleMultiDim< GUM_SCALAR >& res = combine(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < typename GUM_SCALAR >
  float ScheduleCombination< GUM_SCALAR >::nbOperations(
     const Set< const MultiDimImplementation< GUM_SCALAR >* >& set,
     const Schedule< GUM_SCALAR >&                             schedule) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto pot : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*pot));

    // perform the combination
    float res = nbOperations(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < typename GUM_SCALAR >
  template < template < typename > class TABLE >
  float ScheduleCombination< GUM_SCALAR >::nbOperations(
     const Set< const TABLE< GUM_SCALAR >* >& set,
     const Schedule< GUM_SCALAR >&            schedule) {
    // first wrap the TABLES into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto tab : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*tab->content()));

    // perform the combination
    float res = nbOperations(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

  /// returns the memory consumption used during the combination
  template < typename GUM_SCALAR >
  std::pair< long, long > ScheduleCombination< GUM_SCALAR >::memoryUsage(
     const Set< const MultiDimImplementation< GUM_SCALAR >* >& set,
     const Schedule< GUM_SCALAR >&                             schedule) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto pot : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*pot));

    // compute the memory consumption of the combination
    std::pair< long, long > res = memoryUsage(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < typename GUM_SCALAR >
  template < template < typename > class TABLE >
  std::pair< long, long > ScheduleCombination< GUM_SCALAR >::memoryUsage(
     const Set< const TABLE< GUM_SCALAR >* >& set,
     const Schedule< GUM_SCALAR >&            schedule) {
    // first wrap the TABLES into ScheduleMultiDims
    Set< const ScheduleMultiDim< GUM_SCALAR >* > sched_set;

    for (const auto tab : set)
      sched_set.insert(new ScheduleMultiDim< GUM_SCALAR >(*tab->content()));

    // compute the memory consumption of the combination
    std::pair< long, long > res = memoryUsage(sched_set, schedule);

    // deallocate the wrappers we just constructed
    for (const auto sched : sched_set)
      delete sched;

    return res;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
