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
 * @brief the base class for all the countings used for learning (BIC, BDeu, etc)
 * as well as all the independence tests.
 *
 * This class contains all the methods that enable to add (possibly conditioned)
 * nodesets to be subsequently counted to produce a score or the result of an
 * independence test. The class considers both symmetric and asymmetric scores or
 * tests. Basically, a symmetric test involves two variables X and Y and, possibly
 * a conditioning set of variables Z. The test usually relies on an equation
 * involving quantities #XYZ, #XZ, #YZ and #Z, where "#" refer to observation
 * counts or frequencies. For instance, the Chi2 independence test uses the
 * following formula: (#XYZ - (#XZ * #YZ) / #Z )^2 / ( (#XZ * #YZ) / #Z ). An
 * asymmetric score, like BIC for instance, involves only one variable X and
 * a conditioning set Z. Basically, this score will involve only #XZ and #Z.
 * As such, the current class offers different methods to compute all these
 * quantities.
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores or tests you need in one pass because
 * this enables parsing the database once in order to fill many counting vectors.
 * To do so, use the appropriate addNodeSet methods. These will compute everything
 * you need. The addNodeSet methods where you do not specify a set of conditioning
 * nodes assume that this set is empty. Once the computations have been performed,
 * use methods _getAllCounts and _getConditioningCounts to get the countings that
 * have been performed. Note that this class is not intended to be used as is,
 * but is rather a basis for classes Score and IndependenceTest
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_COUNTER_H
#define GUM_LEARNING_COUNTER_H

#include <vector>
#include <agrum/config.h>
#include <agrum/core/math/math.h>
#include <agrum/learning/scores_and_tests/recordCounter.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                         BASIC SCORE CLASS                         === */
    /* ========================================================================= */
    /** @class Counter
     * @brief The counting class for all the scores used for learning
     * (BIC, BDeu, etc) as well as for all the independence tests.
     * @ingroup learning_group
     *
     * This class contains all the methods that enable to add (possibly
     * conditioned) nodesets to be subsequently counted to produce a score or the
     * result of an independence test. The class considers both symmetric and
     * asymmetric scores or tests. Basically, a symmetric test involves two
     * variables X and Y and, possibly a conditioning set of variables Z. The test
     * usually relies on an equation involving quantities #XYZ, #XZ, #YZ and #Z,
     * where "#" refer to observation counts or frequencies. For instance, the
     * Chi2 independence test uses the following formula:
     * (#XYZ - (#XZ * #YZ) / #Z )^2 / ( (#XZ * #YZ) / #Z ). An asymmetric score,
     * like BIC for instance, involves only one variable X and a conditioning set
     * Z. Basically, this score will involve only #XZ and #Z. As such, the current
     * class offers different methods to compute all these quantities. Note that,
     * counting vectors are actually multidimensional arrays and the order of the
     * variables in the dimensions of the arrays is always the same: the
     * conditioning nodes (in the order in which they are specified) always come
     * first and the target variable is always the last one.
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores or tests you need in one pass
     * because this enables parsing the database once in order to fill many
     * counting vectors. To do so, use the appropriate addNodeSet methods. These
     * will compute everything you need. The addNodeSet methods where you do not
     * specify a set of conditioning nodes assume that this set is empty. Once the
     * computations have been performed, use methods _getAllCounts and
     * _getConditioningCounts to get the countings that have been performed. Note
     * that this class is not intended to be used as is, but is rather a basis
     * for classes Score and IndependenceTest.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class Counter {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      Counter(const RowFilter &filter,
              const std::vector<unsigned int> &var_modalities);

      /// copy constructor
      Counter(const Counter<IdSetAlloc, CountAlloc> &);

      /// move constructor
      Counter(Counter<IdSetAlloc, CountAlloc> &&);

      /// destructor
      virtual ~Counter();

      /// @}

      // ##########################################################################
      /// @name Modifiers for unconditioned variables
      // ##########################################################################
      /// @{

      /// adds an empty set of variables to count
      unsigned int addEmptyNodeSet();

      /// add a new single variable to be counted
      /** @param var represents the index of the variable in the filtered rows
       * produced by the database cell filters whose observations shall be counted
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index of
       * the observed countings of "var" in this vector. The user shall pass this
       * index as argument to methods _getAllCounts to get the corresponding
       * counting vector. */
      unsigned int addNodeSet(unsigned int var);

      /// @}

      // ##########################################################################
      /// @name Modifiers for conditioned variables
      // ##########################################################################
      /// @{

      /// add a new target node conditioned by another node to be counted
      /** @param var1 represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the conditioning variable in the
       * filtered rows produced by the database cell filters
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores or independence
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
       * class Counter to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * _getAllCounts and _getConditioningCounts to get the observed
       * countings of (vars.second, vars.first) [in this order] and
       * vars.second respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars);

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the countings of (var | conditioning_ids) in this vector. The
       * user shall pass this index as argument to methods _getAllCounts and
       * _getConditioningCounts to get the counting vectors of
       * (conditioning_ids,vars) [in this order] and conditioning_ids
       * respectively. */
      unsigned int addNodeSet(unsigned int var,
                              const std::vector<unsigned int> &conditioning_ids);

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the countings of (var | conditioning_ids) in this vector. The
       * user shall pass this index as argument to methods _getAllCounts and
       * _getConditioningCounts to get the counting vectors of
       * (conditioning_ids,vars) [in this order] and conditioning_ids
       * respectively. */
      unsigned int addNodeSet(unsigned int var,
                              std::vector<unsigned int> &&conditioning_ids);

      /// add a target conditioned by other variables to be counted
      /** @param var1 represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the last conditioning variable in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus var2, which is subsequently
       * apended to it).
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * _getAllCounts and _getConditioningCounts to get the countings of
       * (conditioning_ids, var2, var1) [in this order] and
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
       * class Counter to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * _getAllCounts and _getConditioningCounts to get the countings of
       * (conditioning_ids, var2, var1) [in this order] and
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
       * class Counter to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * _getAllCounts and _getConditioningCounts to get the observed countings
       * of (conditioning_ids, vars.second, vars.first) [in this order] and
       * (conditioning_ids, vars.second) [in this order] respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars,
                              const std::vector<unsigned int> &conditioning_ids);

      /// add a target conditioned by other variables to be counted
      /** @param vars represents the index of the target variable (first) in the
       * filtered rows produced by the database cell filters, and the index
       * of the last conditioning variable (second)
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows (minus vars.second which is appended to it)
       * @return the index of the produced counting vector: the user should use
       * class Counter to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * _getAllCounts and _getConditioningCounts to get the observed countings
       * of (conditioning_ids, vars.second, vars.first) [in this order] and
       * (conditioning_ids, vars.second) [in this order] respectively. */
      unsigned int addNodeSet(const std::pair<unsigned int, unsigned int> &vars,
                              std::vector<unsigned int> &&conditioning_ids);

      /// @}

      // ##########################################################################
      /// @name Accessors / General modifiers
      // ##########################################################################
      /// @{

      /// clears all the data structures from memory
      void clear();

      /// returns the modalities of the variables
      const std::vector<unsigned int> &modalities() const noexcept;

      /// sets the maximum number of threads used to perform countings
      void setMaxNbThreads(unsigned int nb) noexcept;

      /// @}

      protected:
      /// 1 / log(2)
      const float _1log2{M_LOG2E};

      /// the modalities of the variables
      const std::vector<unsigned int> &_modalities;

      /// indicates whether we have already computed the countings of the nodesets
      bool _counts_computed{false};

      /// the recordCounter that will parse the database
      RecordCounter<IdSetAlloc, CountAlloc> _record_counter;

      /// the target id sets to count and their indices in the record counter
      std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>, unsigned int> *>
          _target_nodesets;

      /// the conditioning id sets to count and their indices in the record counter
      std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>, unsigned int> *>
          _conditioning_nodesets;

      /// perform the computation of the countings
      void _count();

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observtion countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSet. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method countConditioning.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes).
       * @warning whenever you call this function, if the counts have not been
       * computed yet, they are computed before the function returns. */
      const std::vector<float, CountAlloc> &_getAllCounts(unsigned int index);

      /// returns the counting vector for a conditioning set
      /** @warning whenever you call this function, if the counts have not been
       * computed yet, they are computed before the function returns. */
      const std::vector<float, CountAlloc> &
      _getConditioningCounts(unsigned int index);

      /// returns all the countings performed (both targets and conditioned)
      /** this method returns the countings of the record counter. It should
       * be used in conjunction with methods _getConditioningNodes () and
       * _getTargetNodes () that indicate, for each nodeset, the index of
       * the corresponding counting in the vector returned by _getCounts (). */
      std::vector<std::vector<float, CountAlloc>> &_getCounts() noexcept;

      /// returns the set of target + conditioning nodes
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      const std::vector<unsigned int, IdSetAlloc> &
      _getAllNodes(unsigned int index) const noexcept;

      /// returns all the sets of target + cond nodes, and their counting indices
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      const std::vector<
          std::pair<std::vector<unsigned int, IdSetAlloc>, unsigned int> *> &
      _getAllNodes() const noexcept;

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      const std::vector<unsigned int, IdSetAlloc> *
      _getConditioningNodes(unsigned int index) const noexcept;

      /// returns all the sets of conditioning nodes
      const std::vector<
          std::pair<std::vector<unsigned int, IdSetAlloc>, unsigned int> *> &
      _getConditioningNodes() const noexcept;

      // ##########################################################################
      // ##########################################################################

      /// prevent copy operator
      Counter<IdSetAlloc, CountAlloc> &
      operator=(const Counter<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/scores_and_tests/counter.tcc>

#endif /* GUM_LEARNING_COUNTER_H */
