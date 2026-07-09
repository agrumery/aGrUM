/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the base class for all the independence tests used for learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_INDEPENDENCE_TEST_H
#define GUM_LEARNING_INDEPENDENCE_TEST_H

#include <agrum/base/stattests/cachedContingencyCounter.h>

namespace gum {

  namespace learning {

    /** @class IndependenceTest
     * @brief The base class for all the independence tests used for learning
     * @headerfile independenceTest.h <agrum/BN/learning/scores/independenceTest.h>
     * @ingroup learning_scores
     */
    class IndependenceTest: public CachedContingencyCounter {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
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
       * @warning If nodeId2columns is not empty, then only the statistics over
       * the ids belonging to this bijection can be computed: applying method
       * statistics() over other ids will raise exception NotFound. */
      IndependenceTest(const DBRowGeneratorParser&                                 parser,
                       const Prior&                                                external_prior,
                       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                       const Bijection< NodeId, std::size_t >&                     nodeId2columns
                       = Bijection< NodeId, std::size_t >());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the statistics over
       * the ids belonging to this bijection can be computed: applying method
       * statistics() over other ids will raise exception NotFound. */
      IndependenceTest(const DBRowGeneratorParser&             parser,
                       const Prior&                            external_prior,
                       const Bijection< NodeId, std::size_t >& nodeId2columns
                       = Bijection< NodeId, std::size_t >());

      /// virtual copy constructor
      [[nodiscard]] virtual IndependenceTest* clone() const = 0;

      /// destructor
      ~IndependenceTest() override;

      /// @name Statistics
      /// @{

      /// returns the pair (test statistic, p-value) for the independence test X _|_ Y | Z
      virtual std::pair< double, double >
          statistics(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids = {}) = 0;

      /// @}

      /// @}


      protected:
      /// copy constructor
      IndependenceTest(const IndependenceTest& from);

      /// move constructor
      IndependenceTest(IndependenceTest&& from);

      /// copy operator
      IndependenceTest& operator=(const IndependenceTest& from);

      /// move operator
      IndependenceTest& operator=(IndependenceTest&& from);

      /// the domain sizes of the variables (indexed by column id in the database)
      std::vector< std::size_t > _domain_sizes_;

      /// returns the degrees of freedom for a chi2/G2 test X _|_ Y | Z
      /** @param X_size   domain size of X
       * @param Y_size   domain size of Y
       * @param Z_size   product of domain sizes of the conditioning variables
       *                 (1 if no conditioning set)
       * @param n_skipped number of cells excluded from the statistic sum because
       *                 their expected count is zero (silent cells). Each such
       *                 cell effectively removes one degree of freedom. The result
       *                 is clamped to 1 to keep the distribution well-defined. */
      static Size degreesOfFreedom_(std::size_t X_size,
                                    std::size_t Y_size,
                                    std::size_t Z_size    = 1,
                                    std::size_t n_skipped = 0);

      /// returns a counting vector where variables are marginalized from N_xyz
      /** @param node_2_marginalize indicates which node(s) shall be marginalized:
       * - 0 means that X should be marginalized
       * - 1 means that Y should be marginalized
       * - 2 means that Z should be marginalized
       * @param X_size the domain size of variable X
       * @param Y_size the domain size of variable Y
       * @param Z_size the domain size of the set of conditioning variables Z
       * @param N_xyz a counting vector of dimension X * Y * Z (in this order)
       */
      std::vector< double > marginalize_(const std::size_t            node_2_marginalize,
                                         const std::size_t            X_size,
                                         const std::size_t            Y_size,
                                         const std::size_t            Z_size,
                                         const std::vector< double >& N_xyz) const;

      /// shared loop for chi-squared-family statistics
      /** Handles counts, marginalisation, structural-zero detection and the final
       *  p-value.  Only active cells (margX * margY != 0) are forwarded to
       *  @p cellContrib; sampling zeros must be handled inside the lambda.
       *  Signature: double cellContrib(double O, double margX, double margY, double total) */
      template < typename CellContribFn >
      std::pair< double, double > computeStatistics_(const IdCondSet& idset,
                                                     CellContribFn    cellContrib);
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/base/stattests/independenceTest_tpl.h>

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/stattests/independenceTest_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_INDEPENDENCE_TEST_H */
