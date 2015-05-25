/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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
#include <climits>
// WARNING : Do not include this file directlty : instead include <agrum/config.h>

#ifndef GUM_TYPES_H
#define GUM_TYPES_H

namespace gum {

  /// how copies should be performed (usual or shallow copies)
  /// @todo default is ?
  enum class CopyType : char { DEEP_COPY, SHALLOW_COPY };

  /// type for indexes
  typedef unsigned int Idx;

  /// type for ids
  typedef unsigned int Id;

  /** in aGrUM, hashed values are unsigned long int. This is sufficient to deal
   * with Bayes nets and other graphs already big enough to be unmanageable from
   * a computational point of view. */
  typedef unsigned long Size;
#define GUM_MAX_SIZE ULONG_MAX

} /* namespace gum */

#endif /* GUM_TYPES_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
