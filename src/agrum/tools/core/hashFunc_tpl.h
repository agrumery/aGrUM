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
 * @brief Template implementation of the basic hash functions.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to help IDE parser
#include <agrum/tools/core/hashFunc.h>

#include <cstring>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // Update the hash function to take into account a resize of the hash table
  template < typename Key >
  INLINE void HashFuncBase< Key >::resize(const Size new_size) {
    // things work properly only for hashtables with at least 2 elements
    if (new_size < 2) {
      GUM_ERROR(SizeError,
                "the size of the hashtable must be at least 2 but a size of "
                   << new_size << " was provided to the resize function.");
    }

    hash_log2_size_ = _hashTableLog2_(new_size);
    hash_size_      = Size(1) << hash_log2_size_;
    hash_mask_      = hash_size_ - 1;
    right_shift_    = HashFuncConst::offset - hash_log2_size_;
  }

  // Returns the hash table size as known by the hash function
  template < typename Key >
  INLINE Size HashFuncBase< Key >::size() const {
    return hash_size_;
  }

  // ===========================================================================

  // constructor
  template < typename Key >
  INLINE HashFuncSmallKey< Key >::HashFuncSmallKey() {
    static_assert(std::is_integral< Key >::value && sizeof(Key) <= sizeof(Size),
                  "Error: you used HashFuncSmallKey for a key which cannot be "
                  "converted (without narrowing) into a gum::Size");
  }

  // Returns the value of a key as a Size
  template < typename Key >
  INLINE Size HashFuncSmallKey< Key >::castToSize(const Key& key) {
    return Size(key);
  }

  // Returns the hashed value of a key.
  template < typename Key >
  INLINE Size HashFuncSmallKey< Key >::operator()(const Key& key) const {
    return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
  }

  // ===========================================================================

  // constructor
  template < typename Key >
  INLINE HashFuncSmallCastKey< Key >::HashFuncSmallCastKey() {
    static_assert(sizeof(Key) < sizeof(Size),
                  "Error: you used HashFuncSmallCastKey for a key whose size "
                  "is longer than or equal to that of gum::Size");
  }

  // Returns the value of a key as a Size
  template < typename Key >
  INLINE Size HashFuncSmallCastKey< Key >::castToSize(const Key& key) {
    // the code for MVSC differs from the code of the other compilers for
    // speed-up reasons: according to godbolt.org, the first code
    // should be twice faster than the second one
#  ifdef _MSC_VER
    return *((Size*)(&key)) & HashFuncSmallCastKey< Key >::small_key_mask_;
#  else
    Size result = 0;
    memcpy(&result, &key, sizeof(Key));
    return result;
#  endif /* _MSC_VER */
  }

  // Returns the hashed value of a key.
  template < typename Key >
  INLINE Size HashFuncSmallCastKey< Key >::operator()(const Key& key) const {
    return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
  }

  // ===========================================================================

  // constructor
  template < typename Key >
  INLINE HashFuncMediumCastKey< Key >::HashFuncMediumCastKey() {
    static_assert(sizeof(Key) == sizeof(Size),
                  "Error: using HashFuncMediumCastKey for a key whose size "
                  "is different from that of a gum::Size");
  }

  // Returns the value of a key as a Size
  template < typename Key >
  INLINE Size HashFuncMediumCastKey< Key >::castToSize(const Key& key) {
    return *((Size*)(&key));
  }

  // Returns the hashed value of a key.
  template < typename Key >
  INLINE Size HashFuncMediumCastKey< Key >::operator()(const Key& key) const {
    return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
  }

  // ===========================================================================

  // constructor
  template < typename Key >
  INLINE HashFuncLargeCastKey< Key >::HashFuncLargeCastKey() {
    static_assert(sizeof(Key) == 2 * sizeof(Size),
                  "Error: you used HashFuncLargeCastKey for a key whose size "
                  "is different from twice that of a gum::Size");
  }

  // Returns the value of a key as a Size
  template < typename Key >
  INLINE Size HashFuncLargeCastKey< Key >::castToSize(const Key& key) {
    const Size* ptr = reinterpret_cast< const Size* >(&key);
    return ptr[0] ^ ptr[1];
  }

  // Returns the hashed value of a key.
  template < typename Key >
  INLINE Size HashFuncLargeCastKey< Key >::operator()(const Key& key) const {
    return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
  }

  // ===========================================================================

  // Returns the value of a key as a Size
  template < typename Key1, typename Key2 >
  INLINE Size HashFunc< std::pair< Key1, Key2 > >::castToSize(const std::pair< Key1, Key2 >& key) {
    return HashFunc< Key1 >::castToSize(key.first) * HashFuncConst::pi
         + HashFunc< Key2 >::castToSize(key.second);
  }

  // Returns the hashed value of a key.
  template < typename Key1, typename Key2 >
  INLINE Size
     HashFunc< std::pair< Key1, Key2 > >::operator()(const std::pair< Key1, Key2 >& key) const {
    return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
  }

  // ===========================================================================

  // Returns the hashed value of a key.
  template < typename Type >
  INLINE Size HashFunc< RefPtr< Type > >::castToSize(const RefPtr< Type >& key) {
    return HashFunc< Type* >::castToSize(key._refCountPtr_());
  }

  // Returns the hashed value of a key.
  template < typename Type >
  INLINE Size HashFunc< RefPtr< Type > >::operator()(const RefPtr< Type >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
