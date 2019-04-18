
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR_ML_H
#define GUM_LEARNING_PARAM_ESTIMATOR_ML_H

#include <sstream>

#include <agrum/agrum.h>
#include <agrum/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class ParamEstimatorML
     * @brief The class for estimating parameters of CPTs using Maximum Likelihood
     * @headerfile paramEstimatorML.h <agrum/learning/paramUtils/paramEstimatorML.h>
     * @ingroup learning_param_utils
     */
    template < template < typename > class ALLOC = std::allocator >
    class ParamEstimatorML : public ParamEstimator< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_apriori An apriori that we add to the computation
       * of the score
       * @param score_internal_apriori The apriori within the score used
       * to learn the data structure (might be a NoApriori)
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
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
       * @param alloc the allocator used to allocate the structures within the
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      ParamEstimatorML(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              external_apriori,
         const Apriori< ALLOC >&              score_internal__apriori,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
            ranges,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_apriori An apriori that we add to the computation
       * of the score
       * @param score_internal_apriori The apriori within the score used
       * to learn the data structure (might be a NoApriori)
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      ParamEstimatorML(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              external_apriori,
         const Apriori< ALLOC >&              score_internal__apriori,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      ParamEstimatorML(const ParamEstimatorML< ALLOC >& from);

      /// copy constructor with a given allocator
      ParamEstimatorML(const ParamEstimatorML< ALLOC >& from,
                       const allocator_type&            alloc);

      /// move constructor
      ParamEstimatorML(ParamEstimatorML< ALLOC >&& from);

      /// move constructor with a given allocator
      ParamEstimatorML(ParamEstimatorML< ALLOC >&& from,
                       const allocator_type&       alloc);

      /// virtual copy constructor
      virtual ParamEstimatorML< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual ParamEstimatorML< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~ParamEstimatorML();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ParamEstimatorML< ALLOC >& operator=(const ParamEstimatorML< ALLOC >& from);

      /// move operator
      ParamEstimatorML< ALLOC >& operator=(ParamEstimatorML< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      using ParamEstimator< ALLOC >::parameters;

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as
       * follows:
       * first, there is the target node, then the conditioning nodes (in the
       * order in which they were specified).
       * @throw DatabaseError is raised if some values of the conditioning sets
       * were not observed in the database. */
      virtual std::vector< double, ALLOC< double > > parameters(
         const NodeId                                  target_node,
         const std::vector< NodeId, ALLOC< NodeId > >& conditioning_nodes);

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/paramUtils/paramEstimatorML_tpl.h>

#endif /* GUM_LEARNING_PARAM_ESTIMATOR_ML_H */
