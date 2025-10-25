/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


/** @file
 * @brief a cache for caching scores and independence tests results
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/stattests/scoringCache.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE ScoringCache::ScoringCache() { GUM_CONSTRUCTOR(ScoringCache); }

    /// copy constructor
    INLINE ScoringCache::ScoringCache(const ScoringCache& from) : _scores_(from._scores_) {
      GUM_CONS_CPY(ScoringCache);
    }

    /// move constructor
    INLINE ScoringCache::ScoringCache(ScoringCache&& from) : _scores_(std::move(from._scores_)) {
      GUM_CONS_MOV(ScoringCache);
    }

    /// virtual copy constructor
    INLINE ScoringCache* ScoringCache::clone() const { return new ScoringCache(*this); }

    /// destructor
    INLINE ScoringCache::~ScoringCache() { GUM_DESTRUCTOR(ScoringCache); }

    /// copy operator
    INLINE ScoringCache& ScoringCache::operator=(const ScoringCache& from) {
      if (&from != this) { _scores_ = from._scores_; }
      return *this;
    }

    /// move operator
    INLINE ScoringCache& ScoringCache::operator=(ScoringCache&& from) {
      if (&from != this) { _scores_ = std::move(from._scores_); }
      return *this;
    }

    /// insert a new score into the cache
    INLINE void ScoringCache::insert(const IdCondSet& idset, double score) {
      _scores_.insert(idset, score);
    }

    /// insert a new score into the cache
    INLINE void ScoringCache::insert(IdCondSet&& idset, double score) {
      _scores_.insert(std::move(idset), std::move(score));
    }

    /// removes a score (if it exists)
    INLINE void ScoringCache::erase(const IdCondSet& idset) { _scores_.erase(idset); }

    /// indicates whether a given score exists
    INLINE bool ScoringCache::exists(const IdCondSet& idset) { return _scores_.exists(idset); }

    /// returns a given score
    INLINE double ScoringCache::score(const IdCondSet& idset) { return _scores_[idset]; }

    /// removes all the stored scores
    INLINE void ScoringCache::clear() { _scores_.clear(); }

    /// returns the number of scores saved in the cache
    INLINE std::size_t ScoringCache::size() const { return _scores_.size(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
