/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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
 * @brief aGrUM's inline/outline selection
 *
 * aGrUM's INLINE is a substitute to the usual C++ inline. It enables compiling
 * aGrUM with or without inline functions, depending on whether we wish to
 * debug parts of aGrUM.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_INLINE_H
#define GUM_INLINE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef GUM_NO_INLINE
#define INLINE inline
#else
#define INLINE
#endif /* GUM_NO_INLINE */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* GUM_INLINE_H */
