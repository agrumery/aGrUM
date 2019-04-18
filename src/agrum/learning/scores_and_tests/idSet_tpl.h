
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >::IdSetIterator() {
      GUM_CONSTRUCTOR(IdSetIterator);
    }


    /// Constructor for a begin
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >::IdSetIterator(const IdSet< ALLOC >& idset) :
        __seq(&(idset.ids())) {
      GUM_CONSTRUCTOR(IdSetIterator);
    }


    /// Copy constructor.
    template < template < typename > class ALLOC >
    INLINE
       IdSetIterator< ALLOC >::IdSetIterator(const IdSetIterator< ALLOC >& from) :
        __seq(from.__seq),
        __index(from.__index) {
      GUM_CONS_CPY(IdSetIterator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >::IdSetIterator(IdSetIterator< ALLOC >&& from) :
        __seq(from.__seq), __index(from.__index) {
      GUM_CONS_MOV(IdSetIterator);
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >::~IdSetIterator() {
      GUM_DESTRUCTOR(IdSetIterator);
    }


    /// places the index to the end of the sequence
    template < template < typename > class ALLOC >
    INLINE void IdSetIterator< ALLOC >::__gotoEnd() {
      if (__seq != nullptr)
        __index = __seq->size();
      else
        __index = std::size_t(0);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >& IdSetIterator< ALLOC >::
                                   operator=(const IdSetIterator< ALLOC >& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >& IdSetIterator< ALLOC >::
                                   operator=(IdSetIterator< ALLOC >&& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// Gives access to the content of the iterator.
    template < template < typename > class ALLOC >
    INLINE NodeId IdSetIterator< ALLOC >::operator*() const {
      return __seq->operator[](__index);
    }


    /// Checks whether two iterators point toward different elements.
    template < template < typename > class ALLOC >
    INLINE bool IdSetIterator< ALLOC >::
                operator!=(const IdSetIterator< ALLOC >& from) const {
      return (__index != from.__index) || (__seq != from.__seq);
    }


    /// Checks whether two iterators point toward the same elements.
    template < template < typename > class ALLOC >
    INLINE bool IdSetIterator< ALLOC >::
                operator==(const IdSetIterator< ALLOC >& from) const {
      return !operator!=(from);
    }


    /// Makes the iterator point to the next element in the IdSet
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >& IdSetIterator< ALLOC >::operator++() {
      ++__index;
      return *this;
    }


    /// Makes the iterator point to i elements further in the IdSet
    template < template < typename > class ALLOC >
    INLINE IdSetIterator< ALLOC >& IdSetIterator< ALLOC >::
                                   operator+=(const std::size_t i) {
      __index += i;
      return *this;
    }


    /// Returns a new iterator pointing to i further elements in the IdSet
    template < template < typename > class ALLOC >
    IdSetIterator< ALLOC > IdSetIterator< ALLOC >::operator+(const std::size_t i) {
      IdSetIterator< ALLOC > res(*this);
      res += i;
      return res;
    }


    /// Returns the position of the iterator in the IdSet
    template < template < typename > class ALLOC >
    std::size_t IdSetIterator< ALLOC >::pos() const {
      if (__seq == nullptr)
        GUM_ERROR(UndefinedIteratorValue,
                  "The IdSet is empty, so its iterators have no position");
      if (__index >= __seq->size())
        GUM_ERROR(UndefinedIteratorValue,
                  "the IdSet iterator has no position because it reached "
                  "the set's end.");
      return __index;
    }


    /// ==========================================================================
    /// ==========================================================================


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename IdSet< ALLOC >::allocator_type
       IdSet< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       const typename IdSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __end_safe(*this) {
      GUM_CONSTRUCTOR(IdSet);
    }


    /// default constructor with no variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       const std::vector< NodeId, ALLOC< NodeId > >&  ids,
       const bool                                     rhs_ids,
       const bool                                     ordered_ids,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __end_safe(*this) {
      __ids.resize(ids.size());

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id : vect)
          __ids << id;
      } else {
        for (const auto id : ids)
          __ids << id;
      }

      if (!rhs_ids) __nb_lhs_ids = __ids.size();

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdSet);
    }


    /// default constructor with one variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       NodeId                                         var1,
       const std::vector< NodeId, ALLOC< NodeId > >&  rhs_ids,
       const bool                                     ordered_rhs_ids,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __nb_lhs_ids(std::size_t(1)), __end_safe(*this) {
      __ids.resize(rhs_ids.size() + std::size_t(1));
      __ids << var1;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId, ALLOC< NodeId > > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id : vect)
          __ids << id;
      } else {
        for (const auto id : rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdSet);
    }


    /// default constructor with two variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       NodeId                                         var1,
       NodeId                                         var2,
       const std::vector< NodeId, ALLOC< NodeId > >&  rhs_ids,
       const bool                                     ordered_lhs_vars,
       const bool                                     ordered_rhs_ids,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
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
        for (const auto id : vect)
          __ids << id;
      } else {
        for (const auto id : rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdSet);
    }


    /// default constructor with three variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       NodeId                                         var1,
       NodeId                                         var2,
       NodeId                                         var3,
       const std::vector< NodeId, ALLOC< NodeId > >&  rhs_ids,
       const bool                                     ordered_lhs_vars,
       const bool                                     ordered_rhs_ids,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
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
        for (const auto id : vect)
          __ids << id;
      } else {
        for (const auto id : rhs_ids)
          __ids << id;
      }

      // update the end iterator
      __end_safe.__gotoEnd();

      GUM_CONSTRUCTOR(IdSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       const IdSet< ALLOC >&                          from,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(from.__ids), __nb_lhs_ids(from.__nb_lhs_ids), __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_CPY(IdSet);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(const IdSet< ALLOC >& from) :
        IdSet< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(
       IdSet< ALLOC >&&                               from,
       const typename IdSet< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(std::move(from.__ids)), __nb_lhs_ids(from.__nb_lhs_ids),
        __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_MOV(IdSet);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::IdSet(IdSet< ALLOC >&& from) :
        IdSet< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    IdSet< ALLOC >* IdSet< ALLOC >::clone(
       const typename IdSet< ALLOC >::allocator_type& alloc) const {
      ALLOC< IdSet< ALLOC > > allocator(alloc);
      IdSet< ALLOC >*         new_set = allocator.allocate(1);
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
    IdSet< ALLOC >* IdSet< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >::~IdSet() {
      GUM_DESTRUCTOR(IdSet);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >& IdSet< ALLOC >::operator=(const IdSet< ALLOC >& from) {
      if (this != &from) {
        __ids = from.__ids;
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdSet< ALLOC >& IdSet< ALLOC >::operator=(IdSet< ALLOC >&& from) {
      if (this != &from) {
        __ids = std::move(from.__ids);
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// returns the id stored at a given index
    template < template < typename > class ALLOC >
    INLINE NodeId IdSet< ALLOC >::operator[](const std::size_t index) const {
      return __ids.atPos(index);
    }


    /// returns true if both sets are equal
    template < template < typename > class ALLOC >
    INLINE bool IdSet< ALLOC >::operator==(const IdSet< ALLOC >& from) const {
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
    INLINE bool IdSet< ALLOC >::operator!=(const IdSet< ALLOC >& from) const {
      return !operator==(from);
    }


    /// Returns a safe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdSet< ALLOC >::iterator_safe
       IdSet< ALLOC >::beginSafe() const {
      return IdSetIterator< ALLOC >(*this);
    }


    /// Returns the safe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdSet< ALLOC >::iterator_safe&
       IdSet< ALLOC >::endSafe() const {
      return __end_safe;
    }


    /// Returns an unsafe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdSet< ALLOC >::iterator IdSet< ALLOC >::begin() const {
      return IdSetIterator< ALLOC >(*this);
    }


    /// Returns the unsafe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdSet< ALLOC >::iterator& IdSet< ALLOC >::end() const {
      return __end_safe;
    }


    /// returns the set of ids contained in the object
    template < template < typename > class ALLOC >
    INLINE const Sequence< NodeId, ALLOC< NodeId > >& IdSet< ALLOC >::ids() const {
      return __ids;
    }


    /// returns the idSet at the right hand side of the conditioning bar
    template < template < typename > class ALLOC >
    IdSet< ALLOC > IdSet< ALLOC >::conditionalIdSet() const {
      IdSet< ALLOC >    set(this->getAllocator());
      const std::size_t size = __ids.size();
      for (std::size_t i = __nb_lhs_ids; i < size; ++i)
        set.__ids << __ids[i];
      set.__end_safe.__gotoEnd();
      return set;
    }


    /// erase a node in the idset
    template < template < typename > class ALLOC >
    void IdSet< ALLOC >::erase(const NodeId id) {
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
    std::string IdSet< ALLOC >::toString() const {
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
    INLINE std::size_t IdSet< ALLOC >::nbLHSIds() const {
      return __nb_lhs_ids;
    }


    /// returns the number of right hand side ids
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet< ALLOC >::nbRHSIds() const {
      return __ids.size() - __nb_lhs_ids;
    }


    /// indicates whether the IdSet contains the IdSet passed in argument
    template < template < typename > class ALLOC >
    bool IdSet< ALLOC >::contains(const IdSet< ALLOC >& set) const {
      if (set.__ids.size() > __ids.size()) return false;
      for (const auto node : set.__ids) {
        if (!__ids.exists(node)) return false;
      }
      return true;
    }


    /// removes all the nodes from the IdSet
    template < template < typename > class ALLOC >
    INLINE void IdSet< ALLOC >::clear() {
      __ids.clear();
      __nb_lhs_ids = std::size_t(0);
      __end_safe.__gotoEnd();
    }


    /// returns the number of variables (both left and right hand side)
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet< ALLOC >::size() const {
      return __ids.size();
    }


    /// returns the position of a given node in the IdSet
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet< ALLOC >::pos(const NodeId id) const {
      return __ids.pos(id);
    }


    /// indicates whether a given id is contained in the IdSet
    template < template < typename > class ALLOC >
    INLINE bool IdSet< ALLOC >::exists(const NodeId id) const {
      return __ids.exists(id);
    }


    /// indicates whether the idset contains a non-empty conditioning set
    template < template < typename > class ALLOC >
    INLINE bool IdSet< ALLOC >::hasConditioningSet() const {
      return __nb_lhs_ids != __ids.size();
    }


    /// indicates whether the IdSet contains some nodes or not
    template < template < typename > class ALLOC >
    INLINE bool IdSet< ALLOC >::empty() const {
      return __ids.empty();
    }


    // the display operator
    template < template < typename > class ALLOC >
    std::ostream& operator<<(std::ostream& stream, const IdSet< ALLOC >& idset) {
      return stream << idset.toString();
    }

  } /* namespace learning */


  // Returns the value of a key as a Size.
  template < template < typename > class ALLOC >
  Size HashFunc< learning::IdSet< ALLOC > >::castToSize(
     const learning::IdSet< ALLOC >& key) {
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
  INLINE Size HashFunc< learning::IdSet< ALLOC > >::
              operator()(const learning::IdSet< ALLOC >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->_hash_mask;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
