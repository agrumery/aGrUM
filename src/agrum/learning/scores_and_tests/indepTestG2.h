
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
 * @brief the class for computing G2 scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_INDEP_TEST_G2_H
#define GUM_LEARNING_INDEP_TEST_G2_H

#include <agrum/core/math/chi2.h>
#include <agrum/learning/scores_and_tests/independenceTest.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>

namespace gum {

  namespace learning {

    /**
     * @class IndepTestG2
     * @headerfile indepTestG2.h <agrum/learning/scores_and_tests/indepTestG2.h>
     * @brief the class for computing G2 independence test scores
     * @ingroup learning_scores
     */
    // clang-format on
    template < template < typename > class ALLOC = std::allocator >
    class IndepTestG2 : public IndependenceTest< ALLOC > {
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
       * of the score (this should come from expert knowledge): this consists in
       * adding numbers to countings in the contingency tables
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
      IndepTestG2(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              external_apriori,
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
      IndepTestG2(const DBRowGeneratorParser< ALLOC >& parser,
                  const Apriori< ALLOC >&              apriori,
                  const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                     nodeId2columns =
                        Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                  const allocator_type& alloc = allocator_type());

      /// copy constructor
      IndepTestG2(const IndepTestG2< ALLOC >& from);

      /// copy constructor with a given allocator
      IndepTestG2(const IndepTestG2< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      IndepTestG2(IndepTestG2< ALLOC >&& from);

      /// move constructor with a given allocator
      IndepTestG2(IndepTestG2< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual IndepTestG2< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual IndepTestG2< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~IndepTestG2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      IndepTestG2< ALLOC >& operator=(const IndepTestG2< ALLOC >& from);

      /// move operator
      IndepTestG2< ALLOC >& operator=(IndepTestG2< ALLOC >&& from);

      /// get the pair <G2statistic,pvalue> for a test var1 indep var2 given
      /// rhs_ids
      std::pair< double, double >
         statistics(NodeId                                        var1,
                    NodeId                                        var2,
                    const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids = {});

      /// @}


      protected:
      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet< ALLOC >& idset) final;

      /// compute the pair <G2 statistic,pvalue>
      std::pair< double, double > _statistics(const IdSet< ALLOC >& idset);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the domain sizes of the variables
      std::vector< std::size_t, ALLOC< std::size_t > > __domain_sizes;

      /// a chi2 distribution for computing critical values
      Chi2 __chi2;

      /// an empty vector of ids
      const std::vector< Idx > __empty_set;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::IndepTestG2<>;
#endif


// always include the template implementation
#include <agrum/learning/scores_and_tests/indepTestG2_tpl.h>

#endif /* GUM_LEARNING_INDEP_TEST_G2_H */
