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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores you need in one pass. To do so,
 * use the appropriate addNodeSet methods. These will compute everything you
 * need. The addNodeSet methods where you do not specify a set of conditioning
 * nodes assume that this set is empty. Once the computations have been
 * performed, use method _getAllCounts and _getConditioningCounts to get the
 * observed countings if you are developping a new score class, or use
 * method score to get the computed score if you are an end user.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORE_H
#define GUM_LEARNING_SCORE_H

#include <agrum/config.h>
#include <agrum/core/math/math.h>

#include <agrum/learning/scores_and_tests/counter.h>
#include <agrum/learning/scores_and_tests/cache4Score.h>
#include <agrum/learning/scores_and_tests/scoreInternalApriori.h>

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                            SCORE CLASS                            === */
    /* ========================================================================= */
    /** @class Score
     * @brief The base class for all the scores used for learning (BIC, BDeu, etc)
     * @ingroup learning_group
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so,
     * use the appropriate addNodeSet methods. These will compute everything you
     * need. The addNodeSet methods where you do not specify a set of conditioning
     * nodes assume that this set is empty. Once the computations have been
     * performed, use method _getAllCounts and _getConditioningCounts to get the
     * observed countings if you are developping a new score class, or use
     * method score to get the computed score if you are an end user. */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class Score : private Counter<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database
       * @param apriori the a priori that is taken into account in the
       * score/countings */
      template <typename RowFilter>
      Score(const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
            Apriori<IdSetAlloc, CountAlloc> &apriori);

      /// virtual copy factory
      virtual Score<IdSetAlloc, CountAlloc> *copyFactory() const = 0;

      /// destructor
      virtual ~Score();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new single variable to be counted
      /** @param var represents the index of the variable in the filtered rows
       * produced by the database cell filters whose observations shall be counted
       * @return the index of the produced counting vector: the user should use
       * class Score to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index of
       * the observed countings of "var" in this vector. The user shall pass this
       * index as argument to methods _getAllCounts to get the corresponding
       * counting vector. */
      unsigned int addNodeSet(unsigned int var);

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class Score to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the countings of (var | conditioning_ids) in this vector. The
       * user shall pass this index as argument to methods _getAllCounts and
       * _getConditioningCounts to get the counting vectors of
       * (conditioning_ids,vars) [in this order] and conditioning_ids
       * respectively. */
      unsigned int addNodeSet(unsigned int var,
                              const std::vector<unsigned int> &conditioning_ids);

      /// clears all the data structures from memory
      void clear();

      /// clears the current cache (clear nodesets as well)
      void clearCache();

      /// turn on/off the use of a cache of the previously computed score
      void useCache(bool on_off) noexcept;

      /// returns the modalities of the variables
      using Counter<IdSetAlloc, CountAlloc>::modalities;

      /// sets the maximum number of threads used to compute the scores
      using Counter<IdSetAlloc, CountAlloc>::setMaxNbThreads;

      /// returns the score corresponding to a given nodeset
      virtual float score(unsigned int nodeset_index) = 0;

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** The combination of some scores and aprioris can be meaningless. For
       * instance, adding a Dirichlet apriori to the K2 score is not very
       * meaningful since K2 corresonds to a BD score with a 1-smoothing apriori.
       * aGrUM allows you to perform such combination, but yuou can check with
       * method isAprioriCompatible () whether the result the score will give
       * you is meaningful or not. */
      virtual bool isAprioriCompatible() const = 0;

      /// returns the internal apriori of the score
      /** Some scores include an apriori. For instance, the K2 score is a BD score
       * with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with a
       * N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i is
       * the domain size of the target variable and q_i is the domain size of the
       * Cartesian product of its parents. The goal of the score's internal apriori
       * classes is to enable to account for these aprioris outside the score,
       * e.g., when performing parameter estimation. It is important to note that,
       * to be meaningfull a structure + parameter learning requires that the same
       * aprioris are taken into account during structure learning and parameter
       * learning. */
      virtual const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
      internalApriori() const noexcept = 0;

      /// @}

      protected:
      /// 1 / log(2)
      const float _1log2{M_LOG2E};

      /// the a priori used by the score
      Apriori<IdSetAlloc, CountAlloc> *_apriori;

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observation countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSet. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method
       * _getConditioningCounts.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes. */
      using Counter<IdSetAlloc, CountAlloc>::_getAllCounts;

      /// returns the counting vector for a conditioning set
      /** see method _getAllCounts for details */
      using Counter<IdSetAlloc, CountAlloc>::_getConditioningCounts;

      /// returns the set of target + conditioning nodes
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      using Counter<IdSetAlloc, CountAlloc>::_getAllNodes;

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      using Counter<IdSetAlloc, CountAlloc>::_getConditioningNodes;

      /// returns the apriori vector for a given (conditioned) target set
      /** This method returns the observation countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSet. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method
       * _getConditioningApriori.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes. */
      const std::vector<float, CountAlloc> &_getAllApriori(unsigned int index);

      /// returns the apriori vector for a conditioning set
      const std::vector<float, CountAlloc> &
      _getConditioningApriori(unsigned int index);

      /// indicates whether a score belongs to the cache
      bool _isInCache(unsigned int nodeset_index) const noexcept;

      /// inserts a new score into the cache
      void _insertIntoCache(unsigned int nodeset_index, float score);

      /// returns a cached score
      float _cachedScore(unsigned int nodeset_index) const noexcept;

      /// indicates whether we use the cache or not
      bool _isUsingCache() const noexcept;

      /// copy constructor: to be used by the virtual copy constructor
      Score(const Score<IdSetAlloc, CountAlloc> &);

      /// move constructor: to be used by the descendants in the hierarchy
      Score(Score<IdSetAlloc, CountAlloc> &&);

      private:
      /// a cache for the previously computed scores
      Cache4Score __cache;

      /// a Boolean indicating whether we wish to use the cache
      bool __use_cache{true};

      /// indicates whether the ith nodeset's score is in the cache or not
      std::vector<bool> __is_cached_score;

      /// the vector of scores for the current nodesets
      std::vector<float> __cached_score;

      /// has the a priori been computed
      bool __apriori_computed{false};

      /// an empty conditioning set
      const std::vector<unsigned int> __empty_conditioning_set;

      // ##########################################################################
      // ##########################################################################

      /// prevent copy operator
      Score<IdSetAlloc, CountAlloc> &
      operator=(const Score<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/scores_and_tests/score.tcc>

#endif /* GUM_LEARNING_SCORE_H */
