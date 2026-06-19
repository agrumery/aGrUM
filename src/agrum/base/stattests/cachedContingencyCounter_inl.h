/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief implementation of gum::learning::CachedContingencyCounter
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    INLINE CachedContingencyCounter::CachedContingencyCounter(
        const DBRowGeneratorParser&                                 parser,
        const Prior&                                                prior,
        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
        const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        prior_(prior.clone()), counter_(parser, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(CachedContingencyCounter);
    }

    INLINE CachedContingencyCounter::CachedContingencyCounter(
        const DBRowGeneratorParser&             parser,
        const Prior&                            prior,
        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        prior_(prior.clone()), counter_(parser, nodeId2columns) {
      GUM_CONSTRUCTOR(CachedContingencyCounter);
    }

    INLINE CachedContingencyCounter::CachedContingencyCounter(
        const CachedContingencyCounter& from) :
        prior_(from.prior_->clone()), counter_(from.counter_), cache_(from.cache_),
        use_cache_(from.use_cache_) {
      GUM_CONS_CPY(CachedContingencyCounter);
    }

    INLINE CachedContingencyCounter::CachedContingencyCounter(
        CachedContingencyCounter&& from) :
        prior_(from.prior_), counter_(std::move(from.counter_)), cache_(std::move(from.cache_)),
        use_cache_(from.use_cache_) {
      from.prior_ = nullptr;
      GUM_CONS_MOV(CachedContingencyCounter);
    }

    INLINE CachedContingencyCounter::~CachedContingencyCounter() {
      if (prior_ != nullptr) delete prior_;
      GUM_DESTRUCTOR(CachedContingencyCounter);
    }

    INLINE void CachedContingencyCounter::setNumberOfThreads(Size nb) {
      counter_.setNumberOfThreads(nb);
    }

    INLINE Size CachedContingencyCounter::getNumberOfThreads() const {
      return counter_.getNumberOfThreads();
    }

    INLINE bool CachedContingencyCounter::isGumNumberOfThreadsOverriden() const {
      return counter_.isGumNumberOfThreadsOverriden();
    }

    INLINE void CachedContingencyCounter::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }

    INLINE std::size_t CachedContingencyCounter::minNbRowsPerThread() const {
      return counter_.minNbRowsPerThread();
    }

    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
        CachedContingencyCounter::ranges() const {
      return counter_.ranges();
    }

    INLINE void CachedContingencyCounter::clear() {
      counter_.clear();
      cache_.clear();
    }

    INLINE void CachedContingencyCounter::clearCache() { cache_.clear(); }

    INLINE void CachedContingencyCounter::useCache(const bool on_off) { use_cache_ = on_off; }

    INLINE const Bijection< NodeId, std::size_t >&
        CachedContingencyCounter::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }

    INLINE const DatabaseTable& CachedContingencyCounter::database() const {
      return counter_.database();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
