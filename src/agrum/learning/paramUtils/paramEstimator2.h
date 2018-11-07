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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR2_H
#define GUM_LEARNING_PARAM_ESTIMATOR2_H

#include <agrum/agrum.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/aprioris/apriori2.h>
#include <agrum/learning/scores_and_tests/recordCounter2.h>
#include <agrum/multidim/potential.h>

namespace gum {

  namespace learning {


    /** @class ParamEstimator2
     * @brief The base class for estimating parameters of CPTs
     * @headerfile paramEstimator2.h <agrum/learning/paramUtils/paramEstimator2.h>
     * @ingroup learning_param_utils
     */
    template < template < typename > class ALLOC = std::allocator >
    class ParamEstimator2 {
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
      ParamEstimator2(const DBRowGeneratorParser< ALLOC >& parser,
                      const Apriori2< ALLOC >& external_apriori,
                      const Apriori2< ALLOC >& score_internal__apriori,
                      const std::vector< std::pair< std::size_t, std::size_t >,
                      ALLOC< std::pair< std::size_t, std::size_t > > >&
                      ranges,
                      const Bijection< NodeId, std::size_t, ALLOC<std::size_t> >&
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
      ParamEstimator2(const DBRowGeneratorParser< ALLOC >& parser,
                      const Apriori2< ALLOC >& external_apriori,
                      const Apriori2< ALLOC >& score_internal__apriori,
                      const Bijection< NodeId, std::size_t, ALLOC<std::size_t> >&
                      nodeId2columns =
                      Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                      const allocator_type& alloc = allocator_type());

      /// copy constructor
      ParamEstimator2(const ParamEstimator2< ALLOC >& from);

      /// copy constructor with a given allocator
      ParamEstimator2(const ParamEstimator2< ALLOC >& from,
                      const allocator_type& alloc);

      /// move constructor
      ParamEstimator2(ParamEstimator2< ALLOC >&& from);

      /// move constructor with a given allocator
      ParamEstimator2(ParamEstimator2< ALLOC >&& from,
                      const allocator_type& alloc);

      /// virtual copy constructor
      virtual ParamEstimator2< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual ParamEstimator2< ALLOC >*
      clone(const allocator_type& alloc) const = 0;

      /// destructor
      virtual ~ParamEstimator2();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// clears all the data structures from memory
      virtual void clear();

      /// changes the max number of threads used to parse the database
      virtual void setMaxNbThreads(std::size_t nb) const;

      /// returns the number of threads used to parse the database
      virtual std::size_t nbThreads() const;

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as
       * follows:
       * first, there is the target node, then the conditioning nodes (in the
       * order in which they were specified). */
      virtual std::vector< double, ALLOC< double > >
      parameters(const NodeId target_node,
                 const std::vector< NodeId, ALLOC< NodeId> >& conditioning_nodes)
      = 0;

      /// sets the CPT's parameters corresponding to a given Potential
      /** The potential is assumed to be a conditional probability, the first
       * variable of its variablesSequence() being the target variable, the
       * other ones being on the right side of the conditioning bar. */
      template < typename GUM_SCALAR >
      void
      setParameters(const NodeId target_node,
                    const std::vector< NodeId, ALLOC<NodeId> >& conditioning_nodes,
                    Potential< GUM_SCALAR >& pot);

      /// returns the allocator used by the score
      allocator_type getAllocator() const;
      
      /// @}

    protected:
      /// an external a priori
      Apriori2< ALLOC >* _external_apriori {nullptr};

      /** @brief if a score was used for learning the structure of the PGM, this
       * is the a priori internal to the score */
      Apriori2< ALLOC >* _score_internal_apriori {nullptr};

      /// the record counter used to parse the database
      RecordCounter2< ALLOC > _counter;


      /// copy operator
      ParamEstimator2< ALLOC >&
      operator=(const ParamEstimator2< ALLOC >& from);

      /// move operator
      ParamEstimator2< ALLOC >&
      operator=(ParamEstimator2< ALLOC >&& from);

      /** @brief check the coherency between the parameters passed to
       * the setParameters functions */
      template < typename GUM_SCALAR >
      void __checkParameters(
           const NodeId target_node,
           const std::vector< NodeId, ALLOC< NodeId> >& conditioning_nodes,
           Potential< GUM_SCALAR >& pot);
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/paramUtils/paramEstimator2_tpl.h>

#endif /* GUM_LEARNING_PARAM_ESTIMATOR2_H */
