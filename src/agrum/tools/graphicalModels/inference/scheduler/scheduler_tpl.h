/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  Scheduler< GUM_SCALAR >::Scheduler() {
    // for debugging purposes
    GUM_CONSTRUCTOR(Scheduler);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  Scheduler< GUM_SCALAR >::Scheduler(const Scheduler< GUM_SCALAR >& from) {
    // for debugging purposes
    GUM_CONS_CPY(Scheduler);
  }

  /// destructor
  template < typename GUM_SCALAR >
  Scheduler< GUM_SCALAR >::~Scheduler() {
    // for debugging purposes
    GUM_DESTRUCTOR(Scheduler);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */