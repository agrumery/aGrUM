
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
/**
 * @file
 * @brief Sources of gum::SmallObject
 *
 * @author Jean-Christophe Magnan
 *
 */
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObject.h>
// ============================================================================

namespace gum{
  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Constructor.
    // ============================================================================
    SmallObject::SmallObject(){

    }

    // ============================================================================
    // Destructor.
    // ============================================================================
    SmallObject::~SmallObject(){

    }

  // ############################################################################
  // @name Operators New & Delete
  // ############################################################################
//    typedef SingletonHolder<SmallObjectAllocator> MyAlloc;

    // ============================================================================
    // Overprint of operator new so that it uses small object allocation system
    // ============================================================================
    void* SmallObject::operator new(std::size_t size){
//      return MyAlloc::Instance().allocate(size);
        return NULL;
    }

    // ============================================================================
    // Overprint of operator delete so that it uses small object allocation system
    // ============================================================================
    void SmallObject::operator delete(void* p, std::size_t size){
//      return MyAlloc::Instance().deallocate(p,size);
    }
} // namespace gum
