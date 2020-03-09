
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
 * @brief Sources of gum::SmallObjectAllocator
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
// ============================================================================
#include <agrum/tools/core/smallobjectallocator/fixedAllocator.h>
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================
namespace gum {


  /*
   * @variable The default size of chunck of memory.
   * These chuncks are pre-allocated memory space which are
   * then split in small memory space of the size of a small object
   */
  const size_t SmallObjectAllocator::GUM_DEFAULT_CHUNK_SIZE = 8096;

  /*
   * @variable The default maximal size under which an object is considered
   * small. If an object size is over this limit, the normal new allocator is
   * called.
   */
  const size_t SmallObjectAllocator::GUM_DEFAULT_MAX_OBJECT_SIZE = 512;


}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/smallobjectallocator/smallObjectAllocator_inl.h>
#endif
