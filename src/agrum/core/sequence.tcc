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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>

// to ease IDE parser
#include <agrum/core/sequence.h>

namespace gum {

  /// returns the size of the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE Size SequenceImplementation<Key, Alloc, Gen>::size() const noexcept {
    return __h.size();
  }

  /// return true if empty
  template <typename Key, typename Alloc, bool Gen>
  INLINE bool SequenceImplementation<Key, Alloc, Gen>::empty() const noexcept {
    return __h.empty();
  }

  /// returns the size of the sequence
  template <typename Key, typename Alloc>
  INLINE Size SequenceImplementation<Key, Alloc, true>::size() const noexcept {
    return __h.size();
  }

  /// return true if empty
  template <typename Key, typename Alloc>
  INLINE bool SequenceImplementation<Key, Alloc, true>::empty() const noexcept {
    return __h.empty();
  }

  /* =========================================================================== */
  // class SequenceIteratorSafe
  /* =========================================================================== */

  /// default constructor
  template <typename Key>
  template <typename Alloc, bool Gen>
  INLINE SequenceIteratorSafe<Key>::SequenceIteratorSafe(
      const SequenceImplementation<Key, Alloc, Gen> &seq, Idx pos) noexcept
      : __seq{reinterpret_cast<const SequenceImplementation<
            Key, std::allocator<Key>, std::is_scalar<Key>::value> *>(&seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > __seq->size())
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }

  /// default constructor
  template <typename Key>
  template <typename Alloc>
  INLINE
  SequenceIteratorSafe<Key>::SequenceIteratorSafe(const Sequence<Key, Alloc> &seq,
                                                  Idx pos) noexcept
      : __seq{reinterpret_cast<const SequenceImplementation<
            Key, std::allocator<Key>, std::is_scalar<Key>::value> *>(&seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > __seq->size())
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }

  /// copy constructor
  template <typename Key>
  INLINE SequenceIteratorSafe<Key>::SequenceIteratorSafe(
      const SequenceIteratorSafe<Key> &source) noexcept
      : __iterator{source.__iterator},
        __seq{source.__seq} {
    GUM_CONS_CPY(SequenceIteratorSafe);
  }

  /// move constructor
  template <typename Key>
  INLINE SequenceIteratorSafe<Key>::SequenceIteratorSafe(
      SequenceIteratorSafe<Key> &&source) noexcept : __iterator{source.__iterator},
                                                     __seq{source.__seq} {
    GUM_CONS_MOV(SequenceIteratorSafe);
  }

  /// destructor
  template <typename Key>
  INLINE SequenceIteratorSafe<Key>::~SequenceIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SequenceIteratorSafe);
  }

  /// copy operator
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator=(const SequenceIteratorSafe<Key> &source) noexcept {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }

  /// move operator
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator=(SequenceIteratorSafe<Key> &&source) noexcept {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }

  /// point the iterator to the next value in the sequence
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator++() noexcept {
    if (__iterator < __seq->size())
      ++__iterator;
    else
      __iterator = __seq->size();

    return *this;
  }

  /// point the iterator to the preceding value in the sequence
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator--() noexcept {
    if (__iterator != std::numeric_limits<Idx>::max())
      --__iterator;

    return *this;
  }

  /// makes the iterator point to i elements further in the sequence
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator+=(unsigned int nb) noexcept {
    if (__iterator == std::numeric_limits<Idx>::max())
      return *this;
    __iterator += nb;
    if (__iterator > __seq->size())
      __iterator = __seq->size();

    return *this;
  }

  /// makes the iterator point to i elements further in the sequence
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> &SequenceIteratorSafe<Key>::
  operator-=(unsigned int nb) noexcept {
    if (__iterator == std::numeric_limits<Idx>::max())
      return *this;
    __iterator -= nb;
    if (__iterator > __seq->size())
      __iterator = std::numeric_limits<Idx>::max();

    return *this;
  }

  /// returns a new iterator
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> SequenceIteratorSafe<Key>::
  operator+(unsigned int nb) noexcept {
    return SequenceIteratorSafe<Key>{*this} += nb;
  }

  /// returns a new iterator
  template <typename Key>
  INLINE SequenceIteratorSafe<Key> SequenceIteratorSafe<Key>::
  operator-(unsigned int nb) noexcept {
    return SequenceIteratorSafe<Key>{*this} -= nb;
  }

  /// checks whether two iterators are pointing to the same element
  template <typename Key>
  INLINE bool SequenceIteratorSafe<Key>::
  operator==(const SequenceIteratorSafe<Key> &source) const noexcept {
    if (__seq->empty())
      return true; // all iterators are the same if seq is empty

    if ((__iterator != source.__iterator) || (__seq != source.__seq))
      return false;

    return true;
  }

  /// checks whether two iterators are pointing to different elements
  template <typename Key>
  INLINE bool SequenceIteratorSafe<Key>::
  operator!=(const SequenceIteratorSafe<Key> &source) const noexcept {
    return !operator==(source);
  }

  /// returns the position of the iterator in the sequence
  template <typename Key> INLINE Idx SequenceIteratorSafe<Key>::pos() const {
    if (__iterator >= __seq->size()) {
      GUM_ERROR(UndefinedIteratorValue, "iterator is end() or rend()");
    }

    return __iterator;
  }

  /// the iterator points to the posth element (0 = beginning of the sequence).
  template <typename Key>
  INLINE void SequenceIteratorSafe<Key>::__setPos(Idx pos) noexcept {
    if (pos > __seq->size())
      __iterator = __seq->size();
    else
      __iterator = pos;
  }

  /// the iterator points to the posth element (0 = beginning of the sequence).
  template <typename Key>
  INLINE void SequenceIteratorSafe<Key>::__setAtRend() noexcept {
    __iterator = std::numeric_limits<Idx>::max();
  }

  /// the iterator points to the posth element (0 = beginning of the sequence).
  template <typename Key>
  INLINE void SequenceIteratorSafe<Key>::__setAtEnd() noexcept {
    __iterator = __seq->size();
  }

  /// returns the value pointed to by the iterator
  template <typename Key>
  INLINE const Key &SequenceIteratorSafe<Key>::operator*() const {
    return Getter::op_star(__seq->__v[pos()]);
  }

  /// dereferences the value pointed to by the iterator
  template <typename Key>
  INLINE const Key *SequenceIteratorSafe<Key>::operator->() const {
    return Getter::op_arrow(__seq->__v[pos()]);
  }

  /* =========================================================================== */
  /* ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              === */
  /* =========================================================================== */

  /// updates const iterators
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::__update_end() noexcept {
    __end_safe.__setAtEnd();
  }

  /// clear the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::clear() {
    __h.clear();
    __v.clear();
    __update_end();
  }

  /// clears the current sequence and fill it with copies the element of aSeq
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::__copy(
      const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq) {
    clear();

    for (unsigned int i = 0; i < aSeq.size(); ++i) {
      Key &new_key = const_cast<Key &>(__h.insert(*(aSeq.__v[i]), i).first);
      __v.push_back(&new_key);
    }

    __update_end();
  }

  /// Default constructor
  template <typename Key, typename Alloc, bool Gen>
  INLINE
  SequenceImplementation<Key, Alloc, Gen>::SequenceImplementation(Size size_param)
      : __h(size_param), __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONSTRUCTOR(SequenceImplementation);
    __rend_safe.__setAtRend();
    __update_end();
  }

  /// initializer list constructor
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen>::SequenceImplementation(
      std::initializer_list<Key> list)
      : __end_safe{*this}, __rend_safe{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    __rend_safe.__setAtRend();
    for (const auto &elt : list) {
      insert(elt); // performs the __update_end ()
    }
  }

  /// copy constructor
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen>::SequenceImplementation(
      const SequenceImplementation<Key, Alloc, Gen> &aSeq)
      : __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    __rend_safe.__setAtRend();
    __copy(aSeq); // performs the __update_end ()
  }

  /// generalized copy constructor
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE SequenceImplementation<Key, Alloc, Gen>::SequenceImplementation(
      const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq)
      : __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    __rend_safe.__setAtRend();
    __copy(aSeq); // performs the __update_end ()
  }

  /// move constructor
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen>::SequenceImplementation(
      SequenceImplementation<Key, Alloc, Gen> &&aSeq)
      : __h(std::move(aSeq.__h)), __v(std::move(aSeq.__v)), __end_safe{*this},
        __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_MOV(SequenceImplementation);
    __rend_safe.__setAtRend();
    __update_end();
  }

  /// destructor
  template <typename Key, typename Alloc, bool Gen>
  INLINE
      SequenceImplementation<Key, Alloc, Gen>::~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  /// copy operator
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator=(const SequenceImplementation<Key, Alloc, Gen> &aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      __copy(aSeq); // performs the __update_end ()
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator=(const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq) {
    __copy(aSeq); // performs the __update_end ()
    return *this;
  }

  /// move operator
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator=(SequenceImplementation<Key, Alloc, Gen> &&aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      __h = std::move(aSeq.__h);
      __v = std::move(aSeq.__v);
      __update_end();
    }

    return *this;
  }

  /// check the existence of k in the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE bool SequenceImplementation<Key, Alloc, Gen>::exists(const Key &k) const {
    return __h.exists(k);
  }

  /// insert an element at the end of the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::insert(const Key &k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key &new_key = const_cast<Key &>(__h.insert(k, __h.size()).first);
    __v.push_back(&new_key);
    __update_end();
  }

  /// insert an element at the end of the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::insert(Key &&k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key &new_key = const_cast<Key &>(__h.insert(std::move(k), __h.size()).first);
    __v.push_back(&new_key);
    __update_end();
  }

  /// emplace a new element in the sequence
  template <typename Key, typename Alloc, bool Gen>
  template <typename... Args>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::emplace(Args &&... args) {
    Key key(std::forward<Args>(args)...);
    Key &new_key = const_cast<Key &>(__h.insert(std::move(key), __h.size()).first);
    __v.push_back(&new_key);
    __update_end();
  }

  /// insert k in the sequence (synonym for insert)
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator<<(const Key &k) {
    insert(k);
    return *this;
  }

  /// insert k in the sequence (synonym for insert)
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator<<(Key &&k) {
    insert(std::move(k));
    return *this;
  }

  /// remove an element from the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::erase(const Key &k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = __h[k];
    } catch (NotFound &) {
      return;
    }

    // erase the element
    __v.erase(__v.begin() + pos);
    for (Idx i = pos, nb_elts = __h.size() - 1; i < nb_elts; ++i) {
      --__h[*(__v[i])];
    }
    __h.erase(k);

    __update_end();
  }

  /// remove from the sequence the element pointed to by the iterator
  template <typename Key, typename Alloc, bool Gen>
  INLINE void
  SequenceImplementation<Key, Alloc, Gen>::erase(const iterator_safe &iter) {
    if (iter.pos() >= size())
      return;

    // erase the element
    Idx pos = iter.pos();
    Key *key = __v[pos];
    __v.erase(__v.begin() + pos);

    for (Idx i = pos, nb_elts = __h.size() - 1; i < nb_elts; ++i) {
      --__h[*(__v[i])];
    }
    __h.erase(*key);

    __update_end();
  }

  /// remove k in the sequence (synonym for erase)
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceImplementation<Key, Alloc, Gen> &
      SequenceImplementation<Key, Alloc, Gen>::
      operator>>(const Key &k) {
    erase(k);
    return *this;
  }

  // returns the object at position i ( first elt = index 0 )
  template <typename Key, typename Alloc, bool Gen>
  INLINE const Key &SequenceImplementation<Key, Alloc, Gen>::atPos(Idx i) const {
    if (i >= __h.size()) {
      GUM_ERROR(OutOfBounds, "index " << i << " for a sequence of size"
                                      << __h.size());
    }

    return *(__v[i]);
  }

  /// returns the element at position i (synonym for atPos)
  template <typename Key, typename Alloc, bool Gen>
  INLINE const Key &SequenceImplementation<Key, Alloc, Gen>::
  operator[](Idx i) const {
    return atPos(i);
  }

  /// returns the position of the object passed in argument (if it exists)
  template <typename Key, typename Alloc, bool Gen>
  INLINE Idx SequenceImplementation<Key, Alloc, Gen>::pos(const Key &key) const {
    return __h[key];
  }

  // inserts and returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::setAtPos(Idx i,
                                                                const Key &newKey) {
    if (i >= __h.size()) {
      GUM_ERROR(NotFound, "index too large");
    }

    Key &new_key = const_cast<Key &>(__h.insert(newKey, i).first);
    __h.erase(*(__v[i]));
    __v[i] = &new_key;
  }

  // inserts and returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::setAtPos(Idx i,
                                                                Key &&newKey) {
    if (i >= __h.size()) {
      GUM_ERROR(NotFound, "index too large");
    }

    Key &new_key = const_cast<Key &>(__h.insert(std::move(newKey), i).first);
    __h.erase(*(__v[i]));
    __v[i] = &new_key;
  }

  /// swap two elements in the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void SequenceImplementation<Key, Alloc, Gen>::swap(Idx i, Idx j) {
    if (i == j)
      return;

    Key &ki = const_cast<Key &>(atPos(i));
    Key &kj = const_cast<Key &>(atPos(j));

    __h[ki] = j;
    __h[kj] = i;

    __v[i] = &kj;
    __v[j] = &ki;
  }

  /// returns the first element
  template <typename Key, typename Alloc, bool Gen>
  INLINE const Key &SequenceImplementation<Key, Alloc, Gen>::front() const {
    return atPos(0);
  }

  /// returns the last element
  template <typename Key, typename Alloc, bool Gen>
  INLINE const Key &SequenceImplementation<Key, Alloc, Gen>::back() const {
    return atPos(size() - 1);
  }

  /// Print a sequence
  template <typename Key, typename Alloc, bool Gen>
  std::string SequenceImplementation<Key, Alloc, Gen>::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!__h.empty()) {
      stream << 0 << ":" << *__v[0];

      for (Idx i = 1; i < __h.size(); ++i) {
        stream << " - " << i << ":" << *__v[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  /// returns true if the content of k equals that of *this
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  bool SequenceImplementation<Key, Alloc, Gen>::
  operator==(const SequenceImplementation<Key, OtherAlloc, Gen> &k) const {
    if (size() != k.size())
      return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (*__v[i] != *(k.__v[i]))
          return false;
    }

    return true;
  }

  /// returns true if the content of k is different from that of *this
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE bool SequenceImplementation<Key, Alloc, Gen>::
  operator!=(const SequenceImplementation<Key, OtherAlloc, Gen> &k) const {
    return !operator==(k);
  }

  /// a << operator for displaying the content of the Sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE std::ostream &
  operator<<(std::ostream &stream,
             const SequenceImplementation<Key, Alloc, Gen> &seq) {
    stream << seq.toString();
    return stream;
  }

  /// returns the safe begin iterator
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceIteratorSafe<Key>
  SequenceImplementation<Key, Alloc, Gen>::beginSafe() const {
    return SequenceIteratorSafe<Key>{*this};
  }

  /// returns the safe end iterator
  template <typename Key, typename Alloc, bool Gen>
  INLINE const SequenceIteratorSafe<Key> &
  SequenceImplementation<Key, Alloc, Gen>::endSafe() const noexcept {
    return __end_safe;
  }

  /// return an iterator pointing to the last element
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceIteratorSafe<Key>
  SequenceImplementation<Key, Alloc, Gen>::rbeginSafe() const {
    SequenceIteratorSafe<Key> it{*this};
    it.__setPos(size() - 1);
    return it;
  }

  /// returns an iterator pointing just before the first element
  template <typename Key, typename Alloc, bool Gen>
  INLINE const SequenceIteratorSafe<Key> &
  SequenceImplementation<Key, Alloc, Gen>::rendSafe() const noexcept {
    return __rend_safe;
  }

  /// returns the unsafe begin iterator
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceIterator<Key>
  SequenceImplementation<Key, Alloc, Gen>::begin() const {
    return SequenceIterator<Key>{*this};
  }

  /// returns the unsafe end iterator
  template <typename Key, typename Alloc, bool Gen>
  INLINE const SequenceIterator<Key> &
  SequenceImplementation<Key, Alloc, Gen>::end() const noexcept {
    return __end_safe;
  }

  /// return an iterator pointing to the last element
  template <typename Key, typename Alloc, bool Gen>
  INLINE SequenceIterator<Key>
  SequenceImplementation<Key, Alloc, Gen>::rbegin() const {
    SequenceIterator<Key> it{*this};
    it.__setPos(size() - 1);
    return it;
  }

  /// returns an iterator pointing just before the first element
  template <typename Key, typename Alloc, bool Gen>
  INLINE const SequenceIterator<Key> &
  SequenceImplementation<Key, Alloc, Gen>::rend() const noexcept {
    return __rend_safe;
  }

  /// modifies the size of the internal structures of the sequence
  template <typename Key, typename Alloc, bool Gen>
  INLINE void
  SequenceImplementation<Key, Alloc, Gen>::resize(unsigned int new_size) {
    if (new_size < __h.size())
      return;

    __h.resize(new_size);
    __v.reserve(new_size);
  }

  /* =========================================================================== */
  /* ===                   SCALAR GUM SEQUENCE IMPLEMENTATION                === */
  /* =========================================================================== */

  /// updates the end iterators
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::__update_end() noexcept {
    __end_safe.__setAtEnd();
  }

  /// clear the sequence
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::clear() {
    __h.clear();
    __v.clear();
    __update_end();
  }

  /// clears the current sequence and fill it with copies the element of aSeq
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::__copy(
      const SequenceImplementation<Key, OtherAlloc, true> &aSeq) {
    clear();

    for (unsigned int i = 0; i < aSeq.size(); ++i) {
      __h.insert(aSeq.__v[i], i);
      __v.push_back(aSeq.__v[i]);
    }

    __update_end();
  }

  /// Default constructor
  template <typename Key, typename Alloc>
  INLINE
  SequenceImplementation<Key, Alloc, true>::SequenceImplementation(Size size_param)
      : __h(size_param), __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONSTRUCTOR(SequenceImplementation);
    __rend_safe.__setAtRend();
    __end_safe.__setAtEnd();
  }

  /// initializer list constructor
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true>::SequenceImplementation(
      std::initializer_list<Key> list)
      : __end_safe{*this}, __rend_safe{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    __rend_safe.__setAtRend();
    for (const auto &elt : list) {
      insert(elt);
    }
  }

  /// copy constructor
  template <typename Key, typename Alloc>
  SequenceImplementation<Key, Alloc, true>::SequenceImplementation(
      const SequenceImplementation<Key, Alloc, true> &aSeq)
      : __h(aSeq.__h), __v(aSeq.__v), __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    __rend_safe.__setAtRend();
    __end_safe.__setAtEnd();
  }

  /// generalized copy constructor
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  SequenceImplementation<Key, Alloc, true>::SequenceImplementation(
      const SequenceImplementation<Key, OtherAlloc, true> &aSeq)
      : __h(aSeq.size() / 2), __end_safe{*this}, __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    __rend_safe.__setAtRend();
    __copy(aSeq);
  }

  /// move constructor
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true>::SequenceImplementation(
      SequenceImplementation<Key, Alloc, true> &&aSeq)
      : __h(std::move(aSeq.__h)), __v(std::move(aSeq.__v)), __end_safe{*this},
        __rend_safe{*this} {
    // for debugging purposes
    GUM_CONS_MOV(SequenceImplementation);
    __rend_safe.__setAtRend();
    __end_safe.__setAtEnd();
  }

  /// destructor
  template <typename Key, typename Alloc>
  SequenceImplementation<Key, Alloc, true>::~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  /// copy operator
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true> &
      SequenceImplementation<Key, Alloc, true>::
      operator=(const SequenceImplementation<Key, Alloc, true> &aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      __copy(aSeq);
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE SequenceImplementation<Key, Alloc, true> &
      SequenceImplementation<Key, Alloc, true>::
      operator=(const SequenceImplementation<Key, OtherAlloc, true> &aSeq) {
    __copy(aSeq);
    return *this;
  }

  /// move operator
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true> &
      SequenceImplementation<Key, Alloc, true>::
      operator=(SequenceImplementation<Key, Alloc, true> &&aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      __h = std::move(aSeq.__h);
      __v = std::move(aSeq.__v);
      __update_end();
    }

    return *this;
  }

  /// check the existence of k in the sequence
  template <typename Key, typename Alloc>
  INLINE bool SequenceImplementation<Key, Alloc, true>::exists(Key k) const {
    return __h.exists(k);
  }

  /// insert an element at the end of the sequence
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::insert(Key k) {
    // k will be added at the end. Insert the new key into the hashtable
    __h.insert(k, __h.size());
    __v.push_back(k);
    __update_end();
  }

  /// emplace a new element in the sequence
  template <typename Key, typename Alloc>
  template <typename... Args>
  INLINE void SequenceImplementation<Key, Alloc, true>::emplace(Args &&... args) {
    Key new_key(std::forward<Args>(args)...);
    __h.insert(new_key, __h.size());
    __v.push_back(new_key);
    __update_end();
  }

  /// insert k in the sequence (synonym for insert)
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true> &
      SequenceImplementation<Key, Alloc, true>::
      operator<<(Key k) {
    insert(k);
    return *this;
  }

  /// remove an element from the sequence
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::erase(Key k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = __h[k];
    } catch (NotFound &) {
      return;
    }

    // erase the element
    __v.erase(__v.begin() + pos);
    for (Idx i = pos, nb_elts = __h.size() - 1; i < nb_elts; ++i) {
      --__h[__v[i]];
    }
    __h.erase(k);

    __update_end();
  }

  /// remove from the sequence the element pointed to by the iterator
  template <typename Key, typename Alloc>
  INLINE void
  SequenceImplementation<Key, Alloc, true>::erase(const iterator_safe &iter) {
    if (iter.pos() >= size())
      return;

    // erase the element
    Idx pos = iter.pos();
    Key key = __v[pos];
    __v.erase(__v.begin() + pos);

    for (Idx i = pos, nb_elts = __h.size() - 1; i < nb_elts; ++i) {
      --__h[__v[i]];
    }
    __h.erase(key);

    __update_end();
  }

  /// remove k in the sequence (synonym for erase)
  template <typename Key, typename Alloc>
  INLINE SequenceImplementation<Key, Alloc, true> &
      SequenceImplementation<Key, Alloc, true>::
      operator>>(Key k) {
    erase(k);
    return *this;
  }

  // returns the object at position i
  template <typename Key, typename Alloc>
  INLINE const Key &SequenceImplementation<Key, Alloc, true>::atPos(Idx i) const {
    if (i >= __h.size()) {
      GUM_ERROR(NotFound, "not enough elements in the sequence");
    }

    return __v[i];
  }

  /// returns the element at position i (synonym for atPos)
  template <typename Key, typename Alloc>
  INLINE const Key &SequenceImplementation<Key, Alloc, true>::
  operator[](Idx i) const {
    return atPos(i);
  }

  /// returns the position of the object passed in argument (if it exists)
  template <typename Key, typename Alloc>
  INLINE Idx SequenceImplementation<Key, Alloc, true>::pos(Key key) const {
    return __h[key];
  }

  // sets the object at position i
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::setAtPos(Idx i, Key newKey) {
    if (i >= __h.size()) {
      GUM_ERROR(NotFound, "index too large");
    }

    __h.insert(newKey, i);
    __h.erase(__v[i]);
    __v[i] = newKey;
  }

  /// swap two elements in the sequence
  template <typename Key, typename Alloc>
  INLINE void SequenceImplementation<Key, Alloc, true>::swap(Idx i, Idx j) {
    if (i == j)
      return;

    Key ki = atPos(i);
    Key kj = atPos(j);

    __h[ki] = j;
    __h[kj] = i;

    __v[i] = kj;
    __v[j] = ki;
  }

  /// returns the first element
  template <typename Key, typename Alloc>
  INLINE const Key &SequenceImplementation<Key, Alloc, true>::front() const {
    return atPos(0);
  }

  /// returns the last element
  template <typename Key, typename Alloc>
  INLINE const Key &SequenceImplementation<Key, Alloc, true>::back() const {
    return atPos(size() - 1);
  }

  /// Print a sequence
  template <typename Key, typename Alloc>
  std::string SequenceImplementation<Key, Alloc, true>::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!__h.empty()) {
      stream << 0 << ":" << __v[0];

      for (Idx i = 1; i < __h.size(); ++i) {
        stream << " - " << i << ":" << __v[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  /// returns true if the content of k equals that of *this
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  bool SequenceImplementation<Key, Alloc, true>::
  operator==(const SequenceImplementation<Key, OtherAlloc, true> &k) const {
    if (size() != k.size())
      return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (__v[i] != k.__v[i])
          return false;
    }

    return true;
  }

  /// returns true if the content of k is different from that of *this
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE bool SequenceImplementation<Key, Alloc, true>::
  operator!=(const SequenceImplementation<Key, OtherAlloc, true> &k) const {
    return !operator==(k);
  }

  /// a << operator for displaying the content of the Sequence
  template <typename Key, typename Alloc>
  INLINE std::ostream &
  operator<<(std::ostream &stream,
             const SequenceImplementation<Key, Alloc, true> &seq) {
    stream << seq.toString();
    return stream;
  }

  /// returns the safe begin iterator
  template <typename Key, typename Alloc>
  INLINE SequenceIteratorSafe<Key>
  SequenceImplementation<Key, Alloc, true>::beginSafe() const {
    return SequenceIteratorSafe<Key>{*this};
  }

  /// return the safe end iterator
  template <typename Key, typename Alloc>
  INLINE const SequenceIteratorSafe<Key> &
  SequenceImplementation<Key, Alloc, true>::endSafe() const noexcept {
    return __end_safe;
  }

  /// return an iterator pointing to the last element
  template <typename Key, typename Alloc>
  INLINE SequenceIteratorSafe<Key>
  SequenceImplementation<Key, Alloc, true>::rbeginSafe() const {
    SequenceIteratorSafe<Key> it{*this};
    it.__setPos(size() - 1);
    return it;
  }

  /// returns an iterator pointing just before the first element
  template <typename Key, typename Alloc>
  INLINE const SequenceIteratorSafe<Key> &
  SequenceImplementation<Key, Alloc, true>::rendSafe() const noexcept {
    return __rend_safe;
  }

  /// returns the unsafe begin iterator
  template <typename Key, typename Alloc>
  INLINE SequenceIterator<Key>
  SequenceImplementation<Key, Alloc, true>::begin() const {
    return SequenceIterator<Key>{*this};
  }

  /// return the unsafe end iterator
  template <typename Key, typename Alloc>
  INLINE const SequenceIterator<Key> &
  SequenceImplementation<Key, Alloc, true>::end() const noexcept {
    return __end_safe;
  }

  /// return an unsafe iterator pointing to the last element
  template <typename Key, typename Alloc>
  INLINE SequenceIterator<Key>
  SequenceImplementation<Key, Alloc, true>::rbegin() const {
    SequenceIterator<Key> it{*this};
    it.__setPos(size() - 1);
    return it;
  }

  /// returns an unsafe iterator pointing just before the first element
  template <typename Key, typename Alloc>
  INLINE const SequenceIterator<Key> &
  SequenceImplementation<Key, Alloc, true>::rend() const noexcept {
    return __rend_safe;
  }

  /// modifies the size of the internal structures of the sequence
  template <typename Key, typename Alloc>
  INLINE void
  SequenceImplementation<Key, Alloc, true>::resize(unsigned int new_size) {
    if (new_size < __h.size())
      return;

    __h.resize(new_size);
    __v.reserve(new_size);
  }

  /* =========================================================================== */
  // Sequence
  /* =========================================================================== */

  /// Default constructor
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc>::Sequence(Size size_param)
      : SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>(size_param) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Sequence);
  }

  /// initializer list constructor
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc>::Sequence(std::initializer_list<Key> list)
      : SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>{list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(Sequence);
  }

  /// copy constructor
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc>::Sequence(const Sequence<Key, Alloc> &aSeq)
      : SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>(aSeq) {
    // for debugging purposes
    GUM_CONS_CPY(Sequence);
  }

  /// generalized copy constructor
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE Sequence<Key, Alloc>::Sequence(const Sequence<Key, OtherAlloc> &aSeq)
      : SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>(aSeq) {
    // for debugging purposes
    GUM_CONS_CPY(Sequence);
  }

  /// move constructor
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc>::Sequence(Sequence<Key, Alloc> &&aSeq)
      : SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>(
            std::move(aSeq)) {
    // for debugging purposes
    GUM_CONS_MOV(Sequence);
  }

  /// destructor
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc>::~Sequence() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(Sequence);
  }

  /// copy operator
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc> &Sequence<Key, Alloc>::
  operator=(const Sequence<Key, Alloc> &aSeq) {
    Implementation::operator=(aSeq);
    return *this;
  }

  /// generalized copy operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE Sequence<Key, Alloc> &Sequence<Key, Alloc>::
  operator=(const Sequence<Key, OtherAlloc> &aSeq) {
    Implementation::operator=(aSeq);
    return *this;
  }

  /// move operator
  template <typename Key, typename Alloc>
  INLINE Sequence<Key, Alloc> &Sequence<Key, Alloc>::
  operator=(Sequence<Key, Alloc> &&aSeq) {
    Implementation::operator=(std::move(aSeq));
    return *this;
  }

  /// returns the set difference : this \ seq
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  Set<Key, Alloc>
  Sequence<Key, Alloc>::diffSet(const Sequence<Key, OtherAlloc> &seq) const {
    Set<Key, Alloc> res;

    for (iterator iter = this->begin(); iter != this->end(); ++iter) {
      if (!seq.exists(*iter))
        res << *iter;
    }

    return res;
  }

  /// a << operator for displaying the content of the Sequence
  template <typename Key, typename Alloc>
  INLINE std::ostream &operator<<(std::ostream &stream,
                                  const Sequence<Key, Alloc> &seq) {
    stream << seq.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
