
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
 * @brief the class for computing Log2-likelihood scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_LOG2_LIKELIHOOD_H
#define GUM_LEARNING_SCORE_LOG2_LIKELIHOOD_H

#include <string>

#include <agrum/agrum.h>
#include <agrum/core/math/math.h>
#include <agrum/learning/scores_and_tests/score.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>

namespace gum {

  namespace learning {

    /** @class ScoreLog2Likelihood
     * @brief the class for computing Log2-likelihood scores
     * @headerfile scoreLog2Likelihood.h <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>
     * @ingroup learning_scores
     *
     * @warning If you pass an apriori to the score, this one will be added
     * into the log-likelihood part of the score.
     */
    template < template < typename > class ALLOC = std::allocator >
    class ScoreLog2Likelihood : public Score< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of the score
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
      ScoreLog2Likelihood(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              apriori,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
            ranges,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of the score
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
      ScoreLog2Likelihood(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              apriori,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      ScoreLog2Likelihood(const ScoreLog2Likelihood< ALLOC >& from);

      /// copy constructor with a given allocator
      ScoreLog2Likelihood(const ScoreLog2Likelihood< ALLOC >& from,
                          const allocator_type&               alloc);

      /// move constructor
      ScoreLog2Likelihood(ScoreLog2Likelihood< ALLOC >&& from);

      /// move constructor with a given allocator
      ScoreLog2Likelihood(ScoreLog2Likelihood< ALLOC >&& from,
                          const allocator_type&          alloc);

      /// virtual copy constructor
      virtual ScoreLog2Likelihood< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual ScoreLog2Likelihood< ALLOC >*
         clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~ScoreLog2Likelihood();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ScoreLog2Likelihood< ALLOC >&
         operator=(const ScoreLog2Likelihood< ALLOC >& from);

      /// move operator
      ScoreLog2Likelihood< ALLOC >& operator=(ScoreLog2Likelihood< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** The combination of some scores and aprioris can be meaningless. For
       * instance, adding a Dirichlet apriori to the K2 score is not very
       * meaningful since K2 corresonds to a BD score with a 1-smoothing
       * apriori.
       * aGrUM allows you to perform such combination, but you can check with
       * method isAprioriCompatible () whether the result the score will give
       * you is meaningful or not.
       * @returns a non empty string if the apriori is compatible with the
       * score.*/
      virtual std::string isAprioriCompatible() const final;

      /// returns the internal apriori of the score
      /** Some scores include an apriori. For instance, the K2 score is a BD
       * score with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with
       * a N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i
       * is the domain size of the target variable and q_i is the domain size of
       * the Cartesian product of its parents. The goal of the score's internal
       * apriori classes is to enable to account for these aprioris outside the
       * score, e.g., when performing parameter estimation. It is important to
       * note that, to be meaningful, a structure + parameter learning requires
       * that the same aprioris are taken into account during structure learning
       * and parameter learning. */
      virtual const Apriori< ALLOC >& internalApriori() const final;

      using Score< ALLOC >::score;

      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      double score(const IdSet< ALLOC >& idset);

      /// @}


      /// indicates whether the apriori is compatible (meaningful) with the score
      /** @returns a non empty string if the apriori is compatible with the score.
       */
      static std::string isAprioriCompatible(const std::string& apriori_type,
                                             double             weight = 1.0f);

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** a non empty string if the apriori is compatible with the score. */
      static std::string isAprioriCompatible(const Apriori< ALLOC >& apriori);


      protected:
      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet< ALLOC >& idset) final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the internal apriori of the score
      AprioriNoApriori< ALLOC > __internal_apriori;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::ScoreLog2Likelihood<>;
#endif


// always include the template implementation
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood_tpl.h>

#endif /* GUM_LEARNING_SCORE_LOG2_LIKELIHOOD_H */
