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


/** @file
 * @brief Common counting infrastructure for independence tests and KNML scoring
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_CACHED_CONTINGENCY_COUNTER_H
#define GUM_LEARNING_CACHED_CONTINGENCY_COUNTER_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/base/stattests/recordCounter.h>
#include <agrum/base/stattests/scoringCache.h>
#include <agrum/BN/learning/priors/prior.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    /** @class CachedContingencyCounter
     * @brief Common counting infrastructure shared by IndependenceTest and KNML
     * @headerfile cachedContingencyCounter.h <agrum/base/stattests/cachedContingencyCounter.h>
     * @ingroup learning_scores
     *
     * Holds the database parser, prior, record counter, and scoring cache.
     * Both IndependenceTest (Chi2, G2) and KNML inherit from this class.
     * Copy and move constructors/operators are protected to prevent slicing.
     */
    class CachedContingencyCounter: public IThreadNumberManager {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of the score
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database.
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. An empty bijection means identity mapping. */
      CachedContingencyCounter(
          const DBRowGeneratorParser&                                 parser,
          const Prior&                                                prior,
          const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
          const Bijection< NodeId, std::size_t >&                     nodeId2columns
          = Bijection< NodeId, std::size_t >());

      /// default constructor (no ranges)
      CachedContingencyCounter(
          const DBRowGeneratorParser&             parser,
          const Prior&                            prior,
          const Bijection< NodeId, std::size_t >& nodeId2columns
          = Bijection< NodeId, std::size_t >());

      /// destructor
      virtual ~CachedContingencyCounter();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the number max of threads that can be used
      virtual void setNumberOfThreads(Size nb);

      /// returns the current max number of threads of the scheduler
      virtual Size getNumberOfThreads() const;

      /// indicates whether the user set herself the number of threads
      virtual bool isGumNumberOfThreadsOverriden() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context */
      virtual void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      virtual std::size_t minNbRowsPerThread() const;

      /// sets new ranges to perform the counts
      void setRanges(const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t > >& ranges() const;

      /// clears all the data structures from memory, including the cache
      virtual void clear();

      /// clears the current cache
      virtual void clearCache();

      /// turn on/off the use of a cache of the previously computed score
      virtual void useCache(const bool on_off);

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t >& nodeId2Columns() const;

      /// return the database used by the score
      const DatabaseTable& database() const;

      /// @}


      protected:
      /// 1 / log(2)
      const double one_log2_{M_LOG2E};

      /// the expert knowledge prior added to the contingency tables
      Prior* prior_{nullptr};

      /// the record counter used for the counts over discrete variables
      RecordCounter counter_;

      /// the scoring cache
      ScoringCache cache_;

      /// a Boolean indicating whether we wish to use the cache
      bool use_cache_{true};

      /// an empty vector
      const std::vector< NodeId > empty_ids_;


      /// copy constructor
      CachedContingencyCounter(const CachedContingencyCounter& from);

      /// move constructor
      CachedContingencyCounter(CachedContingencyCounter&& from);

      /// copy operator
      CachedContingencyCounter& operator=(const CachedContingencyCounter& from);

      /// move operator
      CachedContingencyCounter& operator=(CachedContingencyCounter&& from);
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/stattests/cachedContingencyCounter_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CACHED_CONTINGENCY_COUNTER_H */
