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
 * @brief template implementations of priority queues
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/priorityQueue.h>

namespace gum {

  // ===========================================================================
  // ===             GENERAL IMPLEMENTATIION OF PRIORITY QUEUES              ===
  // ===========================================================================

  // basic constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(Cmp compare, Size capacity) :
       _indices_(capacity >> 1, true, true),
       _cmp_(compare) {
     _heap_.reserve(capacity);

    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // initializer list constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        std::initializer_list< std::pair< Val, Priority > > list) :
       _indices_(Size(list.size()) / 2, true, true) {
    // fill the queue
     _heap_.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // copy constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
           from) :
       _heap_(from. _heap_),
       _indices_(from. _indices_),  _nb_elements_(from. _nb_elements_),
       _cmp_(from. _cmp_) {
    // fill the heap structure
    for (const auto& elt:  _indices_) {
       _heap_[elt.second].second = &(elt.first);
    }

    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // generalized copy constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, Gen >&
           from) :
       _indices_(from. _indices_),
       _nb_elements_(from. _nb_elements_),  _cmp_(from. _cmp_) {
    // fill the heap structure
    if ( _nb_elements_) {
       _heap_.reserve(from. _heap_.size());
      for (const auto& elt: from. _heap_) {
         _heap_.push_back(elt);
      }
      for (const auto& elt:  _indices_) {
         _heap_[elt.second].second = &(elt.first);
      }
    }

    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // move constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&& from) :
       _heap_(std::move(from. _heap_)),
       _indices_(std::move(from. _indices_)),
       _nb_elements_(std::move(from. _nb_elements_)),  _cmp_(std::move(from. _cmp_)) {
    GUM_CONS_MOV(PriorityQueueImplementation)
  }

  // destructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     ~PriorityQueueImplementation() {
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  // copy operator
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
           from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(PriorityQueueImplementation)

      try {
        // set the comparison function
         _cmp_ = from. _cmp_;

        // copy the indices and the heap
         _indices_     = from. _indices_;
         _heap_        = from. _heap_;
         _nb_elements_ = from. _nb_elements_;

        // restore the link between  _indices_ and  _heap_
        for (const auto& elt:  _indices_) {
           _heap_[elt.second].second = &(elt.first);
        }
      } catch (...) {
         _heap_.clear();
         _indices_.clear();
         _nb_elements_ = 0;

        throw;
      }
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, Gen >&
           from) {
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
       _cmp_ = from. _cmp_;

      // copy the indices and the heap
       _indices_     = from. _indices_;
       _nb_elements_ = from. _nb_elements_;

       _heap_.clear();
      if ( _nb_elements_) {
         _heap_.reserve(from. _heap_.size());
        for (const auto& elt: from. _heap_) {
           _heap_.push_back(elt);
        }
        for (const auto& elt:  _indices_) {
           _heap_[elt.second].second = &(elt.first);
        }
      }
    } catch (...) {
       _heap_.clear();
       _indices_.clear();
       _nb_elements_ = 0;

      throw;
    }

    return *this;
  }

  // move operator
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&& from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_MOV(PriorityQueueImplementation)

       _indices_     = std::move(from. _indices_);
       _heap_        = std::move(from. _heap_);
       _cmp_         = std::move(from. _cmp_);
       _nb_elements_ = std::move(from. _nb_elements_);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::top() const {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return *( _heap_[0].second);
  }

  // returns the priority of the top element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::topPriority()
        const {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return  _heap_[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::size()
     const noexcept {
    return  _nb_elements_;
  }

  // return the size of the array storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::capacity()
        const noexcept {
    return Size( _heap_.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::resize(
        Size new_size) {
    if (new_size <  _nb_elements_) return;

     _heap_.reserve(new_size);
     _indices_.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::clear() {
     _nb_elements_ = 0;
     _heap_.clear();
     _indices_.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::eraseByPos(
     Size index) {
    if (index >=  _nb_elements_) return;

    // remove the element from the hashtable
     _indices_.erase(*( _heap_[index].second));

    // put the last element at the "index" location
    std::pair< Priority, const Val* > last = std::move( _heap_[ _nb_elements_ - 1]);
     _heap_.pop_back();
    -- _nb_elements_;

    if (! _nb_elements_ || (index ==  _nb_elements_)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if ( _cmp_(last.first,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // put "last" back into the heap
     _heap_[i]                      = std::move(last);
     _indices_[*( _heap_[i].second)] = i;
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::erase(
     const Val& val) {
    try {
      eraseByPos( _indices_[val]);
    } catch (NotFound&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Val PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::pop() {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    Val v = *( _heap_[0].second);
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const HashTable< Val, Size >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::allValues()
        const noexcept {
    return reinterpret_cast< const HashTable< Val, Size >& >( _indices_);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::insert(
        const Val&      val,
        const Priority& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    //  _indices_.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       =  _indices_.insert(val, 0);

    try {
       _heap_.push_back(
         std::pair< Priority, const Val* >(priority, &new_elt.first));
    } catch (...) {
       _indices_.erase(val);
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move( _heap_[ _nb_elements_]);
    ++ _nb_elements_;

    // restore the heap property
    Size i =  _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_heap_val.first,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // put the new bucket into the heap
     _heap_[i].first  = std::move(new_heap_val.first);
     _heap_[i].second = &(new_elt.first);
    new_elt.second   = i;

    return i;
  }

  // inserts by move a new element in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::insert(
        Val&&      val,
        Priority&& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    //  _indices_.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       =  _indices_.insert(std::move(val), 0);

    try {
       _heap_.push_back(
         std::pair< Priority, const Val* >(std::move(priority), &(new_elt.first)));
    } catch (...) {
       _indices_.erase(new_elt.first);
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move( _heap_[ _nb_elements_]);
    ++ _nb_elements_;

    // restore the heap property
    Size i =  _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_heap_val.first,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // put the new bucket into the heap
     _heap_[i].first  = std::move(new_heap_val.first);
     _heap_[i].second = &(new_elt.first);
    new_elt.second   = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename... Args >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::emplace(
        Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::empty()
        const noexcept {
    return ( _nb_elements_ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::contains(
        const Val& val) const {
    return  _indices_.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator[](
        Size index) const {
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    return *( _heap_[index].second);
  }

  // displays the content of the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  std::string
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::toString()
        const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i !=  _nb_elements_; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << "(" <<  _heap_[i].first << " , " << *( _heap_[i].second) << ")";
    }

    stream << "]";

    return stream.str();
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     setPriorityByPos(Size index, const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    const Val* val =  _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_priority,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( _cmp_(new_priority,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // update the index of val
     _heap_[i].first  = new_priority;
     _heap_[i].second = val;
     _indices_[*val]  = i;

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     setPriorityByPos(Size index, Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    const Val* val =  _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_priority,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( _cmp_(new_priority,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                      = std::move( _heap_[j]);
       _indices_[*( _heap_[i].second)] = i;
    }

    // update the index of val
     _heap_[i].first  = std::move(new_priority);
     _heap_[i].second = val;
     _indices_[*val]  = i;

    return i;
  }

  // modifies the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::setPriority(
     const Val&      elt,
     const Priority& new_priority) {
    setPriorityByPos( _indices_[elt], new_priority);
  }

  // modifies the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::setPriority(
     const Val& elt,
     Priority&& new_priority) {
    setPriorityByPos( _indices_[elt], std::move(new_priority));
  }

  // returns the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::priority(
        const Val& elt) const {
    return  _heap_[ _indices_[elt]].first;
  }

  // returns the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::priorityByPos(
        Size index) const {
    if (index >  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }
    return  _heap_[index].first;
  }

  // ===========================================================================
  // ===          SCALAR OPTIMIZED IMPLEMENTATION OF PRIORITY QUEUES         ===
  // ===========================================================================

  // basic constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(Cmp compare, Size capacity) :
       _indices_(capacity >> 1, true, true),
       _cmp_(compare) {
     _heap_.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        std::initializer_list< std::pair< Val, Priority > > list) :
       _indices_(Size(list.size()) / 2, true, true) {
    // fill the queue
     _heap_.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
           from) :
       _heap_(from. _heap_),
       _indices_(from. _indices_),  _nb_elements_(from. _nb_elements_),
       _cmp_(from. _cmp_) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // generalized copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, true >&
           from) :
       _indices_(from. _indices_),
       _nb_elements_(from. _nb_elements_),  _cmp_(from. _cmp_) {
    // fill the heap structure
    if ( _nb_elements_) {
       _heap_.reserve(from. _heap_.size());
      for (const auto& elt: from. _heap_) {
         _heap_.push_back(elt);
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&& from) :
       _heap_(std::move(from. _heap_)),
       _indices_(std::move(from. _indices_)),
       _nb_elements_(std::move(from. _nb_elements_)),  _cmp_(std::move(from. _cmp_)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueueImplementation);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     ~PriorityQueueImplementation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
           from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(PriorityQueueImplementation);

      try {
        // set the comparison function
         _cmp_ = from. _cmp_;

        // copy the indices and the heap
         _indices_     = from. _indices_;
         _heap_        = from. _heap_;
         _nb_elements_ = from. _nb_elements_;
      } catch (...) {
         _heap_.clear();
         _indices_.clear();
         _nb_elements_ = 0;

        throw;
      }
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, true >&
           from) {
    // for debugging purposes
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
       _cmp_ = from. _cmp_;

      // copy the indices and the heap
       _indices_     = from. _indices_;
       _nb_elements_ = from. _nb_elements_;

       _heap_.clear();
      if ( _nb_elements_) {
         _heap_.reserve(from. _heap_.size());
        for (const auto& elt: from. _heap_) {
           _heap_.push_back(elt);
        }
      }
    } catch (...) {
       _heap_.clear();
       _indices_.clear();
       _nb_elements_ = 0;

      throw;
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(PriorityQueueImplementation);

       _indices_     = std::move(from. _indices_);
       _heap_        = std::move(from. _heap_);
       _cmp_         = std::move(from. _cmp_);
       _nb_elements_ = std::move(from. _nb_elements_);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::top() const {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return  _heap_[0].second;
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::topPriority()
        const {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return  _heap_[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::size()
        const noexcept {
    return  _nb_elements_;
  }

  // return the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::capacity()
        const noexcept {
    return Size( _heap_.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::resize(
        Size new_size) {
    if (new_size <  _nb_elements_) return;

     _heap_.reserve(new_size);
     _indices_.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::clear() {
     _nb_elements_ = 0;
     _heap_.clear();
     _indices_.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::eraseByPos(
     Size index) {
    if (index >=  _nb_elements_) return;

    // remove the element from the hashtable
     _indices_.erase( _heap_[index].second);

    // put the last element at the "index" location
    std::pair< Priority, Val > last = std::move( _heap_[ _nb_elements_ - 1]);
     _heap_.pop_back();
    -- _nb_elements_;

    if (! _nb_elements_ || (index ==  _nb_elements_)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if ( _cmp_(last.first,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // put "last" back into the heap
     _heap_[i]                   = std::move(last);
     _indices_[ _heap_[i].second] = i;
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::erase(
        Val val) {
    try {
      eraseByPos( _indices_[val]);
    } catch (NotFound&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Val
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::pop() {
    if (! _nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    Val v =  _heap_[0].second;
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const HashTable< Val, Size >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::allValues()
        const noexcept {
    return reinterpret_cast< const HashTable< Val, Size >& >( _indices_);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::insert(
        Val             val,
        const Priority& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    //  _indices_.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       =  _indices_.insert(val, 0);

    try {
       _heap_.push_back(std::pair< Priority, Val >(priority, val));
    } catch (...) {
       _indices_.erase(val);
      throw;
    }

    std::pair< Priority, Val > new_heap_val = std::move( _heap_[ _nb_elements_]);
    ++ _nb_elements_;

    // restore the heap property
    Size i =  _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_heap_val.first,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // put the new bucket into the heap
     _heap_[i].first  = std::move(new_heap_val.first);
     _heap_[i].second = val;
    new_elt.second   = i;

    return i;
  }

  // inserts by move a new element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::insert(
        Val        val,
        Priority&& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    //  _indices_.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       =  _indices_.insert(val, 0);

    try {
       _heap_.push_back(std::pair< Priority, Val >(std::move(priority), val));
    } catch (...) {
       _indices_.erase(val);
      throw;
    }

    std::pair< Priority, Val > new_heap_val = std::move( _heap_[ _nb_elements_]);
    ++ _nb_elements_;

    // restore the heap property
    Size i =  _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_heap_val.first,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // put the new bucket into the heap
     _heap_[i].first  = std::move(new_heap_val.first);
     _heap_[i].second = val;
    new_elt.second   = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename... Args >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::emplace(
        Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(new_elt.first, std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::empty()
        const noexcept {
    return ( _nb_elements_ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::contains(
        Val val) const {
    return  _indices_.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator[](
        Size index) const {
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    return  _heap_[index].second;
  }

  // displays the content of the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  std::string
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::toString()
        const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i !=  _nb_elements_; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << "(" <<  _heap_[i].first << " , " <<  _heap_[i].second << ")";
    }

    stream << "]";

    return stream.str();
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     setPriorityByPos(Size index, const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    Val val =  _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_priority,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( _cmp_(new_priority,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // update the index of val
     _heap_[i].first  = new_priority;
     _heap_[i].second = val;
     _indices_[val]   = i;

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     setPriorityByPos(Size index, Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >=  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    Val val =  _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i &&  _cmp_(new_priority,  _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j <  _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 <  _nb_elements_) &&  _cmp_( _heap_[j + 1].first,  _heap_[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( _cmp_(new_priority,  _heap_[j].first)) break;

      // else pull up the jth node
       _heap_[i]                   = std::move( _heap_[j]);
       _indices_[ _heap_[i].second] = i;
    }

    // update the index of val
     _heap_[i].first  = std::move(new_priority);
     _heap_[i].second = val;
     _indices_[val]   = i;

    return i;
  }

  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::setPriority(
     Val             elt,
     const Priority& new_priority) {
    setPriorityByPos( _indices_[elt], new_priority);
  }

  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::setPriority(
     Val        elt,
     Priority&& new_priority) {
    setPriorityByPos( _indices_[elt], std::move(new_priority));
  }

  // returns the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::priority(
        Val elt) const {
    return  _heap_[ _indices_[elt]].first;
  }

  // returns the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::priorityByPos(
        Size index) const {
    if (index >  _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }
    return  _heap_[index].first;
  }

  // ===========================================================================
  // ===                           PRIORITY QUEUES                           ===
  // ===========================================================================

  // basic constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(Cmp  cmp,
                                                                   Size capacity) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(cmp, capacity) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     std::initializer_list< std::pair< Val, Priority > > list) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation{list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     const PriorityQueue< Val, Priority, Cmp, Alloc >& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  // generalized copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     const PriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     PriorityQueue< Val, Priority, Cmp, Alloc >&& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueue);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::~PriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueue);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const PriorityQueue< Val, Priority, Cmp, Alloc >& from) {
    Implementation::operator=(from);
    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const PriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) {
    Implementation::operator=(from);
    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        PriorityQueue< Val, Priority, Cmp, Alloc >&& from) {
    Implementation::operator=(std::move(from));
    return *this;
  }

  // A \c << operator for priority queues
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE std::ostream&
         operator<<(std::ostream&                                     stream,
                const PriorityQueue< Val, Priority, Cmp, Alloc >& queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */
