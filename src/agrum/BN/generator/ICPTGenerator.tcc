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
 * @brief Source implementation of gumCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/BN/generator/ICPTGenerator.h>

namespace gum {

  // Default constructor.
  template <typename GUM_SCALAR> ICPTGenerator<GUM_SCALAR>::ICPTGenerator() {
    GUM_CONSTRUCTOR(ICPTGenerator);
  }

  // Destructor.
  template <typename GUM_SCALAR> ICPTGenerator<GUM_SCALAR>::~ICPTGenerator() {
    GUM_DESTRUCTOR(ICPTGenerator);
  }

} /* namespace gum */
