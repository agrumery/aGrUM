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
/**
 * @file
 * @brief Implementation of gum::learning::Entropy.
 *
 * @author Quentin FALCAND.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < typename IdSetAlloc, typename CountAlloc >
    template < typename RowFilter >
    INLINE PartialEntropy< IdSetAlloc, CountAlloc >::PartialEntropy(
      const RowFilter&           filter,
      const std::vector< Size >& var_modalities,
      Size                       min_range,
      Size                       max_range)
        : Counter< IdSetAlloc, CountAlloc >(
            filter, var_modalities, min_range, max_range) {
      GUM_CONSTRUCTOR(PartialEntropy);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    INLINE PartialEntropy< IdSetAlloc, CountAlloc >::~PartialEntropy() {
      // for debugging purposes
      GUM_DESTRUCTOR(PartialEntropy);
    }

    /// add a new target variable plus some conditioning vars
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE Idx PartialEntropy< IdSetAlloc, CountAlloc >::addNodeSet(
      const std::vector< Idx >& var_set) {
      // If using cache, verify if the set isn't already known
      if (__use_cache) {
        try {
          double score = __cache.score(var_set);
          __is_cached_score.push_back(true);
          __cached_score.push_back(score);
          return Counter< IdSetAlloc, CountAlloc >::addEmptyNodeSet();
        } catch (const NotFound&) {
        }
      }

      // else, add the set
      __is_cached_score.push_back(false);
      __cached_score.push_back(0);

      if (var_set.size() == 1) {

        return Counter< IdSetAlloc, CountAlloc >::addNodeSet(var_set[0]);

      } else if (var_set.size() > 1) {
        Idx                var = var_set[0];
        std::vector< Idx > var_set_bis = var_set;
        var_set_bis.erase(var_set_bis.begin());

        return Counter< IdSetAlloc, CountAlloc >::addNodeSet(var, var_set_bis);

      } else {
        return Counter< IdSetAlloc, CountAlloc >::addEmptyNodeSet();
      }
    }


    template < typename IdSetAlloc, typename CountAlloc >
    INLINE Idx PartialEntropy< IdSetAlloc, CountAlloc >::addNodeSet(Idx var1,
                                                                    Idx var2) {

      std::vector< Idx > vars = {var1, var2};

      return PartialEntropy< IdSetAlloc, CountAlloc >::addNodeSet(vars);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    INLINE Idx PartialEntropy< IdSetAlloc, CountAlloc >::addNodeSet(Idx var) {

      std::vector< Idx > vars = {var};

      return PartialEntropy< IdSetAlloc, CountAlloc >::addNodeSet(vars);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double PartialEntropy< IdSetAlloc, CountAlloc >::score(Idx nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // get the counts for all the targets and for the conditioning nodes
      const std::vector< double, CountAlloc >& Nzyx =
        this->_getAllCounts(nodeset_index);

      const auto ZXY_size = Size(Nzyx.size());
      double     score = 0.0;
      // count N
      if (this->__N == 0.0) {
        for (Idx i = 0; i < ZXY_size; ++i) {
          this->__N += Nzyx[i];
        }
      }

      for (Idx zyx = 0; zyx < ZXY_size; ++zyx) {
        if (Nzyx[zyx]) {
          // score -= Nzyx[zyx] / this->__N * log2(Nzyx[zyx] / this->__N);
          score -= Nzyx[zyx] / this->__N * log(Nzyx[zyx] / this->__N);
        }
      }

      // shall we put the score into the cache?
      if (this->_isUsingCache()) {
        this->_insertIntoCache(nodeset_index, score);
      }

      return score;
    }

    /// clears all the data structures from memory
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void PartialEntropy< IdSetAlloc, CountAlloc >::clear() {
      Counter< IdSetAlloc, CountAlloc >::clear();
      __is_cached_score.clear();
      __cached_score.clear();
    }

    /// indicates whether a score belongs to the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE bool
    PartialEntropy< IdSetAlloc, CountAlloc >::_isInCache(Idx nodeset_index) const
      noexcept {
      return ((nodeset_index < __is_cached_score.size()) &&
              __is_cached_score[nodeset_index]);
    }

    /// inserts a new score into the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    PartialEntropy< IdSetAlloc, CountAlloc >::_insertIntoCache(Idx nodeset_index,
                                                               double score) {
      const std::vector< Idx, IdSetAlloc >& all_nodes =
        _getAllNodes(nodeset_index);
      try {
        __cache.insert(all_nodes, score);
      } catch (const gum::DuplicateElement&) {
      }
    }

    /// returns a cached score
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE double
    PartialEntropy< IdSetAlloc, CountAlloc >::_cachedScore(Idx nodeset_index) const
      noexcept {
      return __cached_score[nodeset_index];
    }

    /// indicates whether we use the cache or not
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE bool PartialEntropy< IdSetAlloc, CountAlloc >::_isUsingCache() const
      noexcept {
      return __use_cache;
    }

    /// turn on/off the use of a cache of the previously computed score
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    PartialEntropy< IdSetAlloc, CountAlloc >::useCache(bool on_off) noexcept {
      if (!on_off) clear();
      __use_cache = on_off;
    }

    /// clears the current cache (clear nodesets as well)
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void PartialEntropy< IdSetAlloc, CountAlloc >::clearCache() {
      clear();
      __cache.clear();
    }

    /// sets the range of records taken into account by the counter
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    PartialEntropy< IdSetAlloc, CountAlloc >::setRange(Size min_range,
                                                       Size max_range) {
      Counter< IdSetAlloc, CountAlloc >::setRange(min_range, max_range);
    }

    /// returns the size of the database
    template < typename IdSetAlloc, typename CountAlloc >
    const Size PartialEntropy< IdSetAlloc, CountAlloc >::N() {
      if (this->__N != 0) {
        return this->__N;
      }

      Idx idN = this->addNodeSet(0);
      const std::vector< double, CountAlloc >& Nzyx = this->_getAllCounts(idN);
      for (Idx i = 0; i < Nzyx.size(); ++i) {
        this->__N += Nzyx[i];
      }
      return this->__N;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
