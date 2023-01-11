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
 * @brief
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_TUPLE_HASH_FUNC_H
#define GUM_TUPLE_HASH_FUNC_H

#include <agrum/tools/core/hashFunc.h>
#include <tuple>

namespace gum {

  /// the hash function for tuple (unsigned int, unsigned int,unsigned int)
  template <>
  class HashFunc< std::tuple< unsigned int, unsigned int, unsigned int > >:
      public HashFuncBase< std::tuple< unsigned int, unsigned int, unsigned int > > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const std::tuple< unsigned int, unsigned int, unsigned int >& key);

    /// computes the hashed value of a key
    virtual Size operator()(
       const std::tuple< unsigned int, unsigned int, unsigned int >& key) const override final;
  };
}   // namespace gum

#endif
