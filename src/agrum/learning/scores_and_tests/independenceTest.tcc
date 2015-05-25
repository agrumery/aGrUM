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
 * @brief the abstract class for all the independence tests
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE IndependenceTest<IdSetAlloc, CountAlloc>::IndependenceTest(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities)
        : Counter<IdSetAlloc, CountAlloc>(filter, var_modalities) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE IndependenceTest<IdSetAlloc, CountAlloc>::~IndependenceTest() {
      GUM_DESTRUCTOR(IndependenceTest);
    }

    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int
    IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(unsigned int var1,
                                                         unsigned int var2) {
      if (__use_cache) {
        // check whether the score is already in the cache
        try {
          float score = __cache.score(var1, var2, __empty_conditioning_set);
          __is_cached_score.push_back(true);
          __cached_score.push_back(score);
          return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
        } catch (const NotFound &) {
        }
      }

      // check if the number of parameters is not too high compared to the
      // size of the database (basically, if there are fewer than an average
      // of 5 observations per parameter in the database, the independence
      // test will be incorrect)
      if (this->_modalities[var1] * this->_modalities[var2] * 5 >
          this->_record_counter.DBSize()) {
        __is_cached_score.push_back(true);
        __cached_score.push_back(std::numeric_limits<float>::max());
        return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
      }

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      const unsigned int index =
          Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1, var2);

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1);
      return index;
    }

    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<unsigned int, unsigned int> &vars) {
      return addNodeSet(vars.first, vars.second);
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(
        unsigned int var1, unsigned int var2,
        const std::vector<unsigned int> &conditioning_ids) {
      if (__use_cache) {
        try {
          float score = __cache.score(var1, var2, conditioning_ids);
          __is_cached_score.push_back(true);
          __cached_score.push_back(score);
          return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
        } catch (const NotFound &) {
        }
      }

      // check if the number of parameters is not too high compared to the
      // size of the database (basically, if there are fewer than an average
      // of 5 observations per parameter in the database, the independence
      // test will be incorrect)
      unsigned long cpt_size = this->_modalities[var1] * this->_modalities[var2] * 5;
      for (auto node : conditioning_ids) {
        cpt_size *= this->_modalities[node];
      }
      if (cpt_size > this->_record_counter.DBSize()) {
        __is_cached_score.push_back(true);
        __cached_score.push_back(std::numeric_limits<float>::max());
        return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
      }

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      const unsigned int index =
          Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1, var2, conditioning_ids);

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1, conditioning_ids);
      return index;
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<unsigned int, unsigned int> &vars,
        const std::vector<unsigned int> &conditioning_ids) {
      return addNodeSet(vars.first, vars.second, conditioning_ids);
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(
        unsigned int var1, unsigned int var2,
        std::vector<unsigned int> &&conditioning_ids) {
      if (__use_cache) {
        try {
          float score = __cache.score(var1, var2, conditioning_ids);
          __is_cached_score.push_back(true);
          __cached_score.push_back(score);
          return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
        } catch (const NotFound &) {
        }
      }

      // check if the number of parameters is not too high compared to the
      // size of the database (basically, if there are fewer than an average
      // of 5 observations per parameter in the database, the independence
      // test will be incorrect)
      unsigned long cpt_size = this->_modalities[var1] * this->_modalities[var2] * 5;
      for (auto node : conditioning_ids) {
        cpt_size *= this->_modalities[node];
      }
      if (cpt_size > this->_record_counter.DBSize()) {
        __is_cached_score.push_back(true);
        __cached_score.push_back(std::numeric_limits<float>::max());
        return Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
      }

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      const unsigned int index =
          Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1, var2, conditioning_ids);

      __is_cached_score.push_back(false);
      __cached_score.push_back(0);
      Counter<IdSetAlloc, CountAlloc>::addNodeSet(var1, std::move(conditioning_ids));
      return index;
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int IndependenceTest<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<unsigned int, unsigned int> &vars,
        std::vector<unsigned int> &&conditioning_ids) {
      return addNodeSet(vars.first, vars.second, std::move(conditioning_ids));
    }

    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void IndependenceTest<IdSetAlloc, CountAlloc>::clear() {
      Counter<IdSetAlloc, CountAlloc>::clear();
      __is_cached_score.clear();
      __cached_score.clear();
    }

    /// indicates whether a score belongs to the cache
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool IndependenceTest<IdSetAlloc, CountAlloc>::_isInCache(
        unsigned int nodeset_index) const noexcept {
      return ((nodeset_index < __is_cached_score.size()) &&
              __is_cached_score[nodeset_index]);
    }

    /// inserts a new score into the cache
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void IndependenceTest<IdSetAlloc, CountAlloc>::_insertIntoCache(
        unsigned int nodeset_index, float score) {
      const std::vector<unsigned int, IdSetAlloc> &all_nodes =
          _getAllNodes(nodeset_index);
      std::vector<unsigned int, IdSetAlloc> conditioning_nodes =
          *(_getConditioningNodes(nodeset_index));
      conditioning_nodes.pop_back();

      if (!conditioning_nodes.empty()) {
        try {
          __cache.insert(all_nodes[all_nodes.size() - 1],
                         all_nodes[all_nodes.size() - 2], conditioning_nodes, score);
        } catch (const gum::DuplicateElement &) {
        }
      } else {
        try {
          __cache.insert(all_nodes[0], all_nodes[1], __empty_conditioning_set,
                         score);
        } catch (const gum::DuplicateElement &) {
        }
      }
    }

    /// returns a cached score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE float IndependenceTest<IdSetAlloc, CountAlloc>::_cachedScore(
        unsigned int nodeset_index) const noexcept {
      return __cached_score[nodeset_index];
    }

    /// indicates whether we use the cache or not
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool IndependenceTest<IdSetAlloc, CountAlloc>::_isUsingCache() const
        noexcept {
      return __use_cache;
    }

    /// turn on/off the use of a cache of the previously computed score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    IndependenceTest<IdSetAlloc, CountAlloc>::useCache(bool on_off) noexcept {
      if (!on_off)
        clear();
      __use_cache = on_off;
    }

    /// clears the current cache (clear nodesets as well)
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void IndependenceTest<IdSetAlloc, CountAlloc>::clearCache() {
      clear();
      __cache.clear();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
