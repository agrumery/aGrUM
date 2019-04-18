
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
 * @brief a cache for caching scores and independence tests results
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename ScoringCache< ALLOC >::allocator_type
       ScoringCache< ALLOC >::getAllocator() const {
      return *this;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::ScoringCache(
       const typename ScoringCache< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc) {
      GUM_CONSTRUCTOR(ScoringCache);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::ScoringCache(
       const ScoringCache< ALLOC >&                          from,
       const typename ScoringCache< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __scores(from.__scores) {
      GUM_CONS_CPY(ScoringCache);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::ScoringCache(const ScoringCache< ALLOC >& from) :
        ScoringCache< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::ScoringCache(
       ScoringCache< ALLOC >&&                               from,
       const typename ScoringCache< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        __scores(std::move(from.__scores)) {
      GUM_CONS_MOV(ScoringCache);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::ScoringCache(ScoringCache< ALLOC >&& from) :
        ScoringCache< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoringCache< ALLOC >* ScoringCache< ALLOC >::clone(
       const typename ScoringCache< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScoringCache< ALLOC > > allocator(alloc);
      ScoringCache< ALLOC >*         cache = allocator.allocate(1);
      try {
        allocator.construct(cache, *this, alloc);
      } catch (...) {
        allocator.deallocate(cache, 1);
        throw;
      }
      return cache;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    ScoringCache< ALLOC >* ScoringCache< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >::~ScoringCache() {
      GUM_DESTRUCTOR(ScoringCache);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >& ScoringCache< ALLOC >::
                                  operator=(const ScoringCache< ALLOC >& from) {
      if (&from != this) { __scores = from.__scores; }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE ScoringCache< ALLOC >& ScoringCache< ALLOC >::
                                  operator=(ScoringCache< ALLOC >&& from) {
      if (&from != this) { __scores = std::move(from.__scores); }
      return *this;
    }


    /// insert a new score into the cache
    template < template < typename > class ALLOC >
    INLINE void ScoringCache< ALLOC >::insert(const IdSet< ALLOC >& idset,
                                              double                score) {
      __scores.insert(idset, score);
    }


    /// insert a new score into the cache
    template < template < typename > class ALLOC >
    INLINE void ScoringCache< ALLOC >::insert(IdSet< ALLOC >&& idset,
                                              double           score) {
      __scores.insert(std::move(idset), std::move(score));
    }


    /// removes a score (if it exists)
    template < template < typename > class ALLOC >
    INLINE void ScoringCache< ALLOC >::erase(const IdSet< ALLOC >& idset) {
      __scores.erase(idset);
    }


    /// indicates whether a given score exists
    template < template < typename > class ALLOC >
    INLINE bool ScoringCache< ALLOC >::exists(const IdSet< ALLOC >& idset) {
      return __scores.exists(idset);
    }


    /// returns a given score
    template < template < typename > class ALLOC >
    INLINE double ScoringCache< ALLOC >::score(const IdSet< ALLOC >& idset) {
      return __scores[idset];
    }


    /// removes all the stored scores
    template < template < typename > class ALLOC >
    INLINE void ScoringCache< ALLOC >::clear() {
      __scores.clear();
    }


    /// returns the number of scores saved in the cache
    template < template < typename > class ALLOC >
    INLINE std::size_t ScoringCache< ALLOC >::size() const {
      return __scores.size();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
