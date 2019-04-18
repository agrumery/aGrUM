
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


/**
 * @file
 * @brief Class of gum::Signaler4.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#ifndef GUM_SIGNALER4_H
#define GUM_SIGNALER4_H

#include <agrum/core/signal/signaler.h>

#define MAKE_NAME(nom) nom##4
#define LIST_DECL_CLASSES class type1, class type2, class type3, class type4
#define LIST_CLASSES type1, type2, type3, type4
#define LIST_DECL_ARGS type1 arg1, type2 arg2, type3 arg3, type4 arg4
#define LIST_ARGS arg1, arg2, arg3, arg4

#define GUM_EMIT4(signal, arg1, arg2, arg3, arg4) \
  this->signal(this, arg1, arg2, arg3, arg4)

#define SIGNALER_PATRON_ACCEPTED
#include <agrum/core/signal/signaler_with_args.pattern.h>

#endif   // GUM_SIGNALER4_H
