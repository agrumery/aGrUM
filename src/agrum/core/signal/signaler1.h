
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
 * @brief Class of gum::Signaler1.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#ifndef GUM_SIGNALER1_H
#define GUM_SIGNALER1_H

#include <agrum/core/signal/signaler.h>

#define MAKE_NAME(nom) nom##1
#define LIST_DECL_CLASSES class type1
#define LIST_CLASSES type1
#define LIST_DECL_ARGS type1 arg1
#define LIST_ARGS arg1

#define GUM_EMIT1(signal, arg1) this->signal(this, arg1)

#define SIGNALER_PATRON_ACCEPTED
#include <agrum/core/signal/signaler_with_args.pattern.h>

#endif   // GUM_SIGNALER1_H
