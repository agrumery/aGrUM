/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Interface for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS and Christophe GONZALES
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>
#include <agrum/graphs/spanningForest.h>

namespace gum {

  SpanningForest::SpanningForest() { GUM_CONSTRUCTOR(SpanningForest); }

  SpanningForest::~SpanningForest() { GUM_DESTRUCTOR(SpanningForest); }

  SpanningForest::SpanningForest(const SpanningForest &toCopy) {
    GUM_CONS_CPY(SpanningForest);
  }

  SpanningForest &SpanningForest::operator=(const SpanningForest &toCopy) {
    return *this;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
