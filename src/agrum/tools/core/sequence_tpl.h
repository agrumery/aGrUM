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
 * @brief Template implementation file of gum::Sequence, a class for storing
 * (ordered) sequences of objects.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

// to ease IDE parser
#include <agrum/tools/core/sequence.h>

namespace gum {

  // returns the size of the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE Size SequenceImplementation< Key, Alloc, Gen >::size() const noexcept {
    return h__.size();
  }

  // return true if empty
  template < typename Key, typename Alloc, bool Gen >
  INLINE bool SequenceImplementation< Key, Alloc, Gen >::empty() const noexcept {
    return h__.empty();
  }

  // returns the size of the sequence
  template < typename Key, typename Alloc >
  INLINE Size SequenceImplementation< Key, Alloc, true >::size() const noexcept {
    return h__.size();
  }

  // return true if empty
  template < typename Key, typename Alloc >
  INLINE bool SequenceImplementation< Key, Alloc, true >::empty() const noexcept {
    return h__.empty();
  }

  // ===========================================================================
  // class SequenceIteratorSafe
  // ===========================================================================

  // default constructor
  template < typename Key >
  template < typename Alloc, bool Gen >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     const SequenceImplementation< Key, Alloc, Gen >& seq, Idx pos) noexcept :
      seq__{reinterpret_cast<
         const SequenceImplementation< Key,
                                       std::allocator< Key >,
                                       std::is_scalar< Key >::value >* >(&seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > seq__->size())
      iterator__ = seq__->size();   // make the iterator point to end
    else
      iterator__ = pos;
  }

  // default constructor
  template < typename Key >
  template < typename Alloc >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     const Sequence< Key, Alloc >& seq, Idx pos) noexcept :
      seq__{reinterpret_cast<
         const SequenceImplementation< Key,
                                       std::allocator< Key >,
                                       std::is_scalar< Key >::value >* >(&seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > seq__->size())
      iterator__ = seq__->size();   // make the iterator point to end
    else
      iterator__ = pos;
  }

  // copy constructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     const SequenceIteratorSafe< Key >& source) noexcept :
      iterator__{source.iterator__},
      seq__{source.seq__} {
    GUM_CONS_CPY(SequenceIteratorSafe);
  }

  // move constructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     SequenceIteratorSafe< Key >&& source) noexcept :
      iterator__{source.iterator__},
      seq__{source.seq__} {
    GUM_CONS_MOV(SequenceIteratorSafe);
  }

  // destructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::~SequenceIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SequenceIteratorSafe);
  }

  // copy operator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator=(
     const SequenceIteratorSafe< Key >& source) noexcept {
    iterator__ = source.iterator__;
    seq__ = source.seq__;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator=(
     SequenceIteratorSafe< Key >&& source) noexcept {
    iterator__ = source.iterator__;
    seq__ = source.seq__;
    return *this;
  }

  // point the iterator to the next value in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator++() noexcept {
    if (iterator__ < seq__->size())
      ++iterator__;
    else
      iterator__ = seq__->size();

    return *this;
  }

  // point the iterator to the preceding value in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator--() noexcept {
    if (iterator__ != std::numeric_limits< Idx >::max()) --iterator__;

    return *this;
  }

  // makes the iterator point to i elements further in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator+=(Size nb) noexcept {
    if (iterator__ == std::numeric_limits< Idx >::max()) return *this;
    iterator__ += nb;
    if (iterator__ > seq__->size()) iterator__ = seq__->size();

    return *this;
  }

  // makes the iterator point to i elements further in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator-=(Size nb) noexcept {
    if (iterator__ == std::numeric_limits< Idx >::max()) return *this;
    iterator__ -= nb;
    if (iterator__ > seq__->size()) iterator__ = std::numeric_limits< Idx >::max();

    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >
     SequenceIteratorSafe< Key >::operator+(Size nb) noexcept {
    return SequenceIteratorSafe< Key >{*this} += nb;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >
     SequenceIteratorSafe< Key >::operator-(Size nb) noexcept {
    return SequenceIteratorSafe< Key >{*this} -= nb;
  }

  // checks whether two iterators are pointing to the same element
  template < typename Key >
  INLINE bool SequenceIteratorSafe< Key >::operator==(
     const SequenceIteratorSafe< Key >& source) const noexcept {
    if (seq__->empty())
      return true;   // all iterators are the same if seq is empty

    if ((iterator__ != source.iterator__) || (seq__ != source.seq__)) return false;

    return true;
  }

  // checks whether two iterators are pointing to different elements
  template < typename Key >
  INLINE bool SequenceIteratorSafe< Key >::operator!=(
     const SequenceIteratorSafe< Key >& source) const noexcept {
    return !operator==(source);
  }

  // returns the position of the iterator in the sequence
  template < typename Key >
  INLINE Idx SequenceIteratorSafe< Key >::pos() const {
    if (iterator__ >= seq__->size()) {
      GUM_ERROR(UndefinedIteratorValue, "iterator is end() or rend()");
    }

    return iterator__;
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::setPos__(Idx pos) noexcept {
    if (pos > seq__->size())
      iterator__ = seq__->size();
    else
      iterator__ = pos;
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::setAtRend__() noexcept {
    iterator__ = std::numeric_limits< Idx >::max();
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::setAtEnd__() noexcept {
    iterator__ = seq__->size();
  }

  // returns the value pointed to by the iterator
  template < typename Key >
  INLINE const Key& SequenceIteratorSafe< Key >::operator*() const {
    return Getter::op_star(seq__->v__[pos()]);
  }

  // dereferences the value pointed to by the iterator
  template < typename Key >
  INLINE const Key* SequenceIteratorSafe< Key >::operator->() const {
    return Getter::op_arrow(seq__->v__[pos()]);
  }

  // ===========================================================================
  // ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              ===
  // ===========================================================================

  // updates const iterators
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::update_end__() noexcept {
    end_safe__.setAtEnd__();
  }

  // clear the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::clear() {
    h__.clear();
    v__.clear();
    update_end__();
  }

  // clears the current sequence and fill it with copies the element of aSeq
  template < typename Key, typename Alloc, bool Gen >
  template < typename OtherAlloc >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::copy__(
     const SequenceImplementation< Key, OtherAlloc, Gen >& aSeq) {
    clear();

    for (Size i = 0; i < aSeq.size(); ++i) {
      Key& new_key = const_cast< Key& >(h__.insert(*(aSeq.v__[i]), i).first);
      v__.push_back(&new_key);
    }

    update_end__();
  }

  // Default constructor
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >::SequenceImplementation(
     Size size_param) :
      h__(size_param),
      end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONSTRUCTOR(SequenceImplementation);
    rend_safe__.setAtRend__();
    update_end__();
  }

  // initializer list constructor
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >::SequenceImplementation(
     std::initializer_list< Key > list) :
      end_safe__{*this},
      rend_safe__{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    rend_safe__.setAtRend__();
    for (const auto& elt: list) {
      insert(elt);   // performs the update_end__ ()
    }
  }

  // copy constructor
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >::SequenceImplementation(
     const SequenceImplementation< Key, Alloc, Gen >& aSeq) :
      end_safe__{*this},
      rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    rend_safe__.setAtRend__();
    copy__(aSeq);   // performs the update_end__ ()
  }

  // generalized copy constructor
  template < typename Key, typename Alloc, bool Gen >
  template < typename OtherAlloc >
  INLINE SequenceImplementation< Key, Alloc, Gen >::SequenceImplementation(
     const SequenceImplementation< Key, OtherAlloc, Gen >& aSeq) :
      end_safe__{*this},
      rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    rend_safe__.setAtRend__();
    copy__(aSeq);   // performs the update_end__ ()
  }

  // move constructor
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >::SequenceImplementation(
     SequenceImplementation< Key, Alloc, Gen >&& aSeq) :
      h__(std::move(aSeq.h__)),
      v__(std::move(aSeq.v__)), end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_MOV(SequenceImplementation);
    rend_safe__.setAtRend__();
    update_end__();
  }

  // destructor
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >::
     ~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  // copy operator
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator=(
        const SequenceImplementation< Key, Alloc, Gen >& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      copy__(aSeq);   // performs the update_end__ ()
    }

    return *this;
  }

  // generalized copy operator
  template < typename Key, typename Alloc, bool Gen >
  template < typename OtherAlloc >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator=(
        const SequenceImplementation< Key, OtherAlloc, Gen >& aSeq) {
    copy__(aSeq);   // performs the update_end__ ()
    return *this;
  }

  // move operator
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator=(
        SequenceImplementation< Key, Alloc, Gen >&& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      h__ = std::move(aSeq.h__);
      v__ = std::move(aSeq.v__);
      update_end__();
    }

    return *this;
  }

  // check the existence of k in the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE bool
     SequenceImplementation< Key, Alloc, Gen >::exists(const Key& k) const {
    return h__.exists(k);
  }

  // insert an element at the end of the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::insert(const Key& k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key& new_key = const_cast< Key& >(h__.insert(k, h__.size()).first);
    v__.push_back(&new_key);
    update_end__();
  }

  // insert an element at the end of the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::insert(Key&& k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key& new_key = const_cast< Key& >(h__.insert(std::move(k), h__.size()).first);
    v__.push_back(&new_key);
    update_end__();
  }

  // emplace a new element in the sequence
  template < typename Key, typename Alloc, bool Gen >
  template < typename... Args >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::emplace(Args&&... args) {
    Key  key(std::forward< Args >(args)...);
    Key& new_key =
       const_cast< Key& >(h__.insert(std::move(key), h__.size()).first);
    v__.push_back(&new_key);
    update_end__();
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator<<(const Key& k) {
    insert(k);
    return *this;
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator<<(Key&& k) {
    insert(std::move(k));
    return *this;
  }

  // remove an element from the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::erase(const Key& k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = h__[k];
    } catch (NotFound&) { return; }

    // erase the element
    v__.erase(v__.begin() + pos);
    for (Idx i = pos, nb_elts = h__.size() - 1; i < nb_elts; ++i) {
      --h__[*(v__[i])];
    }
    h__.erase(k);

    update_end__();
  }

  // remove from the sequence the element pointed to by the iterator
  template < typename Key, typename Alloc, bool Gen >
  INLINE void
     SequenceImplementation< Key, Alloc, Gen >::erase(const iterator_safe& iter) {
    if (iter.pos() >= size()) return;

    // erase the element
    Idx  pos = iter.pos();
    Key* key = v__[pos];
    v__.erase(v__.begin() + pos);

    for (Idx i = pos, nb_elts = h__.size() - 1; i < nb_elts; ++i) {
      --h__[*(v__[i])];
    }
    h__.erase(*key);

    update_end__();
  }

  // remove k in the sequence (synonym for erase)
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceImplementation< Key, Alloc, Gen >&
     SequenceImplementation< Key, Alloc, Gen >::operator>>(const Key& k) {
    erase(k);
    return *this;
  }

  // returns the object at position i ( first elt = index 0 )
  template < typename Key, typename Alloc, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Alloc, Gen >::atPos(Idx i) const {
    if (i >= h__.size()) {
      GUM_ERROR(OutOfBounds,
                "index " << i << " for a sequence of size" << h__.size());
    }

    return *(v__[i]);
  }

  // returns the element at position i (synonym for atPos)
  template < typename Key, typename Alloc, bool Gen >
  INLINE const Key&
     SequenceImplementation< Key, Alloc, Gen >::operator[](Idx i) const {
    return atPos(i);
  }

  // returns the position of the object passed in argument (if it exists)
  template < typename Key, typename Alloc, bool Gen >
  INLINE Idx SequenceImplementation< Key, Alloc, Gen >::pos(const Key& key) const {
    return h__[key];
  }

  // inserts and returns the object at the pos i
  template < typename Key, typename Alloc, bool Gen >
  INLINE void
     SequenceImplementation< Key, Alloc, Gen >::setAtPos(Idx        i,
                                                         const Key& newKey) {
    if (i >= h__.size()) { GUM_ERROR(NotFound, "index too large"); }

    Key& new_key = const_cast< Key& >(h__.insert(newKey, i).first);
    h__.erase(*(v__[i]));
    v__[i] = &new_key;
  }

  // inserts and returns the object at the pos i
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::setAtPos(Idx   i,
                                                                  Key&& newKey) {
    if (i >= h__.size()) { GUM_ERROR(NotFound, "index too large"); }

    Key& new_key = const_cast< Key& >(h__.insert(std::move(newKey), i).first);
    h__.erase(*(v__[i]));
    v__[i] = &new_key;
  }

  // replace two elements in the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::swap(Idx i, Idx j) {
    if (i == j) return;

    Key& ki = const_cast< Key& >(atPos(i));
    Key& kj = const_cast< Key& >(atPos(j));

    h__[ki] = j;
    h__[kj] = i;

    v__[i] = &kj;
    v__[j] = &ki;
  }

  // returns the first element
  template < typename Key, typename Alloc, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Alloc, Gen >::front() const {
    return atPos(0);
  }

  // returns the last element
  template < typename Key, typename Alloc, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Alloc, Gen >::back() const {
    return atPos(size() - 1);
  }

  // Print a sequence
  template < typename Key, typename Alloc, bool Gen >
  std::string SequenceImplementation< Key, Alloc, Gen >::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!h__.empty()) {
      stream << 0 << ":" << *v__[0];

      for (Idx i = 1; i < h__.size(); ++i) {
        stream << " - " << i << ":" << *v__[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  // returns true if the content of k equals that of *this
  template < typename Key, typename Alloc, bool Gen >
  template < typename OtherAlloc >
  bool SequenceImplementation< Key, Alloc, Gen >::operator==(
     const SequenceImplementation< Key, OtherAlloc, Gen >& k) const {
    if (size() != k.size())
      return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (*v__[i] != *(k.v__[i])) return false;
    }

    return true;
  }

  // returns true if the content of k is different from that of *this
  template < typename Key, typename Alloc, bool Gen >
  template < typename OtherAlloc >
  INLINE bool SequenceImplementation< Key, Alloc, Gen >::operator!=(
     const SequenceImplementation< Key, OtherAlloc, Gen >& k) const {
    return !operator==(k);
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE std::ostream&
         operator<<(std::ostream&                                    stream,
                const SequenceImplementation< Key, Alloc, Gen >& seq) {
    stream << seq.toString();
    return stream;
  }

  // returns the safe begin iterator
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceIteratorSafe< Key >
         SequenceImplementation< Key, Alloc, Gen >::beginSafe() const {
    return SequenceIteratorSafe< Key >{*this};
  }

  // returns the safe end iterator
  template < typename Key, typename Alloc, bool Gen >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Alloc, Gen >::endSafe() const noexcept {
    return end_safe__;
  }

  // return an iterator pointing to the last element
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceIteratorSafe< Key >
         SequenceImplementation< Key, Alloc, Gen >::rbeginSafe() const {
    SequenceIteratorSafe< Key > it{*this};
    it.setPos__(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key, typename Alloc, bool Gen >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Alloc, Gen >::rendSafe() const noexcept {
    return rend_safe__;
  }

  // returns the unsafe begin iterator
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceIterator< Key >
         SequenceImplementation< Key, Alloc, Gen >::begin() const {
    return SequenceIterator< Key >{*this};
  }

  // returns the unsafe end iterator
  template < typename Key, typename Alloc, bool Gen >
  INLINE const SequenceIterator< Key >&
               SequenceImplementation< Key, Alloc, Gen >::end() const noexcept {
    return end_safe__;
  }

  // return an iterator pointing to the last element
  template < typename Key, typename Alloc, bool Gen >
  INLINE SequenceIterator< Key >
         SequenceImplementation< Key, Alloc, Gen >::rbegin() const {
    SequenceIterator< Key > it{*this};
    it.setPos__(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key, typename Alloc, bool Gen >
  INLINE const SequenceIterator< Key >&
               SequenceImplementation< Key, Alloc, Gen >::rend() const noexcept {
    return rend_safe__;
  }

  // modifies the size of the internal structures of the sequence
  template < typename Key, typename Alloc, bool Gen >
  INLINE void SequenceImplementation< Key, Alloc, Gen >::resize(Size new_size) {
    if (new_size < h__.size()) return;

    h__.resize(new_size);
    v__.reserve(new_size);
  }

  // ===========================================================================
  // ===                   SCALAR GUM SEQUENCE IMPLEMENTATION                ===
  // ===========================================================================

  // updates the end iterators
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::update_end__() noexcept {
    end_safe__.setAtEnd__();
  }

  // clear the sequence
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::clear() {
    h__.clear();
    v__.clear();
    update_end__();
  }

  // clears the current sequence and fill it with copies the element of aSeq
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::copy__(
     const SequenceImplementation< Key, OtherAlloc, true >& aSeq) {
    clear();

    for (Size i = 0; i < aSeq.size(); ++i) {
      h__.insert(aSeq.v__[i], i);
      v__.push_back(aSeq.v__[i]);
    }

    update_end__();
  }

  // Default constructor
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >::SequenceImplementation(
     Size size_param) :
      h__(size_param),
      end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONSTRUCTOR(SequenceImplementation);
    rend_safe__.setAtRend__();
    end_safe__.setAtEnd__();
  }

  // initializer list constructor
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >::SequenceImplementation(
     std::initializer_list< Key > list) :
      end_safe__{*this},
      rend_safe__{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    rend_safe__.setAtRend__();
    for (const auto& elt: list) {
      insert(elt);
    }
  }

  // copy constructor
  template < typename Key, typename Alloc >
  SequenceImplementation< Key, Alloc, true >::SequenceImplementation(
     const SequenceImplementation< Key, Alloc, true >& aSeq) :
      h__(aSeq.h__),
      v__(aSeq.v__), end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    rend_safe__.setAtRend__();
    end_safe__.setAtEnd__();
  }

  // generalized copy constructor
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  SequenceImplementation< Key, Alloc, true >::SequenceImplementation(
     const SequenceImplementation< Key, OtherAlloc, true >& aSeq) :
      h__(aSeq.size() / 2),
      end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_CPY(SequenceImplementation);
    rend_safe__.setAtRend__();
    copy__(aSeq);
  }

  // move constructor
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >::SequenceImplementation(
     SequenceImplementation< Key, Alloc, true >&& aSeq) :
      h__(std::move(aSeq.h__)),
      v__(std::move(aSeq.v__)), end_safe__{*this}, rend_safe__{*this} {
    // for debugging purposes
    GUM_CONS_MOV(SequenceImplementation);
    rend_safe__.setAtRend__();
    end_safe__.setAtEnd__();
  }

  // destructor
  template < typename Key, typename Alloc >
  SequenceImplementation< Key, Alloc, true >::~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  // copy operator
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >&
     SequenceImplementation< Key, Alloc, true >::operator=(
        const SequenceImplementation< Key, Alloc, true >& aSeq) {
    // avoid self assignment
    if (&aSeq != this) { copy__(aSeq); }

    return *this;
  }

  // generalized copy operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE SequenceImplementation< Key, Alloc, true >&
     SequenceImplementation< Key, Alloc, true >::operator=(
        const SequenceImplementation< Key, OtherAlloc, true >& aSeq) {
    copy__(aSeq);
    return *this;
  }

  // move operator
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >&
     SequenceImplementation< Key, Alloc, true >::operator=(
        SequenceImplementation< Key, Alloc, true >&& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      h__ = std::move(aSeq.h__);
      v__ = std::move(aSeq.v__);
      update_end__();
    }

    return *this;
  }

  // check the existence of k in the sequence
  template < typename Key, typename Alloc >
  INLINE bool SequenceImplementation< Key, Alloc, true >::exists(Key k) const {
    return h__.exists(k);
  }

  // insert an element at the end of the sequence
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::insert(Key k) {
    // k will be added at the end. Insert the new key into the hashtable
    h__.insert(k, h__.size());
    v__.push_back(k);
    update_end__();
  }

  // emplace a new element in the sequence
  template < typename Key, typename Alloc >
  template < typename... Args >
  INLINE void SequenceImplementation< Key, Alloc, true >::emplace(Args&&... args) {
    Key new_key(std::forward< Args >(args)...);
    h__.insert(new_key, h__.size());
    v__.push_back(new_key);
    update_end__();
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >&
     SequenceImplementation< Key, Alloc, true >::operator<<(Key k) {
    insert(k);
    return *this;
  }

  // remove an element from the sequence
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::erase(Key k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = h__[k];
    } catch (NotFound&) { return; }

    // erase the element
    v__.erase(v__.begin() + pos);
    for (Idx i = pos, nb_elts = h__.size() - 1; i < nb_elts; ++i) {
      --h__[v__[i]];
    }
    h__.erase(k);

    update_end__();
  }

  // remove from the sequence the element pointed to by the iterator
  template < typename Key, typename Alloc >
  INLINE void
     SequenceImplementation< Key, Alloc, true >::erase(const iterator_safe& iter) {
    if (iter.pos() >= size()) return;

    // erase the element
    Idx pos = iter.pos();
    Key key = v__[pos];
    v__.erase(v__.begin() + pos);

    for (Idx i = pos, nb_elts = h__.size() - 1; i < nb_elts; ++i) {
      --h__[v__[i]];
    }
    h__.erase(key);

    update_end__();
  }

  // remove k in the sequence (synonym for erase)
  template < typename Key, typename Alloc >
  INLINE SequenceImplementation< Key, Alloc, true >&
     SequenceImplementation< Key, Alloc, true >::operator>>(Key k) {
    erase(k);
    return *this;
  }

  // returns the object at position i
  template < typename Key, typename Alloc >
  INLINE const Key&
               SequenceImplementation< Key, Alloc, true >::atPos(Idx i) const {
    if (i >= h__.size()) {
      GUM_ERROR(NotFound, "not enough elements in the sequence");
    }

    return v__[i];
  }

  // returns the element at position i (synonym for atPos)
  template < typename Key, typename Alloc >
  INLINE const Key&
     SequenceImplementation< Key, Alloc, true >::operator[](Idx i) const {
    return atPos(i);
  }

  // returns the position of the object passed in argument (if it exists)
  template < typename Key, typename Alloc >
  INLINE Idx SequenceImplementation< Key, Alloc, true >::pos(Key key) const {
    return h__[key];
  }

  // sets the object at position i
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::setAtPos(Idx i,
                                                                   Key newKey) {
    if (i >= h__.size()) { GUM_ERROR(NotFound, "index too large"); }

    h__.insert(newKey, i);
    h__.erase(v__[i]);
    v__[i] = newKey;
  }

  // replace two elements in the sequence
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::swap(Idx i, Idx j) {
    if (i == j) return;

    Key ki = atPos(i);
    Key kj = atPos(j);

    h__[ki] = j;
    h__[kj] = i;

    v__[i] = kj;
    v__[j] = ki;
  }

  // returns the first element
  template < typename Key, typename Alloc >
  INLINE const Key& SequenceImplementation< Key, Alloc, true >::front() const {
    return atPos(0);
  }

  // returns the last element
  template < typename Key, typename Alloc >
  INLINE const Key& SequenceImplementation< Key, Alloc, true >::back() const {
    return atPos(size() - 1);
  }

  // Print a sequence
  template < typename Key, typename Alloc >
  std::string SequenceImplementation< Key, Alloc, true >::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!h__.empty()) {
      stream << 0 << ":" << v__[0];

      for (Idx i = 1; i < h__.size(); ++i) {
        stream << " - " << i << ":" << v__[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  // returns true if the content of k equals that of *this
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  bool SequenceImplementation< Key, Alloc, true >::operator==(
     const SequenceImplementation< Key, OtherAlloc, true >& k) const {
    if (size() != k.size())
      return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (v__[i] != k.v__[i]) return false;
    }

    return true;
  }

  // returns true if the content of k is different from that of *this
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool SequenceImplementation< Key, Alloc, true >::operator!=(
     const SequenceImplementation< Key, OtherAlloc, true >& k) const {
    return !operator==(k);
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key, typename Alloc >
  INLINE std::ostream&
         operator<<(std::ostream&                                     stream,
                const SequenceImplementation< Key, Alloc, true >& seq) {
    stream << seq.toString();
    return stream;
  }

  // returns the safe begin iterator
  template < typename Key, typename Alloc >
  INLINE SequenceIteratorSafe< Key >
         SequenceImplementation< Key, Alloc, true >::beginSafe() const {
    return SequenceIteratorSafe< Key >{*this};
  }

  // return the safe end iterator
  template < typename Key, typename Alloc >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Alloc, true >::endSafe() const noexcept {
    return end_safe__;
  }

  // return an iterator pointing to the last element
  template < typename Key, typename Alloc >
  INLINE SequenceIteratorSafe< Key >
         SequenceImplementation< Key, Alloc, true >::rbeginSafe() const {
    SequenceIteratorSafe< Key > it{*this};
    it.setPos__(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key, typename Alloc >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Alloc, true >::rendSafe() const noexcept {
    return rend_safe__;
  }

  // returns the unsafe begin iterator
  template < typename Key, typename Alloc >
  INLINE SequenceIterator< Key >
         SequenceImplementation< Key, Alloc, true >::begin() const {
    return SequenceIterator< Key >{*this};
  }

  // return the unsafe end iterator
  template < typename Key, typename Alloc >
  INLINE const SequenceIterator< Key >&
               SequenceImplementation< Key, Alloc, true >::end() const noexcept {
    return end_safe__;
  }

  // return an unsafe iterator pointing to the last element
  template < typename Key, typename Alloc >
  INLINE SequenceIterator< Key >
         SequenceImplementation< Key, Alloc, true >::rbegin() const {
    SequenceIterator< Key > it{*this};
    it.setPos__(size() - 1);
    return it;
  }

  // returns an unsafe iterator pointing just before the first element
  template < typename Key, typename Alloc >
  INLINE const SequenceIterator< Key >&
               SequenceImplementation< Key, Alloc, true >::rend() const noexcept {
    return rend_safe__;
  }

  // modifies the size of the internal structures of the sequence
  template < typename Key, typename Alloc >
  INLINE void SequenceImplementation< Key, Alloc, true >::resize(Size new_size) {
    if (new_size < h__.size()) return;

    h__.resize(new_size);
    v__.reserve(new_size);
  }

  // ===========================================================================
  // Sequence
  // ===========================================================================

  // Default constructor
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >::Sequence(Size size_param) :
      SequenceImplementation< Key, Alloc, std::is_scalar< Key >::value >(
         size_param) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Sequence);
  }

  // initializer list constructor
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >::Sequence(std::initializer_list< Key > list) :
      SequenceImplementation< Key, Alloc, std::is_scalar< Key >::value >(list) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Sequence);
  }

  // copy constructor
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >::Sequence(const Sequence< Key, Alloc >& aSeq) :
      SequenceImplementation< Key, Alloc, std::is_scalar< Key >::value >(aSeq) {
    // for debugging purposes
    GUM_CONS_CPY(Sequence);
  }

  // generalized copy constructor
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE
     Sequence< Key, Alloc >::Sequence(const Sequence< Key, OtherAlloc >& aSeq) :
      SequenceImplementation< Key, Alloc, std::is_scalar< Key >::value >(aSeq) {
    // for debugging purposes
    GUM_CONS_CPY(Sequence);
  }

  // move constructor
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >::Sequence(Sequence< Key, Alloc >&& aSeq) :
      SequenceImplementation< Key, Alloc, std::is_scalar< Key >::value >(
         std::move(aSeq)) {
    // for debugging purposes
    GUM_CONS_MOV(Sequence);
  }

  // destructor
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >::~Sequence() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(Sequence);
  }

  // copy operator
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >&
     Sequence< Key, Alloc >::operator=(const Sequence< Key, Alloc >& aSeq) {
    Implementation::operator=(aSeq);
    return *this;
  }

  // generalized copy operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE Sequence< Key, Alloc >&
     Sequence< Key, Alloc >::operator=(const Sequence< Key, OtherAlloc >& aSeq) {
    Implementation::operator=(aSeq);
    return *this;
  }

  // move operator
  template < typename Key, typename Alloc >
  INLINE Sequence< Key, Alloc >&
     Sequence< Key, Alloc >::operator=(Sequence< Key, Alloc >&& aSeq) {
    Implementation::operator=(std::move(aSeq));
    return *this;
  }

  // returns the set difference : this \ seq
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  Set< Key, Alloc > Sequence< Key, Alloc >::diffSet(
     const Sequence< Key, OtherAlloc >& seq) const {
    Set< Key, Alloc > res;

    for (iterator iter = this->begin(); iter != this->end(); ++iter) {
      if (!seq.exists(*iter)) res << *iter;
    }

    return res;
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key, typename Alloc >
  INLINE std::ostream& operator<<(std::ostream&                 stream,
                                  const Sequence< Key, Alloc >& seq) {
    stream << seq.toString();
    return stream;
  }

} /* namespace gum */
