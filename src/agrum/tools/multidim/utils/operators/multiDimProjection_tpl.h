
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


/**
 * @file
 * @brief A generic interface to project efficiently a MultiDim table over a
 * subset of its variables
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

namespace gum {

  // constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::MultiDimProjection(
     TABLE< GUM_SCALAR >* (*proj)(const TABLE< GUM_SCALAR >&,
                                  const Set< const DiscreteVariable* >&)) :
      proj_(proj) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimProjection);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::MultiDimProjection(
     const MultiDimProjection< GUM_SCALAR, TABLE >& from) :
      proj_(from.proj_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimProjection);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >::~MultiDimProjection() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimProjection);
  }

  // virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimProjection< GUM_SCALAR, TABLE >*
     MultiDimProjection< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimProjection< GUM_SCALAR, TABLE >(*this);
  }

  // creates and returns the projection of the table over a subset of its vars
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* MultiDimProjection< GUM_SCALAR, TABLE >::project(
     const TABLE< GUM_SCALAR >&            table,
     const Set< const DiscreteVariable* >& del_vars) {
    return proj_(table, del_vars);
  }

  // creates and returns the projection of the table over a subset of its vars
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void MultiDimProjection< GUM_SCALAR, TABLE >::project(
     TABLE< GUM_SCALAR >&                     container,
     const TABLE< GUM_SCALAR >&               table,
     const Set< const TABLE< GUM_SCALAR >* >& del_vars) {
    TABLE< GUM_SCALAR >* res = project(table, del_vars);
    container = std::move(*res);
    delete res;
  }

  // changes the function used for projecting TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  void MultiDimProjection< GUM_SCALAR, TABLE >::setProjectFunction(
     TABLE< GUM_SCALAR >* (*proj)(const TABLE< GUM_SCALAR >&,
                                  const Set< const DiscreteVariable* >&)) {
    proj_ = proj;
  }

  // returns the projection function currently used by the projector
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* (
     *MultiDimProjection< GUM_SCALAR, TABLE >::projectFunction())(
     const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&) {
    return proj_;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE float MultiDimProjection< GUM_SCALAR, TABLE >::nbOperations(
     const TABLE< GUM_SCALAR >&            table,
     const Set< const DiscreteVariable* >& del_vars) const {
    return float(table.domainSize());
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  float MultiDimProjection< GUM_SCALAR, TABLE >::nbOperations(
     const Sequence< const DiscreteVariable* >& vars,
     const Set< const DiscreteVariable* >&      del_vars) const {
    float res = 1.0f;

    for (typename Sequence< const DiscreteVariable* >::const_iterator_safe iter =
            vars.beginSafe();
         iter != vars.endSafe();
         ++iter) {
      res *= (*iter)->domainSize();
    }

    return res;
  }

  // returns the memory consumption used during the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< long, long > MultiDimProjection< GUM_SCALAR, TABLE >::memoryUsage(
     const Sequence< const DiscreteVariable* >& vars,
     const Set< const DiscreteVariable* >&      del_vars) const {
    long res = 1;

    for (typename Sequence< const DiscreteVariable* >::const_iterator_safe iter =
            vars.beginSafe();
         iter != vars.endSafe();
         ++iter) {
      if (!del_vars.contains(*iter)) {
        if (std::numeric_limits< long >::max() / (long)(*iter)->domainSize()
            < res) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        res *= long((*iter)->domainSize());
      }
    }

    return std::pair< long, long >(res, res);
  }

  // returns the memory consumption used during the projection
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE std::pair< long, long >
         MultiDimProjection< GUM_SCALAR, TABLE >::memoryUsage(
        const TABLE< GUM_SCALAR >&            table,
        const Set< const DiscreteVariable* >& del_vars) const {
    return memoryUsage(table.variablesSequence(), del_vars);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
