/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of the HashTable
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <iostream>
#include <sstream>
#include <string>

// to help IDE parser
#include <agrum/tools/core/hashTable.h>

namespace gum {


  // ===========================================================================
  // ===     IMPLEMENTATION OF THE CHAINED LISTS USED IN THE HASH TABLES     ===
  // ===========================================================================

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  void HashTableList< Key, Val, Alloc >::copy__(
     const HashTableList< Key, Val, OtherAlloc >& from) {
    Bucket *ptr, *old_ptr{nullptr}, *new_elt{nullptr};
    // set the defaults
    deb_list__ = nullptr;

    // copy from's list
    try {
      for (ptr = from.deb_list__; ptr != nullptr; ptr = ptr->next) {
        // copy the current from's bucket (may throw an exception either because
        // new cannot allocate the bucket or because the copy constructor of Val
        // throws an exception)
        new_elt = alloc_bucket__->allocate(1);

        try {
          alloc_bucket__->construct(new_elt, *ptr);
        } catch (...) {
          alloc_bucket__->deallocate(new_elt, 1);
          throw;
        }

        // rechain properly the new list
        new_elt->prev = old_ptr;

        if (old_ptr != nullptr)
          old_ptr->next = new_elt;
        else
          deb_list__ = new_elt;

        old_ptr = new_elt;
      }

      if (old_ptr != nullptr) old_ptr->next = nullptr;

      // update the number of elements stored into the list and the end of the
      // list
      nb_elements__ = from.nb_elements__;
      end_list__    = new_elt;
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (; deb_list__ != nullptr; deb_list__ = new_elt) {
        new_elt = deb_list__->next;
        alloc_bucket__->destroy(deb_list__);
        alloc_bucket__->deallocate(deb_list__, 1);
      }

      nb_elements__ = 0;
      end_list__    = nullptr;

      throw;
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableBucket< Key, Val >*
         HashTableList< Key, Val, Alloc >::bucket(const Key& key) const {
    for (Bucket* ptr = deb_list__; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr;

    return nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void
     HashTableList< Key, Val, Alloc >::erase(HashTableBucket< Key, Val >* ptr) {
    // check that the pointer is not nullptr
    if (ptr == nullptr) {
      GUM_ERROR(NullElement, "trying to erase a nullptr bucket");
    }

    // relink properly the doubly chained list
    if (ptr->prev != nullptr)
      ptr->prev->next = ptr->next;
    else
      deb_list__ = ptr->next;

    if (ptr->next != nullptr)
      ptr->next->prev = ptr->prev;
    else
      end_list__ = ptr->prev;

    // remove the current element from the list
    alloc_bucket__->destroy(ptr);
    alloc_bucket__->deallocate(ptr, 1);

    --nb_elements__;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     typename HashTableList< Key, Val, Alloc >::BucketAllocator*
        allocator) noexcept :
      alloc_bucket__{allocator} {}

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     const HashTableList< Key, Val, Alloc >& from) :
      alloc_bucket__{from.alloc_bucket__} {
    copy__(from);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     HashTableList< Key, Val, Alloc >&& from) noexcept :
      deb_list__{from.deb_list__},
      end_list__{from.end_list__}, nb_elements__{from.nb_elements__},
      alloc_bucket__{from.alloc_bucket__} {
    from.deb_list__ = nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::~HashTableList() {
    for (Bucket *next_ptr, *ptr = deb_list__; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
      alloc_bucket__->destroy(ptr);
      alloc_bucket__->deallocate(ptr, 1);
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::clear() {
    for (Bucket *next_ptr, *ptr = deb_list__; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
      alloc_bucket__->destroy(ptr);
      alloc_bucket__->deallocate(ptr, 1);
    }

    nb_elements__ = Size(0);
    deb_list__    = nullptr;
    end_list__    = nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >&
     HashTableList< Key, Val, Alloc >::operator=(
        const HashTableList< Key, Val, Alloc >& from) {
    // avoid self assignment
    if (this != &from) {
      clear();
      copy__(from);
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE HashTableList< Key, Val, Alloc >&
     HashTableList< Key, Val, Alloc >::operator=(
        const HashTableList< Key, Val, OtherAlloc >& from) {
    // avoid self assignment
    if (this
        != reinterpret_cast< const HashTableList< Key, Val, Alloc >* >(&from)) {
      clear();
      copy__(from);
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >&
     HashTableList< Key, Val, Alloc >::operator=(
        HashTableList< Key, Val, Alloc >&& from) noexcept {
    // avoid self assignment
    if (this != &from) {
      deb_list__      = from.deb_list__;
      end_list__      = from.end_list__;
      nb_elements__   = from.nb_elements__;
      from.deb_list__ = nullptr;
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::setAllocator(
     typename HashTableList< Key, Val, Alloc >::BucketAllocator& alloc) {
    alloc_bucket__ = &alloc;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTableList< Key, Val, Alloc >::value_type&
     HashTableList< Key, Val, Alloc >::at(Size i) {
    if (i >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    Bucket* ptr;

    for (ptr = deb_list__; i; --i, ptr = ptr->next) {}

    return ptr->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTableList< Key, Val, Alloc >::value_type&
     HashTableList< Key, Val, Alloc >::at(Size i) const {
    if (i >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    Bucket* ptr;

    for (ptr = deb_list__; i; --i, ptr = ptr->next) {}

    return ptr->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTableList< Key, Val, Alloc >::mapped_type&
     HashTableList< Key, Val, Alloc >::operator[](const Key& key) const {
    for (Bucket* ptr = deb_list__; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr->val();

    GUM_ERROR(NotFound, "No element with the key <" << key << ">");
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTableList< Key, Val, Alloc >::mapped_type&
     HashTableList< Key, Val, Alloc >::operator[](const Key& key) {
    for (Bucket* ptr = deb_list__; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr->val();

    GUM_ERROR(NotFound, "No element with the key <" << key << ">");
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTableList< Key, Val, Alloc >::exists(const Key& key) const {
    for (Bucket* ptr = deb_list__; ptr != nullptr; ptr = ptr->next) {
      if (ptr->key() == key) { return true; }
    }

    return false;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTableList< Key, Val, Alloc >::empty() const noexcept {
    return (nb_elements__ == Size(0));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::insert(
     HashTableBucket< Key, Val >* new_elt) noexcept {
    // place the bucket at the beginning of the list
    new_elt->prev = nullptr;
    new_elt->next = deb_list__;

    if (deb_list__ != nullptr)
      deb_list__->prev = new_elt;
    else
      end_list__ = new_elt;

    deb_list__ = new_elt;

    ++nb_elements__;
  }

  // ===========================================================================
  // ===                   GENERIC HASH TABLE IMPLEMENTATION                 ===
  // ===========================================================================

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  void HashTable< Key, Val, Alloc >::copy__(
     const HashTable< Key, Val, OtherAlloc >& table) {
    // in debug mode, check that this and table have '__nodes' arrays of the
    // same size
    GUM_ASSERT(table.size__ == size__);

    // try to fill the array of chained lists
    for (Size i = 0; i < table.size__; ++i) {
      try {
        nodes__[i] = table.nodes__[i];
      } catch (...) {
        // here we could allocate the nodes__[j], j=0..i-1, so we should
        // deallocate them
        for (Size j = 0; j < size__; ++j)
          nodes__[j].clear();

        nb_elements__ = Size(0);

        // propagate the exception
        throw;
      }
    }

    nb_elements__ = table.nb_elements__;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::iterator&
     HashTable< Key, Val, Alloc >::end4Statics() {
    return *(reinterpret_cast< const iterator* >(
       HashTableIteratorStaticEnd::end4Statics()));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator&
     HashTable< Key, Val, Alloc >::constEnd4Statics() {
    return *(reinterpret_cast< const const_iterator* >(
       HashTableIteratorStaticEnd::constEnd4Statics()));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::iterator_safe&
     HashTable< Key, Val, Alloc >::endSafe4Statics() {
    return *(reinterpret_cast< const iterator_safe* >(
       HashTableIteratorStaticEnd::endSafe4Statics()));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator_safe&
     HashTable< Key, Val, Alloc >::constEndSafe4Statics() {
    return *(reinterpret_cast< const const_iterator_safe* >(
       HashTableIteratorStaticEnd::constEndSafe4Statics()));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::create__(Size size) {
    // setup the nodes__ vector (contains only empty lists)
    nodes__.resize(size);

    for (auto& list: nodes__) {
      list.setAllocator(alloc__);
    }

    // set up properly the hash function
    hash_func__.resize(size);

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(Size size_param,
                                          bool resize_pol,
                                          bool key_uniqueness_pol) :
      // size must be >= 2 else we lose all the bits of the hash function
      size__{Size(1) << hashTableLog2__(std::max(Size(2), size_param))},
      resize_policy__{resize_pol}, key_uniqueness_policy__{key_uniqueness_pol} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // finalize the creation
    create__(size__);
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(
     std::initializer_list< std::pair< Key, Val > > list) :
      // size must be >= 2 else we lose all the bits of the hash function
      size__{Size(1) << hashTableLog2__(
                std::max< Size >(Size(2), Size(list.size()) / 2))} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // setup the nodes__ vector (contains only empty lists)
    create__(size__);

    // insert all the elements
    for (const auto& elt: list) {
      insert(elt);
    }
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(
     const HashTable< Key, Val, Alloc >& table) :
      size__{table.size__},
      resize_policy__{table.resize_policy__},
      key_uniqueness_policy__{table.key_uniqueness_policy__},
      begin_index__{table.begin_index__} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the nodes__ vector (contains only empty lists)
    create__(size__);

    // fill with the content of table
    copy__(table);
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  HashTable< Key, Val, Alloc >::HashTable(
     const HashTable< Key, Val, OtherAlloc >& table) :
      size__{table.size__},
      resize_policy__{table.resize_policy__},
      key_uniqueness_policy__{table.key_uniqueness_policy__},
      begin_index__{table.begin_index__} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the nodes__ vector (contains only empty lists)
    create__(size__);

    // fill with the content of table
    copy__(table);
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(HashTable< Key, Val, Alloc >&& table) :
      nodes__(std::move(table.nodes__)), size__{table.size__},
      nb_elements__{table.nb_elements__}, hash_func__{table.hash_func__},
      resize_policy__{table.resize_policy__},
      key_uniqueness_policy__{table.key_uniqueness_policy__},
      begin_index__{table.begin_index__},
      safe_iterators__(std::move(table.safe_iterators__)),
      alloc__(std::move(table.alloc__)) {
    // for debugging purposes
    table.size__ = 0;
    GUM_CONS_MOV(HashTable);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::clearIterators__() {
    const Size len = safe_iterators__.size();
    for (Size i = Size(0); i < len; ++i)
      safe_iterators__[i]->clear();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::clear() {
    // update all the registered iterators: they should now point to nullptr
    // and they are positioned to the end of the hashtable.
    clearIterators__();

    // remove the buckets
    for (Size i = Size(0); i < size__; ++i)
      nodes__[i].clear();

    nb_elements__ = Size(0);
    begin_index__ = std::numeric_limits< Size >::max();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTable< Key, Val, Alloc >::~HashTable() {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTable);

    // update all the registered iterators: they should now point to nullptr
    // and their hashtable should be set to nullptr
    clearIterators__();
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >& HashTable< Key, Val, Alloc >::operator=(
     const HashTable< Key, Val, Alloc >& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      // if sizes of from's and this' nodes__ vectors are not the same,
      // we need to remove the current nodes__' array and to create a
      // new array with the correct size
      if (size__ != from.size__) {
        nodes__.resize(from.size__);

        for (Size i = Size(0); i < from.size__; ++i) {
          nodes__[i].setAllocator(alloc__);
        }

        size__ = from.size__;

        // update the hash function : this is important as the computation of
        // the hash values heavily depends on the size of the hash table
        hash_func__.resize(size__);
      }

      resize_policy__         = from.resize_policy__;
      key_uniqueness_policy__ = from.key_uniqueness_policy__;
      begin_index__           = from.begin_index__;

      // perform the copy
      copy__(from);
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  HashTable< Key, Val, Alloc >& HashTable< Key, Val, Alloc >::operator=(
     const HashTable< Key, Val, OtherAlloc >& from) {
    // avoid self assignment
    if (this != reinterpret_cast< const HashTable< Key, Val, Alloc >* >(&from)) {
      // for debugging purposes
      GUM_OP_CPY(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      // if sizes of from's and this' nodes__ vectors are not the same,
      // we need to remove the current nodes__' array and to create a
      // new array with the correct size
      if (size__ != from.size__) {
        nodes__.resize(from.size__);

        for (Size i = 0; i < from.size__; ++i) {
          nodes__[i].setAllocator(alloc__);
        }

        size__ = from.size__;

        // update the hash function : this is important as the computation of
        // the hash values heavily depends on the size of the hash table
        hash_func__.resize(size__);
      }

      resize_policy__         = from.resize_policy__;
      key_uniqueness_policy__ = from.key_uniqueness_policy__;
      begin_index__           = from.begin_index__;

      // perform the copy
      copy__(from);
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >& HashTable< Key, Val, Alloc >::operator=(
     HashTable< Key, Val, Alloc >&& table) {
    // avoid self assignment
    if (this != &table) {
      // for debugging purposes
      GUM_OP_MOV(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      nodes__                 = std::move(table.nodes__);
      safe_iterators__        = std::move(table.safe_iterators__);
      alloc__                 = std::move(table.alloc__);
      size__                  = table.size__;
      nb_elements__           = table.nb_elements__;
      hash_func__             = table.hash_func__;
      resize_policy__         = table.resize_policy__;
      key_uniqueness_policy__ = table.key_uniqueness_policy__;
      begin_index__           = table.begin_index__;

      table.size__ = 0;   // necessary if we wish to perform moves iteratively,
                          // i.e. x = std::move ( y ); y = std::move ( z ); ...
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::iterator&
     HashTable< Key, Val, Alloc >::end() noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const iterator* >(
       HashTableIteratorStaticEnd::HashTableIterEnd__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator&
     HashTable< Key, Val, Alloc >::end() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator* >(
       HashTableIteratorStaticEnd::HashTableIterEnd__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator&
     HashTable< Key, Val, Alloc >::cend() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator* >(
       HashTableIteratorStaticEnd::HashTableIterEnd__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::iterator
     HashTable< Key, Val, Alloc >::begin() {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return iterator{end()};
    else
      return iterator{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator
     HashTable< Key, Val, Alloc >::begin() const {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return const_iterator{end()};
    else
      return const_iterator{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator
     HashTable< Key, Val, Alloc >::cbegin() const {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return const_iterator{cend()};
    else
      return const_iterator{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::iterator_safe&
     HashTable< Key, Val, Alloc >::endSafe() noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const iterator_safe* >(
       HashTableIteratorStaticEnd::HashTableIterEndSafe__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator_safe&
     HashTable< Key, Val, Alloc >::endSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator_safe* >(
       HashTableIteratorStaticEnd::HashTableIterEndSafe__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator_safe&
     HashTable< Key, Val, Alloc >::cendSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator_safe* >(
       HashTableIteratorStaticEnd::HashTableIterEndSafe__));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::iterator_safe
     HashTable< Key, Val, Alloc >::beginSafe() {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return iterator_safe{endSafe()};
    else
      return iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator_safe
     HashTable< Key, Val, Alloc >::beginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return const_iterator_safe{endSafe()};
    else
      return const_iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator_safe
     HashTable< Key, Val, Alloc >::cbeginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if (nb_elements__ == Size(0))
      return const_iterator_safe{cendSafe()};
    else
      return const_iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Val& HashTable< Key, Val, Alloc >::operator[](const Key& key) {
    return nodes__[hash_func__(key)][key];
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const Val&
     HashTable< Key, Val, Alloc >::operator[](const Key& key) const {
    return nodes__[hash_func__(key)][key];
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Size HashTable< Key, Val, Alloc >::size() const noexcept {
    return nb_elements__;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Size HashTable< Key, Val, Alloc >::capacity() const noexcept {
    return size__;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::exists(const Key& key) const {
    return nodes__[hash_func__(key)].exists(key);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::setResizePolicy(
     const bool new_policy) noexcept {
    resize_policy__ = new_policy;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::resizePolicy() const noexcept {
    return resize_policy__;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::setKeyUniquenessPolicy(
     const bool new_policy) noexcept {
    key_uniqueness_policy__ = new_policy;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::keyUniquenessPolicy() const noexcept {
    return key_uniqueness_policy__;
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >::resize(Size new_size) {
    // new_size must be >= 2 else all the bits of the hash function are lost
    new_size = std::max(Size(2), new_size);

    // find the real size for allocation (the smallest power of 2 greater
    // than or equal to new_size) and get its base-2 logarithm
    int log_size = hashTableLog2__(new_size);
    new_size     = Size(1) << log_size;

    // check if the new size is different from the actual size
    // if not, nothing else need be done

    if (new_size != size__) {
      // under automatic resize policy, check if the new size leaves
      // enough space for storing all the current elements
      if (!resize_policy__
          || (nb_elements__
              <= new_size * HashTableConst::default_mean_val_by_slot)) {
        // create a new array of nodes__ to store the elements
        std::vector< HashTableList< Key, Val, Alloc > > new_nodes(new_size);

        for (auto& list: new_nodes) {
          list.setAllocator(alloc__);
        }

        // set the new hash function
        hash_func__.resize(new_size);

        // put all the elements of the current nodes__ array into the new one
        Bucket* bucket;
        Size    new_hashed_key;

        for (Size i = Size(0); i < size__; ++i) {
          while ((bucket = nodes__[i].deb_list__) != nullptr) {
            // compute the new hashed key
            new_hashed_key = hash_func__(bucket->key());

            // remove the bucket from the list of buckets of the current
            // node vector
            nodes__[i].deb_list__ = bucket->next;

            // put the bucket into the new nodes__ vector
            new_nodes[new_hashed_key].insert(bucket);
          }
        }

        // update the size of the hash table
        size__        = new_size;
        begin_index__ = std::numeric_limits< Size >::max();

        // substitute the current nodes__ array by the new one
        std::swap(nodes__, new_nodes);

        // update the iterators
        for (auto iter: safe_iterators__) {
          if (iter->bucket__)
            iter->index__ = hash_func__(iter->bucket__->key());
          else {
            iter->next_bucket__ = nullptr;
            iter->index__       = 0;
          }
        }
      }
    }
  }

  template < typename Key, typename Val, typename Alloc >
  void
     HashTable< Key, Val, Alloc >::insert__(HashTableBucket< Key, Val >* bucket) {
    Size hash_key = hash_func__(bucket->key());

    // check that there does not already exist an element with the same key
    if (key_uniqueness_policy__ && nodes__[hash_key].exists(bucket->key())) {
      // remove the bucket from memory
      Key k = bucket->key();
      alloc__.destroy(bucket);
      alloc__.deallocate(bucket, 1);
      GUM_ERROR(DuplicateElement,
                "the hashtable contains an element with the same key (" << k
                                                                        << ")");
    }

    // check whether there is sufficient space to insert the new pair
    // if not, resize the current hashtable
    if (resize_policy__
        && (nb_elements__ >= size__ * HashTableConst::default_mean_val_by_slot)) {
      resize(size__ << 1);
      hash_key = hash_func__(bucket->key());
    }

    // add the new pair
    nodes__[hash_key].insert(bucket);
    ++nb_elements__;

    // recompute the index of the beginning of the hashtable if possible
    // WARNING: if begin_index__ = std::numeric_limits<Size>::max (), we CANNOT
    // recompute the index because we cannot know whether the current index is
    // equal to max because there was no element in the hashtable or whether a
    // previous erase__() has set the index to max.
    if (begin_index__ < hash_key) { begin_index__ = hash_key; }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(const Key& thekey, const Val& theval) {
    Bucket* bucket = alloc__.allocate(1);

    try {
      alloc__.construct(bucket, thekey, theval);
    } catch (...) {
      alloc__.deallocate(bucket, 1);
      throw;
    }

    insert__(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(Key&& thekey, Val&& theval) {
    Bucket* bucket = alloc__.allocate(1);

    try {
      alloc__.construct(bucket, std::move(thekey), std::move(theval));
    } catch (...) {
      alloc__.deallocate(bucket, 1);
      throw;
    }

    insert__(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(const std::pair< Key, Val >& elt) {
    Bucket* bucket = alloc__.allocate(1);

    try {
      alloc__.construct(bucket, reinterpret_cast< const value_type& >(elt));
    } catch (...) {
      alloc__.deallocate(bucket, 1);
      throw;
    }

    insert__(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(std::pair< Key, Val >&& elt) {
    Bucket* bucket = alloc__.allocate(1);

    try {
      alloc__.construct(bucket, std::move(reinterpret_cast< value_type& >(elt)));
    } catch (...) {
      alloc__.deallocate(bucket, 1);
      throw;
    }

    insert__(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename... Args >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::emplace(Args&&... args) {
    Bucket* bucket = alloc__.allocate(1);

    try {
      alloc__.construct(bucket,
                        HashTableBucket< Key, Val >::Emplace::EMPLACE,
                        std::forward< Args >(args)...);
    } catch (...) {
      alloc__.deallocate(bucket, 1);
      throw;
    }

    insert__(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::mapped_type&
     HashTable< Key, Val, Alloc >::getWithDefault(const Key& key,
                                                  const Val& default_value) {
    Bucket* bucket = nodes__[hash_func__(key)].bucket(key);

    if (bucket == nullptr)
      return insert(key, default_value).second;
    else
      return bucket->val();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::mapped_type&
     HashTable< Key, Val, Alloc >::getWithDefault(Key&& key, Val&& default_value) {
    Bucket* bucket = nodes__[hash_func__(key)].bucket(key);

    if (bucket == nullptr)
      return insert(std::move(key), std::move(default_value)).second;
    else
      return bucket->val();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::set(const Key& key, const Val& value) {
    Bucket* bucket = nodes__[hash_func__(key)].bucket(key);

    if (bucket == nullptr)
      insert(key, value);
    else
      bucket->val() = value;
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >::erase__(HashTableBucket< Key, Val >* bucket,
                                             Size                         index) {
    if (bucket == nullptr) return;

    // update the registered iterators pointing to this bucket
    for (auto iter: safe_iterators__) {
      if (iter->bucket__ == bucket) {
        iter->operator++();
        iter->next_bucket__ = iter->bucket__;
        iter->bucket__      = nullptr;
      } else if (iter->next_bucket__ == bucket) {
        iter->bucket__ = bucket;
        iter->operator++();
        iter->next_bucket__ = iter->bucket__;
        iter->bucket__      = nullptr;
      }
    }

    // remove the element from the nodes__ vector
    nodes__[index].erase(bucket);

    --nb_elements__;

    if ((index == begin_index__) && nodes__[index].empty()) {
      begin_index__ = std::numeric_limits< Size >::max();
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::erase(const Key& key) {
    // get the hashed key
    Size hash = hash_func__(key);

    // get the bucket containing the element to erase
    HashTableBucket< Key, Val >* bucket = nodes__[hash].bucket(key);

    erase__(bucket, hash);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::erase(const iterator_safe& iter) {
    erase__(iter.getBucket__(), iter.getIndex__());
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void
     HashTable< Key, Val, Alloc >::erase(const const_iterator_safe& iter) {
    erase__(iter.getBucket__(), iter.getIndex__());
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::eraseByVal(const Val& val) {
    for (auto iter = cbegin(); iter != cend(); ++iter)
      if (iter.bucket__->val() == val) {
        erase__(iter.getBucket__(), iter.getIndex__());
        return;
      }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::reset(const Key& key) {
    erase(key);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const Key& HashTable< Key, Val, Alloc >::keyByVal(const Val& val) const {
    for (auto iter = begin(); iter != end(); ++iter)
      if (iter.bucket__->val() == val) return iter.key();

    GUM_ERROR(NotFound, "not enough elements in the chained list");
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const Key& HashTable< Key, Val, Alloc >::key(const Key& key) const {
    // get the bucket corresponding to the key
    Bucket* bucket = nodes__[hash_func__(key)].bucket(key);

    if (bucket == nullptr) {
      GUM_ERROR(NotFound, "key does not belong to the hashtable");
    }

    return bucket->key();
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >::eraseAllVal(const Val& val) {
    for (auto iterAll = cbeginSafe(); iterAll != cendSafe(); ++iterAll) {
      if (iterAll.bucket__->val() == val) {
        erase__(iterAll.bucket__, iterAll.index__);
      }
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::empty() const noexcept {
    return (nb_elements__ == Size(0));
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  HashTable< Key, Mount, OtherAlloc >
     INLINE HashTable< Key, Val, Alloc >::map(Mount (*f)(Val),
                                              Size size,
                                              bool resize_pol,
                                              bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == 0) size = std::max(Size(2), nb_elements__ / 2);

    // create a new table
    HashTable< Key, Mount, OtherAlloc > table(size,
                                              resize_pol,
                                              key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(iter.val()));
    }

    return table;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  HashTable< Key, Mount, OtherAlloc >
     INLINE HashTable< Key, Val, Alloc >::map(Mount (*f)(Val&),
                                              Size size,
                                              bool resize_pol,
                                              bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == Size(0)) size = std::max(Size(2), nb_elements__ / 2);

    // create a new table
    HashTable< Key, Mount, OtherAlloc > table(size,
                                              resize_pol,
                                              key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(const_cast< Val& >(iter.val())));
    }

    return table;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  HashTable< Key, Mount, OtherAlloc >
     INLINE HashTable< Key, Val, Alloc >::map(Mount (*f)(const Val&),
                                              Size size,
                                              bool resize_pol,
                                              bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == Size(0)) size = std::max(Size(2), nb_elements__ / 2);

    // create a new table
    HashTable< Key, Mount, OtherAlloc > table(size,
                                              resize_pol,
                                              key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(iter.val()));
    }

    return table;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  HashTable< Key, Mount, OtherAlloc >
     INLINE HashTable< Key, Val, Alloc >::map(const Mount& val,
                                              Size         size,
                                              bool         resize_pol,
                                              bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == Size(0)) size = std::max(Size(2), nb_elements__ / 2);

    // create a new table
    HashTable< Key, Mount, OtherAlloc > table(size,
                                              resize_pol,
                                              key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), val);
    }

    return table;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  bool HashTable< Key, Val, Alloc >::operator==(
     const HashTable< Key, Val, OtherAlloc >& from) const {
    // checks whether the two hashtables contain the same number of elements
    if (from.nb_elements__ != nb_elements__) return false;

    // parse this and check that each element also belongs to from
    for (auto iter = begin(); iter != end(); ++iter) {
      try {
        if (iter.val() != from[iter.key()]) return false;
      } catch (NotFound&) { return false; }
    }

    return true;
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool HashTable< Key, Val, Alloc >::operator!=(
     const HashTable< Key, Val, OtherAlloc >& from) const {
    return !operator==(from);
  }

  template < typename Key, typename Val, typename Alloc >
  std::ostream& operator<<(std::ostream&                           stream,
                           const HashTableList< Key, Val, Alloc >& list) {
    bool deja = false;
    stream << "[";

    for (HashTableBucket< Key, Val >* ptr = list.deb_list__; ptr;
         ptr = ptr->list.next, deja = true) {
      if (deja) stream << " , ";

      stream << ptr->key() << "=>" << ptr->val();
    }

    stream << "]";

    return stream;
  }

  template < typename Key, typename Val, typename Alloc >
  std::ostream& operator<<(std::ostream&                            stream,
                           const HashTableList< Key*, Val, Alloc >& list) {
    bool deja = false;
    stream << "[";

    for (HashTableBucket< Key, Val >* ptr = list.deb_list__; ptr;
         ptr = ptr->list.next, deja = true) {
      if (deja) stream << " , ";

      stream << ptr->key() << "=>" << ptr->val();
    }

    stream << "]";

    return stream;
  }

  template < typename Key, typename Val, typename Alloc >
  std::ostream& operator<<(std::ostream&                       stream,
                           const HashTable< Key, Val, Alloc >& table) {
    bool deja = false;
    stream << "{";

    for (Size i = Size(0); i < table.size__; ++i)
      for (auto ptr = table.nodes__[i].deb_list__; ptr; ptr = ptr->next) {
        if (deja) stream << " , ";

        stream << ptr->key() << "=>" << ptr->val();

        deja = true;
      }

    stream << "}";

    return stream;
  }

  template < typename Key, typename Val, typename Alloc >
  std::ostream& operator<<(std::ostream&                        stream,
                           const HashTable< Key*, Val, Alloc >& table) {
    bool deja = false;
    stream << "{";

    for (Size i = Size(0); i < table.size__; ++i)
      for (auto ptr = table.nodes__[i].deb_list__; ptr; ptr = ptr->next) {
        if (deja) stream << " , ";

        stream << ptr->key() << "=>" << ptr->val();

        deja = true;
      }

    stream << "}";

    return stream;
  }

  // ===========================================================================
  // ===                SAFE HASH TABLE ITERATORS IMPLEMENTATION             ===
  // ===========================================================================

  template < typename Key, typename Val >
  INLINE void
     HashTableConstIteratorSafe< Key, Val >::insertIntoSafeList__() const {
    table__->safe_iterators__.push_back(
       const_cast< HashTableConstIteratorSafe< Key, Val >* >(this));
  }

  template < typename Key, typename Val >
  INLINE void
     HashTableConstIteratorSafe< Key, Val >::removeFromSafeList__() const {
    if (table__ == nullptr) return;

    // find where the iterator is
    std::vector< HashTableConstIteratorSafe< Key, Val >* >& iter_vect
       = table__->safe_iterators__;

    auto len = iter_vect.size();
    for (Size i = Size(0); i < len; ++i) {
      if (iter_vect[i] == this) {
        iter_vect.erase(iter_vect.begin() + i);
        break;
      }
    }
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe() {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTable< Key, Val, Alloc >& tab) :
      table__{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
    insertIntoSafeList__();

    if (table__->nb_elements__) {
      if (table__->begin_index__ != std::numeric_limits< Size >::max()) {
        index__  = table__->begin_index__;
        bucket__ = table__->nodes__[index__].end_list__;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (Size i = table__->size__ - Size(1);; --i) {   // no test on i since
                                                           // nb_elements__ != 0
          if (table__->nodes__[i].nb_elements__) {
            index__                = i;
            bucket__               = table__->nodes__[index__].end_list__;
            table__->begin_index__ = index__;
            break;
          }
        }
      }
    }
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTable< Key, Val, Alloc >& tab,
     Size                                ind_elt) :
      table__{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == Size(0))
        && (table__->begin_index__ != std::numeric_limits< Size >::max())) {
      index__  = table__->begin_index__;
      bucket__ = table__->nodes__[index__].end_list__;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < (table__->nb_elements__ >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i = table__->size__ - 1;; --i) {   // no test on i since
          // ind_elt < table_->nb_elements__
          if (table__->nodes__[i].nb_elements__) {
            if (ind_elt >= table__->nodes__[i].nb_elements__)
              ind_elt -= table__->nodes__[i].nb_elements__;
            else {
              for (bucket__ = table__->nodes__[i].end_list__; ind_elt;
                   --ind_elt, bucket__ = bucket__->prev) {}

              index__ = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >= table__->nb_elements__) {
          GUM_ERROR(UndefinedIteratorValue,
                    "Not enough elements in the hashtable");
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt = table__->nb_elements__ - ind_elt - 1;; ++i) {
          if (table__->nodes__[i].nb_elements__) {
            if (ind_elt >= table__->nodes__[i].nb_elements__)
              ind_elt -= table__->nodes__[i].nb_elements__;
            else {
              for (bucket__ = table__->nodes__[i].deb_list__; ind_elt;
                   --ind_elt, bucket__ = bucket__->next) {}

              index__ = i;
              break;
            }
          }
        }
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
    insertIntoSafeList__();
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTableConstIteratorSafe< Key, Val >& from) :
      table__{from.table__},
      index__{from.index__}, bucket__{from.bucket__}, next_bucket__{
                                                         from.next_bucket__} {
    // make the hashtable keep track of this iterator
    if (table__ != nullptr) { insertIntoSafeList__(); }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTableConstIterator< Key, Val >& from) :
      table__{from.table__},
      index__{from.index__}, bucket__{from.bucket__} {
    // make the hashtable keep track of this iterator
    if (table__ != nullptr) { insertIntoSafeList__(); }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     HashTableConstIteratorSafe< Key, Val >&& from) :
      table__{from.table__},
      index__{from.index__}, bucket__{from.bucket__}, next_bucket__{
                                                         from.next_bucket__} {
    GUM_CONS_MOV(HashTableConstIteratorSafe);

    // find "from" in the hashtable's list of safe iterators and substitute
    // it by this
    if (table__ != nullptr) {
      std::vector< HashTableConstIteratorSafe< Key, Val >* >& vect
         = table__->safe_iterators__;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
        if (*ptr == &from) {
          *ptr         = this;
          from.table__ = nullptr;
          break;
        }
      }
    }
  }

  template < typename Key, typename Val >
  INLINE
     HashTableConstIteratorSafe< Key,
                                 Val >::~HashTableConstIteratorSafe() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTableConstIteratorSafe);

    // remove the iterator from the table's iterator list
    removeFromSafeList__();
  }

  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator=(
        const HashTableConstIteratorSafe< Key, Val >& from) {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable
    // iterator's
    // list and add it to the new hashtable iterator's list
    if (table__ != from.table__) {
      // remove the iterator from its hashtable iterator's list'
      removeFromSafeList__();

      table__ = from.table__;

      // add to the new table
      if (table__) { insertIntoSafeList__(); }
    }

    index__       = from.index__;
    bucket__      = from.bucket__;
    next_bucket__ = from.next_bucket__;

    return *this;
  }

  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator=(
        const HashTableConstIterator< Key, Val >& from) {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable
    // iterator's
    // list and add it to the new hashtable iterator's list
    if (table__ != from.table__) {
      // remove the iterator from its hashtable iterator's list'
      removeFromSafeList__();

      table__ = from.table__;

      // add to the new table
      if (table__) { insertIntoSafeList__(); }
    }

    index__       = from.index__;
    bucket__      = from.bucket__;
    next_bucket__ = nullptr;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator=(
        HashTableConstIteratorSafe< Key, Val >&& from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable
    // iterator's
    // list and add it to the new hashtable iterator's list
    if (table__ != from.table__) {
      // remove the iterator from its hashtable iterator's list'
      removeFromSafeList__();

      if (from.table__ != nullptr) {
        // substitute from by this in the list of safe iterators
        std::vector< HashTableConstIteratorSafe< Key, Val >* >& vect
           = from.table__->safe_iterators__;

        for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
          if (*ptr == &from) {
            *ptr = this;
            break;
          }
        }
      }

      table__      = from.table__;
      from.table__ = nullptr;
    }

    index__       = from.index__;
    bucket__      = from.bucket__;
    next_bucket__ = from.next_bucket__;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::key_type&
     HashTableConstIteratorSafe< Key, Val >::key() const {
    if (bucket__ != nullptr)
      return bucket__->key();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::mapped_type&
     HashTableConstIteratorSafe< Key, Val >::val() const {
    if (bucket__ != nullptr)
      return bucket__->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE void HashTableConstIteratorSafe< Key, Val >::clear() noexcept {
    // remove the iterator from the table's iterator list
    removeFromSafeList__();

    // set its table as well as the element it points to to 0
    table__       = nullptr;
    bucket__      = nullptr;
    next_bucket__ = nullptr;
    index__       = Size(0);
  }

  // WARNING: never inline this function: this result in g++4.3.3 producing a
  // code that segfaults.
  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator++() noexcept {
    // if bucket__ != nullptr then use it, else use next_bucket
    if (bucket__ == nullptr) {
      // note that this case only happens when the iterator pointed to an
      // element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the next_bucket__ and index__
      // fields.
      bucket__      = next_bucket__;
      next_bucket__ = nullptr;
    } else {
      // ok, here we can use bucket__ as a starting point

      // if we are not pointing on the first element of the chained list, just
      // point to the preceding bucket in this list
      if (bucket__->prev) {
        bucket__ = bucket__->prev;
        // here, no need to update next_bucket__, which is compulsorily
        // equal to nullptr, nor index__ which has not changed.
      } else {
        // ok, here we are on the beginning of a chained list,
        // so 2 cases can obtain:
        // 1/ index = 0 : then we have reached the end of the hashtable
        // 2/ index != 0 => we must search for a new slot containing elements

        // case 1:
        if (index__ == Size(0)) {
          bucket__ = nullptr;
          // we are thus at the end() of the hashTable
        }
        // case 2:
        else {
          // arrived here, we need to parse the hash table until we find a new
          // bucket because we are pointing on a chained list with no more
          // element
          // to the left of the current element
          if (index__ > Size(0)) {
            for (Size i = index__ - Size(1); i > Size(0); --i) {
              if (table__->nodes__[i].nb_elements__) {
                index__  = i;
                bucket__ = table__->nodes__[i].end_list__;
                return *this;
              }
            }
          }

          if (table__->nodes__[0].nb_elements__)
            bucket__ = table__->nodes__[0].end_list__;
          else
            bucket__ = nullptr;

          index__ = 0;
        }
      }
    }

    return *this;
  }

  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator+=(Size nb) noexcept {
    if ((nb == Size(0)) || (table__ == nullptr)) return *this;

    // if bucket__ != nullptr then use it, else use next_bucket
    if (bucket__ == nullptr) {
      // note that this case only happens when the iterator pointed to an
      // element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the next_bucket__ and index__
      // fields.
      bucket__      = next_bucket__;
      next_bucket__ = nullptr;
      --nb;
    }

    // ok, here we can use bucket__ as a starting point: parse all the elements
    // of the current chained list
    for (; nb && bucket__ != nullptr; --nb, bucket__ = bucket__->prev) {}

    if (bucket__ != nullptr) return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    --index__;

    for (; index__ < table__->size__
           && nb >= table__->nodes__[index__].nb_elements__;
         nb -= table__->nodes__[index__].nb_elements__, --index__) {}

    // here: either index__ >= table__->size__, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb < table__->nodes__[index__].nb_elements__, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if (index__ >= table__->size__) {
      index__ = Size(0);
      return *this;
    }

    for (bucket__ = table__->nodes__[index__].end_list__; nb;
         --nb, bucket__ = bucket__->prev) {}

    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >
     HashTableConstIteratorSafe< Key, Val >::operator+(Size nb) const {
    return HashTableConstIteratorSafe< Key, Val >{*this} += nb;
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIteratorSafe< Key, Val >::operator!=(
     const HashTableConstIteratorSafe< Key, Val >& from) const noexcept {
    return ((bucket__ != from.bucket__) || (index__ != from.index__));
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIteratorSafe< Key, Val >::operator==(
     const HashTableConstIteratorSafe< Key, Val >& from) const noexcept {
    return ((bucket__ == from.bucket__) && (index__ == from.index__));
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::value_type&
     HashTableConstIteratorSafe< Key, Val >::operator*() const {
    if (bucket__)
      return bucket__->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE HashTableBucket< Key, Val >*
         HashTableConstIteratorSafe< Key, Val >::getBucket__() const noexcept {
    return bucket__;
  }

  template < typename Key, typename Val >
  INLINE Size HashTableConstIteratorSafe< Key, Val >::getIndex__() const noexcept {
    return index__;
  }

  // ===========================================================================
  // ===                SAFE HASH TABLE ITERATORS IMPLEMENTATION             ===
  // ===========================================================================

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe() :
      HashTableConstIteratorSafe< Key, Val >() {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe(
     const HashTable< Key, Val, Alloc >& tab) :
      HashTableConstIteratorSafe< Key, Val >(tab) {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe(
     const HashTable< Key, Val, Alloc >& tab,
     Size                                ind_elt) :
      HashTableConstIteratorSafe< Key, Val >(tab, ind_elt) {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe(
     const HashTableIteratorSafe< Key, Val >& from) :
      HashTableConstIteratorSafe< Key, Val >(from) {
    GUM_CONS_CPY(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe(
     const HashTableIterator< Key, Val >& from) :
      HashTableConstIteratorSafe< Key, Val >(from) {
    GUM_CONS_CPY(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >::HashTableIteratorSafe(
     HashTableIteratorSafe< Key, Val >&& from) noexcept :
      HashTableConstIteratorSafe< Key, Val >(std::move(from)) {
    GUM_CONS_MOV(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >::~HashTableIteratorSafe() noexcept {
    GUM_DESTRUCTOR(HashTableIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE typename HashTableIteratorSafe< Key, Val >::mapped_type&
     HashTableIteratorSafe< Key, Val >::val() {
    return const_cast< Val& >(HashTableConstIteratorSafe< Key, Val >::val());
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >&
     HashTableIteratorSafe< Key, Val >::operator=(
        const HashTableIteratorSafe< Key, Val >& from) {
    GUM_OP_CPY(HashTableIteratorSafe);
    HashTableConstIteratorSafe< Key, Val >::operator=(from);
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >&
     HashTableIteratorSafe< Key, Val >::operator=(
        const HashTableIterator< Key, Val >& from) {
    GUM_OP_CPY(HashTableIteratorSafe);
    HashTableConstIteratorSafe< Key, Val >::operator=(from);
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >&
     HashTableIteratorSafe< Key, Val >::operator=(
        HashTableIteratorSafe< Key, Val >&& from) noexcept {
    HashTableConstIteratorSafe< Key, Val >::operator=(std::move(from));
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >&
     HashTableIteratorSafe< Key, Val >::operator++() noexcept {
    HashTableConstIteratorSafe< Key, Val >::operator++();
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >&
     HashTableIteratorSafe< Key, Val >::operator+=(Size nb) noexcept {
    HashTableConstIteratorSafe< Key, Val >::operator+=(nb);
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIteratorSafe< Key, Val >
     HashTableIteratorSafe< Key, Val >::operator+(Size nb) const {
    HashTableIteratorSafe< Key, Val > iter{*this};
    iter += nb;
    return iter;
  }

  template < typename Key, typename Val >
  INLINE bool HashTableIteratorSafe< Key, Val >::operator!=(
     const HashTableIteratorSafe< Key, Val >& from) const noexcept {
    return HashTableConstIteratorSafe< Key, Val >::operator!=(from);
  }

  template < typename Key, typename Val >
  INLINE bool HashTableIteratorSafe< Key, Val >::operator==(
     const HashTableIteratorSafe< Key, Val >& from) const noexcept {
    return HashTableConstIteratorSafe< Key, Val >::operator==(from);
  }

  template < typename Key, typename Val >
  INLINE typename HashTableIteratorSafe< Key, Val >::value_type&
     HashTableIteratorSafe< Key, Val >::operator*() {
    return const_cast< Val& >(HashTableConstIteratorSafe< Key, Val >::operator*());
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableIteratorSafe< Key, Val >::value_type&
     HashTableIteratorSafe< Key, Val >::operator*() const {
    return HashTableConstIteratorSafe< Key, Val >::operator*();
  }

  // ===========================================================================
  // ===            UNSAFE HASH TABLE CONST ITERATORS IMPLEMENTATION         ===
  // ===========================================================================

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >::HashTableConstIterator() noexcept {
    GUM_CONSTRUCTOR(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableConstIterator< Key, Val >::HashTableConstIterator(
     const HashTable< Key, Val, Alloc >& tab) noexcept :
      table__{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIterator);

    if (table__->nb_elements__) {
      if (table__->begin_index__ != std::numeric_limits< Size >::max()) {
        index__  = table__->begin_index__;
        bucket__ = table__->nodes__[index__].end_list__;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (Size i = table__->size__ - Size(1);; --i) {   // no test on i since
          // nb_elements__ != 0
          if (table__->nodes__[i].nb_elements__) {
            index__                = i;
            bucket__               = table__->nodes__[index__].end_list__;
            table__->begin_index__ = index__;
            break;
          }
        }
      }
    }
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  HashTableConstIterator< Key, Val >::HashTableConstIterator(
     const HashTable< Key, Val, Alloc >& tab,
     Size                                ind_elt) :
      table__{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == Size(0))
        && (table__->begin_index__ != std::numeric_limits< Size >::max())) {
      index__  = table__->begin_index__;
      bucket__ = table__->nodes__[index__].end_list__;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < (table__->nb_elements__ >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i = table__->size__ - 1;; --i) {   // no test on i since
          // ind_elt < table_->nb_elements__
          if (table__->nodes__[i].nb_elements__) {
            if (ind_elt >= table__->nodes__[i].nb_elements__)
              ind_elt -= table__->nodes__[i].nb_elements__;
            else {
              for (bucket__ = table__->nodes__[i].end_list__; ind_elt;
                   --ind_elt, bucket__ = bucket__->prev) {}

              index__ = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >= table__->nb_elements__) {
          GUM_ERROR(UndefinedIteratorValue,
                    "Not enough elements in the hashtable");
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt = table__->nb_elements__ - ind_elt - 1;; ++i) {
          if (table__->nodes__[i].nb_elements__) {
            if (ind_elt >= table__->nodes__[i].nb_elements__)
              ind_elt -= table__->nodes__[i].nb_elements__;
            else {
              for (bucket__ = table__->nodes__[i].deb_list__; ind_elt;
                   --ind_elt, bucket__ = bucket__->next) {}

              index__ = i;
              break;
            }
          }
        }
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >::HashTableConstIterator(
     const HashTableConstIterator< Key, Val >& from) noexcept :
      table__{from.table__},
      index__{from.index__}, bucket__{from.bucket__} {
    GUM_CONS_CPY(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >::HashTableConstIterator(
     HashTableConstIterator< Key, Val >&& from) noexcept :
      table__{from.table__},
      index__{from.index__}, bucket__{from.bucket__} {
    GUM_CONS_MOV(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >::~HashTableConstIterator() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator=(
        const HashTableConstIterator< Key, Val >& from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    table__  = from.table__;
    index__  = from.index__;
    bucket__ = from.bucket__;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator=(
        HashTableConstIterator< Key, Val >&& from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    table__  = from.table__;
    index__  = from.index__;
    bucket__ = from.bucket__;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::key_type&
     HashTableConstIterator< Key, Val >::key() const {
    if (bucket__)
      return bucket__->pair.first;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::mapped_type&
     HashTableConstIterator< Key, Val >::val() const {
    if (bucket__)
      return bucket__->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE void HashTableConstIterator< Key, Val >::clear() noexcept {
    table__  = nullptr;
    bucket__ = nullptr;
    index__  = 0;
  }

  template < typename Key, typename Val >
  HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator++() noexcept {
    // if bucket__ == nullptr then we are at the end of the hashtable
    if (bucket__ == nullptr) return *this;

    // if we are not pointing on the first element of the chained list, just
    // point to the next bucket in this list
    if (bucket__->prev) {
      bucket__ = bucket__->prev;
      // here, no need to update index__ which has not changed.
    } else {
      // ok, here we are on the end of a chained list,
      // so 2 cases can obtain:
      // 1/ index = 0 : then we have reached the end of the hashtable
      // 2/ index != 0 => we must search for a new slot containing elements

      // case 1:
      if (index__ == Size(0)) {
        bucket__ = nullptr;
        // we are thus at the end() of the hashTable
      }

      // case 2:
      else {
        // arrived here, we need to parse the hash table until we find a new
        // bucket because we are pointing on a chained list with no more element
        // to the right of the current element
        for (Size i = index__ - Size(1); i; --i) {
          if (table__->nodes__[i].nb_elements__) {
            index__  = i;
            bucket__ = table__->nodes__[i].end_list__;
            return *this;
          }
        }

        if (table__->nodes__[0].nb_elements__)
          bucket__ = table__->nodes__[0].end_list__;
        else
          bucket__ = nullptr;

        index__ = Size(0);
      }
    }

    return *this;
  }

  template < typename Key, typename Val >
  HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator+=(Size nb) noexcept {
    if ((nb == 0) || (table__ == nullptr) || (bucket__ == nullptr)) return *this;

    // ok, here we can use bucket__ as a starting point: parse all the elements
    // of the current chained list
    for (; nb && bucket__ != nullptr; --nb, bucket__ = bucket__->prev) {}

    if (bucket__ != nullptr) return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    --index__;

    for (; index__ < table__->size__
           && nb >= table__->nodes__[index__].nb_elements__;
         nb -= table__->nodes__[index__].nb_elements__, --index__) {}

    // here: either index__ >= table__->size__, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb < table__->nodes__[index__].nb_elements__, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if (index__ >= table__->size__) {
      index__ = 0;
      return *this;
    }

    for (bucket__ = table__->nodes__[index__].end_list__; nb;
         --nb, bucket__ = bucket__->prev) {}

    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >
     HashTableConstIterator< Key, Val >::operator+(Size nb) const noexcept {
    return HashTableConstIterator< Key, Val >{*this} += nb;
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIterator< Key, Val >::operator!=(
     const HashTableConstIterator< Key, Val >& from) const noexcept {
    return (bucket__ != from.bucket__);
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIterator< Key, Val >::operator==(
     const HashTableConstIterator< Key, Val >& from) const noexcept {
    return (bucket__ == from.bucket__);
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::value_type&
     HashTableConstIterator< Key, Val >::operator*() const {
    if (bucket__)
      return bucket__->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE typename HashTable< Key, Val >::Bucket*
     HashTableConstIterator< Key, Val >::getBucket__() const noexcept {
    return bucket__;
  }

  template < typename Key, typename Val >
  INLINE Size HashTableConstIterator< Key, Val >::getIndex__() const noexcept {
    return index__;
  }

  // ===========================================================================
  // ===               UNSAFE HASH TABLE ITERATORS IMPLEMENTATION            ===
  // ===========================================================================

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >::HashTableIterator() noexcept :
      HashTableConstIterator< Key, Val >() {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableIterator< Key, Val >::HashTableIterator(
     const HashTable< Key, Val, Alloc >& tab) noexcept :
      HashTableConstIterator< Key, Val >(tab) {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  template < typename Key, typename Val >
  template < typename Alloc >
  INLINE HashTableIterator< Key, Val >::HashTableIterator(
     const HashTable< Key, Val, Alloc >& tab,
     Size                                ind_elt) :
      HashTableConstIterator< Key, Val >(tab, ind_elt) {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >::HashTableIterator(
     const HashTableIterator< Key, Val >& from) noexcept :
      HashTableConstIterator< Key, Val >(from) {
    GUM_CONS_CPY(HashTableIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >::HashTableIterator(
     HashTableIterator< Key, Val >&& from) noexcept :
      HashTableConstIterator< Key, Val >(std::move(from)) {
    GUM_CONS_MOV(HashTableIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >::~HashTableIterator() noexcept {
    GUM_DESTRUCTOR(HashTableIterator);
  }

  template < typename Key, typename Val >
  INLINE typename HashTableIterator< Key, Val >::mapped_type&
     HashTableIterator< Key, Val >::val() {
    if (this->bucket__)
      return this->bucket__->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >& HashTableIterator< Key, Val >::operator=(
     const HashTableIterator< Key, Val >& from) noexcept {
    HashTableConstIterator< Key, Val >::operator=(from);
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >& HashTableIterator< Key, Val >::operator=(
     HashTableIterator< Key, Val >&& from) noexcept {
    HashTableConstIterator< Key, Val >::operator=(std::move(from));
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >&
     HashTableIterator< Key, Val >::operator++() noexcept {
    HashTableConstIterator< Key, Val >::operator++();
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >&
     HashTableIterator< Key, Val >::operator+=(Size nb) noexcept {
    HashTableConstIterator< Key, Val >::operator+=(nb);
    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableIterator< Key, Val >
     HashTableIterator< Key, Val >::operator+(Size nb) const noexcept {
    HashTableIterator< Key, Val > iter{*this};
    iter += nb;
    return iter;
  }

  template < typename Key, typename Val >
  INLINE bool HashTableIterator< Key, Val >::operator!=(
     const HashTableIterator< Key, Val >& from) const noexcept {
    return HashTableConstIterator< Key, Val >::operator!=(from);
  }

  template < typename Key, typename Val >
  INLINE bool HashTableIterator< Key, Val >::operator==(
     const HashTableIterator< Key, Val >& from) const noexcept {
    return HashTableConstIterator< Key, Val >::operator==(from);
  }

  template < typename Key, typename Val >
  INLINE typename HashTableIterator< Key, Val >::value_type&
     HashTableIterator< Key, Val >::operator*() {
    return const_cast< value_type& >(
       HashTableConstIterator< Key, Val >::operator*());
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableIterator< Key, Val >::value_type&
     HashTableIterator< Key, Val >::operator*() const {
    return HashTableConstIterator< Key, Val >::operator*();
  }

} /* namespace gum */
