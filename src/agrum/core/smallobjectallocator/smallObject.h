
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
 * @brief Headers of gum::SmallObject
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_SMALL_OBJECT_H
#define GUM_SMALL_OBJECT_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================
#define DEFAULT_CHUNK_SIZE 4096
#define MAX_SMALL_OBJECT_SIZE 64
// ============================================================================

namespace gum {
  /**
   * @class SmallObjectAllocator smallObjectAllocator.h <agrum/core/smallObjectAllocator.h>
   *
   * @brief Allocates objects of any size
   *
   * SmallObjectAllocator does so by aggregating several FixedAllocator objects.
   * When SmallObjectAllocator receives an allocation request, it either forwards
   * it to the best matching FixedAllocator object or passes it to the default
   * operator new
   *
   * @ingroup core
   */

  class SmallObject {

    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

        // ============================================================================
        /// Constructor.
        // ============================================================================
        SmallObject();

        // ============================================================================
        /// Destructor.
        // ============================================================================
        virtual ~SmallObject();

      /// @}

      // ############################################################################
      /// @name Operators New & Delete
      // ############################################################################
      /// @{

        // ============================================================================
        /// Overprint of operator new so that it uses small object allocation system
        // ============================================================================
        static void* operator new(std::size_t size);

        // ============================================================================
        /// Overprint of operator delete so that it uses small object allocation system
        /// No need to mention size when using delete.
        /// Compiler will do the work for you.
        // ============================================================================
        static void operator delete(void* p, std::size_t size);

      /// @}
  };
} // namespace gum

#endif // GUM_SMALL_OBJECT_H
