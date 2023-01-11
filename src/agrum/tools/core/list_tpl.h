/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
  INLINE ListBucket< Val >::ListBucket(const Val& v) : _val_{v} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // constructor for Val rvalues
  template < typename Val >
  INLINE ListBucket< Val >::ListBucket(Val&& v) noexcept : _val_{std::move(v)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // emplace constructor
  template < typename Val >
  template < typename... Args >
  INLINE ListBucket< Val >::ListBucket(typename ListBucket< Val >::Emplace, Args&&... args) :
      _val_(std::forward< Args >(args)...) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  // copy constructor
  template < typename Val >
  INLINE ListBucket< Val >::ListBucket(const ListBucket< Val >& src) : _val_{src._val_} {
    // for debugging purposes
    GUM_CONS_CPY(ListBucket);
  }

  // copy operator
  template < typename Val >
  INLINE ListBucket< Val >& ListBucket< Val >::operator=(const ListBucket< Val >& src) {
    // for debugging purposes
    GUM_OP_CPY(ListBucket);

    // no need to avoid self assignment
    _val_ = src._val_;
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
    return (src._val_ == _val_);
  }

  // inequality check
  template < typename Val >
  INLINE bool ListBucket< Val >::operator!=(const ListBucket< Val >& src) const {
    return (src._val_ != _val_);
  }

  // dereferencing operator
  template < typename Val >
  INLINE const Val& ListBucket< Val >::operator*() const noexcept {
    return _val_;
  }

  // dereferencing operator
  template < typename Val >
  INLINE Val& ListBucket< Val >::operator*() noexcept {
    return _val_;
  }

  // returns the bucket toward the next element
  template < typename Val >
  INLINE const ListBucket< Val >* ListBucket< Val >::next() const noexcept {
    return _next_;
  }

  // returns the bucket toward the preceding element
  template < typename Val >
  INLINE const ListBucket< Val >* ListBucket< Val >::previous() const noexcept {
    return _prev_;
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
  INLINE ListConstIterator< Val >::ListConstIterator(const List< Val >& theList) noexcept :
      _bucket_{theList._deb_list_} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);
  }

  // copy constructor
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(const ListConstIterator< Val >& src) noexcept :
      _bucket_{src._bucket_} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIterator);
  }

  // move constructor
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(ListConstIterator< Val >&& src) noexcept :
      _bucket_{std::move(src._bucket_)} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIterator);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  INLINE ListConstIterator< Val >::ListConstIterator(const List< Val >& theList, Size ind_elt) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= theList._nb_elements_) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list")
    }

    // check if it is faster to find the indexth element from the start or
    // from the end of the list
    if (ind_elt < (theList._nb_elements_ >> 1)) {
      // find the element we shall point to src the start of the list
      for (_bucket_ = theList._deb_list_; ind_elt; --ind_elt, _bucket_ = _bucket_->_next_) {}
    } else {
      // find the element we shall point to src the end of the list
      for (_bucket_ = theList._end_list_, ind_elt = theList._nb_elements_ - ind_elt - 1; ind_elt;
           --ind_elt, _bucket_                    = _bucket_->_prev_) {}
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
  INLINE ListConstIterator< Val >&
     ListConstIterator< Val >::operator=(const ListConstIterator< Val >& src) noexcept {
    // for debugging purposes
    GUM_OP_CPY(ListConstIterator);

    _bucket_ = src._bucket_;
    return *this;
  }

  // move operator
  template < typename Val >
  INLINE ListConstIterator< Val >&
     ListConstIterator< Val >::operator=(ListConstIterator< Val >&& src) noexcept {
    // for debugging purposes
    GUM_OP_MOV(ListConstIterator);
    _bucket_ = src._bucket_;
    return *this;
  }

  // returns the bucket the iterator is pointing to
  template < typename Val >
  INLINE ListBucket< Val >* ListConstIterator< Val >::_getBucket_() const noexcept {
    return _bucket_;
  }

  // Makes the iterator point toward nothing
  template < typename Val >
  INLINE void ListConstIterator< Val >::clear() noexcept {
    _bucket_ = nullptr;
  }

  // positions the iterator to the end of the list
  template < typename Val >
  INLINE void ListConstIterator< Val >::setToEnd() noexcept {
    _bucket_ = nullptr;
  }

  // returns a bool indicating whether the iterator points to the end of the
  // list
  template < typename Val >
  INLINE bool ListConstIterator< Val >::isEnd() const noexcept {
    return (_bucket_ == nullptr);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator++() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the next bucket in this list
    if (_bucket_ != nullptr) { _bucket_ = _bucket_->_next_; }

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator+=(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (_bucket_ != nullptr); --i, _bucket_ = _bucket_->_next_) {}
    } else {
      for (; i && (_bucket_ != nullptr); ++i, _bucket_ = _bucket_->_prev_) {}
    }
    return *this;
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator--() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the preceding bucket in this list
    if (_bucket_ != nullptr) { _bucket_ = _bucket_->_prev_; }

    return *this;
  }

  // makes the iterator point to i elements before in the list
  template < typename Val >
  INLINE ListConstIterator< Val >& ListConstIterator< Val >::operator-=(
     typename ListConstIterator< Val >::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (_bucket_ != nullptr); --i, _bucket_ = _bucket_->_prev_) {}
    } else {
      for (; i && (_bucket_ != nullptr); ++i, _bucket_ = _bucket_->_next_) {}
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
  INLINE bool
     ListConstIterator< Val >::operator!=(const ListConstIterator< Val >& src) const noexcept {
    return (_bucket_ != src._bucket_);
  }

  // checks whether two iterators point toward the same elements.
  template < typename Val >
  INLINE bool
     ListConstIterator< Val >::operator==(const ListConstIterator< Val >& src) const noexcept {
    return (_bucket_ == src._bucket_);
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListConstIterator< Val >::operator->() const {
    if (_bucket_ != nullptr) return &(_bucket_->_val_);
    else { GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object") }
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListConstIterator< Val >::operator*() const {
    if (_bucket_ != nullptr) return _bucket_->_val_;
    else { GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object") }
  }

  // for STL compliance, a distance operator
  template < typename Val >
  INLINE typename ListConstIterator< Val >::difference_type
     operator-(const ListConstIterator< Val >& iter1, const ListConstIterator< Val >& iter2) {
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
  INLINE ListIterator< Val >::ListIterator() noexcept : ListConstIterator< Val >() {
    GUM_CONSTRUCTOR(ListIterator);
  }

  // constructor for a begin
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator(const List< Val >& theList) noexcept :
      ListConstIterator< Val >(theList) {
    GUM_CONSTRUCTOR(ListIterator);
  }

  // copy constructor
  template < typename Val >
  INLINE ListIterator< Val >::ListIterator(const ListIterator< Val >& src) noexcept :
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
  INLINE ListIterator< Val >::ListIterator(const List< Val >& theList, Size ind_elt) :
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
  INLINE ListIterator< Val >& ListIterator< Val >::operator=(ListIterator< Val >&& src) noexcept {
    GUM_OP_MOV(ListIterator);
    ListConstIterator< Val >::operator=(std::move(src));
    return *this;
  }

  // Destructor
  template < typename Val >
  INLINE ListIterator< Val >::~ListIterator() noexcept {
    GUM_DESTRUCTOR(ListIterator);
  }

  // test equality
  template < typename Val >
  INLINE bool ListIterator< Val >::operator==(const ListIterator< Val >& src) const noexcept {
    return ListConstIterator< Val >::operator==(src);
  }

  // test inequality
  template < typename Val >
  INLINE bool ListIterator< Val >::operator!=(const ListIterator< Val >& src) const noexcept {
    return !operator==(src);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListIterator< Val >& ListIterator< Val >::operator++() noexcept {
    ListConstIterator< Val >::operator++();
    return *this;
  }

  // makes the iterator point to i elements further in the List
  template < typename Val >
  INLINE ListIterator< Val >&
     ListIterator< Val >::operator+=(typename ListIterator< Val >::difference_type i) noexcept {
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
  INLINE ListIterator< Val >&
     ListIterator< Val >::operator-=(typename ListIterator< Val >::difference_type i) noexcept {
    ListConstIterator< Val >::operator-=(i);
    return *this;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIterator< Val >
     ListIterator< Val >::operator+(typename ListIterator< Val >::difference_type i) noexcept {
    return ListIterator< Val >(*this) += i;
  }

  // returns a new iterator
  template < typename Val >
  INLINE ListIterator< Val >
     ListIterator< Val >::operator-(typename ListIterator< Val >::difference_type i) noexcept {
    return ListIterator< Val >(*this) -= i;
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE Val* ListIterator< Val >::operator->() {
    return const_cast< Val* >(ListConstIterator< Val >::operator->());
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListIterator< Val >::operator->() const {
    return ListConstIterator< Val >::operator->();
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE Val& ListIterator< Val >::operator*() {
    return const_cast< Val& >(ListConstIterator< Val >::operator*());
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListIterator< Val >::operator*() const {
    return ListConstIterator< Val >::operator*();
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
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe(const List< Val >& theList) :
      _list_{&theList}, _bucket_{theList._deb_list_} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    theList._safe_iterators_.push_back(this);
  }

  // copy constructor
  template < typename Val >
  INLINE
     ListConstIteratorSafe< Val >::ListConstIteratorSafe(const ListConstIteratorSafe< Val >& src) :
      _list_{src._list_},
      _bucket_{src._bucket_}, _next_current_bucket_{src._next_current_bucket_},
      _prev_current_bucket_{src._prev_current_bucket_}, _null_pointing_{src._null_pointing_} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    if (_list_ != nullptr) _list_->_safe_iterators_.push_back(this);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >

  ListConstIteratorSafe< Val >::ListConstIteratorSafe(const List< Val >& theList, Size ind_elt) :
      _list_{&theList} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= _list_->_nb_elements_) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list")
    }

    // check if it is faster to find the indexth element src the start or
    // src the end of the list
    if (ind_elt < (_list_->_nb_elements_ >> 1)) {
      // find the element we shall point to src the start of the list
      for (_bucket_ = _list_->_deb_list_; ind_elt; --ind_elt, _bucket_ = _bucket_->_next_) {}
    } else {
      // find the element we shall point to src the end of the list
      for (_bucket_ = _list_->_end_list_, ind_elt = _list_->_nb_elements_ - ind_elt - 1; ind_elt;
           --ind_elt, _bucket_                    = _bucket_->_prev_) {}
    }

    // add the iterator to the list of safe iterators
    theList._safe_iterators_.push_back(this);
  }

  // move constructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::ListConstIteratorSafe(ListConstIteratorSafe< Val >&& src) :
      _list_{src._list_}, _bucket_{src._bucket_}, _next_current_bucket_{src._next_current_bucket_},
      _prev_current_bucket_{src._prev_current_bucket_}, _null_pointing_{src._null_pointing_} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIteratorSafe);

    if (_list_ != nullptr) {
      // substitute src by this in the list of safe iterators
      std::vector< ListConstIteratorSafe< Val >* >& vect = _list_->_safe_iterators_;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); --ptr) {
        if (*ptr == &src) {
          *ptr = this;
          break;
        }
      }

      src._list_          = nullptr;
      src._bucket_        = nullptr;
      src._null_pointing_ = false;
    }
  }

  // remove the iterator for its list' safe iterators list
  template < typename Val >
  INLINE void ListConstIteratorSafe< Val >::_removeFromSafeList_() const {
    // find where the iterator is
    std::vector< ListConstIteratorSafe< Val >* >& vect = _list_->_safe_iterators_;

    for (auto i = vect.size() - 1; i >= 0; --i) {
      if (vect[i] == this) {
        vect.erase(vect.begin() + i);
        break;
      }
    }
  }

  // Copy operator
  template < typename Val >
  ListConstIteratorSafe< Val >&
     ListConstIteratorSafe< Val >::operator=(const ListConstIteratorSafe< Val >& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(ListConstIteratorSafe);

      // check if src and this belong to the same list. If this is not
      // the case, we shall remove this from its iterator's list and
      // put it into src's list one.
      if (_list_ && (src._list_ != _list_)) {
        _removeFromSafeList_();
        _list_ = nullptr;
      }

      // if necessary, put this into the same list of safe iterators as src
      if (src._list_ && (src._list_ != _list_)) {
        try {
          src._list_->_safe_iterators_.push_back(this);
        } catch (...) {
          _list_          = nullptr;
          _bucket_        = nullptr;
          _null_pointing_ = false;
          throw;
        }
      }

      _list_                = src._list_;
      _bucket_              = src._bucket_;
      _prev_current_bucket_ = src._prev_current_bucket_;
      _next_current_bucket_ = src._next_current_bucket_;
      _null_pointing_       = src._null_pointing_;
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
      if ((_list_ != nullptr) && (src._list_ != _list_)) {
        _removeFromSafeList_();
        _list_ = nullptr;
      }

      // now if src points to a list, put this at its location
      if ((src._list_ != nullptr)) {
        std::vector< ListConstIteratorSafe< Val >* >& vect      = src._list_->_safe_iterators_;
        Idx                                           index_src = Size(vect.size()) - 1;

        for (;; --index_src) {
          if (vect[index_src] == &src) { break; }
        }

        if (_list_ == nullptr) {
          vect[index_src] = this;
        } else {
          vect.erase(vect.begin() + index_src);
        }
      }

      _list_                = src._list_;
      _bucket_              = src._bucket_;
      _prev_current_bucket_ = src._prev_current_bucket_;
      _next_current_bucket_ = src._next_current_bucket_;
      _null_pointing_       = src._null_pointing_;

      src._list_          = nullptr;
      src._bucket_        = nullptr;
      src._null_pointing_ = false;
    }

    return *this;
  }

  // Destructor
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >::~ListConstIteratorSafe() {
    // for debugging purposes
    GUM_DESTRUCTOR(ListConstIteratorSafe);

    // remove the iterator src the table's iterator list
    if (_list_) _removeFromSafeList_();
  }

  // returns the bucket the iterator is pointing to
  template < typename Val >
  INLINE ListBucket< Val >* ListConstIteratorSafe< Val >::_getBucket_() const noexcept {
    return _bucket_;
  }

  // Makes the iterator point toward nothing
  template < typename Val >
  INLINE void ListConstIteratorSafe< Val >::clear() {
    // remove the iterator src the list's iterator list
    if (_list_) _removeFromSafeList_();

    // set its list as well as the element it points to to nullptr
    _list_          = nullptr;
    _bucket_        = nullptr;
    _null_pointing_ = false;
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
    return _null_pointing_
            ? (_next_current_bucket_ == nullptr) && (_prev_current_bucket_ == nullptr)
            : (_bucket_ == nullptr);
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator++() noexcept {
    // check if we are pointing to something that has been deleted
    if (_null_pointing_) {
      _null_pointing_ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a next element, just point on the latter
      if (_next_current_bucket_ != nullptr) {
        _bucket_ = _next_current_bucket_->_next_;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if prev_current_bucket is not null, then we are at rend and doing
      // a ++ shall now point to the beginning of the list
      if (_prev_current_bucket_ != nullptr) {
        _bucket_ = _prev_current_bucket_;
        return *this;
      }

      // here, we are at the end of the chained list, hence we shall remain at
      // end
      _bucket_ = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (_bucket_ != nullptr) { _bucket_ = _bucket_->_next_; }

      return *this;
    }
  }

  // makes the iterator point to i elements before in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::_opMinus_(Size i) noexcept {
    // check if we are pointing to something that has been deleted
    if (_null_pointing_) {
      _null_pointing_ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a preceding element, just point on the latter
      if (_prev_current_bucket_ != nullptr) {
        _bucket_ = _prev_current_bucket_->_prev_;
      } else {
        // here we were pointing on an extremity of the list (either end or
        // rend)
        // if next_current_bucket is not null, then we are at end and doing
        // a -- shall now point to the beginning of the list
        if (_next_current_bucket_ != nullptr) {
          _bucket_ = _next_current_bucket_;
        } else {
          // here, we are at the rend of the chained list, hence we shall remain
          // at rend
          _bucket_ = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (_bucket_ != nullptr) { _bucket_ = _bucket_->_prev_; }
    }

    for (--i; i && (_bucket_ != nullptr); --i, _bucket_ = _bucket_->_prev_) {}

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::_opPlus_(Size i) noexcept {
    // check if we are pointing to something that has been deleted
    if (_null_pointing_) {
      _null_pointing_ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a next element, just point on the latter
      if (_next_current_bucket_ != nullptr) {
        _bucket_ = _next_current_bucket_->_next_;
      } else {
        // here we were pointing on an extremity of the list (either end or
        // rend)
        // if prev_current_bucket is not null, then we are at rend and doing
        // a ++ shall now point to the beginning of the list
        if (_prev_current_bucket_ != nullptr) {
          _bucket_ = _prev_current_bucket_;
        } else {
          // here, we are at the end of the chained list, hence we shall
          // remain at end
          _bucket_ = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (_bucket_ != nullptr) { _bucket_ = _bucket_->_next_; }
    }

    for (--i; i && (_bucket_ != nullptr); --i, _bucket_ = _bucket_->_next_) {}

    return *this;
  }

  // makes the iterator point to the next element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator+=(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    if (!i) return *this;

    if (i < 0) return _opMinus_(-i);
    else return _opPlus_(i);
  }

  // makes the iterator point to the preceding element in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator--() noexcept {
    // check if we are pointing to something that has been deleted
    if (_null_pointing_) {
      _null_pointing_ = false;

      // if we are pointing to an element of the chained list that has been
      // deleted
      // but that has a preceding element, just point on the latter
      if (_prev_current_bucket_ != nullptr) {
        _bucket_ = _prev_current_bucket_->_prev_;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if next_current_bucket is not null, then we are at end and doing
      // a -- shall now point to the beginning of the list
      if (_next_current_bucket_ != nullptr) {
        _bucket_ = _next_current_bucket_;
        return *this;
      }

      // here, we are at the rend of the chained list, hence we shall remain
      // at rend
      _bucket_ = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (_bucket_ != nullptr) { _bucket_ = _bucket_->_prev_; }

      return *this;
    }
  }

  // makes the iterator point to i elements before in the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val >& ListConstIteratorSafe< Val >::operator-=(
     typename ListConstIteratorSafe< Val >::difference_type i) noexcept {
    if (!i) return *this;

    if (i < 0) return _opPlus_(-i);
    else return _opMinus_(i);
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
  INLINE bool
     ListConstIteratorSafe< Val >::operator!=(const ListConstIteratorSafe< Val >& src) const {
    return _null_pointing_ ? (_next_current_bucket_ != src._next_current_bucket_)
                                || (_prev_current_bucket_ != src._prev_current_bucket_)
                           : (_bucket_ != src._bucket_);
  }

  // checks whether two iterators point toward the same elements.
  template < typename Val >
  INLINE bool
     ListConstIteratorSafe< Val >::operator==(const ListConstIteratorSafe< Val >& src) const {
    return _null_pointing_ ? (_next_current_bucket_ == src._next_current_bucket_)
                                && (_prev_current_bucket_ == src._prev_current_bucket_)
                           : (_bucket_ == src._bucket_);
  }

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListConstIteratorSafe< Val >::operator->() const {
    if (_bucket_ != nullptr) return &(_bucket_->_val_);
    else { GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object") }
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListConstIteratorSafe< Val >::operator*() const {
    if (_bucket_ != nullptr) return _bucket_->_val_;
    else { GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object") }
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
  INLINE ListIteratorSafe< Val >::ListIteratorSafe() noexcept : ListConstIteratorSafe< Val >() {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  // constructor for a begin
  template < typename Val >

  INLINE ListIteratorSafe< Val >::ListIteratorSafe(const List< Val >& theList) :
      ListConstIteratorSafe< Val >(theList) {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  // copy constructor
  template < typename Val >
  INLINE ListIteratorSafe< Val >::ListIteratorSafe(const ListIteratorSafe< Val >& src) :
      ListConstIteratorSafe< Val >(src) {
    GUM_CONS_CPY(ListIteratorSafe);
  }

  // Constructor for an iterator pointing to the \e ind_eltth element of a
  // List.
  template < typename Val >
  INLINE ListIteratorSafe< Val >::ListIteratorSafe(const List< Val >& theList, Size ind_elt) :
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

  // test equality
  template < typename Val >
  INLINE bool ListIteratorSafe< Val >::operator==(const ListIteratorSafe< Val >& src) const {
    return ListConstIteratorSafe< Val >::operator==(src);
  }

  // test inequality
  template < typename Val >
  INLINE bool ListIteratorSafe< Val >::operator!=(const ListIteratorSafe< Val >& src) const {
    return !operator==(src);
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

  // dereferences the value pointed to by the iterator
  template < typename Val >
  INLINE const Val* ListIteratorSafe< Val >::operator->() const {
    return ListConstIteratorSafe< Val >::operator->();
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE Val& ListIteratorSafe< Val >::operator*() {
    return const_cast< Val& >(ListConstIteratorSafe< Val >::operator*());
  }

  // gives access to the content of the iterator
  template < typename Val >
  INLINE const Val& ListIteratorSafe< Val >::operator*() const {
    return ListConstIteratorSafe< Val >::operator*();
  }


  // ===========================================================================
  // ===========================================================================
  // ===                          LIST IMPLEMENTATION                        ===
  // ===========================================================================
  // ===========================================================================

  // a function used to perform copies of elements of Lists
  template < typename Val >
  void List< Val >::_copy_elements_(const List< Val >& src) {
    ListBucket< Val >* ptr;
    ListBucket< Val >* old_ptr = nullptr;
    ListBucket< Val >* new_elt = nullptr;

    // copy src's list
    try {
      for (ptr = src._deb_list_; ptr != nullptr; ptr = ptr->_next_) {
        // create a copy bucket
        new_elt = new ListBucket< Val >(*ptr);

        // rechain properly the new list (the next field is already initialized
        // with nullptr)
        new_elt->_prev_ = old_ptr;

        if (old_ptr) old_ptr->_next_ = new_elt;
        else _deb_list_ = new_elt;

        old_ptr = new_elt;
      }
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (; _deb_list_ != nullptr; _deb_list_ = const_cast< ListBucket< Val >* >(ptr)) {
        ptr = _deb_list_->_next_;
        delete _deb_list_;
      }

      _deb_list_ = nullptr;
      throw;
    }

    // update properly the end of the chained list and the number of elements
    _end_list_    = old_ptr;
    _nb_elements_ = src._nb_elements_;
  }

  // deletes all the elements of a chained list.
  template < typename Val >
  void List< Val >::clear() {
    // first we update all the safe iterators of the list : they should now
    // point to end/rend
    for (const auto ptr_iter: _safe_iterators_) {
      ptr_iter->clear();
    }

    // clear all the values
    for (ListBucket< Val >*ptr = _deb_list_, *next_ptr = nullptr; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->_next_;
      delete ptr;
    }

    _nb_elements_ = 0;
    _deb_list_    = nullptr;
    _end_list_    = nullptr;
  }

  // A basic constructor that creates an empty list
  template < typename Val >
  INLINE List< Val >::List() {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // reserve space for only the default number of iterators
    _safe_iterators_.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // Copy constructor
  template < typename Val >
  INLINE List< Val >::List(const List< Val >& src) {
    // for debugging purposes
    GUM_CONS_CPY(List);

    // copy the elements
    _copy_elements_(src);

    // reserve space for only the default number of iterators
    _safe_iterators_.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // move constructor
  template < typename Val >
  INLINE List< Val >::List(List< Val >&& src) noexcept :
      _deb_list_{std::move(src._deb_list_)}, _end_list_{std::move(src._end_list_)},
      _nb_elements_{std::move(src._nb_elements_)}, _safe_iterators_{
                                                      std::move(src._safe_iterators_)} {
    // for debugging purposes
    GUM_CONS_MOV(List);

    src._deb_list_    = nullptr;
    src._end_list_    = nullptr;
    src._nb_elements_ = 0;
    src._safe_iterators_.clear();
  }

  // initializer_list constructor
  template < typename Val >
  INLINE List< Val >::List(std::initializer_list< Val > list) {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // adding all the elements
    for (const auto& val: list) {
      pushBack(val);
    }

    // reserve space for only the default number of iterators
    _safe_iterators_.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  // Destructor
  template < typename Val >
  INLINE List< Val >::~List() {
    // for debugging (although this program is bug-free)
    GUM_DESTRUCTOR(List);

    // we detach all the safe iterators attached to the current List and we
    // remove all the elements from the list
    clear();
  }

  // Copy operator. The List iterator's list is not shared with that of \e src.
  template < typename Val >
  INLINE List< Val >& List< Val >::operator=(const List< Val >& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      _copy_elements_(src);
    }

    return *this;
  }

  // move operator
  template < typename Val >
  INLINE List< Val >& List< Val >::operator=(List< Val >&& src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_MOV(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the move
      _deb_list_       = std::move(src._deb_list_);
      _end_list_       = std::move(src._end_list_);
      _nb_elements_    = std::move(src._nb_elements_);
      _safe_iterators_ = std::move(src._safe_iterators_);

      src._deb_list_    = nullptr;
      src._end_list_    = nullptr;
      src._nb_elements_ = 0;
      src._safe_iterators_.clear();
    }

    return *this;
  }

  // the iterator pointing to the end of the List
  template < typename Val >
  INLINE const ListConstIteratorSafe< Val >& List< Val >::cendSafe() const noexcept {
    return *(reinterpret_cast< const ListConstIteratorSafe< Val >* >(_list_end_safe_));
  }

  // the iterator pointing to the end of the List
  template < typename Val >
  INLINE const ListIteratorSafe< Val >& List< Val >::endSafe() noexcept {
    return *(reinterpret_cast< const ListIteratorSafe< Val >* >(_list_end_safe_));
  }

  // the iterator pointing to the end of the List
  template < typename Val >
  INLINE const ListConstIterator< Val >& List< Val >::cend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the end of the List
  template < typename Val >
  INLINE const ListIterator< Val >& List< Val >::end() noexcept {
    return *(reinterpret_cast< const ListIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the end of the List
  template < typename Val >
  INLINE const ListConstIterator< Val >& List< Val >::end() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val >
  INLINE const ListConstIteratorSafe< Val >& List< Val >::crendSafe() const noexcept {
    return *(reinterpret_cast< const ListConstIteratorSafe< Val >* >(_list_end_safe_));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val >
  INLINE const ListIteratorSafe< Val >& List< Val >::rendSafe() noexcept {
    return *(reinterpret_cast< const ListIteratorSafe< Val >* >(_list_end_safe_));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val >
  INLINE const ListConstIterator< Val >& List< Val >::crend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val >
  INLINE const ListIterator< Val >& List< Val >::rend() noexcept {
    return *(reinterpret_cast< const ListIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the rend (just before the beginning) of the List
  template < typename Val >
  INLINE const ListConstIterator< Val >& List< Val >::rend() const noexcept {
    return *(reinterpret_cast< const ListConstIterator< Val >* >(_list_end_));
  }

  // the iterator pointing to the beginning of the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val > List< Val >::cbeginSafe() const {
    return ListConstIteratorSafe< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val >
  INLINE ListIteratorSafe< Val > List< Val >::beginSafe() {
    return ListIteratorSafe< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val >
  INLINE ListConstIterator< Val > List< Val >::cbegin() const {
    return ListConstIterator< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val >
  INLINE ListIterator< Val > List< Val >::begin() {
    return ListIterator< Val >{*this};
  }

  // the iterator pointing to the beginning of the List
  template < typename Val >
  INLINE ListConstIterator< Val > List< Val >::begin() const {
    return ListConstIterator< Val >{*this};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val >
  INLINE ListConstIteratorSafe< Val > List< Val >::crbeginSafe() const {
    if (_nb_elements_) return ListConstIteratorSafe< Val >{*this, _nb_elements_ - 1};
    else return ListConstIteratorSafe< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val >
  INLINE ListIteratorSafe< Val > List< Val >::rbeginSafe() {
    if (_nb_elements_) return ListIteratorSafe< Val >{*this, _nb_elements_ - 1};
    else return ListIteratorSafe< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val >
  INLINE ListConstIterator< Val > List< Val >::crbegin() const {
    if (_nb_elements_) return ListConstIterator< Val >{*this, _nb_elements_ - 1};
    else return ListConstIterator< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val >
  INLINE ListIterator< Val > List< Val >::rbegin() {
    if (_nb_elements_) return ListIterator< Val >{*this, _nb_elements_ - 1};
    else return ListIterator< Val >{};
  }

  // the iterator pointing to the rbegin (the last element) of the List
  template < typename Val >
  INLINE ListConstIterator< Val > List< Val >::rbegin() const {
    if (_nb_elements_) return ListConstIterator< Val >{*this, _nb_elements_ - 1};
    else return ListConstIterator< Val >{};
  }

  // create a new bucket with a given value
  template < typename Val >
  INLINE ListBucket< Val >* List< Val >::_createBucket_(const Val& val) const {
    return new ListBucket< Val >(val);
  }

  // create a new bucket with a given value
  template < typename Val >
  INLINE ListBucket< Val >* List< Val >::_createBucket_(Val&& val) const {
    return new ListBucket< Val >(std::move(val));
  }

  // create an emplace bucket
  template < typename Val >
  template < typename... Args >
  INLINE ListBucket< Val >* List< Val >::_createEmplaceBucket_(Args&&... args) const {
    return new ListBucket< Val >(ListBucket< Val >::Emplace::EMPLACE,
                                 std::forward< Args >(args)...);
  }

  // insert a bucket at the front of the list
  template < typename Val >
  INLINE Val& List< Val >::_pushFront_(ListBucket< Val >* new_elt) {
    new_elt->_next_ = _deb_list_;

    if (_deb_list_ != nullptr) _deb_list_->_prev_ = new_elt;
    else _end_list_ = new_elt;

    _deb_list_ = new_elt;

    // update the number of elements
    ++_nb_elements_;

    // return the inserted element
    return new_elt->_val_;
  }

  // insert a bucket at the end of the list
  template < typename Val >
  INLINE Val& List< Val >::_pushBack_(ListBucket< Val >* new_elt) {
    // place the bucket at the end of the list
    new_elt->_prev_ = _end_list_;

    if (_end_list_ != nullptr) _end_list_->_next_ = new_elt;
    else _deb_list_ = new_elt;

    _end_list_ = new_elt;

    // update the number of elements
    ++_nb_elements_;

    // returns the current value
    return new_elt->_val_;
  }

  // Insertion of a new element (a copy) at the beginning of the chained list.
  template < typename Val >
  INLINE Val& List< Val >::pushFront(const Val& val) {
    return _pushFront_(_createBucket_(val));
  }

  // Insertion of a new element (a copy) at the beginning of the chained list.
  template < typename Val >
  INLINE Val& List< Val >::pushFront(Val&& val) {
    return _pushFront_(_createBucket_(std::move(val)));
  }

  // an alias for pushFront used for STL compliance
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::push_front(Args&&... args) {
    return pushFront(std::forward< Args >(args)...);
  }

  // emplace elements at the beginning of the chained list
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::emplaceFront(Args&&... args) {
    return _pushFront_(_createEmplaceBucket_(std::forward< Args >(args)...));
  }

  // Insertion of a new element (a copy) at the end of the chained list.
  template < typename Val >
  INLINE Val& List< Val >::pushBack(const Val& val) {
    return _pushBack_(_createBucket_(val));
  }

  // pushBack for rvalues
  template < typename Val >
  INLINE Val& List< Val >::pushBack(Val&& val) {
    return _pushBack_(_createBucket_(std::move(val)));
  }

  // an alias for pushBack used for STL compliance
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::push_back(Args&&... args) {
    return pushBack(std::forward< Args >(args)...);
  }

  // emplace elements at the end of the chained list
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::emplaceBack(Args&&... args) {
    return _pushBack_(_createEmplaceBucket_(std::forward< Args >(args)...));
  }

  // Insertion of a new element at the end of the chained list (alias of
  // pushBack)
  template < typename Val >
  INLINE Val& List< Val >::insert(const Val& val) {
    return pushBack(val);
  }

  // insert for rvalues
  template < typename Val >
  INLINE Val& List< Val >::insert(Val&& val) {
    return pushBack(std::move(val));
  }

  // returns the bucket corresponding to the ith position
  template < typename Val >
  INLINE ListBucket< Val >* List< Val >::_getIthBucket_(Size i) const noexcept {
    ListBucket< Val >* ptr;

    if (i < _nb_elements_ / 2) {
      for (ptr = _deb_list_; i; --i, ptr = ptr->_next_) {}
    } else {
      for (ptr = _end_list_, i = _nb_elements_ - i - 1; i; --i, ptr = ptr->_prev_) {}
    }

    return ptr;
  }

  // insert a new bucket before another one
  template < typename Val >
  INLINE Val& List< Val >::_insertBefore_(ListBucket< Val >* new_elt,
                                          ListBucket< Val >* current_elt) {
    new_elt->_next_     = current_elt;
    new_elt->_prev_     = current_elt->_prev_;
    current_elt->_prev_ = new_elt;

    if (new_elt->_prev_ == nullptr) _deb_list_ = new_elt;
    else new_elt->_prev_->_next_ = new_elt;

    // update the number of elements
    ++_nb_elements_;

    // returns the current value
    return new_elt->_val_;
  }

  // insert a new bucket after another one
  template < typename Val >
  INLINE Val& List< Val >::_insertAfter_(ListBucket< Val >* new_elt,
                                         ListBucket< Val >* current_elt) {
    new_elt->_prev_     = current_elt;
    new_elt->_next_     = current_elt->_next_;
    current_elt->_next_ = new_elt;

    if (new_elt->_next_ == nullptr) _end_list_ = new_elt;
    else new_elt->_next_->_prev_ = new_elt;

    // update the number of elements
    ++_nb_elements_;

    // returns the current value
    return new_elt->_val_;
  }

  // inserts a new element at the ith pos of the chained list
  template < typename Val >
  INLINE Val& List< Val >::insert(Size pos, const Val& val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (_nb_elements_ <= pos) { return pushBack(val); }

    return _insertBefore_(_createBucket_(val), _getIthBucket_(pos));
  }

  // insert an rvalue at the ith pos of the chained list
  template < typename Val >
  INLINE Val& List< Val >::insert(Size pos, Val&& val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (_nb_elements_ <= pos) { return pushBack(std::move(val)); }

    return _insertBefore_(_createBucket_(std::move(val)), _getIthBucket_(pos));
  }

  // inserts a new bucket before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val& List< Val >::_insert_(const const_iterator_safe& iter,
                                    ListBucket< Val >*         new_elt,
                                    location                   place) {
    // find the location around which the new element should be inserted
    ListBucket< Val >* ptr;

    if (iter._null_pointing_) {
      if (place == location::BEFORE) {
        ptr = iter._next_current_bucket_;
      } else {
        ptr = iter._prev_current_bucket_;
      }
    } else {
      ptr = iter._getBucket_();
    }

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return _pushBack_(new_elt);
    } else {
      switch (place) {
        case location::BEFORE: return _insertBefore_(new_elt, ptr);

        case location::AFTER: return _insertAfter_(new_elt, ptr);

        default: GUM_ERROR(FatalError, "List insertion for this location unimplemented")
      }
    }
  }

  // inserts a new bucket before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val&
     List< Val >::_insert_(const const_iterator& iter, ListBucket< Val >* new_elt, location place) {
    // find the location around which the new element should be inserted
    ListBucket< Val >* ptr = iter._getBucket_();

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return _pushBack_(new_elt);
    } else {
      switch (place) {
        case location::BEFORE: return _insertBefore_(new_elt, ptr);

        case location::AFTER: return _insertAfter_(new_elt, ptr);

        default: GUM_ERROR(FatalError, "List insertion for this location unimplemented")
      }
    }
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val& List< Val >::insert(const const_iterator_safe& iter, const Val& val, location place) {
    // if the iterator does not point to the list, raise an exception
    if (iter._list_ != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list")
    }

    return _insert_(iter, _createBucket_(val), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val& List< Val >::insert(const const_iterator_safe& iter, Val&& val, location place) {
    // if the iterator does not point to the list, raise an exception
    if (iter._list_ != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list")
    }

    return _insert_(iter, _createBucket_(std::move(val)), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val& List< Val >::insert(const const_iterator& iter, const Val& val, location place) {
    return _insert_(iter, _createBucket_(val), place);
  }

  // inserts a new element before or after the location pointed to by an
  // iterator
  template < typename Val >
  INLINE Val& List< Val >::insert(const const_iterator& iter, Val&& val, location place) {
    return _insert_(iter, _createBucket_(std::move(val)), place);
  }

  // emplace a new element before a given iterator
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::emplace(const const_iterator& iter, Args&&... args) {
    return _insert_(iter, _createEmplaceBucket_(std::forward< Args >(args)...), location::BEFORE);
  }

  // emplace a new element before a given iterator
  template < typename Val >
  template < typename... Args >
  INLINE Val& List< Val >::emplace(const const_iterator_safe& iter, Args&&... args) {
    return _insert_(iter, _createEmplaceBucket_(std::forward< Args >(args)...), location::BEFORE);
  }

  // returns a reference to first element of a list
  template < typename Val >
  INLINE Val& List< Val >::front() const {
    if (_nb_elements_ == Size(0)) { GUM_ERROR(NotFound, "not enough elements in the chained list") }

    return _deb_list_->_val_;
  }

  // returns a reference to last element of a list
  template < typename Val >
  INLINE Val& List< Val >::back() const {
    if (_nb_elements_ == Size(0)) { GUM_ERROR(NotFound, "not enough elements in the chained list") }

    return _end_list_->_val_;
  }

  // returns the number of elements in the list.
  template < typename Val >
  INLINE Size List< Val >::size() const noexcept {
    return _nb_elements_;
  }

  // checks whether there exists a given element in the list.
  template < typename Val >
  INLINE bool List< Val >::exists(const Val& val) const {
    for (ListBucket< Val >* ptr = _deb_list_; ptr != nullptr; ptr = ptr->_next_)
      if (ptr->_val_ == val) return true;

    return false;
  }

  // suppresses a given bucket from a chained list.
  template < typename Val >
  INLINE void List< Val >::_erase_(ListBucket< Val >* bucket) {
    // perform deletion only if there is a bucket to remove
    if (bucket != nullptr) {
      // update the iterators pointing on this element
      for (const auto ptr_iter: _safe_iterators_) {
        if (ptr_iter->_bucket_ == bucket) {
          ptr_iter->_next_current_bucket_ = bucket->_prev_;
          ptr_iter->_prev_current_bucket_ = bucket->_next_;
          ptr_iter->_bucket_              = nullptr;
          ptr_iter->_null_pointing_       = true;
        } else {
          if (ptr_iter->_null_pointing_) {
            if (ptr_iter->_next_current_bucket_ == bucket)
              ptr_iter->_next_current_bucket_ = bucket->_prev_;

            if (ptr_iter->_prev_current_bucket_ == bucket)
              ptr_iter->_prev_current_bucket_ = bucket->_next_;
          }
        }
      }

      // set properly the begin and end of the chained list (the other chainings
      // will be performed by operator delete)
      if (bucket->_prev_ == nullptr) _deb_list_ = bucket->_next_;
      else bucket->_prev_->_next_ = bucket->_next_;

      if (bucket->_next_ == nullptr) _end_list_ = bucket->_prev_;
      else bucket->_next_->_prev_ = bucket->_prev_;

      // remove the current element src the list
      delete bucket;

      --_nb_elements_;
    }
  }

  // erases the ith element of the List (the first one is in position 0)
  template < typename Val >
  INLINE void List< Val >::erase(Size i) {
    if (i >= _nb_elements_) return;

    // erase the ith bucket
    _erase_(_getIthBucket_(i));
  }

  // erases the element of the List pointed to by the iterator
  template < typename Val >
  INLINE void List< Val >::erase(const iterator_safe& iter) {
    _erase_(iter._getBucket_());
  }

  // erases the element of the List pointed to by the iterator
  template < typename Val >
  INLINE void List< Val >::erase(const const_iterator_safe& iter) {
    _erase_(iter._getBucket_());
  }

  // returns the bucket corresponding to a given value.
  template < typename Val >
  INLINE ListBucket< Val >* List< Val >::_getBucket_(const Val& val) const noexcept {
    for (ListBucket< Val >* ptr = _deb_list_; ptr != nullptr; ptr = ptr->_next_)
      if (ptr->_val_ == val) return ptr;

    return nullptr;
  }

  // erases the first element encountered with a given value
  template < typename Val >
  INLINE void List< Val >::eraseByVal(const Val& val) {
    _erase_(_getBucket_(val));
  }

  // erases all the elements encountered with a given value
  template < typename Val >
  INLINE void List< Val >::eraseAllVal(const Val& val) {
    for (ListBucket< Val >*iter = _deb_list_, *next_bucket = nullptr; iter != nullptr;
         iter = next_bucket) {
      next_bucket = iter->_next_;

      if (val == iter->_val_) _erase_(iter);
    }
  }

  // removes the last element of a List
  template < typename Val >
  INLINE void List< Val >::popBack() {
    _erase_(_end_list_);
  }

  // removes the first element of a List
  template < typename Val >
  INLINE void List< Val >::popFront() {
    _erase_(_deb_list_);
  }

  // returns a boolean indicating whether the chained list is empty
  template < typename Val >
  INLINE bool List< Val >::empty() const noexcept {
    return (_nb_elements_ == Size(0));
  }

  // displays the content of a chained list
  template < typename Val >
  std::string List< Val >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (ListBucket< Val >* ptr = _deb_list_; ptr != nullptr; ptr = ptr->_next_, deja = true) {
      if (deja) stream << " --> ";

      stream << ptr->_val_;
    }

    stream << "]";

    return stream.str();
  }

  // creates a list of mountains src a list of val
  template < typename Val >
  template < typename Mount >
  List< Mount > List< Val >::map(Mount (*f)(Val)) const {
    // create a new empty list
    List< Mount > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains src a list of val
  template < typename Val >
  template < typename Mount >
  List< Mount > List< Val >::map(Mount (*f)(Val&)) const {
    // create a new empty list
    List< Mount > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains src a list of val
  template < typename Val >
  template < typename Mount >
  List< Mount > List< Val >::map(Mount (*f)(const Val&)) const {
    // create a new empty list
    List< Mount > list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  // creates a list of mountains with a given value src a list of val
  template < typename Val >
  template < typename Mount >
  List< Mount > List< Val >::map(const Mount& mount) const {
    // create a new empty list
    List< Mount > list;

    // fill the new list
    for (Size i = Size(0); i < _nb_elements_; ++i)
      list.pushBack(mount);

    return list;
  }

  // creates and insert a new element at the end of the list (alias of
  // pushBack).
  template < typename Val >
  INLINE Val& List< Val >::operator+=(const Val& val) {
    return pushBack(val);
  }

  // creates and insert a new element at the end of the list (alias of
  // pushBack).
  template < typename Val >
  INLINE Val& List< Val >::operator+=(Val&& val) {
    return pushBack(std::move(val));
  }

  // checks whether two lists are identical (same elements in the same order)
  template < typename Val >
  INLINE bool List< Val >::operator==(const List< Val >& src) const {
    // check if the two lists have at least the same number of elements
    if (_nb_elements_ != src._nb_elements_) return false;

    // parse the two lists
    for (ListBucket< Val >*iter1 = _deb_list_, *iter2 = src._deb_list_; iter1 != nullptr;
         iter1 = iter1->_next_, iter2 = iter2->_next_)
      if (*iter1 != *iter2) return false;

    return true;
  }

  // checks whether two lists are different (different elements or orders)
  template < typename Val >
  INLINE bool List< Val >::operator!=(const List< Val >& src) const {
    return !operator==(src);
  }

  // returns the ith element in the current chained list.
  template < typename Val >
  INLINE Val& List< Val >::operator[](const Size i) {
    // check if we can return the element we ask for
    if (i >= _nb_elements_) { GUM_ERROR(NotFound, "not enough elements in the chained list") }

    return **_getIthBucket_(i);
  }

  // returns the ith element in the current chained list.
  template < typename Val >
  INLINE const Val& List< Val >::operator[](const Size i) const {
    // check if we can return the element we ask for
    if (i >= _nb_elements_) { GUM_ERROR(NotFound, "not enough elements in the chained list") }

    return **_getIthBucket_(i);
  }

  // replace the current list with another one
  template < typename Val >
  INLINE void List< Val >::swap(List& other_list) {
    std::swap(_deb_list_, other_list._deb_list_);
    std::swap(_end_list_, other_list._end_list_);
    std::swap(_nb_elements_, other_list._nb_elements_);
    std::swap(_safe_iterators_, other_list._safe_iterators_);
  }

  // A \c << operator for List
  template < typename Val >
  std::ostream& operator<<(std::ostream& stream, const List< Val >& list) {
    stream << list.toString();
    return stream;
  }

} /* namespace gum */
