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
 * @brief template implementation of chained lists
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

// to ease parser
#include <agrum/tools/core/list.h>

namespace gum {

  // ===========================================================================
  // ===========================================================================
  // ===                         BUCKET IMPLEMENTATION                       ===
  // ===========================================================================
  // ===========================================================================

  // default constructor
  template < typename Val >
  INLINE ListBucket< Val >::ListBucket(const Val& v) : val__{v} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // constructor for Val rvalues
  template < typename Val >
  INLINE ListBucket< Val >::ListBucket(Val&& v) noexcept : val__{std::move(v)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // emplace constructor
  template < typename Val >
  template < typename... Args >
  INLINE ListBucket< Val >::ListBucket(typename ListBucket< Val >::Emplace,
                                       Args&&... args) :
      val__(std::forward< Args >(args)...) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // copy constructor
  template < typename Val >
  INLINE ListBucket< Val >::ListBucket(const ListBucket< Val >& src) :
      val__{src.val__} {
    // for debugging purposes
    GUM_CONS_CPY(ListBucket);
  }

  // copy operator
  template < typename Val >
  INLINE ListBucket< Val >&
     ListBucket< Val >::operator=(const ListBucket< Val >& src) {
    // for debugging purposes
    GUM_OP_CPY(ListBucket);

    // no need to avoid self assignment
    val__ = src.val__;
    return *this;
  }

  // WARNING: during its deletion, the bucket does not take care of properly
  // re-chaining the chained list. This should be done by the Lists themselves
  template < typename Val >
  INLINE ListBucket< Val >::~ListBucket() {
    // for debugging purposes
    GUM_DESTRUCTOR(ListBucket);
  }

  // equality check
  template < typename Val >
  INLINE bool ListBucket< Val >::operator==(const ListBucket< Val >& src) const {
    return (src.val__ == val__);
  }

  // inequality check
  template < typename Val >
  INLINE bool ListBucket< Val >::operator!=(const ListBucket< Val >& src) const {
    return (src.val__ != val__);
  }

  // dereferencing operator
  template < typename Val >
  INLINE const Val& ListBucket< Val >::operator*() const noexcept {
    return val__;
  }

  // dereferencing operator
  template < typename Val >
  INLINE Val& ListBucket< Val >::operator*() noexcept {
    return val__;
  }

  // returns the bucket toward the next element
  template < typename Val >
  INLINE const ListBucket< Val >* ListBucket< Val >::next() const noexcept {
    return next__;
  }

  // returns the bucket toward the preceding element
  template < typename Val >
  INLINE const ListBucket< Val >* ListBucket< Val >::previous() const noexcept {
    return prev__;
  }

  // ===========================================================================
  // ===========================================================================
  // ===               UNSAFE_CONST_LIST_ITERATOR IMPLEMENTATION             ===
  // ===========================================================================
  // ===========================================================================

  // default constructor
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator() noexcept {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);
  }

  // default constructor
  template < typename Val >
  template < typename Alloc >
  INLINE ListConstIterator< Val >::ListConstIterator(
     const List< Val, Alloc >& theList) noexcept :
      bucket__{theList.deb_list__} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);
  }

  // copy constructor
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(
     const ListConstIterator< Val >& src) noexcept :
      bucket__{src.bucket__} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIterator);
  }

  // move constructor
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(
     ListConstIterator< Val >&& src) noexcept :
      bucket__{std::move(src.bucket__)} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIterator);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(const List< Val >& theList,
                                                     Size               ind_elt) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= theList.nb_elements__) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list")
    }

    // check if it is faster to find the indexth element from the start or
    // from the end of the list
    if (ind_elt < (theList.nb_elements__ >> 1)) {
      // find the element we shall point to src the start of the list
      for (bucket__ = theList.deb_list__; ind_elt;
           --ind_elt, bucket__ = bucket__->next__) {}
    } else {
      // find the element we shall point to src the end of the list
      for (bucket__ = theList.end_list__,
          ind_elt   = theList.nb_elements__ - ind_elt - 1;
           ind_elt;
           --ind_elt, bucket__ = bucket__->prev__) {}
    }
  }

  // Destructor
  template < typename Val >
  INLINE ListConstIterator< Val >::~ListConstIterator() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(ListConstIterator);
  }

  // Copy operator
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator=(
     const ListConstIterator< Val >& src) noexcept {
    // for debugging purposes
    GUM_OP_CPY(ListConstIterator);

    bucket__ = src.bucket__;
    return *this;
  }

  // move operator
  template < typename Val >
  INLINE ListConstIterator< Val >&
     ListConstIterator< Val >::operator=(ListConstIterator< Val >&& src) noexcept {
    // for debugging purposes
    GUM_OP_MOV(ListConstIterator);
    bucket__ = src.bucket__;
    return *this;
  }

  // returns the bucket the iterator is pointing to
  template < typename Val >
  INLINE ListBucket< Val >*
         ListConstIterator< Val >::getBucket__() const noexcept {
    return bucket__;
  }

  // Makes the iterator point toward nothing
  template < typename Val >
  INLINE void ListConstIterator< Val >::clear() noexcept {
    bucket__ = nullptr;
  }

  // positions the iterator to the end of the list
  template < typename Val >
  INLINE void ListConstIterator< Val >::setToEnd() noexcept {
    bucket__ = nullptr;
  }

  // returns a bool indicating whether the iterator points to the end of the
  // list
  template < typename Val >
  INLINE bool ListConstIterator< Val >::isEnd() const noexcept {
    return (bucket__ == nullptr);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >&
     ListConstIterator< Val >::operator++() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the next bucket in this list
    if (bucket__ != nullptr) { bucket__ = bucket__->next__; }

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator+=(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (bucket__ != nullptr); --i, bucket__ = bucket__->next__) {}
    } else {
      for (; i && (bucket__ != nullptr); ++i, bucket__ = bucket__->prev__) {}
    }
    return *this;
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >&
     ListConstIterator< Val >::operator--() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the preceding bucket in this list
    if (bucket__ != nullptr) { bucket__ = bucket__->prev__; }

    return *this;
  }

  // makes the iterator point to i elements before in the list
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator-=(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (bucket__ != nullptr); --i, bucket__ = bucket__->prev__) {}
    } else {
      for (; i && (bucket__ != nullptr); ++i, bucket__ = bucket__->next__) {}
    }
    return *this;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListConstIterator< Val > ListConstIterator< Val >::operator+(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    return ListConstIterator< Val >(*this) += i;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListConstIterator< Val > ListConstIterator< Val >::operator-(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    return ListConstIterator< Val >(*this) -= i;
  }

  // checks whether two iterators point toward different elements
  template < typename Val >
  INLINE bool ListConstIterator< Val >::operator!=(
     const ListConstIterator< Val >& src) const noexcept {
    return (bucket__ != src.bucket__);
  }

  // checks whether two iterators point toward the same elements.
  template < typename Val >
  INLINE bool ListConstIterator< Val >::operator==(
     const ListConstIterator< Val >& src) const noexcept {
    return (bucket__ == src.bucket__);
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListConstIterator< Val >::operator->() const {
    if (bucket__ != nullptr)
      return &(bucket__->val__);
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object")
    }
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListConstIterator< Val >::operator*() const {
    if (bucket__ != nullptr)
      return bucket__->val__;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object")
    }
  }

  // for STL compliance, a distance operator
  template < typename Val >
  INLINE typename ListConstIterator< Val >::difference_type
     operator-(const ListConstIterator< Val >& iter1,
               const ListConstIterator< Val >& iter2) {
    typename ListConstIterator< Val >::difference_type res = 0;

    for (ListConstIterator< Val > iter3 = iter2; iter1 != iter3; ++iter3, ++res) {}

    return res;
  }

  // ===========================================================================
  // ===========================================================================
  // ===                  UNSAFE_LIST_ITERATOR IMPLEMENTATION                ===
  // ===========================================================================
  // ===========================================================================

  // basic constructor
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator() noexcept :
      ListConstIterator< Val >() {
    GUM_CONSTRUCTOR(ListIterator);
  }

  // constructor for a begin
  template < typename Val >
  template < typename Alloc >
  INLINE
     ListIterator< Val >::ListIterator(const List< Val, Alloc >& theList) noexcept
      :
      ListConstIterator< Val >(theList) {
    GUM_CONSTRUCTOR(ListIterator);
  }

  // copy constructor
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator(const ListIterator< Val >& src) noexcept
      :
      ListConstIterator< Val >(src) {
    GUM_CONS_CPY(ListIterator);
  }

  // move constructor
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator(ListIterator< Val >&& src) noexcept :
      ListConstIterator< Val >(std::move(src)) {
    GUM_CONS_MOV(ListIterator);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator(const List< Val >& theList,
                                           Size               ind_elt) :
      ListConstIterator< Val >(theList, ind_elt) {
    GUM_CONSTRUCTOR(ListIterator);
  }

  // Copy operator
  template < typename Val >
  INLINE ListIterator< Val >&
     ListIterator< Val >::operator=(const ListIterator< Val >& src) noexcept {
    GUM_OP_CPY(ListIterator);
    ListConstIterator< Val >::operator=(src);
    return *this;
  }

  // move operator
  template < typename Val >
  INLINE ListIterator< Val >&
     ListIterator< Val >::operator=(ListIterator< Val >&& src) noexcept {
    GUM_OP_MOV(ListIterator);
    ListConstIterator< Val >::operator=(std::move(src));
    return *this;
  }

  // Destructor
  template < typename Val >
  INLINE ListIterator< Val >::~ListIterator() noexcept {
    GUM_DESTRUCTOR(ListIterator);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListIterator< Val >& ListIterator< Val >::operator++() noexcept {
    ListConstIterator< Val >::operator++();
    return *this;
  }

  // makes the iterator point to i elements further in the List
  template < typename Val >
  INLINE ListIterator< Val >& ListIterator< Val >::operator+=(
     typename ListIterator< Val >::difference_type i) noexcept {
    ListConstIterator< Val >::operator+=(i);
    return *this;
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListIterator< Val >& ListIterator< Val >::operator--() noexcept {
    ListConstIterator< Val >::operator--();
    return *this;
  }

  // makes the iterator point to i elements before in the List
  template < typename Val >
  INLINE ListIterator< Val >& ListIterator< Val >::operator-=(
     typename ListIterator< Val >::difference_type i) noexcept {
    ListConstIterator< Val >::operator-=(i);
    return *this;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIterator< Val > ListIterator< Val >::operator+(
     typename ListIterator< Val >::difference_type i) noexcept {
    return ListIterator< Val >(*this) += i;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIterator< Val > ListIterator< Val >::operator-(
     typename ListIterator< Val >::difference_type i) noexcept {
    return ListIterator< Val >(*this) -= i;
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE Val* ListIterator< Val >::operator->() {
    return const_cast< Val* >(ListConstIterator< Val >::operator->());
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE Val& ListIterator< Val >::operator*() {
    return const_cast< Val& >(ListConstIterator< Val >::operator*());
  }

  // ===========================================================================
  // ===========================================================================
  // ===               SAFE LIST CONST ITERATOR IMPLEMENTATION               ===
  // ===========================================================================
  // ===========================================================================

  // basic constructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe() noexcept {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);
  }

  // Constructor for a begin
  template < typename Val >
  template < typename Alloc >
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe(
     const List< Val, Alloc >& theList) :
      list__{
         reinterpret_cast< const List< Val, std::allocator< Val > >* >(&theList)},
      bucket__{theList.deb_list__} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    theList.safe_iterators__.push_back(this);
  }

  // copy constructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe(
     const ListConstIteratorSafe< Val >& src) :
      list__{src.list__},
      bucket__{src.bucket__}, next_current_bucket__{src.next_current_bucket__},
      prev_current_bucket__{src.prev_current_bucket__}, null_pointing__{
                                                           src.null_pointing__} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    if (list__ != nullptr) list__->safe_iterators__.push_back(this);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  template < typename Alloc >
  ListConstIteratorSafe< Val >::ListConstIteratorSafe(
     const List< Val, Alloc >& theList,
     Size                      ind_elt) :
      list__{
         reinterpret_cast< const List< Val, std::allocator< Val > >* >(&theList)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= list__->nb_elements__) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list")
    }

    // check if it is faster to find the indexth element src the start or
    // src the end of the list
    if (ind_elt < (list__->nb_elements__ >> 1)) {
      // find the element we shall point to src the start of the list
      for (bucket__ = list__->deb_list__; ind_elt;
           --ind_elt, bucket__ = bucket__->next__) {}
    } else {
      // find the element we shall point to src the end of the list
      for (bucket__ = list__->end_list__,
          ind_elt   = list__->nb_elements__ - ind_elt - 1;
           ind_elt;
           --ind_elt, bucket__ = bucket__->prev__) {}
    }

    // add the iterator to the list of safe iterators
    theList.safe_iterators__.push_back(this);
  }

  // move constructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe(
     ListConstIteratorSafe< Val >&& src) :
      list__{src.list__},
      bucket__{src.bucket__}, next_current_bucket__{src.next_current_bucket__},
      prev_current_bucket__{src.prev_current_bucket__}, null_pointing__{
                                                           src.null_pointing__} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIteratorSafe);

    if (list__ != nullptr) {
      // substitute src by this in the list of safe iterators
      std::vector< ListConstIteratorSafe< Val >* >& vect
         = list__->safe_iterators__;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); --ptr) {
        if (*ptr == &src) {
          *ptr = this;
          break;
        }
      }

      src.list__          = nullptr;
      src.bucket__        = nullptr;
      src.null_pointing__ = false;
    }
  }

  // remove the iterator for its list' safe iterators list
  template < typename Val >
  INLINE void ListConstIteratorSafe< Val >::removeFromSafeList__() const {
    // find where the iterator is
    std::vector< ListConstIteratorSafe< Val >* >& vect = list__->safe_iterators__;

    for (auto i = vect.size() - 1; i >= 0; --i) {
      if (vect[i] == this) {
        vect.erase(vect.begin() + i);
        break;
      }
    }
  }

  // Copy operator
  template < typename Val >
  ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator=(
     const ListConstIteratorSafe< Val >& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(ListConstIteratorSafe);

      // check if src and this belong to the same list. If this is not
      // the case, we shall remove this from its iterator's list and
      // put it into src's list one.
      if (list__ && (src.list__ != list__)) {
        removeFromSafeList__();
        list__ = nullptr;
      }

      // if necessary, put this into the same list of safe iterators as src
      if (src.list__ && (src.list__ != list__)) {
        try {
          src.list__->safe_iterators__.push_back(this);
        } catch (...) {
          list__          = nullptr;
          bucket__        = nullptr;
          null_pointing__ = false;
          throw;
        }
      }

      list__                = src.list__;
      bucket__              = src.bucket__;
      prev_current_bucket__ = src.prev_current_bucket__;
      next_current_bucket__ = src.next_current_bucket__;
      null_pointing__       = src.null_pointing__;
    }

    return *this;
  }

  // move operator
  template < typename Val >
  ListConstIteratorSafe< Val >&
     ListConstIteratorSafe< Val >::operator=(ListConstIteratorSafe< Val >&& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_MOV(ListConstIteratorSafe);

      // if the two iterators do not point to the same list, remove
      // the current iterator from its safe iterators list
      if ((list__ != nullptr) && (src.list__ != list__)) {
        removeFromSafeList__();
        list__ = nullptr;
      }

      // now if src points to a list, put this at its location
      if ((src.list__ != nullptr)) {
        std::vector< ListConstIteratorSafe< Val >* >& vect
           = src.list__->safe_iterators__;
        Idx index_src = Size(vect.size()) - 1;

        for (;; --index_src) {
          if (vect[index_src] == &src) { break; }
        }

        if (list__ == nullptr) {
          vect[index_src] = this;
        } else {
          vect.erase(vect.begin() + index_src);
        }
      }

      list__                = src.list__;
      bucket__              = src.bucket__;
      prev_current_bucket__ = src.prev_current_bucket__;
      next_current_bucket__ = src.next_current_bucket__;
      null_pointing__       = src.null_pointing__;

      src.list__          = nullptr;
      src.bucket__        = nullptr;
      src.null_pointing__ = false;
    }

    return *this;
  }

  // Destructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::~ListConstIteratorSafe() {
    // for debugging purposes
    GUM_DESTRUCTOR(ListConstIteratorSafe);

    // remove the iterator src the table's iterator list
    if (list__) removeFromSafeList__();
  }

  // returns the bucket the iterator is pointing to
  template < typename Val >
  INLINE ListBucket< Val >*
         ListConstIteratorSafe< Val >::getBucket__() const noexcept {
    return bucket__;
  }

  // Makes the iterator point toward nothing
  template < typename Val >
  INLINE void ListConstIteratorSafe< Val >::clear() {
    // remove the iterator src the list's iterator list
    if (list__) removeFromSafeList__();

    // set its list as well as the element it points to to nullptr
    list__          = nullptr;
    bucket__        = nullptr;
    null_pointing__ = false;
  }

  // positions the iterator to the end of the list
  template < typename Val >
  INLINE void ListConstIteratorSafe< Val >::setToEnd() {
    clear();
  }

  // returns a bool indicating whether the iterator points to the end of the
  // list
  template < typename Val >
  INLINE bool ListConstIteratorSafe< Val >::isEnd() const {
    return null_pointing__ ? (next_current_bucket__ == nullptr)
                                && (prev_current_bucket__ == nullptr)
                           : (bucket__ == nullptr);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >&
     ListConstIteratorSafe< Val >::operator++() noexcept {
    // check if we are pointing to something that has been deleted
    if (null_pointing__) {
      null_pointing__ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a next element, just point on the latter
      if (next_current_bucket__ != nullptr) {
        bucket__ = next_current_bucket__->next__;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if prev_current_bucket is not null, then we are at rend and doing
      // a ++ shall now point to the beginning of the list
      if (prev_current_bucket__ != nullptr) {
        bucket__ = prev_current_bucket__;
        return *this;
      }

      // here, we are at the end of the chained list, hence we shall remain at
      // end
      bucket__ = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (bucket__ != nullptr) { bucket__ = bucket__->next__; }

      return *this;
    }
  }

  // makes the iterator point to i elements before in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >&
         ListConstIteratorSafe< Val >::opMinus__(Size i) noexcept {
    // check if we are pointing to something that has been deleted
    if (null_pointing__) {
      null_pointing__ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a preceding element, just point on the latter
      if (prev_current_bucket__ != nullptr) {
        bucket__ = prev_current_bucket__->prev__;
      } else {
        // here we were pointing on an extremity of the list (either end or
        // rend)
        // if next_current_bucket is not null, then we are at end and doing
        // a -- shall now point to the beginning of the list
        if (next_current_bucket__ != nullptr) {
          bucket__ = next_current_bucket__;
        } else {
          // here, we are at the rend of the chained list, hence we shall remain
          // at rend
          bucket__ = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (bucket__ != nullptr) { bucket__ = bucket__->prev__; }
    }

    for (--i; i && (bucket__ != nullptr); --i, bucket__ = bucket__->prev__) {}

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >&
         ListConstIteratorSafe< Val >::opPlus__(Size i) noexcept {
    // check if we are pointing to something that has been deleted
    if (null_pointing__) {
      null_pointing__ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a next element, just point on the latter
      if (next_current_bucket__ != nullptr) {
        bucket__ = next_current_bucket__->next__;
      } else {
        // here we were pointing on an extremity of the list (either end or
        // rend)
        // if prev_current_bucket is not null, then we are at rend and doing
        // a ++ shall now point to the beginning of the list
        if (prev_current_bucket__ != nullptr) {
          bucket__ = prev_current_bucket__;
        } else {
          // here, we are at the end of the chained list, hence we shall
          // remain at end
          bucket__ = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (bucket__ != nullptr) { bucket__ = bucket__->next__; }
    }

    for (--i; i && (bucket__ != nullptr); --i, bucket__ = bucket__->next__) {}

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator+=(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    if (!i) return *this;

    if (i < 0)
      return opMinus__(-i);
    else
      return opPlus__(i);
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >&
     ListConstIteratorSafe< Val >::operator--() noexcept {
    // check if we are pointing to something that has been deleted
    if (null_pointing__) {
      null_pointing__ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a preceding element, just point on the latter
      if (prev_current_bucket__ != nullptr) {
        bucket__ = prev_current_bucket__->prev__;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if next_current_bucket is not null, then we are at end and doing
      // a -- shall now point to the beginning of the list
      if (next_current_bucket__ != nullptr) {
        bucket__ = next_current_bucket__;
        return *this;
      }

      // here, we are at the rend of the chained list, hence we shall remain
      // at rend
      bucket__ = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (bucket__ != nullptr) { bucket__ = bucket__->prev__; }

      return *this;
    }
  }

  // makes the iterator point to i elements before in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator-=(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    if (!i) return *this;

    if (i < 0)
      return opPlus__(-i);
    else
      return opMinus__(i);
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListConstIteratorSafe< Val > ListConstIteratorSafe< Val >::operator+(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    return ListConstIteratorSafe< Val >(*this) += i;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListConstIteratorSafe< Val > ListConstIteratorSafe< Val >::operator-(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    return ListConstIteratorSafe< Val >(*this) -= i;
  }

  // checks whether two iterators point toward different elements
  template < typename Val >
  INLINE bool ListConstIteratorSafe< Val >::operator!=(
     const ListConstIteratorSafe< Val >& src) const {
    return null_pointing__
            ? (next_current_bucket__ != src.next_current_bucket__)
                 || (prev_current_bucket__ != src.prev_current_bucket__)
            : (bucket__ != src.bucket__);
  }

  // checks whether two iterators point toward the same elements.
  template < typename Val >
  INLINE bool ListConstIteratorSafe< Val >::operator==(
     const ListConstIteratorSafe< Val >& src) const {
    return null_pointing__
            ? (next_current_bucket__ == src.next_current_bucket__)
                 && (prev_current_bucket__ == src.prev_current_bucket__)
            : (bucket__ == src.bucket__);
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListConstIteratorSafe< Val >::operator->() const {
    if (bucket__ != nullptr)
      return &(bucket__->val__);
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object")
    }
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListConstIteratorSafe< Val >::operator*() const {
    if (bucket__ != nullptr)
      return bucket__->val__;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object")
    }
  }

  // for STL compliance, a distance operator
  template < typename Val >
  INLINE typename ListConstIteratorSafe< Val >::difference_type
     operator-(const ListConstIteratorSafe< Val >& iter1,
               const ListConstIteratorSafe< Val >& iter2) {
    typename ListConstIteratorSafe< Val >::difference_type res = 0;
    ListConstIteratorSafe< Val >                           iter3{iter2};

    for (; iter1 != iter3; ++iter3, ++res) {}

    return res;
  }

  // ===========================================================================
  // ===========================================================================
  // ===                     LIST ITERATOR IMPLEMENTATION                    ===
  // ===========================================================================
  // ===========================================================================

  // basic constructor
  template < typename Val >
  INLINE ListIteratorSafe< Val >::ListIteratorSafe() noexcept :
      ListConstIteratorSafe< Val >() {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  // constructor for a begin
  template < typename Val >
  template < typename Alloc >
  INLINE
     ListIteratorSafe< Val >::ListIteratorSafe(const List< Val, Alloc >& theList) :
      ListConstIteratorSafe< Val >(theList) {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  // copy constructor
  template < typename Val >
  INLINE ListIteratorSafe< Val >::ListIteratorSafe(
     const ListIteratorSafe< Val >& src) :
      ListConstIteratorSafe< Val >(src) {
    GUM_CONS_CPY(ListIteratorSafe);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  template < typename Alloc >
  INLINE
     ListIteratorSafe< Val >::ListIteratorSafe(const List< Val, Alloc >& theList,
                                               Size                      ind_elt) :
      ListConstIteratorSafe< Val >(theList, ind_elt) {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  // move constructor
  template < typename Val >
  INLINE ListIteratorSafe< Val >::ListIteratorSafe(ListIteratorSafe< Val >&& src) :
      ListConstIteratorSafe< Val >(std::move(src)) {
    GUM_CONS_MOV(ListIteratorSafe);
  }

  // Copy operator
  template < typename Val >
  INLINE ListIteratorSafe< Val >&
     ListIteratorSafe< Val >::operator=(const ListIteratorSafe< Val >& src) {
    // for debugging purposes
    GUM_OP_CPY(ListIteratorSafe);
    ListConstIteratorSafe< Val >::operator=(src);
    return *this;
  }

  // move operator
  template < typename Val >
  INLINE ListIteratorSafe< Val >&
     ListIteratorSafe< Val >::operator=(ListIteratorSafe< Val >&& src) {
    // for debugging purposes
    GUM_OP_MOV(ListIteratorSafe);
    ListConstIteratorSafe< Val >::operator=(std::move(src));
    return *this;
  }

  // Destructor
  template < typename Val >
  INLINE ListIteratorSafe< Val >::~ListIteratorSafe() {
    GUM_DESTRUCTOR(ListIteratorSafe);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListIteratorSafe< Val >& ListIteratorSafe< Val >::operator++() noexcept {
    ListConstIteratorSafe< Val >::operator++();
    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListIteratorSafe< Val >& ListIteratorSafe< Val >::operator+=(
     typename ListIteratorSafe< Val >::difference_type i) noexcept {
    ListConstIteratorSafe< Val >::operator+=(i);
    return *this;
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListIteratorSafe< Val >& ListIteratorSafe< Val >::operator--() noexcept {
    ListConstIteratorSafe< Val >::operator--();
    return *this;
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListIteratorSafe< Val >& ListIteratorSafe< Val >::operator-=(
     typename ListIteratorSafe< Val >::difference_type i) noexcept {
    ListConstIteratorSafe< Val >::operator-=(i);
    return *this;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIteratorSafe< Val > ListIteratorSafe< Val >::operator+(
     typename ListIteratorSafe< Val >::difference_type i) noexcept {
    return ListIteratorSafe< Val >(*this) += i;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIteratorSafe< Val > ListIteratorSafe< Val >::operator-(
     typename ListIteratorSafe< Val >::difference_type i) noexcept {
    return ListIteratorSafe< Val >(*this) -= i;
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE Val* ListIteratorSafe< Val >::operator->() {
    return const_cast< Val* >(ListConstIteratorSafe< Val >::operator->());
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE Val& ListIteratorSafe< Val >::operator*() {
    return const_cast< Val& >(ListConstIteratorSafe< Val >::operator*());
  }

  // ===========================================================================
  // ===========================================================================
  // ===                          LIST IMPLEMENTATION                        ===
  // ===========================================================================
  // ===========================================================================

  // a function used to perform copies of elements of Lists
  template < typename Val, typename Alloc >
  template < typename OtherAlloc >
  void List< Val, Alloc >::copy_elements__(const List< Val, OtherAlloc >& src) {
    ListBucket< Val >* ptr;
    ListBucket< Val >* old_ptr = nullptr;
    ListBucket< Val >* new_elt = nullptr;

    // copy src's list
    try {
      for (ptr = src.deb_list__; ptr != nullptr; ptr = ptr->next__) {
        // create a copy bucket
        new_elt = alloc_bucket__.allocate(1);

        try {
          alloc_bucket__.construct(new_elt, *ptr);
        } catch (...) {
          alloc_bucket__.deallocate(new_elt, 1);
          throw;
        }

        // rechain properly the new list (the next field is already initialized
        // with nullptr)
        new_elt->prev__ = old_ptr;

        if (old_ptr)
          old_ptr->next__ = new_elt;
        else
          deb_list__ = new_elt;

        old_ptr = new_elt;
      }
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (; deb_list__ != nullptr;
           deb_list__ = const_cast< ListBucket< Val >* >(ptr)) {
        ptr = deb_list__->next__;
        alloc_bucket__.destroy(deb_list__);
        alloc_bucket__.deallocate(deb_list__, 1);
      }

      deb_list__ = nullptr;
      throw;
    }

    // update properly the end of the chained list and the number of elements
    end_list__    = old_ptr;
    nb_elements__ = src.nb_elements__;
  }

  // deletes all the elements of a chained list.
  template < typename Val, typename Alloc >
  void List< Val, Alloc >::clear() {
    // first we update all the safe iterators of the list : they should now
    // point to end/rend
    for (const auto ptr_iter: safe_iterators__) {
      ptr_iter->clear();
    }

    // clear all the values
    for (ListBucket< Val >*ptr = deb_list__, *next_ptr = nullptr; ptr != nullptr;
         ptr = next_ptr) {
      next_ptr = ptr->next__;
      alloc_bucket__.destroy(ptr);
      alloc_bucket__.deallocate(ptr, 1);
    }

    nb_elements__ = 0;
    deb_list__    = nullptr;
    end_list__    = nullptr;
  }

  // A basic constructor that creates an empty list
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >::List() {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // reserve space for only the default number of iterators
    safe_iterators__.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // Copy constructor
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >::List(const List< Val, Alloc >& src) {
    // for debugging purposes
    GUM_CONS_CPY(List);

    // copy the elements
    copy_elements__(src);

    // reserve space for only the default number of iterators
    safe_iterators__.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // generalized copy constructor
  template < typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE List< Val, Alloc >::List(const List< Val, OtherAlloc >& src) {
    // for debugging purposes
    GUM_CONS_CPY(List);

    // copy the elements
    copy_elements__(src);

    // reserve space for only the default number of iterators
    safe_iterators__.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // move constructor
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >::List(List< Val, Alloc >&& src) :
      deb_list__{std::move(src.deb_list__)}, end_list__{std::move(src.end_list__)},
      nb_elements__{std::move(src.nb_elements__)},
      safe_iterators__{std::move(src.safe_iterators__)}, alloc_bucket__{std::move(
                                                            src.alloc_bucket__)} {
    // for debugging purposes
    GUM_CONS_MOV(List);

    src.deb_list__    = nullptr;
    src.end_list__    = nullptr;
    src.nb_elements__ = 0;
    src.safe_iterators__.clear();
  }

  // initializer_list constructor
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >::List(std::initializer_list< Val > list) {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // adding all the elements
    for (const auto& val: list) {
      pushBack(val);
    }

    // reserve space for only the default number of iterators
    safe_iterators__.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // Destructor
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >::~List() {
    // for debugging (although this program is bug-free)
    GUM_DESTRUCTOR(List);

    // we detach all the safe iterators attached to the current List and we
    // remove all the elements from the list
    clear();
  }

  // Copy operator. The List iterator's list is not shared with that of \e src.
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >&
     List< Val, Alloc >::operator=(const List< Val, Alloc >& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      copy_elements__(src);
    }

    return *this;
  }

  // Generalized copy operator.
  template < typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE List< Val, Alloc >&
     List< Val, Alloc >::operator=(const List< Val, OtherAlloc >& src) {
    // avoid self assignment
    if (this != reinterpret_cast< List< Val, Alloc >* >(&src)) {
      // for debugging purposes
      GUM_OP_CPY(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      copy_elements__(src);
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Alloc >
  INLINE List< Val, Alloc >&
     List< Val, Alloc >::operator=(List< Val, Alloc >&& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_MOV(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the move
      deb_list__       = std::move(src.deb_list__);
      end_list__       = std::move(src.end_list__);
      nb_elements__    = std::move(src.nb_elements__);
      safe_iterators__ = std::move(src.safe_iterators__);
      alloc_bucket__   = std::move(src.alloc_bucket__);

      src.deb_list__    = nullptr;
      src.end_list__    = nullptr;
      src.nb_elements__ = 0;
      src.safe_iterators__.clear();
    }

    return *this;
  }

  // the iterator pointing to the end of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIteratorSafe< Val >&
               List< Val, Alloc >::cendSafe() const noexcept {
    return *(
       reinterpret_cast< const ListConstIteratorSafe< Val >* >(list_end_safe__));
  }

  // the iterator pointing to the end of the List
  template < typename Val, typename Alloc >
  INLINE const ListIteratorSafe< Val >& List< Val, Alloc >::endSafe() noexcept {
    return *(reinterpret_cast< const ListIteratorSafe< Val >* >(list_end_safe__));
  }

  // the iterator pointing to the end of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIterator< Val >&
               List< Val, Alloc >::cend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the end of the List
  template < typename Val, typename Alloc >
  INLINE const ListIterator< Val >& List< Val, Alloc >::end() noexcept {
    return *(reinterpret_cast< const ListIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the end of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIterator< Val >& List< Val, Alloc >::end() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIteratorSafe< Val >&
               List< Val, Alloc >::crendSafe() const noexcept {
    return *(
       reinterpret_cast< const ListConstIteratorSafe< Val >* >(list_end_safe__));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val, typename Alloc >
  INLINE const ListIteratorSafe< Val >& List< Val, Alloc >::rendSafe() noexcept {
    return *(reinterpret_cast< const ListIteratorSafe< Val >* >(list_end_safe__));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIterator< Val >&
               List< Val, Alloc >::crend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val, typename Alloc >
  INLINE const ListIterator< Val >& List< Val, Alloc >::rend() noexcept {
    return *(reinterpret_cast< const ListIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val, typename Alloc >
  INLINE const ListConstIterator< Val >&
               List< Val, Alloc >::rend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(list_end__));
  }

  // the iterator pointing to the beginning of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIteratorSafe< Val > List< Val, Alloc >::cbeginSafe() const {
    return ListConstIteratorSafe< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val, typename Alloc >
  INLINE ListIteratorSafe< Val > List< Val, Alloc >::beginSafe() {
    return ListIteratorSafe< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIterator< Val > List< Val, Alloc >::cbegin() const {
    return ListConstIterator< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val, typename Alloc >
  INLINE ListIterator< Val > List< Val, Alloc >::begin() {
    return ListIterator< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIterator< Val > List< Val, Alloc >::begin() const {
    return ListConstIterator< Val >{*this};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIteratorSafe< Val > List< Val, Alloc >::crbeginSafe() const {
    if (nb_elements__)
      return ListConstIteratorSafe< Val >{*this, nb_elements__ - 1};
    else
      return ListConstIteratorSafe< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val, typename Alloc >
  INLINE ListIteratorSafe< Val > List< Val, Alloc >::rbeginSafe() {
    if (nb_elements__)
      return ListIteratorSafe< Val >{*this, nb_elements__ - 1};
    else
      return ListIteratorSafe< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIterator< Val > List< Val, Alloc >::crbegin() const {
    if (nb_elements__)
      return ListConstIterator< Val >{*this, nb_elements__ - 1};
    else
      return ListConstIterator< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val, typename Alloc >
  INLINE ListIterator< Val > List< Val, Alloc >::rbegin() {
    if (nb_elements__)
      return ListIterator< Val >{*this, nb_elements__ - 1};
    else
      return ListIterator< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val, typename Alloc >
  INLINE ListConstIterator< Val > List< Val, Alloc >::rbegin() const {
    if (nb_elements__)
      return ListConstIterator< Val >{*this, nb_elements__ - 1};
    else
      return ListConstIterator< Val >{};
  }

  // create a new bucket with a given value
  template < typename Val, typename Alloc >
  INLINE ListBucket< Val >*
         List< Val, Alloc >::createBucket__(const Val& val) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket< Val >* new_elt = alloc_bucket__.allocate(1);

    try {
      alloc_bucket__.construct(new_elt, val);
    } catch (...) {
      alloc_bucket__.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  // create a new bucket with a given value
  template < typename Val, typename Alloc >
  INLINE ListBucket< Val >* List< Val, Alloc >::createBucket__(Val&& val) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket< Val >* new_elt = alloc_bucket__.allocate(1);

    try {
      alloc_bucket__.construct(new_elt, std::move(val));
    } catch (...) {
      alloc_bucket__.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  // create an emplace bucket
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE ListBucket< Val >*
         List< Val, Alloc >::createEmplaceBucket__(Args&&... args) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket< Val >* new_elt = alloc_bucket__.allocate(1);

    try {
      alloc_bucket__.construct(new_elt,
                               ListBucket< Val >::Emplace::EMPLACE,
                               std::forward< Args >(args)...);
    } catch (...) {
      alloc_bucket__.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  // insert a bucket at the front of the list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushFront__(ListBucket< Val >* new_elt) {
    new_elt->next__ = deb_list__;

    if (deb_list__ != nullptr)
      deb_list__->prev__ = new_elt;
    else
      end_list__ = new_elt;

    deb_list__ = new_elt;

    // update the number of elements
    ++nb_elements__;

    // return the inserted element
    return new_elt->val__;
  }

  // insert a bucket at the end of the list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushBack__(ListBucket< Val >* new_elt) {
    // place the bucket at the end of the list
    new_elt->prev__ = end_list__;

    if (end_list__ != nullptr)
      end_list__->next__ = new_elt;
    else
      deb_list__ = new_elt;

    end_list__ = new_elt;

    // update the number of elements
    ++nb_elements__;

    // returns the current value
    return new_elt->val__;
  }

  // Insertion of a new element (a copy) at the beginning of the chained list.
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushFront(const Val& val) {
    return pushFront__(createBucket__(val));
  }

  // Insertion of a new element (a copy) at the beginning of the chained list.
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushFront(Val&& val) {
    return pushFront__(createBucket__(std::move(val)));
  }

  // an alias for pushFront used for STL compliance
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::push_front(Args&&... args) {
    return pushFront(std::forward< Args >(args)...);
  }

  // emplace elements at the beginning of the chained list
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::emplaceFront(Args&&... args) {
    return pushFront__(createEmplaceBucket__(std::forward< Args >(args)...));
  }

  // Insertion of a new element (a copy) at the end of the chained list.
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushBack(const Val& val) {
    return pushBack__(createBucket__(val));
  }

  // pushBack for rvalues
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::pushBack(Val&& val) {
    return pushBack__(createBucket__(std::move(val)));
  }

  // an alias for pushBack used for STL compliance
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::push_back(Args&&... args) {
    return pushBack(std::forward< Args >(args)...);
  }

  // emplace elements at the end of the chained list
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::emplaceBack(Args&&... args) {
    return pushBack__(createEmplaceBucket__(std::forward< Args >(args)...));
  }

  // Insertion of a new element at the end of the chained list (alias of
  // pushBack)
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(const Val& val) {
    return pushBack(val);
  }

  // insert for rvalues
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(Val&& val) {
    return pushBack(std::move(val));
  }

  // returns the bucket corresponding to the ith position
  template < typename Val, typename Alloc >
  INLINE ListBucket< Val >*
         List< Val, Alloc >::getIthBucket__(Size i) const noexcept {
    ListBucket< Val >* ptr;

    if (i < nb_elements__ / 2) {
      for (ptr = deb_list__; i; --i, ptr = ptr->next__) {}
    } else {
      for (ptr = end_list__, i = nb_elements__ - i - 1; i;
           --i, ptr            = ptr->prev__) {}
    }

    return ptr;
  }

  // insert a new bucket before another one
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insertBefore__(ListBucket< Val >* new_elt,
                                                 ListBucket< Val >* current_elt) {
    new_elt->next__     = current_elt;
    new_elt->prev__     = current_elt->prev__;
    current_elt->prev__ = new_elt;

    if (new_elt->prev__ == nullptr)
      deb_list__ = new_elt;
    else
      new_elt->prev__->next__ = new_elt;

    // update the number of elements
    ++nb_elements__;

    // returns the current value
    return new_elt->val__;
  }

  // insert a new bucket after another one
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insertAfter__(ListBucket< Val >* new_elt,
                                                ListBucket< Val >* current_elt) {
    new_elt->prev__     = current_elt;
    new_elt->next__     = current_elt->next__;
    current_elt->next__ = new_elt;

    if (new_elt->next__ == nullptr)
      end_list__ = new_elt;
    else
      new_elt->next__->prev__ = new_elt;

    // update the number of elements
    ++nb_elements__;

    // returns the current value
    return new_elt->val__;
  }

  // inserts a new element at the ith pos of the chained list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(Size pos, const Val& val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (nb_elements__ <= pos) { return pushBack(val); }

    return insertBefore__(createBucket__(val), getIthBucket__(pos));
  }

  // insert an rvalue at the ith pos of the chained list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(Size pos, Val&& val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (nb_elements__ <= pos) { return pushBack(std::move(val)); }

    return insertBefore__(createBucket__(std::move(val)), getIthBucket__(pos));
  }

  // inserts a new bucket before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert__(const const_iterator_safe& iter,
                                           ListBucket< Val >*         new_elt,
                                           location                   place) {
    // find the location around which the new element should be inserted
    ListBucket< Val >* ptr;

    if (iter.null_pointing__) {
      if (place == location::BEFORE) {
        ptr = iter.next_current_bucket__;
      } else {
        ptr = iter.prev_current_bucket__;
      }
    } else {
      ptr = iter.getBucket__();
    }

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return pushBack__(new_elt);
    } else {
      switch (place) {
        case location::BEFORE:
          return insertBefore__(new_elt, ptr);

        case location::AFTER:
          return insertAfter__(new_elt, ptr);

        default:
          GUM_ERROR(FatalError, "List insertion for this location unimplemented")
      }
    }
  }

  // inserts a new bucket before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert__(const const_iterator& iter,
                                           ListBucket< Val >*    new_elt,
                                           location              place) {
    // find the location around which the new element should be inserted
    ListBucket< Val >* ptr = iter.getBucket__();

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return pushBack__(new_elt);
    } else {
      switch (place) {
        case location::BEFORE:
          return insertBefore__(new_elt, ptr);

        case location::AFTER:
          return insertAfter__(new_elt, ptr);

        default:
          GUM_ERROR(FatalError, "List insertion for this location unimplemented")
      }
    }
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(const const_iterator_safe& iter,
                                         const Val&                 val,
                                         location                   place) {
    // if the iterator does not point to the list, raise an exception
    if (iter.list__ != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list")
    }

    return insert__(iter, createBucket__(val), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(const const_iterator_safe& iter,
                                         Val&&                      val,
                                         location                   place) {
    // if the iterator does not point to the list, raise an exception
    if (iter.list__ != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list")
    }

    return insert__(iter, createBucket__(std::move(val)), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(const const_iterator& iter,
                                         const Val&            val,
                                         location              place) {
    return insert__(iter, createBucket__(val), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::insert(const const_iterator& iter,
                                         Val&&                 val,
                                         location              place) {
    return insert__(iter, createBucket__(std::move(val)), place);
  }

  // emplace a new element before a given iterator
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::emplace(const const_iterator& iter,
                                          Args&&... args) {
    return insert__(iter,
                    createEmplaceBucket__(std::forward< Args >(args)...),
                    location::BEFORE);
  }

  // emplace a new element before a given iterator
  template < typename Val, typename Alloc >
  template < typename... Args >
  INLINE Val& List< Val, Alloc >::emplace(const const_iterator_safe& iter,
                                          Args&&... args) {
    return insert__(iter,
                    createEmplaceBucket__(std::forward< Args >(args)...),
                    location::BEFORE);
  }

  // returns a reference to first element of a list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::front() const {
    if (nb_elements__ == Size(0)) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    return deb_list__->val__;
  }

  // returns a reference to last element of a list
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::back() const {
    if (nb_elements__ == Size(0)) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    return end_list__->val__;
  }

  // returns the number of elements in the list.
  template < typename Val, typename Alloc >
  INLINE Size List< Val, Alloc >::size() const noexcept {
    return nb_elements__;
  }

  // checks whether there exists a given element in the list.
  template < typename Val, typename Alloc >
  INLINE bool List< Val, Alloc >::exists(const Val& val) const {
    for (ListBucket< Val >* ptr = deb_list__; ptr != nullptr; ptr = ptr->next__)
      if (ptr->val__ == val) return true;

    return false;
  }

  // suppresses a given bucket from a chained list.
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::erase__(ListBucket< Val >* bucket) {
    // perform deletion only if there is a bucket to remove
    if (bucket != nullptr) {
      // update the iterators pointing on this element
      for (const auto ptr_iter: safe_iterators__) {
        if (ptr_iter->bucket__ == bucket) {
          ptr_iter->next_current_bucket__ = bucket->prev__;
          ptr_iter->prev_current_bucket__ = bucket->next__;
          ptr_iter->bucket__              = nullptr;
          ptr_iter->null_pointing__       = true;
        } else {
          if (ptr_iter->null_pointing__) {
            if (ptr_iter->next_current_bucket__ == bucket)
              ptr_iter->next_current_bucket__ = bucket->prev__;

            if (ptr_iter->prev_current_bucket__ == bucket)
              ptr_iter->prev_current_bucket__ = bucket->next__;
          }
        }
      }

      // set properly the begin and end of the chained list (the other chainings
      // will be performed by operator delete)
      if (bucket->prev__ == nullptr)
        deb_list__ = bucket->next__;
      else
        bucket->prev__->next__ = bucket->next__;

      if (bucket->next__ == nullptr)
        end_list__ = bucket->prev__;
      else
        bucket->next__->prev__ = bucket->prev__;

      // remove the current element src the list
      alloc_bucket__.destroy(bucket);
      alloc_bucket__.deallocate(bucket, 1);

      --nb_elements__;
    }
  }

  // erases the ith element of the List (the first one is in position 0)
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::erase(Size i) {
    if (i >= nb_elements__) return;

    // erase the ith bucket
    erase__(getIthBucket__(i));
  }

  // erases the element of the List pointed to by the iterator
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::erase(const iterator_safe& iter) {
    erase__(iter.getBucket__());
  }

  // erases the element of the List pointed to by the iterator
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::erase(const const_iterator_safe& iter) {
    erase__(iter.getBucket__());
  }

  // returns the bucket corresponding to a given value.
  template < typename Val, typename Alloc >
  INLINE ListBucket< Val >*
         List< Val, Alloc >::getBucket__(const Val& val) const noexcept {
    for (ListBucket< Val >* ptr = deb_list__; ptr != nullptr; ptr = ptr->next__)
      if (ptr->val__ == val) return ptr;

    return nullptr;
  }

  // erases the first element encountered with a given value
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::eraseByVal(const Val& val) {
    erase__(getBucket__(val));
  }

  // erases all the elements encountered with a given value
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::eraseAllVal(const Val& val) {
    for (ListBucket< Val >*iter = deb_list__, *next_bucket = nullptr;
         iter != nullptr;
         iter = next_bucket) {
      next_bucket = iter->next__;

      if (val == iter->val__) erase__(iter);
    }
  }

  // removes the last element of a List
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::popBack() {
    erase__(end_list__);
  }

  // removes the first element of a List
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::popFront() {
    erase__(deb_list__);
  }

  // returns a boolean indicating whether the chained list is empty
  template < typename Val, typename Alloc >
  INLINE bool List< Val, Alloc >::empty() const noexcept {
    return (nb_elements__ == Size(0));
  }

  // displays the content of a chained list
  template < typename Val, typename Alloc >
  std::string List< Val, Alloc >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (ListBucket< Val >* ptr = deb_list__; ptr != nullptr;
         ptr = ptr->next__, deja = true) {
      if (deja) stream << " --> ";

      stream << ptr->val__;
    }

    stream << "]";

    return stream.str();
  }

  // creates a list of mountains src a list of val
  template < typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  List< Mount, OtherAlloc > List< Val, Alloc >::map(Mount (*f)(Val)) const {
    // create a new empty list
    List< Mount, OtherAlloc > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains src a list of val
  template < typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  List< Mount, OtherAlloc > List< Val, Alloc >::map(Mount (*f)(Val&)) const {
    // create a new empty list
    List< Mount, OtherAlloc > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains src a list of val
  template < typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  List< Mount, OtherAlloc > List< Val, Alloc >::map(Mount (*f)(const Val&)) const {
    // create a new empty list
    List< Mount, OtherAlloc > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains with a given value src a list of val
  template < typename Val, typename Alloc >
  template < typename Mount, typename OtherAlloc >
  List< Mount, OtherAlloc > List< Val, Alloc >::map(const Mount& mount) const {
    // create a new empty list
    List< Mount, OtherAlloc > list;

    // fill the new list
    for (Size i = Size(0); i < nb_elements__; ++i)
      list.pushBack(mount);

    return list;
  }

  // creates and insert a new element at the end of the list (alias of
  // pushBack).
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::operator+=(const Val& val) {
    return pushBack(val);
  }

  // creates and insert a new element at the end of the list (alias of
  // pushBack).
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::operator+=(Val&& val) {
    return pushBack(std::move(val));
  }

  // checks whether two lists are identical (same elements in the same order)
  template < typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     List< Val, Alloc >::operator==(const List< Val, OtherAlloc >& src) const {
    // check if the two lists have at least the same number of elements
    if (nb_elements__ != src.nb_elements__) return false;

    // parse the two lists
    for (ListBucket< Val >*iter1 = deb_list__, *iter2 = src.deb_list__;
         iter1 != nullptr;
         iter1 = iter1->next__, iter2 = iter2->next__)
      if (*iter1 != *iter2) return false;

    return true;
  }

  // checks whether two lists are different (different elements or orders)
  template < typename Val, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     List< Val, Alloc >::operator!=(const List< Val, OtherAlloc >& src) const {
    return !operator==(src);
  }

  // returns the ith element in the current chained list.
  template < typename Val, typename Alloc >
  INLINE Val& List< Val, Alloc >::operator[](const Size i) {
    // check if we can return the element we ask for
    if (i >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    return **getIthBucket__(i);
  }

  // returns the ith element in the current chained list.
  template < typename Val, typename Alloc >
  INLINE const Val& List< Val, Alloc >::operator[](const Size i) const {
    // check if we can return the element we ask for
    if (i >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the chained list")
    }

    return **getIthBucket__(i);
  }

  // replace the current list with another one
  template < typename Val, typename Alloc >
  INLINE void List< Val, Alloc >::swap(List& other_list) {
    std::swap(deb_list__, other_list.deb_list__);
    std::swap(end_list__, other_list.end_list__);
    std::swap(nb_elements__, other_list.nb_elements__);
    std::swap(safe_iterators__, other_list.safe_iterators__);
    std::swap(alloc_bucket__, other_list.alloc_bucket__);
  }

  // A \c << operator for List
  template < typename Val >
  std::ostream& operator<<(std::ostream& stream, const List< Val >& list) {
    stream << list.toString();
    return stream;
  }

} /* namespace gum */
