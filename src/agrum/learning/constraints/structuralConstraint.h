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
 * @brief the base class for all structural constraints
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_H

#include <agrum/learning/constraints/structuralConstraintSetStatic.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintEmpty
     * @brief the base class for all structural constraints
     * @ingroup learning_group
     */
    struct StructuralConstraintEmpty {

      using allConstraints = __ConstraintSet<void>;
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_H */
