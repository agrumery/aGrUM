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
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the parameters you need in one pass. To do so,
 * use the appropriate addNodeSet methods. These will compute everything you
 * need. The addNodeSet methods where you do not specify a set of conditioning
 * nodes assume that this set is empty. Once the computations have been
 * performed, use method parameters to retrieve the parameters of interest.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR_ML_H
#define GUM_LEARNING_PARAM_ESTIMATOR_ML_H

#include <agrum/config.h>
#include <agrum/learning/paramUtils/paramEstimator.h>
#include <agrum/learning/scores_and_tests/scoreInternalNoApriori.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                     PARAM ESTIMATOR ML CLASS                      === */
    /* ========================================================================= */
    /** @class ParamEstimatorML
     * @brief The class for estimating parameters of CPTs using Maximum Likelihood
     * @ingroup learning_group
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the parameters you need in one pass. To do so,
     * use the appropriate addNodeSet methods. These will compute everything you
     * need. The addNodeSet methods where you do not specify a set of conditioning
     * nodes assume that this set is empty. Once the computations have been
     * performed, use method parameters to retrieve the parameters of interest.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class ParamEstimatorML : public ParamEstimator<IdSetAlloc, CountAlloc> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      ParamEstimatorML(const RowFilter &filter,
                       const std::vector<unsigned int> &var_modalities,
                       Apriori<IdSetAlloc, CountAlloc> &apriori,
                       const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
                           score_internal_apriori =
                               ScoreInternalNoApriori<IdSetAlloc, CountAlloc>());

      /// copy constructor
      ParamEstimatorML(const ParamEstimatorML<IdSetAlloc, CountAlloc> &);

      /// move constructor
      ParamEstimatorML(ParamEstimatorML<IdSetAlloc, CountAlloc> &&);

      /// virtual copy factory
      virtual ParamEstimatorML<IdSetAlloc, CountAlloc> *copyFactory() const;

      /// destructor
      virtual ~ParamEstimatorML();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as follows:
       * first, there are the conditioning nodes (in the order in which they
       * were specified) and, then, the target node.
       * @throw CPTError is raised if some values of the conditioning sets were
       * not observed in the database. */
      const std::vector<float, CountAlloc> &parameters(unsigned int nodeset_index);

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/paramUtils/paramEstimatorML.tcc>

#endif /* GUM_LEARNING_PARAM_ESTIMATOR_ML_H */
