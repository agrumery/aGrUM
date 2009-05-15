/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Headers of the class MultiDimBucket.
 *
 * @author Lionel Torti
 */
// ============================================================================
#ifndef GUM_MULTIDIM_BUCKET_H
#define GUM_MULTIDIM_BUCKET_H
// ============================================================================
#include <agrum-trunk/src/agrum/multidim/multiDimWithOffset.h>
// ============================================================================
namespace gum {
// ============================================================================
/**
 * @class MultiDimBucket
 * @brief A multidim implementation for buckets.
 *
 * This class behaves in two different ways, depending on the value of memory
 * allowed and the size of the resulting multidimensional table. If the
 * table is above the allowed amount of memory then each time a value is asked
 * it is computed at the demand, having a cache for each registered
 * instantiation to not compute several times in a row the same value.
 *
 *
 *
 * @ingroup multidim_group
 */
// ============================================================================
template<typename T_DATA>
class MultiDimBucket : public MultiDimWithOffset<T_DATA> {
  public:

  private:

};
// ============================================================================
} /* namespace gum */
// ============================================================================
#include <agrum/multidim/multiDimBucket.tcc>
// ============================================================================
#endif /* GUM_MULTIDIM_BUCKET_H */
// ============================================================================
