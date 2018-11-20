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
 * @brief Template implementation of idSets
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >::IdSet2Iterator() {
      GUM_CONSTRUCTOR(IdSet2Iterator);
    }


    /// Constructor for a begin
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >::IdSet2Iterator(const IdSet2< ALLOC >& idset) :
        __seq(&(idset.ids())) {
      GUM_CONSTRUCTOR(IdSet2Iterator);
    }


    /// Copy constructor.
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >::IdSet2Iterator(
      const IdSet2Iterator< ALLOC >& from) :
        __seq(from.__seq),
        __index(from.__index) {
      GUM_CONS_CPY(IdSet2Iterator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE
      IdSet2Iterator< ALLOC >::IdSet2Iterator(IdSet2Iterator< ALLOC >&& from) :
        __seq(from.__seq),
        __index(from.__index) {
      GUM_CONS_MOV(IdSet2Iterator);
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >::~IdSet2Iterator() {
      GUM_DESTRUCTOR(IdSet2Iterator);
    }


    /// places the index to the end of the sequence
    template < template < typename > class ALLOC >
    INLINE void IdSet2Iterator< ALLOC >::__gotoEnd() {
      if (__seq != nullptr)
        __index = __seq->size();
      else
        __index = std::size_t(0);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >& IdSet2Iterator< ALLOC >::
                                    operator=(const IdSet2Iterator< ALLOC >& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >& IdSet2Iterator< ALLOC >::
                                    operator=(IdSet2Iterator< ALLOC >&& from) {
      __seq = from.__seq;
      __index = from.__index;
      return *this;
    }


    /// Gives access to the content of the iterator.
    template < template < typename > class ALLOC >
    INLINE NodeId IdSet2Iterator< ALLOC >::operator*() const {
      return __seq->operator[](__index);
    }


    /// Checks whether two iterators point toward different elements.
    template < template < typename > class ALLOC >
    INLINE bool IdSet2Iterator< ALLOC >::
                operator!=(const IdSet2Iterator< ALLOC >& from) const {
      return (__index != from.__index) || (__seq != from.__seq);
    }


    /// Checks whether two iterators point toward the same elements.
    template < template < typename > class ALLOC >
    INLINE bool IdSet2Iterator< ALLOC >::
                operator==(const IdSet2Iterator< ALLOC >& from) const {
      return !operator!=(from);
    }


    /// Makes the iterator point to the next element in the IdSet
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >& IdSet2Iterator< ALLOC >::operator++() {
      ++__index;
      return *this;
    }


    /// Makes the iterator point to i elements further in the IdSet
    template < template < typename > class ALLOC >
    INLINE IdSet2Iterator< ALLOC >& IdSet2Iterator< ALLOC >::
                                    operator+=(const std::size_t i) {
      __index += i;
      return *this;
    }


    /// Returns a new iterator pointing to i further elements in the IdSet
    template < template < typename > class ALLOC >
    IdSet2Iterator< ALLOC > IdSet2Iterator< ALLOC >::
                            operator+(const std::size_t i) {
      IdSet2Iterator< ALLOC > res(*this);
      res += i;
      return res;
    }


    /// Returns the position of the iterator in the IdSet2
    template < template < typename > class ALLOC >
    std::size_t IdSet2Iterator< ALLOC >::pos() const {
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
    INLINE typename IdSet2< ALLOC >::allocator_type
      IdSet2< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __end_safe(*this) {
      GUM_CONSTRUCTOR(IdSet2);
    }


    /// default constructor with no variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      const std::vector< NodeId, ALLOC< NodeId > >&   ids,
      const bool                                      rhs_ids,
      const bool                                      ordered_ids,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
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

      GUM_CONSTRUCTOR(IdSet2);
    }


    /// default constructor with one variable on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      NodeId                                          var1,
      const std::vector< NodeId, ALLOC< NodeId > >&   rhs_ids,
      const bool                                      ordered_rhs_ids,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
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

      GUM_CONSTRUCTOR(IdSet2);
    }


    /// default constructor with two variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      NodeId                                          var1,
      NodeId                                          var2,
      const std::vector< NodeId, ALLOC< NodeId > >&   rhs_ids,
      const bool                                      ordered_lhs_vars,
      const bool                                      ordered_rhs_ids,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
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

      GUM_CONSTRUCTOR(IdSet2);
    }


    /// default constructor with three variables on the left side
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      NodeId                                          var1,
      NodeId                                          var2,
      NodeId                                          var3,
      const std::vector< NodeId, ALLOC< NodeId > >&   rhs_ids,
      const bool                                      ordered_lhs_vars,
      const bool                                      ordered_rhs_ids,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
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

      GUM_CONSTRUCTOR(IdSet2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      const IdSet2< ALLOC >&                          from,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(from.__ids), __nb_lhs_ids(from.__nb_lhs_ids), __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_CPY(IdSet2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(const IdSet2< ALLOC >& from) :
        IdSet2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(
      IdSet2< ALLOC >&&                               from,
      const typename IdSet2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __ids(std::move(from.__ids)), __nb_lhs_ids(from.__nb_lhs_ids),
        __end_safe(*this) {
      __end_safe.__gotoEnd();
      GUM_CONS_MOV(IdSet2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::IdSet2(IdSet2< ALLOC >&& from) :
        IdSet2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    IdSet2< ALLOC >* IdSet2< ALLOC >::clone(
      const typename IdSet2< ALLOC >::allocator_type& alloc) const {
      ALLOC< IdSet2< ALLOC > > allocator(alloc);
      IdSet2< ALLOC >*         new_set = allocator.allocate(1);
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
    IdSet2< ALLOC >* IdSet2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >::~IdSet2() {
      GUM_DESTRUCTOR(IdSet2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >& IdSet2< ALLOC >::
                            operator=(const IdSet2< ALLOC >& from) {
      if (this != &from) {
        __ids = from.__ids;
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE IdSet2< ALLOC >& IdSet2< ALLOC >::operator=(IdSet2< ALLOC >&& from) {
      if (this != &from) {
        __ids = std::move(from.__ids);
        __nb_lhs_ids = from.__nb_lhs_ids;
        __end_safe.__gotoEnd();
      }
      return *this;
    }


    /// returns the id stored at a given index
    template < template < typename > class ALLOC >
    INLINE NodeId IdSet2< ALLOC >::operator[](const std::size_t index) const {
      return __ids.atPos(index);
    }


    /// returns true if both sets are equal
    template < template < typename > class ALLOC >
    INLINE bool IdSet2< ALLOC >::operator==(const IdSet2< ALLOC >& from) const {
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
    INLINE bool IdSet2< ALLOC >::operator!=(const IdSet2< ALLOC >& from) const {
      return !operator==(from);
    }


    /// Returns a safe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdSet2< ALLOC >::iterator_safe
      IdSet2< ALLOC >::beginSafe() const {
      return IdSet2Iterator< ALLOC >(*this);
    }


    /// Returns the safe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdSet2< ALLOC >::iterator_safe&
      IdSet2< ALLOC >::endSafe() const {
      return __end_safe;
    }


    /// Returns an unsafe begin iterator.
    template < template < typename > class ALLOC >
    INLINE typename IdSet2< ALLOC >::iterator IdSet2< ALLOC >::begin() const {
      return IdSet2Iterator< ALLOC >(*this);
    }


    /// Returns the unsafe end iterator.
    template < template < typename > class ALLOC >
    INLINE const typename IdSet2< ALLOC >::iterator& IdSet2< ALLOC >::end() const {
      return __end_safe;
    }


    /// returns the set of ids contained in the object
    template < template < typename > class ALLOC >
    INLINE const Sequence< NodeId, ALLOC< NodeId > >&
                 IdSet2< ALLOC >::ids() const {
      return __ids;
    }


    /// returns the idSet at the right hand side of the conditioning bar
    template < template < typename > class ALLOC >
    IdSet2< ALLOC > IdSet2< ALLOC >::conditionalIdSet() const {
      IdSet2< ALLOC >   set(this->getAllocator());
      const std::size_t size = __ids.size();
      for (std::size_t i = __nb_lhs_ids; i < size; ++i)
        set.__ids << __ids[i];
      set.__end_safe.__gotoEnd();
      return set;
    }


    /// erase a node in the idset
    template < template < typename > class ALLOC >
    void IdSet2< ALLOC >::erase(const NodeId id) {
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
    std::string IdSet2< ALLOC >::toString() const {
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
    INLINE std::size_t IdSet2< ALLOC >::nbLHSIds() const {
      return __nb_lhs_ids;
    }


    /// returns the number of right hand side ids
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet2< ALLOC >::nbRHSIds() const {
      return __ids.size() - __nb_lhs_ids;
    }


    /// indicates whether the IdSet contains the IdSet passed in argument
    template < template < typename > class ALLOC >
    bool IdSet2< ALLOC >::contains(const IdSet2< ALLOC >& set) const {
      if (set.__ids.size() > __ids.size()) return false;
      for (const auto node : set.__ids) {
        if (!__ids.exists(node)) return false;
      }
      return true;
    }


    /// removes all the nodes from the IdSet
    template < template < typename > class ALLOC >
    INLINE void IdSet2< ALLOC >::clear() {
      __ids.clear();
      __nb_lhs_ids = std::size_t(0);
      __end_safe.__gotoEnd();
    }


    /// returns the number of variables (both left and right hand side)
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet2< ALLOC >::size() const {
      return __ids.size();
    }


    /// returns the position of a given node in the IdSet
    template < template < typename > class ALLOC >
    INLINE std::size_t IdSet2< ALLOC >::pos(const NodeId id) const {
      return __ids.pos(id);
    }


    /// indicates whether a given id is contained in the IdSet
    template < template < typename > class ALLOC >
    INLINE bool IdSet2< ALLOC >::exists(const NodeId id) const {
      return __ids.exists(id);
    }


    /// indicates whether the idset contains a non-empty conditioning set
    template < template < typename > class ALLOC >
    INLINE bool IdSet2< ALLOC >::hasConditioningSet() const {
      return __nb_lhs_ids != __ids.size();
    }


    /// indicates whether the IdSet contains some nodes or not
    template < template < typename > class ALLOC >
    INLINE bool IdSet2< ALLOC >::empty() const {
      return __ids.empty();
    }


    // the display operator
    template < template < typename > class ALLOC >
    std::ostream& operator<<(std::ostream& stream, const IdSet2< ALLOC >& idset) {
      return stream << idset.toString();
    }

  } /* namespace learning */


  // the hash function for idSets
  template < template < typename > class ALLOC >
  Size HashFunc< learning::IdSet2< ALLOC > >::
       operator()(const learning::IdSet2< ALLOC >& key) const {
    Size                                       h = Size(key.nbLHSIds());
    const Sequence< NodeId, ALLOC< NodeId > >& vect = key.ids();
    const std::size_t                          size = vect.size();

    std::size_t i = std::size_t(0);
    while (i < size) {
      Size id = Size(vect[i]);
      ++i;
      h += Size(i) * id;
    }

    return ((h * HashFuncConst::gold) & this->_hash_mask);
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
