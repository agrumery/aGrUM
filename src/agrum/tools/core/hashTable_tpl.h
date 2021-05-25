/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
  void HashTableList< Key, Val, Alloc >:: _copy_(
     const HashTableList< Key, Val, OtherAlloc >& from) {
    Bucket *ptr, *old_ptr{nullptr}, *new_elt{nullptr};
    // set the defaults
     _deb_list_ = nullptr;

    // copy from's list
    try {
      for (ptr = from. _deb_list_; ptr != nullptr; ptr = ptr->next) {
        // copy the current from's bucket (may throw an exception either because
        // new cannot allocate the bucket or because the copy constructor of Val
        // throws an exception)
        new_elt =  _alloc_bucket_->allocate(1);

        try {
           _alloc_bucket_->construct(new_elt, *ptr);
        } catch (...) {
           _alloc_bucket_->deallocate(new_elt, 1);
          throw;
        }

        // rechain properly the new list
        new_elt->prev = old_ptr;

        if (old_ptr != nullptr)
          old_ptr->next = new_elt;
        else
           _deb_list_ = new_elt;

        old_ptr = new_elt;
      }

      if (old_ptr != nullptr) old_ptr->next = nullptr;

      // update the number of elements stored into the list and the end of the
      // list
       _nb_elements_ = from. _nb_elements_;
       _end_list_    = new_elt;
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (;  _deb_list_ != nullptr;  _deb_list_ = new_elt) {
        new_elt =  _deb_list_->next;
         _alloc_bucket_->destroy( _deb_list_);
         _alloc_bucket_->deallocate( _deb_list_, 1);
      }

       _nb_elements_ = 0;
       _end_list_    = nullptr;

      throw;
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableBucket< Key, Val >*
         HashTableList< Key, Val, Alloc >::bucket(const Key& key) const {
    for (Bucket* ptr =  _deb_list_; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr;

    return nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void
     HashTableList< Key, Val, Alloc >::erase(HashTableBucket< Key, Val >* ptr) {
    // check that the pointer is not nullptr
    if (ptr == nullptr) {
      GUM_ERROR(NullElement, "trying to erase a nullptr bucket")
    }

    // relink properly the doubly chained list
    if (ptr->prev != nullptr)
      ptr->prev->next = ptr->next;
    else
       _deb_list_ = ptr->next;

    if (ptr->next != nullptr)
      ptr->next->prev = ptr->prev;
    else
       _end_list_ = ptr->prev;

    // remove the current element from the list
     _alloc_bucket_->destroy(ptr);
     _alloc_bucket_->deallocate(ptr, 1);

    -- _nb_elements_;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     typename HashTableList< Key, Val, Alloc >::BucketAllocator*
        allocator) noexcept :
       _alloc_bucket_{allocator} {}

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     const HashTableList< Key, Val, Alloc >& from) :
       _alloc_bucket_{from. _alloc_bucket_} {
     _copy_(from);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::HashTableList(
     HashTableList< Key, Val, Alloc >&& from) noexcept :
       _deb_list_{from. _deb_list_},
       _end_list_{from. _end_list_},  _nb_elements_{from. _nb_elements_},
       _alloc_bucket_{from. _alloc_bucket_} {
    from. _deb_list_ = nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >::~HashTableList() {
    for (Bucket *next_ptr, *ptr =  _deb_list_; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
       _alloc_bucket_->destroy(ptr);
       _alloc_bucket_->deallocate(ptr, 1);
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::clear() {
    for (Bucket *next_ptr, *ptr =  _deb_list_; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
       _alloc_bucket_->destroy(ptr);
       _alloc_bucket_->deallocate(ptr, 1);
    }

     _nb_elements_ = Size(0);
     _deb_list_    = nullptr;
     _end_list_    = nullptr;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >&
     HashTableList< Key, Val, Alloc >::operator=(
        const HashTableList< Key, Val, Alloc >& from) {
    // avoid self assignment
    if (this != &from) {
      clear();
       _copy_(from);
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
       _copy_(from);
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTableList< Key, Val, Alloc >&
     HashTableList< Key, Val, Alloc >::operator=(
        HashTableList< Key, Val, Alloc >&& from) noexcept {
    // avoid self assignment
    if (this != &from) {
       _deb_list_      = from. _deb_list_;
       _end_list_      = from. _end_list_;
       _nb_elements_   = from. _nb_elements_;
      from. _deb_list_ = nullptr;
    }

    return *this;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::setAllocator(
     typename HashTableList< Key, Val, Alloc >::BucketAllocator& alloc) {
     _alloc_bucket_ = &alloc;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTableList< Key, Val, Alloc >::value_type&
     HashTableList< Key, Val, Alloc >::at(Size i) {
    if (i >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    Bucket* ptr;

    for (ptr =  _deb_list_; i; --i, ptr = ptr->next) {}

    return ptr->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTableList< Key, Val, Alloc >::value_type&
     HashTableList< Key, Val, Alloc >::at(Size i) const {
    if (i >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    Bucket* ptr;

    for (ptr =  _deb_list_; i; --i, ptr = ptr->next) {}

    return ptr->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTableList< Key, Val, Alloc >::mapped_type&
     HashTableList< Key, Val, Alloc >::operator[](const Key& key) const {
    for (Bucket* ptr =  _deb_list_; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr->val();

    GUM_ERROR(NotFound, "No element with the key <" << key << ">")
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTableList< Key, Val, Alloc >::mapped_type&
     HashTableList< Key, Val, Alloc >::operator[](const Key& key) {
    for (Bucket* ptr =  _deb_list_; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key) return ptr->val();

    GUM_ERROR(NotFound, "No element with the key <" << key << ">")
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTableList< Key, Val, Alloc >::exists(const Key& key) const {
    for (Bucket* ptr =  _deb_list_; ptr != nullptr; ptr = ptr->next) {
      if (ptr->key() == key) { return true; }
    }

    return false;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTableList< Key, Val, Alloc >::empty() const noexcept {
    return ( _nb_elements_ == Size(0));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTableList< Key, Val, Alloc >::insert(
     HashTableBucket< Key, Val >* new_elt) noexcept {
    // place the bucket at the beginning of the list
    new_elt->prev = nullptr;
    new_elt->next =  _deb_list_;

    if ( _deb_list_ != nullptr)
       _deb_list_->prev = new_elt;
    else
       _end_list_ = new_elt;

     _deb_list_ = new_elt;

    ++ _nb_elements_;
  }

  // ===========================================================================
  // ===                   GENERIC HASH TABLE IMPLEMENTATION                 ===
  // ===========================================================================

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  void HashTable< Key, Val, Alloc >:: _copy_(
     const HashTable< Key, Val, OtherAlloc >& table) {
    // in debug mode, check that this and table have ' __nodes' arrays of the
    // same size
    GUM_ASSERT(table. _size_ ==  _size_);

    // try to fill the array of chained lists
    for (Size i = 0; i < table. _size_; ++i) {
      try {
         _nodes_[i] = table. _nodes_[i];
      } catch (...) {
        // here we could allocate the  _nodes_[j], j=0..i-1, so we should
        // deallocate them
        for (Size j = 0; j <  _size_; ++j)
           _nodes_[j].clear();

         _nb_elements_ = Size(0);

        // propagate the exception
        throw;
      }
    }

     _nb_elements_ = table. _nb_elements_;
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
  INLINE void HashTable< Key, Val, Alloc >:: _create_(Size size) {
    // setup the  _nodes_ vector (contains only empty lists)
     _nodes_.resize(size);

    for (auto& list:  _nodes_) {
      list.setAllocator( _alloc_);
    }

    // set up properly the hash function
     _hash_func_.resize(size);

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(Size size_param,
                                          bool resize_pol,
                                          bool key_uniqueness_pol) :
      // size must be >= 2 else we lose all the bits of the hash function
       _size_{Size(1) <<  _hashTableLog2_(std::max(Size(2), size_param))},
       _resize_policy_{resize_pol},  _key_uniqueness_policy_{key_uniqueness_pol} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // finalize the creation
     _create_( _size_);
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(
     std::initializer_list< std::pair< Key, Val > > list) :
      // size must be >= 2 else we lose all the bits of the hash function
       _size_{Size(1) <<  _hashTableLog2_(
                std::max< Size >(Size(2), Size(list.size()) / 2))} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // setup the  _nodes_ vector (contains only empty lists)
     _create_( _size_);

    // insert all the elements
    for (const auto& elt: list) {
      insert(elt);
    }
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(
     const HashTable< Key, Val, Alloc >& table) :
       _size_{table. _size_},
       _resize_policy_{table. _resize_policy_},
       _key_uniqueness_policy_{table. _key_uniqueness_policy_},
       _begin_index_{table. _begin_index_} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the  _nodes_ vector (contains only empty lists)
     _create_( _size_);

    // fill with the content of table
     _copy_(table);
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename OtherAlloc >
  HashTable< Key, Val, Alloc >::HashTable(
     const HashTable< Key, Val, OtherAlloc >& table) :
       _size_{table. _size_},
       _resize_policy_{table. _resize_policy_},
       _key_uniqueness_policy_{table. _key_uniqueness_policy_},
       _begin_index_{table. _begin_index_} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the  _nodes_ vector (contains only empty lists)
     _create_( _size_);

    // fill with the content of table
     _copy_(table);
  }

  template < typename Key, typename Val, typename Alloc >
  HashTable< Key, Val, Alloc >::HashTable(HashTable< Key, Val, Alloc >&& table) :
       _nodes_(std::move(table. _nodes_)),  _size_{table. _size_},
       _nb_elements_{table. _nb_elements_},  _hash_func_{table. _hash_func_},
       _resize_policy_{table. _resize_policy_},
       _key_uniqueness_policy_{table. _key_uniqueness_policy_},
       _begin_index_{table. _begin_index_},
       _safe_iterators_(std::move(table. _safe_iterators_)),
       _alloc_(std::move(table. _alloc_)) {
    // for debugging purposes
    table. _size_ = 0;
    GUM_CONS_MOV(HashTable);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >:: _clearIterators_() {
    const Size len =  _safe_iterators_.size();
    for (Size i = Size(0); i < len; ++i)
       _safe_iterators_[i]->clear();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::clear() {
    // update all the registered iterators: they should now point to nullptr
    // and they are positioned to the end of the hashtable.
     _clearIterators_();

    // remove the buckets
    for (Size i = Size(0); i <  _size_; ++i)
       _nodes_[i].clear();

     _nb_elements_ = Size(0);
     _begin_index_ = std::numeric_limits< Size >::max();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE HashTable< Key, Val, Alloc >::~HashTable() {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTable);

    // update all the registered iterators: they should now point to nullptr
    // and their hashtable should be set to nullptr
     _clearIterators_();
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

      // if sizes of from's and this'  _nodes_ vectors are not the same,
      // we need to remove the current  _nodes_' array and to create a
      // new array with the correct size
      if ( _size_ != from. _size_) {
         _nodes_.resize(from. _size_);

        for (Size i = Size(0); i < from. _size_; ++i) {
           _nodes_[i].setAllocator( _alloc_);
        }

         _size_ = from. _size_;

        // update the hash function : this is important as the computation of
        // the hash values heavily depends on the size of the hash table
         _hash_func_.resize( _size_);
      }

       _resize_policy_         = from. _resize_policy_;
       _key_uniqueness_policy_ = from. _key_uniqueness_policy_;
       _begin_index_           = from. _begin_index_;

      // perform the copy
       _copy_(from);
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

      // if sizes of from's and this'  _nodes_ vectors are not the same,
      // we need to remove the current  _nodes_' array and to create a
      // new array with the correct size
      if ( _size_ != from. _size_) {
         _nodes_.resize(from. _size_);

        for (Size i = 0; i < from. _size_; ++i) {
           _nodes_[i].setAllocator( _alloc_);
        }

         _size_ = from. _size_;

        // update the hash function : this is important as the computation of
        // the hash values heavily depends on the size of the hash table
         _hash_func_.resize( _size_);
      }

       _resize_policy_         = from. _resize_policy_;
       _key_uniqueness_policy_ = from. _key_uniqueness_policy_;
       _begin_index_           = from. _begin_index_;

      // perform the copy
       _copy_(from);
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

       _nodes_                 = std::move(table. _nodes_);
       _safe_iterators_        = std::move(table. _safe_iterators_);
       _alloc_                 = std::move(table. _alloc_);
       _size_                  = table. _size_;
       _nb_elements_           = table. _nb_elements_;
       _hash_func_             = table. _hash_func_;
       _resize_policy_         = table. _resize_policy_;
       _key_uniqueness_policy_ = table. _key_uniqueness_policy_;
       _begin_index_           = table. _begin_index_;

      table. _size_ = 0;   // necessary if we wish to perform moves iteratively,
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
       HashTableIteratorStaticEnd:: _HashTableIterEnd_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator&
     HashTable< Key, Val, Alloc >::end() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator* >(
       HashTableIteratorStaticEnd:: _HashTableIterEnd_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator&
     HashTable< Key, Val, Alloc >::cend() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator* >(
       HashTableIteratorStaticEnd:: _HashTableIterEnd_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::iterator
     HashTable< Key, Val, Alloc >::begin() {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
      return iterator{end()};
    else
      return iterator{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator
     HashTable< Key, Val, Alloc >::begin() const {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
      return const_iterator{end()};
    else
      return const_iterator{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator
     HashTable< Key, Val, Alloc >::cbegin() const {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
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
       HashTableIteratorStaticEnd:: _HashTableIterEndSafe_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator_safe&
     HashTable< Key, Val, Alloc >::endSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator_safe* >(
       HashTableIteratorStaticEnd:: _HashTableIterEndSafe_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const typename HashTable< Key, Val, Alloc >::const_iterator_safe&
     HashTable< Key, Val, Alloc >::cendSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast< const const_iterator_safe* >(
       HashTableIteratorStaticEnd:: _HashTableIterEndSafe_));
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::iterator_safe
     HashTable< Key, Val, Alloc >::beginSafe() {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
      return iterator_safe{endSafe()};
    else
      return iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator_safe
     HashTable< Key, Val, Alloc >::beginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
      return const_iterator_safe{endSafe()};
    else
      return const_iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::const_iterator_safe
     HashTable< Key, Val, Alloc >::cbeginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if ( _nb_elements_ == Size(0))
      return const_iterator_safe{cendSafe()};
    else
      return const_iterator_safe{*this};
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Val& HashTable< Key, Val, Alloc >::operator[](const Key& key) {
    return  _nodes_[ _hash_func_(key)][key];
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const Val&
     HashTable< Key, Val, Alloc >::operator[](const Key& key) const {
    return  _nodes_[ _hash_func_(key)][key];
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Size HashTable< Key, Val, Alloc >::size() const noexcept {
    return  _nb_elements_;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE Size HashTable< Key, Val, Alloc >::capacity() const noexcept {
    return  _size_;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::exists(const Key& key) const {
    return  _nodes_[ _hash_func_(key)].exists(key);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::setResizePolicy(
     const bool new_policy) noexcept {
     _resize_policy_ = new_policy;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::resizePolicy() const noexcept {
    return  _resize_policy_;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::setKeyUniquenessPolicy(
     const bool new_policy) noexcept {
     _key_uniqueness_policy_ = new_policy;
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::keyUniquenessPolicy() const noexcept {
    return  _key_uniqueness_policy_;
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >::resize(Size new_size) {
    // new_size must be >= 2 else all the bits of the hash function are lost
    new_size = std::max(Size(2), new_size);

    // find the real size for allocation (the smallest power of 2 greater
    // than or equal to new_size) and get its base-2 logarithm
    int log_size =  _hashTableLog2_(new_size);
    new_size     = Size(1) << log_size;

    // check if the new size is different from the actual size
    // if not, nothing else need be done

    if (new_size !=  _size_) {
      // under automatic resize policy, check if the new size leaves
      // enough space for storing all the current elements
      if (! _resize_policy_
          || ( _nb_elements_
              <= new_size * HashTableConst::default_mean_val_by_slot)) {
        // create a new array of  _nodes_ to store the elements
        std::vector< HashTableList< Key, Val, Alloc > > new_nodes(new_size);

        for (auto& list: new_nodes) {
          list.setAllocator( _alloc_);
        }

        // set the new hash function
         _hash_func_.resize(new_size);

        // put all the elements of the current  _nodes_ array into the new one
        Bucket* bucket;
        Size    new_hashed_key;

        for (Size i = Size(0); i <  _size_; ++i) {
          while ((bucket =  _nodes_[i]. _deb_list_) != nullptr) {
            // compute the new hashed key
            new_hashed_key =  _hash_func_(bucket->key());

            // remove the bucket from the list of buckets of the current
            // node vector
             _nodes_[i]. _deb_list_ = bucket->next;

            // put the bucket into the new  _nodes_ vector
            new_nodes[new_hashed_key].insert(bucket);
          }
        }

        // update the size of the hash table
         _size_        = new_size;
         _begin_index_ = std::numeric_limits< Size >::max();

        // substitute the current  _nodes_ array by the new one
        std::swap( _nodes_, new_nodes);

        // update the iterators
        for (auto iter:  _safe_iterators_) {
          if (iter-> _bucket_)
            iter-> _index_ =  _hash_func_(iter-> _bucket_->key());
          else {
            iter-> _next_bucket_ = nullptr;
            iter-> _index_       = 0;
          }
        }
      }
    }
  }

  template < typename Key, typename Val, typename Alloc >
  void
     HashTable< Key, Val, Alloc >:: _insert_(HashTableBucket< Key, Val >* bucket) {
    Size hash_key =  _hash_func_(bucket->key());

    // check that there does not already exist an element with the same key
    if ( _key_uniqueness_policy_ &&  _nodes_[hash_key].exists(bucket->key())) {
      // remove the bucket from memory
      Key k = bucket->key();
       _alloc_.destroy(bucket);
       _alloc_.deallocate(bucket, 1);
      GUM_ERROR(DuplicateElement,
                "the hashtable contains an element with the same key (" << k
                                                                        << ")");
    }

    // check whether there is sufficient space to insert the new pair
    // if not, resize the current hashtable
    if ( _resize_policy_
        && ( _nb_elements_ >=  _size_ * HashTableConst::default_mean_val_by_slot)) {
      resize( _size_ << 1);
      hash_key =  _hash_func_(bucket->key());
    }

    // add the new pair
     _nodes_[hash_key].insert(bucket);
    ++ _nb_elements_;

    // recompute the index of the beginning of the hashtable if possible
    // WARNING: if  _begin_index_ = std::numeric_limits<Size>::max (), we CANNOT
    // recompute the index because we cannot know whether the current index is
    // equal to max because there was no element in the hashtable or whether a
    // previous  _erase_() has set the index to max.
    if ( _begin_index_ < hash_key) {  _begin_index_ = hash_key; }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(const Key& thekey, const Val& theval) {
    Bucket* bucket =  _alloc_.allocate(1);

    try {
       _alloc_.construct(bucket, thekey, theval);
    } catch (...) {
       _alloc_.deallocate(bucket, 1);
      throw;
    }

     _insert_(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(Key&& thekey, Val&& theval) {
    Bucket* bucket =  _alloc_.allocate(1);

    try {
       _alloc_.construct(bucket, std::move(thekey), std::move(theval));
    } catch (...) {
       _alloc_.deallocate(bucket, 1);
      throw;
    }

     _insert_(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(const std::pair< Key, Val >& elt) {
    Bucket* bucket =  _alloc_.allocate(1);

    try {
       _alloc_.construct(bucket, reinterpret_cast< const value_type& >(elt));
    } catch (...) {
       _alloc_.deallocate(bucket, 1);
      throw;
    }

     _insert_(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::insert(std::pair< Key, Val >&& elt) {
    Bucket* bucket =  _alloc_.allocate(1);

    try {
       _alloc_.construct(bucket, std::move(reinterpret_cast< value_type& >(elt)));
    } catch (...) {
       _alloc_.deallocate(bucket, 1);
      throw;
    }

     _insert_(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  template < typename... Args >
  INLINE typename HashTable< Key, Val, Alloc >::value_type&
     HashTable< Key, Val, Alloc >::emplace(Args&&... args) {
    Bucket* bucket =  _alloc_.allocate(1);

    try {
       _alloc_.construct(bucket,
                        HashTableBucket< Key, Val >::Emplace::EMPLACE,
                        std::forward< Args >(args)...);
    } catch (...) {
       _alloc_.deallocate(bucket, 1);
      throw;
    }

     _insert_(bucket);
    return bucket->elt();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::mapped_type&
     HashTable< Key, Val, Alloc >::getWithDefault(const Key& key,
                                                  const Val& default_value) {
    Bucket* bucket =  _nodes_[ _hash_func_(key)].bucket(key);

    if (bucket == nullptr)
      return insert(key, default_value).second;
    else
      return bucket->val();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE typename HashTable< Key, Val, Alloc >::mapped_type&
     HashTable< Key, Val, Alloc >::getWithDefault(Key&& key, Val&& default_value) {
    Bucket* bucket =  _nodes_[ _hash_func_(key)].bucket(key);

    if (bucket == nullptr)
      return insert(std::move(key), std::move(default_value)).second;
    else
      return bucket->val();
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::set(const Key& key, const Val& value) {
    Bucket* bucket =  _nodes_[ _hash_func_(key)].bucket(key);

    if (bucket == nullptr)
      insert(key, value);
    else
      bucket->val() = value;
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >:: _erase_(HashTableBucket< Key, Val >* bucket,
                                             Size                         index) {
    if (bucket == nullptr) return;

    // update the registered iterators pointing to this bucket
    for (auto iter:  _safe_iterators_) {
      if (iter-> _bucket_ == bucket) {
        iter->operator++();
        iter-> _next_bucket_ = iter-> _bucket_;
        iter-> _bucket_      = nullptr;
      } else if (iter-> _next_bucket_ == bucket) {
        iter-> _bucket_ = bucket;
        iter->operator++();
        iter-> _next_bucket_ = iter-> _bucket_;
        iter-> _bucket_      = nullptr;
      }
    }

    // remove the element from the  _nodes_ vector
     _nodes_[index].erase(bucket);

    -- _nb_elements_;

    if ((index ==  _begin_index_) &&  _nodes_[index].empty()) {
       _begin_index_ = std::numeric_limits< Size >::max();
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::erase(const Key& key) {
    // get the hashed key
    Size hash =  _hash_func_(key);

    // get the bucket containing the element to erase
    HashTableBucket< Key, Val >* bucket =  _nodes_[hash].bucket(key);

     _erase_(bucket, hash);
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::erase(const iterator_safe& iter) {
     _erase_(iter. _getBucket_(), iter. _getIndex_());
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void
     HashTable< Key, Val, Alloc >::erase(const const_iterator_safe& iter) {
     _erase_(iter. _getBucket_(), iter. _getIndex_());
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE void HashTable< Key, Val, Alloc >::eraseByVal(const Val& val) {
    for (auto iter = cbegin(); iter != cend(); ++iter)
      if (iter. _bucket_->val() == val) {
         _erase_(iter. _getBucket_(), iter. _getIndex_());
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
      if (iter. _bucket_->val() == val) return iter.key();

    GUM_ERROR(NotFound, "not enough elements in the chained list")
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE const Key& HashTable< Key, Val, Alloc >::key(const Key& key) const {
    // get the bucket corresponding to the key
    Bucket* bucket =  _nodes_[ _hash_func_(key)].bucket(key);

    if (bucket == nullptr) {
      GUM_ERROR(NotFound, "key does not belong to the hashtable")
    }

    return bucket->key();
  }

  template < typename Key, typename Val, typename Alloc >
  void HashTable< Key, Val, Alloc >::eraseAllVal(const Val& val) {
    for (auto iterAll = cbeginSafe(); iterAll != cendSafe(); ++iterAll) {
      if (iterAll. _bucket_->val() == val) {
         _erase_(iterAll. _bucket_, iterAll. _index_);
      }
    }
  }

  template < typename Key, typename Val, typename Alloc >
  INLINE bool HashTable< Key, Val, Alloc >::empty() const noexcept {
    return ( _nb_elements_ == Size(0));
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
    if (size == 0) size = std::max(Size(2),  _nb_elements_ / 2);

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
    if (size == Size(0)) size = std::max(Size(2),  _nb_elements_ / 2);

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
    if (size == Size(0)) size = std::max(Size(2),  _nb_elements_ / 2);

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
    if (size == Size(0)) size = std::max(Size(2),  _nb_elements_ / 2);

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
    if (from. _nb_elements_ !=  _nb_elements_) return false;

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

    for (HashTableBucket< Key, Val >* ptr = list. _deb_list_; ptr;
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

    for (HashTableBucket< Key, Val >* ptr = list. _deb_list_; ptr;
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

    for (Size i = Size(0); i < table. _size_; ++i)
      for (auto ptr = table. _nodes_[i]. _deb_list_; ptr; ptr = ptr->next) {
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

    for (Size i = Size(0); i < table. _size_; ++i)
      for (auto ptr = table. _nodes_[i]. _deb_list_; ptr; ptr = ptr->next) {
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
     HashTableConstIteratorSafe< Key, Val >:: _insertIntoSafeList_() const {
     _table_-> _safe_iterators_.push_back(
       const_cast< HashTableConstIteratorSafe< Key, Val >* >(this));
  }

  template < typename Key, typename Val >
  INLINE void
     HashTableConstIteratorSafe< Key, Val >:: _removeFromSafeList_() const {
    if ( _table_ == nullptr) return;

    // find where the iterator is
    std::vector< HashTableConstIteratorSafe< Key, Val >* >& iter_vect
       =  _table_-> _safe_iterators_;

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
       _table_{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
     _insertIntoSafeList_();

    if ( _table_-> _nb_elements_) {
      if ( _table_-> _begin_index_ != std::numeric_limits< Size >::max()) {
         _index_  =  _table_-> _begin_index_;
         _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (Size i =  _table_-> _size_ - Size(1);; --i) {   // no test on i since
                                                           //  _nb_elements_ != 0
          if ( _table_-> _nodes_[i]. _nb_elements_) {
             _index_                = i;
             _bucket_               =  _table_-> _nodes_[ _index_]. _end_list_;
             _table_-> _begin_index_ =  _index_;
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
       _table_{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == Size(0))
        && ( _table_-> _begin_index_ != std::numeric_limits< Size >::max())) {
       _index_  =  _table_-> _begin_index_;
       _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < ( _table_-> _nb_elements_ >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i =  _table_-> _size_ - 1;; --i) {   // no test on i since
          // ind_elt < table_-> _nb_elements_
          if ( _table_-> _nodes_[i]. _nb_elements_) {
            if (ind_elt >=  _table_-> _nodes_[i]. _nb_elements_)
              ind_elt -=  _table_-> _nodes_[i]. _nb_elements_;
            else {
              for ( _bucket_ =  _table_-> _nodes_[i]. _end_list_; ind_elt;
                   --ind_elt,  _bucket_ =  _bucket_->prev) {}

               _index_ = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >=  _table_-> _nb_elements_) {
          GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the hashtable")
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt =  _table_-> _nb_elements_ - ind_elt - 1;; ++i) {
          if ( _table_-> _nodes_[i]. _nb_elements_) {
            if (ind_elt >=  _table_-> _nodes_[i]. _nb_elements_)
              ind_elt -=  _table_-> _nodes_[i]. _nb_elements_;
            else {
              for ( _bucket_ =  _table_-> _nodes_[i]. _deb_list_; ind_elt;
                   --ind_elt,  _bucket_ =  _bucket_->next) {}

               _index_ = i;
              break;
            }
          }
        }
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
     _insertIntoSafeList_();
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTableConstIteratorSafe< Key, Val >& from) :
       _table_{from. _table_},
       _index_{from. _index_},  _bucket_{from. _bucket_},  _next_bucket_{
                                                         from. _next_bucket_} {
    // make the hashtable keep track of this iterator
    if ( _table_ != nullptr) {  _insertIntoSafeList_(); }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     const HashTableConstIterator< Key, Val >& from) :
       _table_{from. _table_},
       _index_{from. _index_},  _bucket_{from. _bucket_} {
    // make the hashtable keep track of this iterator
    if ( _table_ != nullptr) {  _insertIntoSafeList_(); }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIteratorSafe< Key, Val >::HashTableConstIteratorSafe(
     HashTableConstIteratorSafe< Key, Val >&& from) :
       _table_{from. _table_},
       _index_{from. _index_},  _bucket_{from. _bucket_},  _next_bucket_{
                                                         from. _next_bucket_} {
    GUM_CONS_MOV(HashTableConstIteratorSafe);

    // find "from" in the hashtable's list of safe iterators and substitute
    // it by this
    if ( _table_ != nullptr) {
      std::vector< HashTableConstIteratorSafe< Key, Val >* >& vect
         =  _table_-> _safe_iterators_;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
        if (*ptr == &from) {
          *ptr         = this;
          from. _table_ = nullptr;
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
     _removeFromSafeList_();
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
    if ( _table_ != from. _table_) {
      // remove the iterator from its hashtable iterator's list'
       _removeFromSafeList_();

       _table_ = from. _table_;

      // add to the new table
      if ( _table_) {  _insertIntoSafeList_(); }
    }

     _index_       = from. _index_;
     _bucket_      = from. _bucket_;
     _next_bucket_ = from. _next_bucket_;

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
    if ( _table_ != from. _table_) {
      // remove the iterator from its hashtable iterator's list'
       _removeFromSafeList_();

       _table_ = from. _table_;

      // add to the new table
      if ( _table_) {  _insertIntoSafeList_(); }
    }

     _index_       = from. _index_;
     _bucket_      = from. _bucket_;
     _next_bucket_ = nullptr;

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
    if ( _table_ != from. _table_) {
      // remove the iterator from its hashtable iterator's list'
       _removeFromSafeList_();

      if (from. _table_ != nullptr) {
        // substitute from by this in the list of safe iterators
        std::vector< HashTableConstIteratorSafe< Key, Val >* >& vect
           = from. _table_-> _safe_iterators_;

        for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
          if (*ptr == &from) {
            *ptr = this;
            break;
          }
        }
      }

       _table_      = from. _table_;
      from. _table_ = nullptr;
    }

     _index_       = from. _index_;
     _bucket_      = from. _bucket_;
     _next_bucket_ = from. _next_bucket_;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::key_type&
     HashTableConstIteratorSafe< Key, Val >::key() const {
    if ( _bucket_ != nullptr)
      return  _bucket_->key();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::mapped_type&
     HashTableConstIteratorSafe< Key, Val >::val() const {
    if ( _bucket_ != nullptr)
      return  _bucket_->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE void HashTableConstIteratorSafe< Key, Val >::clear() noexcept {
    // remove the iterator from the table's iterator list
     _removeFromSafeList_();

    // set its table as well as the element it points to to 0
     _table_       = nullptr;
     _bucket_      = nullptr;
     _next_bucket_ = nullptr;
     _index_       = Size(0);
  }

  // WARNING: never inline this function: this result in g++4.3.3 producing a
  // code that segfaults.
  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator++() noexcept {
    // if  _bucket_ != nullptr then use it, else use next_bucket
    if ( _bucket_ == nullptr) {
      // note that this case only happens when the iterator pointed to an
      // element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the  _next_bucket_ and  _index_
      // fields.
       _bucket_      =  _next_bucket_;
       _next_bucket_ = nullptr;
    } else {
      // ok, here we can use  _bucket_ as a starting point

      // if we are not pointing on the first element of the chained list, just
      // point to the preceding bucket in this list
      if ( _bucket_->prev) {
         _bucket_ =  _bucket_->prev;
        // here, no need to update  _next_bucket_, which is compulsorily
        // equal to nullptr, nor  _index_ which has not changed.
      } else {
        // ok, here we are on the beginning of a chained list,
        // so 2 cases can obtain:
        // 1/ index = 0 : then we have reached the end of the hashtable
        // 2/ index != 0 => we must search for a new slot containing elements

        // case 1:
        if ( _index_ == Size(0)) {
           _bucket_ = nullptr;
          // we are thus at the end() of the hashTable
        }
        // case 2:
        else {
          // arrived here, we need to parse the hash table until we find a new
          // bucket because we are pointing on a chained list with no more
          // element
          // to the left of the current element
          if ( _index_ > Size(0)) {
            for (Size i =  _index_ - Size(1); i > Size(0); --i) {
              if ( _table_-> _nodes_[i]. _nb_elements_) {
                 _index_  = i;
                 _bucket_ =  _table_-> _nodes_[i]. _end_list_;
                return *this;
              }
            }
          }

          if ( _table_-> _nodes_[0]. _nb_elements_)
             _bucket_ =  _table_-> _nodes_[0]. _end_list_;
          else
             _bucket_ = nullptr;

           _index_ = 0;
        }
      }
    }

    return *this;
  }

  template < typename Key, typename Val >
  HashTableConstIteratorSafe< Key, Val >&
     HashTableConstIteratorSafe< Key, Val >::operator+=(Size nb) noexcept {
    if ((nb == Size(0)) || ( _table_ == nullptr)) return *this;

    // if  _bucket_ != nullptr then use it, else use next_bucket
    if ( _bucket_ == nullptr) {
      // note that this case only happens when the iterator pointed to an
      // element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the  _next_bucket_ and  _index_
      // fields.
       _bucket_      =  _next_bucket_;
       _next_bucket_ = nullptr;
      --nb;
    }

    // ok, here we can use  _bucket_ as a starting point: parse all the elements
    // of the current chained list
    for (; nb &&  _bucket_ != nullptr; --nb,  _bucket_ =  _bucket_->prev) {}

    if ( _bucket_ != nullptr) return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    -- _index_;

    for (;  _index_ <  _table_-> _size_
           && nb >=  _table_-> _nodes_[ _index_]. _nb_elements_;
         nb -=  _table_-> _nodes_[ _index_]. _nb_elements_, -- _index_) {}

    // here: either  _index_ >=  _table_-> _size_, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb <  _table_-> _nodes_[ _index_]. _nb_elements_, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if ( _index_ >=  _table_-> _size_) {
       _index_ = Size(0);
      return *this;
    }

    for ( _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_; nb;
         --nb,  _bucket_ =  _bucket_->prev) {}

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
    return (( _bucket_ != from. _bucket_) || ( _index_ != from. _index_));
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIteratorSafe< Key, Val >::operator==(
     const HashTableConstIteratorSafe< Key, Val >& from) const noexcept {
    return (( _bucket_ == from. _bucket_) && ( _index_ == from. _index_));
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIteratorSafe< Key, Val >::value_type&
     HashTableConstIteratorSafe< Key, Val >::operator*() const {
    if ( _bucket_)
      return  _bucket_->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE HashTableBucket< Key, Val >*
         HashTableConstIteratorSafe< Key, Val >:: _getBucket_() const noexcept {
    return  _bucket_;
  }

  template < typename Key, typename Val >
  INLINE Size HashTableConstIteratorSafe< Key, Val >:: _getIndex_() const noexcept {
    return  _index_;
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
       _table_{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIterator);

    if ( _table_-> _nb_elements_) {
      if ( _table_-> _begin_index_ != std::numeric_limits< Size >::max()) {
         _index_  =  _table_-> _begin_index_;
         _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (Size i =  _table_-> _size_ - Size(1);; --i) {   // no test on i since
          //  _nb_elements_ != 0
          if ( _table_-> _nodes_[i]. _nb_elements_) {
             _index_                = i;
             _bucket_               =  _table_-> _nodes_[ _index_]. _end_list_;
             _table_-> _begin_index_ =  _index_;
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
       _table_{reinterpret_cast< const HashTable< Key, Val >* >(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == Size(0))
        && ( _table_-> _begin_index_ != std::numeric_limits< Size >::max())) {
       _index_  =  _table_-> _begin_index_;
       _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < ( _table_-> _nb_elements_ >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i =  _table_-> _size_ - 1;; --i) {   // no test on i since
          // ind_elt < table_-> _nb_elements_
          if ( _table_-> _nodes_[i]. _nb_elements_) {
            if (ind_elt >=  _table_-> _nodes_[i]. _nb_elements_)
              ind_elt -=  _table_-> _nodes_[i]. _nb_elements_;
            else {
              for ( _bucket_ =  _table_-> _nodes_[i]. _end_list_; ind_elt;
                   --ind_elt,  _bucket_ =  _bucket_->prev) {}

               _index_ = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >=  _table_-> _nb_elements_) {
          GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the hashtable")
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt =  _table_-> _nb_elements_ - ind_elt - 1;; ++i) {
          if ( _table_-> _nodes_[i]. _nb_elements_) {
            if (ind_elt >=  _table_-> _nodes_[i]. _nb_elements_)
              ind_elt -=  _table_-> _nodes_[i]. _nb_elements_;
            else {
              for ( _bucket_ =  _table_-> _nodes_[i]. _deb_list_; ind_elt;
                   --ind_elt,  _bucket_ =  _bucket_->next) {}

               _index_ = i;
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
       _table_{from. _table_},
       _index_{from. _index_},  _bucket_{from. _bucket_} {
    GUM_CONS_CPY(HashTableConstIterator);
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >::HashTableConstIterator(
     HashTableConstIterator< Key, Val >&& from) noexcept :
       _table_{from. _table_},
       _index_{from. _index_},  _bucket_{from. _bucket_} {
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
     _table_  = from. _table_;
     _index_  = from. _index_;
     _bucket_ = from. _bucket_;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator=(
        HashTableConstIterator< Key, Val >&& from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
     _table_  = from. _table_;
     _index_  = from. _index_;
     _bucket_ = from. _bucket_;

    return *this;
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::key_type&
     HashTableConstIterator< Key, Val >::key() const {
    if ( _bucket_)
      return  _bucket_->pair.first;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::mapped_type&
     HashTableConstIterator< Key, Val >::val() const {
    if ( _bucket_)
      return  _bucket_->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE void HashTableConstIterator< Key, Val >::clear() noexcept {
     _table_  = nullptr;
     _bucket_ = nullptr;
     _index_  = 0;
  }

  template < typename Key, typename Val >
  HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator++() noexcept {
    // if  _bucket_ == nullptr then we are at the end of the hashtable
    if ( _bucket_ == nullptr) return *this;

    // if we are not pointing on the first element of the chained list, just
    // point to the next bucket in this list
    if ( _bucket_->prev) {
       _bucket_ =  _bucket_->prev;
      // here, no need to update  _index_ which has not changed.
    } else {
      // ok, here we are on the end of a chained list,
      // so 2 cases can obtain:
      // 1/ index = 0 : then we have reached the end of the hashtable
      // 2/ index != 0 => we must search for a new slot containing elements

      // case 1:
      if ( _index_ == Size(0)) {
         _bucket_ = nullptr;
        // we are thus at the end() of the hashTable
      }

      // case 2:
      else {
        // arrived here, we need to parse the hash table until we find a new
        // bucket because we are pointing on a chained list with no more element
        // to the right of the current element
        for (Size i =  _index_ - Size(1); i; --i) {
          if ( _table_-> _nodes_[i]. _nb_elements_) {
             _index_  = i;
             _bucket_ =  _table_-> _nodes_[i]. _end_list_;
            return *this;
          }
        }

        if ( _table_-> _nodes_[0]. _nb_elements_)
           _bucket_ =  _table_-> _nodes_[0]. _end_list_;
        else
           _bucket_ = nullptr;

         _index_ = Size(0);
      }
    }

    return *this;
  }

  template < typename Key, typename Val >
  HashTableConstIterator< Key, Val >&
     HashTableConstIterator< Key, Val >::operator+=(Size nb) noexcept {
    if ((nb == 0) || ( _table_ == nullptr) || ( _bucket_ == nullptr)) return *this;

    // ok, here we can use  _bucket_ as a starting point: parse all the elements
    // of the current chained list
    for (; nb &&  _bucket_ != nullptr; --nb,  _bucket_ =  _bucket_->prev) {}

    if ( _bucket_ != nullptr) return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    -- _index_;

    for (;  _index_ <  _table_-> _size_
           && nb >=  _table_-> _nodes_[ _index_]. _nb_elements_;
         nb -=  _table_-> _nodes_[ _index_]. _nb_elements_, -- _index_) {}

    // here: either  _index_ >=  _table_-> _size_, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb <  _table_-> _nodes_[ _index_]. _nb_elements_, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if ( _index_ >=  _table_-> _size_) {
       _index_ = 0;
      return *this;
    }

    for ( _bucket_ =  _table_-> _nodes_[ _index_]. _end_list_; nb;
         --nb,  _bucket_ =  _bucket_->prev) {}

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
    return ( _bucket_ != from. _bucket_);
  }

  template < typename Key, typename Val >
  INLINE bool HashTableConstIterator< Key, Val >::operator==(
     const HashTableConstIterator< Key, Val >& from) const noexcept {
    return ( _bucket_ == from. _bucket_);
  }

  template < typename Key, typename Val >
  INLINE const typename HashTableConstIterator< Key, Val >::value_type&
     HashTableConstIterator< Key, Val >::operator*() const {
    if ( _bucket_)
      return  _bucket_->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
    }
  }

  template < typename Key, typename Val >
  INLINE typename HashTable< Key, Val >::Bucket*
     HashTableConstIterator< Key, Val >:: _getBucket_() const noexcept {
    return  _bucket_;
  }

  template < typename Key, typename Val >
  INLINE Size HashTableConstIterator< Key, Val >:: _getIndex_() const noexcept {
    return  _index_;
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
    if (this-> _bucket_)
      return this-> _bucket_->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object")
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
