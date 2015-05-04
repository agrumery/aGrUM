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
 * @brief Template implementation of the basic hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

// to help IDE parser
#include <agrum/core/hashFunc.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* ===                   GUM_HASH_FUNC_BASE IMPLEMENTATION                  ===*/
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key> INLINE void HashFuncBase<Key>::resize(Size new_size) {
    // things work properly only for hashtables with at least 2 elements
    if (new_size < 2) {
      GUM_ERROR(HashSize, "the size of the hashtable is too small");
    }

    _hash_log2_size = __hashTableLog2(new_size);
    _hash_size = 1UL << _hash_log2_size;
    _hash_mask = _hash_size - 1;
  }

  /// update the hash function to take into account a resize of the hash table
  template <typename Key> INLINE Size HashFuncBase<Key>::size() const noexcept {
    return _hash_size;
  }

  /* =========================================================================== */
  /* ===                 GUM_HASH_FUNC_SMALL_KEY IMPLEMENTATION               ===*/
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key> INLINE void HashFuncSmallKey<Key>::resize(Size new_size) {
    static_assert(std::is_integral<Key>::value &&
                      sizeof(Key) <= sizeof(unsigned long),
                  "Error: using HashFuncSmallKey for a key which cannot be "
                  "converted (without narrowing) into a long int");
    HashFuncBase<Key>::resize(new_size);
    _right_shift = HashFuncConst::offset - HashFuncBase<Key>::_hash_log2_size;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  template <typename Key>
  INLINE Size HashFuncSmallKey<Key>::operator()(const Key &key) const {
    return (((unsigned long)key * HashFuncConst::gold) >> _right_shift);
  }

  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_SMALL_CAST_KEY IMPLEMENTATION             ===*/
  /* =========================================================================== */

  /// basic constructor
  template <typename Key>
  INLINE HashFuncSmallCastKey<Key>::HashFuncSmallCastKey()
      : _small_key_mask((1UL << (8 * sizeof(Key))) - 1UL) {
    static_assert(sizeof(Key) < sizeof(unsigned long),
                  "Error: using HashFuncSmallCastKey for a key whose size "
                  "is longer than or equal to that of long int");
  }

  /// update the hash function to take into account a resize of the hash table
  template <typename Key>
  INLINE void HashFuncSmallCastKey<Key>::resize(Size new_size) {
    HashFuncBase<Key>::resize(new_size);
    _right_shift = HashFuncConst::offset - HashFuncBase<Key>::_hash_log2_size;
  }

  /// returns the value of a key as an unsigned long
  template <typename Key>
  INLINE Size HashFuncSmallCastKey<Key>::castToSize(const Key &key) const {
    return *((unsigned long *)(&key)) & _small_key_mask;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  template <typename Key>
  INLINE Size HashFuncSmallCastKey<Key>::operator()(const Key &key) const {
    return (((*((unsigned long *)(&key)) & _small_key_mask) * HashFuncConst::gold) >>
            _right_shift);
  }

  /* =========================================================================== */
  /* ===             GUM_HASH_FUNC_MEDIUM_CAST_KEY IMPLEMENTATION             ===*/
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key>
  INLINE void HashFuncMediumCastKey<Key>::resize(Size new_size) {
    static_assert(sizeof(Key) == sizeof(unsigned long),
                  "Error: using HashFuncMediumCastKey for a key whose size "
                  "is different from that of long int");
    HashFuncBase<Key>::resize(new_size);
    _right_shift = HashFuncConst::offset - HashFuncBase<Key>::_hash_log2_size;
  }

  /// returns the value of a key as an unsigned long
  template <typename Key>
  INLINE Size HashFuncMediumCastKey<Key>::castToSize(const Key &key) const {
    return *((unsigned long *)(&key));
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  template <typename Key>
  INLINE Size HashFuncMediumCastKey<Key>::operator()(const Key &key) const {
    return ((*((unsigned long *)(&key)) * HashFuncConst::gold) >> _right_shift);
  }

  /* =========================================================================== */
  /* ===             GUM_HASH_FUNC_LARGE_CAST_KEY IMPLEMENTATION             ===*/
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key>
  INLINE void HashFuncLargeCastKey<Key>::resize(Size new_size) {
    static_assert(sizeof(Key) == 2 * sizeof(unsigned long),
                  "Error: using HashFuncLargeCastKey for a key whose size "
                  "is different from twice that of long int");
    HashFuncBase<Key>::resize(new_size);
    _right_shift = HashFuncConst::offset - HashFuncBase<Key>::_hash_log2_size;
  }

  /// returns the value of a key as an unsigned long
  template <typename Key>
  INLINE Size HashFuncLargeCastKey<Key>::castToSize(const Key &key) const {
    const unsigned long *ptr = reinterpret_cast<const unsigned long *>(&key);
    return ptr[0] ^ ptr[1];
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  template <typename Key>
  INLINE Size HashFuncLargeCastKey<Key>::operator()(const Key &key) const {
    const unsigned long *ptr = reinterpret_cast<const unsigned long *>(&key);
    return ((ptr[0] ^ ptr[1]) * HashFuncConst::gold) >> _right_shift;
  }

  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_SMALL_KEY_PAIR IMPLEMENTATION            === */
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key1, typename Key2>
  INLINE void HashFuncSmallKeyPair<Key1, Key2>::resize(Size new_size) {
    static_assert(std::is_integral<Key1>::value &&
                      sizeof(Key1) <= sizeof(unsigned long),
                  "Error: using HashFuncSmallKeyPair for key1 which cannot be "
                  "converted (without narrowing) into a long int");
    static_assert(std::is_integral<Key2>::value &&
                      sizeof(Key2) <= sizeof(unsigned long),
                  "Error: using HashFuncSmallKeyPair for key2 which cannot be "
                  "converted (without narrowing) into a long int");
    HashFuncBase<std::pair<Key1, Key2>>::resize(new_size);
    _right_shift =
        HashFuncConst::offset - HashFuncBase<std::pair<Key1, Key2>>::_hash_log2_size;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  template <typename Key1, typename Key2>
  INLINE Size HashFuncSmallKeyPair<Key1, Key2>::
  operator()(const std::pair<Key1, Key2> &key) const {
    return (((unsigned long)key.first * HashFuncConst::gold +
             (unsigned long)key.second * HashFuncConst::pi) >>
            _right_shift);
  }

  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_CAST_KEY_PAIR IMPLEMENTATION             === */
  /* =========================================================================== */

  /// update the hash function to take into account a resize of the hash table
  template <typename Key1, typename Key2, typename Func1, typename Func2>
  INLINE void
  HashFuncAllCastKeyPair<Key1, Key2, Func1, Func2>::resize(Size new_size) {
    __func1.resize(new_size);
    __func2.resize(new_size);
    HashFuncBase<std::pair<Key1, Key2>>::resize(new_size);
    _right_shift =
        HashFuncConst::offset - HashFuncBase<std::pair<Key1, Key2>>::_hash_log2_size;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  template <typename Key1, typename Key2, typename Func1, typename Func2>
  INLINE Size HashFuncAllCastKeyPair<Key1, Key2, Func1, Func2>::
  operator()(const std::pair<Key1, Key2> &key) const {
    return (__func1.castToSize(key.first) * HashFuncConst::gold +
            __func2.castToSize(key.second) * HashFuncConst::pi) >>
           _right_shift;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by RefPtr<Type>
  template <typename Type>
  INLINE Size HashFunc<RefPtr<Type>>::operator()(const RefPtr<Type> &key) const {
    static_assert(sizeof(RefPtr<Type>) == sizeof(long),
                  "Error: HashFunc<Type*> assumes that pointers have a size "
                  "equal to a long integer");
    return HashFunc<unsigned int *>::operator()(key.__refCountPtr());
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
