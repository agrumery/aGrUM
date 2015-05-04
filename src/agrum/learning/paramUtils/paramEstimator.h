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
 * @brief the base class for estimating parameters of CPTs
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the parameters you need in one pass. To do so,
 * use the appropriate addNodeSet methods. These will compute everything you
 * need. The addNodeSet methods where you do not specify a set of conditioning
 * nodes assume that this set is empty. Once the computations have been
 * performed, use method _getAllCounts and _getConditioningCounts to retrieve
 * the parameters of interest.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR_H
#define GUM_LEARNING_PARAM_ESTIMATOR_H

#include <agrum/config.h>
#include <agrum/multidim/potential.h>
#include <agrum/learning/scores_and_tests/counter.h>
#include <agrum/learning/scores_and_tests/scoreInternalApriori.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                       PARAM ESTIMATOR CLASS                       === */
    /* ========================================================================= */
    /** @class ParamEstimator
     * @brief The base class for estimating parameters of CPTs
     * @ingroup learning_group
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the parameters you need in one pass. To do so,
     * use the appropriate addNodeSet methods. These will compute everything you
     * need. The addNodeSet methods where you do not specify a set of conditioning
     * nodes assume that this set is empty. Once the computations have been
     * performed, use methods _getAllCounts and _getConditioningCounts to retrieve
     * the parameters of interest.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class ParamEstimator : private Counter<IdSetAlloc, CountAlloc> {
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
      ParamEstimator(const RowFilter &filter,
                     const std::vector<unsigned int> &var_modalities,
                     Apriori<IdSetAlloc, CountAlloc> &apriori,
                     const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
                         score_internal_apriori);

      /// virtual copy factory
      virtual ParamEstimator<IdSetAlloc, CountAlloc> *copyFactory() const = 0;

      /// destructor
      virtual ~ParamEstimator();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new CPT with a single variable to be estimated
      /** @param var represents the index of the variable in the filtered rows
       * produced by the database cell filters whose observations shall be counted
       * @return the index of the produced counting vector: the user should use
       * class ParamEstimator to compute in one pass several CPT's parameters.
       * These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index of
       * the observed countings of "var" in this vector. The user shall pass this
       * index as argument to methods _getAllCounts and _getConditioningCounts to
       * get the corresponding counting vectors. */
      unsigned int addNodeSet(unsigned int var);

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class ParamEstimator to compute in one pass several CPT's parameters.
       * These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index of
       * the observed countings of "var" in this vector. The user shall pass this
       * index as argument to methods _getAllCounts and _getConditioningCounts to
       * get the corresponding counting vectors. */
      unsigned int addNodeSet(unsigned int var,
                              const std::vector<unsigned int> &conditioning_ids);

      /// clears all the data structures from memory
      void clear();

      /// returns the modalities of the variables
      using Counter<IdSetAlloc, CountAlloc>::modalities;

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as follows:
       * first, there are the conditioning nodes (in the order in which they
       * were specified) and, then, the target node. */
      virtual const std::vector<float, CountAlloc> &
      parameters(unsigned int nodeset_index) = 0;

      /// sets the CPT's parameters corresponding to a given nodeset
      /** The order of the variables in the potential and in the nodeset
       * are assumed to be identical */
      void setParameters(unsigned int nodeset_index, Potential<float> &pot);

      /// @}

      protected:
      /// the a priori used by the score
      Apriori<IdSetAlloc, CountAlloc> *_apriori;

      /// the score that was use for structure learning (used for its apriori)
      ScoreInternalApriori<IdSetAlloc, CountAlloc> *_score_internal_apriori;

      /// indicate whether we have already normalized the parameters
      std::vector<bool> _is_normalized;

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observtion countings for the set of variables
       * whose index was returned by method addNodeSet. If the
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

      /// returns the apriori vector for a given (conditioned) target set
      /** This method returns the observation countings for the set of variables
       * whose index was returned by method addNodeSet. If the
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

      /// if needed insert the score apriori into the countings
      void _insertScoreApriori();

      /// copy constructor
      ParamEstimator(const ParamEstimator<IdSetAlloc, CountAlloc> &);

      /// move constructor
      ParamEstimator(ParamEstimator<IdSetAlloc, CountAlloc> &&);

      private:
      /// has the a priori been computed
      bool __apriori_computed{false};

      /// has the score's internal apriori been inserted into the countings ?
      bool __score_apriori_inserted{false};

      // ##########################################################################
      // ##########################################################################

      /// prevent copy operator
      ParamEstimator<IdSetAlloc, CountAlloc> &
      operator=(const ParamEstimator<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/paramUtils/paramEstimator.tcc>

#endif /* GUM_LEARNING_PARAM_ESTIMATOR_H */
