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
 * @brief the class for computing fNML scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_FNML2_H
#define GUM_LEARNING_SCORE_FNML2_H

#include <cmath>
#include <string>
#include <fstream>

#include <agrum/agrum.h>
#include <agrum/core/hashFunc.h>
#include <agrum/learning/scores_and_tests/score2.h>
#include <agrum/learning/aprioris/aprioriNoApriori2.h>

namespace gum {

  namespace learning {


    /** @class ScorefNML2
     * @brief the class for computing fNML scores
     * @headerfile scorefNML2.h <agrum/learning/scores_and_tests/scorefNML2.h>
     * @ingroup learning_scores
     *
     * @warning If you pass an apriori to the score, this one will be added
     * into the log-likelihood part of the score.
     */
    template < template < typename > class ALLOC = std::allocator >
    class ScorefNML2 : public Score2< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      // the class for computing efficiently the CTable penalties of Score fNML
      class CTable {
        public:
        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        CTable();

        /// copy constructor
        CTable(const CTable& from);

        /// move constructor
        CTable(CTable&& from);

        /// virtual copy constructor
        virtual CTable* clone() const;

        /// virtual copy constructor with a given allocator
        virtual CTable* clone(const allocator_type& alloc) const;

        /// destructor
        virtual ~CTable();

        /// @}


        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        CTable& operator=(const CTable& from);

        /// move operator
        CTable& operator=(CTable&& from);

        /// @}


        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{

        /// returns the value of the log in base 2 of Cnr
        double log2Cnr(const std::size_t r, const double n);

        /// the function used to write the cpp file with the values of log2(Cnr)
        void CnrToFile(const std::string& filename);

        /// @}

        private:
        /// the value of N above which we should use Szpankowski's approximation
        const double __Szpankowski_threshold{1000.0};

        // constants used to speed-up the computation of the Szpankowski
        // approximation.
        // The formula for the approximation given in Silander, Roos,
        // Kontkanen and Myllymaki (2007) "Factorized Normalized Maximum "
        // Likelihood Criterion for Learning Bayesian Network Structures" paper
        // is incorrect. However, the on in Kontkanen, Buntine, Myllymaki,
        // Rissanen and Tirri (2003) "Efficient Computation of Stochastic
        // Complexity" is correct. So we use the latter and simplify it. Thus,
        // the approximation of log2(Cnr) is equal to:
        // 0.5 log2(n) - 0.5 + log2(sqrt(pi)) + (sqrt(2/pi)/3) / sqrt(n) +
        // (3/36 - 4/(9*pi)) / n.
        // So, given the constants below, it is equal to:
        // 0.5 * std::log2 (n) + __cst1 + __cst2 / std::sqrt(n) + __cst3 / n
        const double __cst1 = -0.5 + std::log2(std::sqrt(M_PI));
        const double __cst2 = std::sqrt(2.0 / M_PI) / 3.0;
        const double __cst3 = 3.0 / 36.0 - 4.0 / (9.0 * M_PI);
      };


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


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
      ScorefNML2(
        const DBRowGeneratorParser< ALLOC >& parser,
        const Apriori2< ALLOC >&             apriori,
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
      ScorefNML2(const DBRowGeneratorParser< ALLOC >& parser,
                 const Apriori2< ALLOC >&             apriori,
                 const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                   nodeId2columns =
                     Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                 const allocator_type& alloc = allocator_type());

      /// copy constructor
      ScorefNML2(const ScorefNML2< ALLOC >& from);

      /// copy constructor with a given allocator
      ScorefNML2(const ScorefNML2< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      ScorefNML2(ScorefNML2< ALLOC >&& from);

      /// move constructor with a given allocator
      ScorefNML2(ScorefNML2< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual ScorefNML2< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual ScorefNML2< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~ScorefNML2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      ScorefNML2< ALLOC >& operator=(const ScorefNML2< ALLOC >& from);

      /// move operator
      ScorefNML2< ALLOC >& operator=(ScorefNML2< ALLOC >&& from);

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
      virtual const Apriori2< ALLOC >& internalApriori() const final;

      /// @}


      /// indicates whether the apriori is compatible (meaningful) with the score
      /** @returns a non empty string if the apriori is compatible with the score.
       */
      static std::string isAprioriCompatible(const std::string& apriori_type,
                                             double             weight = 1.0f);

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** a non empty string if the apriori is compatible with the score. */
      static std::string isAprioriCompatible(const Apriori2< ALLOC >& apriori);


      protected:
      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet2< ALLOC >& idset) final;


      private:
      /// the internal apriori of the score
      AprioriNoApriori2< ALLOC > __internal_apriori;

      /// the CTable cache
      CTable __ctable;
    };


  } /* namespace learning */


} /* namespace gum */


extern template class gum::learning::ScorefNML2<>;


// always include the template implementation
#include <agrum/learning/scores_and_tests/scorefNML2_tpl.h>

#endif /* GUM_LEARNING_SCORE_FNML2_H */
