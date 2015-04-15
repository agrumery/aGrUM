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
 * @brief Template implementation of idSets
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <tuple>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename Alloc> INLINE IdSet<Alloc>::IdSet() {
      GUM_CONSTRUCTOR(IdSet);
    }

    /// initializer list constructor
    template <typename Alloc>
    INLINE IdSet<Alloc>::IdSet(const std::initializer_list<unsigned int> list) {
      GUM_CONSTRUCTOR(IdSet);

      if (list.size())
        __size = 1;

      __ids.reserve(list.size());

      for (auto i : list) {
        __ids.push_back(i);
        __size *= 2;
      }

      std::sort(__ids.begin(), __ids.end());
    }

    /// default constructor
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE IdSet<Alloc>::IdSet(const std::vector<unsigned int, OtherAlloc> &ids,
                               unsigned int sz)
        : __ids(ids), __size(sz) {
      GUM_CONSTRUCTOR(IdSet);
      std::sort(__ids.begin(), __ids.end());
    }

    /// copy constructor
    template <typename Alloc>
    INLINE IdSet<Alloc>::IdSet(const IdSet<Alloc> &from)
        : __ids(from.__ids), __size(from.__size) {
      GUM_CONS_CPY(IdSet);
    }

    /// generalized copy constructor
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE IdSet<Alloc>::IdSet(const IdSet<OtherAlloc> &from)
        : __size(from.__size) {
      __ids.reserve(from.__ids.size());

      for (auto id : from.__ids) {
        __ids.push_back(id);
      }

      GUM_CONS_CPY(IdSet);
    }

    /// move constructor
    template <typename Alloc>
    INLINE IdSet<Alloc>::IdSet(IdSet<Alloc> &&from)
        : __ids(std::move(from.__ids)), __size(from.__size) {
      GUM_CONS_MOV(IdSet);
    }

    /// destructor
    template <typename Alloc> INLINE IdSet<Alloc>::~IdSet() {
      GUM_DESTRUCTOR(IdSet);
    }

    /// copy operator
    template <typename Alloc>
    INLINE IdSet<Alloc> &IdSet<Alloc>::operator=(const IdSet<Alloc> &from) {
      if (this != &from) {
        __ids = from.__ids;
        __size = from.__size;
      }

      return *this;
    }

    /// generalized copy operator
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE IdSet<Alloc> &IdSet<Alloc>::operator=(const IdSet<OtherAlloc> &from) {
      __ids.clear();

      for (auto id : from.__ids) {
        __ids.push_back(id);
      }

      __size = from.__size;

      return *this;
    }

    /// move operator
    template <typename Alloc>
    INLINE IdSet<Alloc> &IdSet<Alloc>::operator=(IdSet<Alloc> &&from) {
      if (this != &from) {
        __ids = std::move(from.__ids);
        __size = from.__size;
      }

      return *this;
    }

    /// returns the id stored at a given index
    template <typename Alloc>
    INLINE unsigned int IdSet<Alloc>::operator[](unsigned int index) const noexcept {
      return __ids[index];
    }

    /// inserts a new element into the set
    template <typename Alloc>
    INLINE IdSet<Alloc> &IdSet<Alloc>::operator<<(unsigned int id) {
      unsigned int min_index = 0, max_index = __ids.size();
      unsigned int middle;

      // search for id by dichotomy
      while (max_index > min_index) {
        middle = (max_index + min_index) / 2;

        if (__ids[middle] == id)
          return *this;

        if (__ids[middle] < id)
          min_index = middle + 1;
        else
          max_index = middle;
      }

      // here, min_index is he location where id should be inserted
      // it can correspond to end(), i.e., the insertion has to take place
      // after the last element of the vector, but this is OK, the STL vector
      // allows for this operation.
      __ids.insert(__ids.begin() + min_index, id);
      __size *= 2;

      return *this;
    }

    /// returns true if both sets are equal
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE bool IdSet<Alloc>::operator==(const IdSet<OtherAlloc> &from) const
        noexcept {
      unsigned int sz = __ids.size();

      if (sz != from.__ids.size())
        return false;

      for (unsigned int i = 0; i < sz; ++i) {
        if (__ids[i] != from.__ids[i])
          return false;
      }

      return true;
    }

    /// returns true if the sets differ
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE bool IdSet<Alloc>::operator!=(const IdSet<OtherAlloc> &from) const
        noexcept {
      return !operator==(from);
    }

    /// returns the set of ids contained in the object
    template <typename Alloc>
    INLINE const std::vector<unsigned int, Alloc> &IdSet<Alloc>::ids() const
        noexcept {
      return __ids;
    }

    /// returns the domain size of the id set
    template <typename Alloc>
    INLINE unsigned int IdSet<Alloc>::size() const noexcept {
      return __size;
    }

    /// sets the domain size of the set
    template <typename Alloc>
    INLINE void IdSet<Alloc>::setSize(unsigned int sz) noexcept {
      __size = sz;
    }

    /// returns the content of the set as a string
    template <typename Alloc> std::string IdSet<Alloc>::toString() const noexcept {
      std::stringstream str;

      str << '{';
      bool deja = false;

      for (unsigned int i = 0; i < __ids.size(); ++i) {
        if (deja)
          str << " , ";
        else
          deja = true;

        str << __ids[i];
      }

      str << '}';

      return str.str();
    }

    /// indicates wether the current object is a subset of 'otherset'
    template <typename Alloc>
    template <typename OtherAlloc>
    INLINE bool IdSet<Alloc>::isSubset(const IdSet<OtherAlloc> &otherset) const
        noexcept {
      unsigned int min_index = 0, max_index = otherset.__ids.size();

      for (unsigned int i = 0, size = __ids.size(); i < size; ++i) {
        unsigned int middle, tmp_max_index = max_index;
        unsigned int nb = __ids[i];

        // search for nb by dichotomy
        while (tmp_max_index > min_index) {
          middle = (tmp_max_index + min_index) / 2;

          if (otherset[middle] == nb) {
            min_index = middle;
            break;
          }

          if (otherset[middle] < nb)
            min_index = middle + 1;
          else
            tmp_max_index = middle;
        }

        // check if we found nb
        if (tmp_max_index <= min_index)
          return false;
      }

      return true;
    }

    // the display operator
    template <typename Alloc>
    std::ostream &operator<<(std::ostream &stream, const IdSet<Alloc> &idset) {
      return stream << idset.toString();
    }

  } /* namespace learning */

  // the hash function for idSets
  template <typename Alloc>
  Size HashFunc<learning::IdSet<Alloc>>::
  operator()(const learning::IdSet<Alloc> &key) const {
    Size h = 0;
    const std::vector<unsigned int, Alloc> &vect = key.ids();

    for (size_t i = 0; i < vect.size(); ++i)
      h += i * vect[i];

    return ((h * HashFuncConst::gold) & this->_hash_mask);
  }

  /// the hash function for pairs (idSet,unsigned int)
  template <typename Alloc>
  Size HashFunc<std::pair<learning::IdSet<Alloc>, unsigned int>>::
  operator()(const std::pair<learning::IdSet<Alloc>, unsigned int> &key) const {
    Size h = 0;
    size_t i;
    const std::vector<unsigned int, Alloc> &vect = key.first.ids();

    for (i = 0; i < vect.size(); ++i)
      h += i * vect[i];

    h *= i * key.second;

    return ((h * HashFuncConst::gold) & this->_hash_mask);
  }

  /// the hash function for pairs (idSet,unsigned int)
  template <typename Alloc>
  Size HashFunc<std::tuple<learning::IdSet<Alloc>, unsigned int, unsigned int>>::
  operator()(const std::tuple<learning::IdSet<Alloc>, unsigned int,
                              unsigned int> &key) const {
    Size h = 0;
    size_t i;
    const std::vector<unsigned int, Alloc> &vect = std::get<0>(key).ids();

    for (i = 0; i < vect.size(); ++i)
      h += i * vect[i];

    h *= i * std::get<1>(key);
    h *= ++i * std::get<2>(key);

    return ((h * HashFuncConst::gold) & this->_hash_mask);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
