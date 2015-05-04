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
 * @brief A generic interface to combine and project efficiently MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProject<GUM_SCALAR, TABLE>::MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProject);
  }

  /// copy constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProject<GUM_SCALAR, TABLE>::MultiDimCombineAndProject(
      const MultiDimCombineAndProject<GUM_SCALAR, TABLE> &) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProject);
  }

  /// destructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProject<GUM_SCALAR, TABLE>::~MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProject);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
