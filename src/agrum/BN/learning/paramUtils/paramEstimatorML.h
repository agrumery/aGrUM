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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR_ML_H
#define GUM_LEARNING_PARAM_ESTIMATOR_ML_H

#include <sstream>

#include <agrum/agrum.h>

#include <agrum/BN/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class ParamEstimatorML
     * @brief The class for estimating parameters of CPTs using Maximum Likelihood
     * @headerfile paramEstimatorML.h <agrum/BN/learning/paramUtils/paramEstimatorML.h>
     * @ingroup learning_param_utils
     */
    class ParamEstimatorML: public ParamEstimator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation
       * of the score
       * @param score_internal_prior The prior within the score used
       * to learn the data structure (might be a NoPrior)
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database.
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      ParamEstimatorML(const DBRowGeneratorParser& parser,
                       const Prior&                external_prior,
                       const Prior&                _score_internal_prior,
                       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                       const Bijection< NodeId, std::size_t >&                     nodeId2columns
                       = Bijection< NodeId, std::size_t >());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation
       * of the score
       * @param score_internal_prior The prior within the score used
       * to learn the data structure (might be a NoPrior)
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      ParamEstimatorML(const DBRowGeneratorParser&             parser,
                       const Prior&                            external_prior,
                       const Prior&                            _score_internal_prior,
                       const Bijection< NodeId, std::size_t >& nodeId2columns
                       = Bijection< NodeId, std::size_t >());

      /// copy constructor
      ParamEstimatorML(const ParamEstimatorML& from);

      /// move constructor
      ParamEstimatorML(ParamEstimatorML&& from);

      /// virtual copy constructor
      virtual ParamEstimatorML* clone() const;

      /// destructor
      virtual ~ParamEstimatorML();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ParamEstimatorML& operator=(const ParamEstimatorML& from);

      /// move operator
      ParamEstimatorML& operator=(ParamEstimatorML&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      using ParamEstimator::parameters;

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as
       * follows:
       * first, there is the target node, then the conditioning nodes (in the
       * order in which they were specified).
       * @throw DatabaseError is raised if some values of the conditioning sets
       * were not observed in the database. */
      virtual std::vector< double > parameters(const NodeId                 target_node,
                                               const std::vector< NodeId >& conditioning_nodes);

      /**
       * @brief returns the parameters of a CPT as well as its log-likelihood
       *
       * The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as
       * follows:
       * first, there is the target node, then the conditioning nodes (in the
       * order in which they were specified).
       * @param target_node the node on the left side of the CPT's conditioning bar
       * @param conditioning_nodes  thes nodes on the right side of the conditioning bar
       * @return a pair containing i) the vector of parameters and ii) the log-likelihood
       */
      virtual std::pair< std::vector< double >, double >
          parametersAndLogLikelihood(const NodeId                 target_node,
                                     const std::vector< NodeId >& conditioning_nodes);

      /// @}

      private:
      std::pair< std::vector< double >, double >
          _parametersAndLogLikelihood_(const NodeId                 target_node,
                                       const std::vector< NodeId >& conditioning_nodes,
                                       const bool                   compute_log_likelihood);
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/paramUtils/paramEstimatorML_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PARAM_ESTIMATOR_ML_H */
