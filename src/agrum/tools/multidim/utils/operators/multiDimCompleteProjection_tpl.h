/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief A generic class to project efficiently a MultiDim table over all
 * of its variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

namespace gum {

  // constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCompleteProjection< GUM_SCALAR, TABLE >::MultiDimCompleteProjection(
     GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&, Instantiation*)) :
      proj_(proj) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCompleteProjection);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCompleteProjection< GUM_SCALAR, TABLE >::MultiDimCompleteProjection(
     const MultiDimCompleteProjection< GUM_SCALAR, TABLE >& from) :
      proj_(from.proj_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCompleteProjection);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCompleteProjection< GUM_SCALAR, TABLE >::~MultiDimCompleteProjection() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCompleteProjection);
  }

  // virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCompleteProjection< GUM_SCALAR, TABLE >*
     MultiDimCompleteProjection< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimCompleteProjection< GUM_SCALAR, TABLE >(*this);
  }

  // creates and returns the projection of the table over a subset of its vars
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE GUM_SCALAR
     MultiDimCompleteProjection< GUM_SCALAR, TABLE >::project(const TABLE< GUM_SCALAR >& table,
                                                              Instantiation*             inst) {
    return proj_(table, inst);
  }

  // changes the function used for projecting TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  void MultiDimCompleteProjection< GUM_SCALAR, TABLE >::setProjectFunction(
     GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&, Instantiation*)) {
    proj_ = proj;
  }

  // returns the projection function currently used by the projector
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE GUM_SCALAR (*MultiDimCompleteProjection< GUM_SCALAR, TABLE >::projectFunction())(
     const TABLE< GUM_SCALAR >&,
     Instantiation*) {
    return proj_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
