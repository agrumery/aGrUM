
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


/** @file
 * @brief Template implementation of idSets
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >::IdCondSetIterator() {
      GUM_CONSTRUCTOR(IdCondSetIterator);
    }


    /// Constructor for a begin
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >::IdCondSetIterator(
       const IdCondSet< ALLOC >& idset) :
        __seq(&(idset.ids())) {
      GUM_CONSTRUCTOR(IdCondSetIterator);
    }


    /// Copy constructor.
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >::IdCondSetIterator(
       const IdCondSetIterator< ALLOC >& from) :
        __seq(from.__seq),
        __index(from.__index) {
      GUM_CONS_CPY(IdCondSetIterator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >::IdCondSetIterator(
       IdCondSetIterator< ALLOC >&& from) :
        __seq(from.__seq),
        __index(from.__index) {
      GUM_CONS_MOV(IdCondSetIterator);
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >::~IdCondSetIterator() {
      GUM_DESTRUCTOR(IdCondSetIterator);
    }


    /// places the index to the end of the sequence
    template < template < typename > class ALLOC >
    INLINE void IdCondSetIterator< ALLOC >::__gotoEnd() {
      if (__seq != nullptr)
        __index = __seq->size();
      else
        __index = std::size_t(0);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >& IdCondSetIterator< ALLOC >::operator=(
       const IdCondSetIterator< ALLOC >& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >&
       IdCondSetIterator< ALLOC >::operator=(IdCondSetIterator< ALLOC >&& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// Gives access to the content of the iterator.
    template < template < typename > class ALLOC >
    INLINE NodeId IdCondSetIterator< ALLOC >::operator*() const {
      return __seq->operator[](__index);
    }


    /// Checks whether two iterators point toward different elements.
    template < template < typename > class ALLOC >
    INLINE bool IdCondSetIterator< ALLOC >::operator!=(
       const IdCondSetIterator< ALLOC >& from) const {
      return (__index != from.__index) || (__seq != from.__seq);
    }


    /// Checks whether two iterators point toward the same elements.
    template < template < typename > class ALLOC >
    INLINE bool IdCondSetIterator< ALLOC >::operator==(
       const IdCondSetIterator< ALLOC >& from) const {
      return !operator!=(from);
    }


    /// Makes the iterator point to the next element in the IdCondSet
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >& IdCondSetIterator< ALLOC >::operator++() {
      ++__index;
      return *this;
    }


    /// Makes the iterator point to i elements further in the IdCondSet
    template < template < typename > class ALLOC >
    INLINE IdCondSetIterator< ALLOC >&
       IdCondSetIterator< ALLOC >::operator+=(const std::size_t i) {
      __index += i;
      return *this;
    }


    /// Returns a new iterator pointing to i further elements in the IdCondSet
    template < template < typename > class ALLOC >
    IdCondSetIterator< ALLOC >
       IdCondSetIterator< ALLOC >::operator+(const std::size_t i) {
      IdCondSetIterator< ALLOC > res(*this);
      res += i;
      return res;
    }


    /// Returns the position of the iterator in the IdCondSet
    template < template < typename > class ALLOC >
    std::size_t IdCondSetIterator< ALLOC >::pos() const {
      if (__seq == nullptr)
        GUM_ERROR(UndefinedIteratorValue,
                  "The IdCondSet is empty, so its iterators have no position");
      if (__index >= __seq->size())
        GUM_ERROR(UndefinedIteratorValue,
                  "the IdCondSet iterator has no position because it reached "
                  "the set's end.");
      return __index;
    }


    /// ==========================================================================
    /// ==========================================================================


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename IdCondSet< ALLOC >::allocator_type
       IdCondSet< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __end_safe(*this) {
      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with no variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       const std::vector< NodeId, ALLOC< NodeId > >&      ids,
       const bool                                         rhs_ids,
       const bool                                         ordered_ids,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __end_safe(*this) {
      __ids.resize(ids.size());

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          __ids << id;
      } else {
        for (const auto id: ids)
          __ids << id;
      }

      if (!rhs_ids) __nb_lhs_ids = __ids.size();

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with one variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       NodeId                                             var1,
       const std::vector< NodeId, ALLOC< NodeId > >&      rhs_ids,
       const bool                                         ordered_rhs_ids,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __nb_lhs_ids(std::size_t(1)), __end_safe(*this) {
      __ids.resize(rhs_ids.size() + std::size_t(1));
      __ids << var1;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          __ids << id;
      } else {
        for (const auto id: rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with two variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       NodeId                                             var1,
       NodeId                                             var2,
       const std::vector< NodeId, ALLOC< NodeId > >&      rhs_ids,
       const bool                                         ordered_lhs_vars,
       const bool                                         ordered_rhs_ids,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __nb_lhs_ids(std::size_t(2)), __end_safe(*this) {
      __ids.resize(rhs_ids.size() + std::size_t(2));

      // if the variables on the left side are unordered, sort them by
      // increasing order
      if (!ordered_lhs_vars && (var1 > var2)) std::swap(var1, var2);
      __ids << var1;
      __ids << var2;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          __ids << id;
      } else {
        for (const auto id: rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with three variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       NodeId                                             var1,
       NodeId                                             var2,
       NodeId                                             var3,
       const std::vector< NodeId, ALLOC< NodeId > >&      rhs_ids,
       const bool                                         ordered_lhs_vars,
       const bool                                         ordered_rhs_ids,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __nb_lhs_ids(std::size_t(3)), __end_safe(*this) {
      __ids.resize(rhs_ids.size() + std::size_t(3));

      // if the variables on the left side are unordered, sort them by
      // increasing order
      if (!ordered_lhs_vars) {
        if (var1 > var2) std::swap(var1, var2);
        if (var1 > var3) std::swap(var1, var3);
        if (var2 > var3) std::swap(var2, var3);
      }
      __ids << var1;
      __ids << var2;
      __ids << var3;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          __ids << id;
      } else {
        for (const auto id: rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       const IdCondSet< ALLOC >&                          from,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(from.__ids), __nb_lhs_ids(from.__nb_lhs_ids), __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_CPY(IdCondSet);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(const IdCondSet< ALLOC >& from) :
        IdCondSet< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(
       IdCondSet< ALLOC >&&                               from,
       const typename IdCondSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(std::move(from.__ids)), __nb_lhs_ids(from.__nb_lhs_ids),
        __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_MOV(IdCondSet);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::IdCondSet(IdCondSet< ALLOC >&& from) :
        IdCondSet< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    IdCondSet< ALLOC >* IdCondSet< ALLOC >::clone(
       const typename IdCondSet< ALLOC >::allocator_type& alloc) const {
      ALLOC< IdCondSet< ALLOC > > allocator(alloc);
      IdCondSet< ALLOC >*         new_set = allocator.allocate(1);
      try {
        allocator.construct(new_set, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_set, 1);
        throw;
      }

      return new_set;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    IdCondSet< ALLOC >* IdCondSet< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >::~IdCondSet() {
      GUM_DESTRUCTOR(IdCondSet);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >&
       IdCondSet< ALLOC >::operator=(const IdCondSet< ALLOC >& from) {
      if (this != &from) {
        __ids = from.__ids;
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdCondSet< ALLOC >&
       IdCondSet< ALLOC >::operator=(IdCondSet< ALLOC >&& from) {
      if (this != &from) {
        __ids = std::move(from.__ids);
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// returns the id stored at a given index
    template < template < typename > class ALLOC >
    INLINE NodeId IdCondSet< ALLOC >::operator[](const std::size_t index) const {
      return __ids.atPos(index);
    }


    /// returns true if both sets are equal
    template < template < typename > class ALLOC >
    INLINE bool
       IdCondSet< ALLOC >::operator==(const IdCondSet< ALLOC >& from) const {
      if (__nb_lhs_ids != from.__nb_lhs_ids) return false;

      const std::size_t size = __ids.size();

      if (size != from.__ids.size()) return false;

      for (std::size_t i = std::size_t(0); i < size; ++i) {
        if (__ids[i] != from.__ids[i]) return false;
      }

      return true;
    }


    /// returns true if the sets differ
    template < template < typename > class ALLOC >
    INLINE bool
       IdCondSet< ALLOC >::operator!=(const IdCondSet< ALLOC >& from) const {
      return !operator==(from);
    }


    /// Returns a safe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdCondSet< ALLOC >::iterator_safe
       IdCondSet< ALLOC >::beginSafe() const {
      return IdCondSetIterator< ALLOC >(*this);
    }


    /// Returns the safe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdCondSet< ALLOC >::iterator_safe&
       IdCondSet< ALLOC >::endSafe() const {
      return __end_safe;
    }


    /// Returns an unsafe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdCondSet< ALLOC >::iterator
       IdCondSet< ALLOC >::begin() const {
      return IdCondSetIterator< ALLOC >(*this);
    }


    /// Returns the unsafe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdCondSet< ALLOC >::iterator&
       IdCondSet< ALLOC >::end() const {
      return __end_safe;
    }


    /// returns the set of ids contained in the object
    template < template < typename > class ALLOC >
    INLINE const Sequence< NodeId, ALLOC< NodeId > >&
                 IdCondSet< ALLOC >::ids() const {
      return __ids;
    }


    /// returns the idSet at the right hand side of the conditioning bar
    template < template < typename > class ALLOC >
    IdCondSet< ALLOC > IdCondSet< ALLOC >::conditionalIdCondSet() const {
      IdCondSet< ALLOC > set(this->getAllocator());
      const std::size_t  size = __ids.size();
      for (std::size_t i = __nb_lhs_ids; i < size; ++i)
        set.__ids << __ids[i];
      set.__end_safe.__gotoEnd();
      return set;
    }


    /// erase a node in the idset
    template < template < typename > class ALLOC >
    void IdCondSet< ALLOC >::erase(const NodeId id) {
      // search for id in Sequence __ids
      const std::size_t size = __ids.size();
      std::size_t       pos = std::size_t(0);
      for (; pos < size; ++pos) {
        if (__ids[pos] == id) break;
      }

      // if we found the id, remove it
      if (pos < size) {
        __ids.erase(SequenceIteratorSafe< NodeId >(__ids, pos));
        if (pos < __nb_lhs_ids) --__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
    }


    /// returns the content of the set as a string
    template < template < typename > class ALLOC >
    std::string IdCondSet< ALLOC >::toString() const {
      std::stringstream str;

      str << '{';
      bool deja = false;

      for (std::size_t i = std::size_t(0); i < __nb_lhs_ids; ++i) {
        if (deja)
          str << " , ";
        else
          deja = true;
        str << __ids[i];
      }

      deja = false;
      for (auto iter = __ids.begin() + __nb_lhs_ids; iter != __ids.end(); ++iter) {
        if (deja)
          str << " , ";
        else {
          deja = true;
          str << " | ";
        }
        str << *iter;
      }

      str << '}';

      return str.str();
    }


    /// returns the number of left hand side ids
    template < template < typename > class ALLOC >
    INLINE std::size_t IdCondSet< ALLOC >::nbLHSIds() const {
      return __nb_lhs_ids;
    }


    /// returns the number of right hand side ids
    template < template < typename > class ALLOC >
    INLINE std::size_t IdCondSet< ALLOC >::nbRHSIds() const {
      return __ids.size() - __nb_lhs_ids;
    }


    /// indicates whether the IdCondSet contains the IdCondSet passed in argument
    template < template < typename > class ALLOC >
    bool IdCondSet< ALLOC >::contains(const IdCondSet< ALLOC >& set) const {
      if (set.__ids.size() > __ids.size()) return false;
      for (const auto node: set.__ids) {
        if (!__ids.exists(node)) return false;
      }
      return true;
    }


    /// removes all the nodes from the IdCondSet
    template < template < typename > class ALLOC >
    INLINE void IdCondSet< ALLOC >::clear() {
      __ids.clear();
      __nb_lhs_ids = std::size_t(0);
      __end_safe.__gotoEnd();
    }


    /// returns the number of variables (both left and right hand side)
    template < template < typename > class ALLOC >
    INLINE std::size_t IdCondSet< ALLOC >::size() const {
      return __ids.size();
    }


    /// returns the position of a given node in the IdCondSet
    template < template < typename > class ALLOC >
    INLINE std::size_t IdCondSet< ALLOC >::pos(const NodeId id) const {
      return __ids.pos(id);
    }


    /// indicates whether a given id is contained in the IdCondSet
    template < template < typename > class ALLOC >
    INLINE bool IdCondSet< ALLOC >::exists(const NodeId id) const {
      return __ids.exists(id);
    }


    /// indicates whether the idset contains a non-empty conditioning set
    template < template < typename > class ALLOC >
    INLINE bool IdCondSet< ALLOC >::hasConditioningSet() const {
      return __nb_lhs_ids != __ids.size();
    }


    /// indicates whether the IdCondSet contains some nodes or not
    template < template < typename > class ALLOC >
    INLINE bool IdCondSet< ALLOC >::empty() const {
      return __ids.empty();
    }


    // the display operator
    template < template < typename > class ALLOC >
    std::ostream& operator<<(std::ostream&             stream,
                             const IdCondSet< ALLOC >& idset) {
      return stream << idset.toString();
    }

  } /* namespace learning */


  // Returns the value of a key as a Size.
  template < template < typename > class ALLOC >
  Size HashFunc< learning::IdCondSet< ALLOC > >::castToSize(
     const learning::IdCondSet< ALLOC >& key) {
    Size                                       h = Size(key.nbLHSIds());
    const Sequence< NodeId, ALLOC< NodeId > >& vect = key.ids();
    const std::size_t                          size = vect.size();

    std::size_t i = std::size_t(0);
    while (i < size) {
      const Size id = Size(vect[i]);
      ++i;
      h += Size(i) * id;
    }

    return h;
  }


  // the hash function for idSets
  template < template < typename > class ALLOC >
  INLINE Size HashFunc< learning::IdCondSet< ALLOC > >::operator()(
     const learning::IdCondSet< ALLOC >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->_hash_mask;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */