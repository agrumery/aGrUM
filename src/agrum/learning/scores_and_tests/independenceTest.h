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
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the independence tests you need in one pass. To
 * do so, use the appropriate addNodeSet methods. These will compute everything
 * you need. The addNodeSet methods where you do not specify a set of conditioning
 * nodes assume that this set is empty. Once the computations have been
 * performed, use method _getAllCounts and _getConditioningCounts to get the
 * observed countings if you are developping a new independence test class, or use
 * method score to get the computed score of the test if you are an end user.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_INDEPENDENCE_TEST_H
#define GUM_LEARNING_INDEPENDENCE_TEST_H

#include <agrum/learning/scores_and_tests/counter.h>
#include <agrum/learning/scores_and_tests/cache4IndepTest.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                      INDEPENDENCE TEST CLASS                      === */
    /* ========================================================================= */
    /** @class IndependenceTest
     * @brief the abstract class for all the independence tests
     * @ingroup learning_group
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the independence tests you need in one pass.
     * To do so, use the appropriate addNodeSet methods. These will compute
     * everything you need. The addNodeSet methods where you do not specify a set
     * of conditioning nodes assume that this set is empty. Once the computations
     * have been performed, use method _getAllCounts and _getConditioningCounts
     * to get the observed countings if you are developping a new independence
     * test class, or use method score to get the computed score of the test if
     * you are an end user. */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class IndependenceTest : private Counter<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      IndependenceTest(const RowFilter &filter,
                       const std::vector<unsigned int> &var_modalities);

      /// destructor
      virtual ~IndependenceTest();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new target node conditioned by another node to be counted
      /** @param var1 represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the conditioning variable in the
       * filtered rows produced by the database cell filters
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index
       * of the counts in this vector. The user shall pass this index as argument
       * to methods _getAllCounts and _getConditioningCounts to get the observed
       * countings of (var2,var1) [in this order] and var2 respectively. */
      unsigned int addNodeSet(unsigned int var1, unsigned int var2);

      /// add a new target node conditioned by another node to be counted
      /** @param vars contains the index of the target variable (first) in the
       * filtered rows produced by the database cell filters, and the index
       * of the conditioning variable (second).
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the counts in this vector. The user shall pass this index as
       * argument to methods _getAllCounts and _getConditioningCounts to get the
       * observed countings of (vars.second, vars.first) [in this order] and
       * vars.second respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars);

      /// add a target conditioned by other variables to be counted
      /** @param var1 represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the last conditioning variable in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus var2, which is subsequently
       * apended to it).
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the counts in this vector. The user shall pass this index as
       * argument to methods _getAllCounts and _getConditioningCounts to get the
       * countings of (conditioning_ids, var2, var1) [in this order] and
       * (conditioning_ids, var2) [in this order] respectively. */
      unsigned int addNodeSet(unsigned int var1, unsigned int var2,
                              const std::vector<unsigned int> &conditioning_ids);

      /// add a target conditioned by other variables to be counted
      /** @param var1 represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the last conditioning variable in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus var2, which is subsequently
       * apended to it).
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the counts in this vector. The user shall pass this index as
       * argument to methods _getAllCounts and _getConditioningCounts to get the
       * countings of (conditioning_ids, var2, var1) [in this order] and
       * (conditioning_ids, var2) [in this order] respectively. */
      unsigned int addNodeSet(unsigned int var1, unsigned int var2,
                              std::vector<unsigned int> &&conditioning_ids);

      /// add a target conditioned by other variables to be counted
      /** @param vars represents the index of the target variable (first) in the
       * filtered rows produced by the database cell filters, and the index
       * of the last conditioning variable (second)
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus vars.second which is appended to it)
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the counts in this vector. The user shall pass this index as
       * argument to methods _getAllCounts and _getConditioningCounts to get the
       * observed countings of (conditioning_ids, vars.second, vars.first) [in
       * this order] and (conditioning_ids, vars.second) [in this order]
       * respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars,
                              const std::vector<unsigned int> &conditioning_ids);

      /// add a target conditioned by other variables to be counted
      /** @param vars represents the index of the target variable (first) in the
       * filtered rows produced by the database cell filters, and the index
       * of the last conditioning variable (second)
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus vars.second which is appended to it)
       * @return the index of the produced counting vector: the user should use
       * class IndependenceTest to compute in one pass several independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the counts in this vector. The user shall pass this index as
       * argument to methods _getAllCounts and _getConditioningCounts to get the
       * observed countings of (conditioning_ids, vars.second, vars.first) [in
       * this order] and (conditioning_ids, vars.second) [in this order]
       * respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars,
                              std::vector<unsigned int> &&conditioning_ids);

      /// clears all the data structures from memory
      void clear();

      /// clears the current cache (clear nodesets as well)
      void clearCache();

      /// turn on/off the use of a cache of the previously computed score
      void useCache(bool on_off) noexcept;

      /// returns the modalities of the variables
      using Counter<IdSetAlloc, CountAlloc>::modalities;

      /// returns the score corresponding to a given nodeset
      /** Scores are computed by counting formulas (for instance, for Chi2,
       * this formula corresponds to: sum_X sum_Y sum_Z
       * ( #XYZ - (#XZ * #YZ) / #Z )^2 / (( #XZ * #YZ) / #Z ), where #XYZ, #XZ,
       * #YZ, #Z correspond to the number of occurences of (X,Y,Z), (X,Z), (Y,Z)
       * and Z respectively in the database); then the critical value alpha of
       * the test is computed. Finally, method score returns
       * ( #sum - alpha ) / alpha, where #sum corresponds to the
       * summations mentioned above. Therefore, any positive result should
       * reflect a dependence whereas negative results should reflect
       * independences. */
      virtual float score(unsigned int nodeset_index) = 0;

      /// @}

      protected:
      /// 1 / log(2)
      const float _1log2{M_LOG2E};

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observtion countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSet. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method countConditioning.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes.
       * @warning it is assumed that, after using addNodeSet, you have executed
       * method count() before calling method countTarget. */
      using Counter<IdSetAlloc, CountAlloc>::_getAllCounts;

      /// returns the counting vector for a conditioning set
      /** @warning it is assumed that, after using addNodeSet, you have executed
       * method count() before calling method countTarget. */
      using Counter<IdSetAlloc, CountAlloc>::_getConditioningCounts;

      /// returns the set of target + conditioning nodes
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      using Counter<IdSetAlloc, CountAlloc>::_getAllNodes;

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      using Counter<IdSetAlloc, CountAlloc>::_getConditioningNodes;

      /// indicates whether a score belongs to the cache
      bool _isInCache(unsigned int nodeset_index) const noexcept;

      /// inserts a new score into the cache
      void _insertIntoCache(unsigned int nodeset_index, float score);

      /// returns a cached score
      float _cachedScore(unsigned int nodeset_index) const noexcept;

      /// indicates whether we use the cache or not
      bool _isUsingCache() const noexcept;

      private:
      /// a cache for the previously computed scores
      Cache4IndepTest __cache;

      /// a Boolean indicating whether we wish to use the cache
      bool __use_cache{true};

      /// indicates whether the ith nodeset's score is in the cache or not
      std::vector<bool> __is_cached_score;

      /// the vector of scores for the current nodesets
      std::vector<float> __cached_score;

      /// an empty conditioning set
      const std::vector<unsigned int> __empty_conditioning_set;

      // ##########################################################################
      // ##########################################################################

      /// prevent copy constructor
      IndependenceTest(const IndependenceTest<IdSetAlloc, CountAlloc> &) = delete;

      /// prevent copy operator
      IndependenceTest &
      operator=(const IndependenceTest<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/scores_and_tests/independenceTest.tcc>

#endif /* GUM_LEARNING_INDEPENDENCE_TEST_H */
