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
 * @brief Class of gum::Signaler3.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_SIGNALER3_H
#define GUM_SIGNALER3_H

#include <agrum/tools/core/signal/signaler.h>

#define MAKE_NAME(nom)    nom##3
#define LIST_DECL_CLASSES class type1, class type2, class type3
#define LIST_CLASSES      type1, type2, type3
#define LIST_DECL_ARGS    type1 arg1, type2 arg2, type3 arg3
#define LIST_ARGS         arg1, arg2, arg3

#define GUM_EMIT3(signal, arg1, arg2, arg3) this->signal(this, arg1, arg2, arg3)

#define SIGNALER_PATRON_ACCEPTED
#include <agrum/tools/core/signal/signaler_with_args.pattern.h>

#endif   // GUM_SIGNALER3_H
