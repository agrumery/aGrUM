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


/**
 * @file
 * @brief The class for the NML penalty used in MIIC
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LEARNING_K_NML_H
#define GUM_LEARNING_K_NML_H

#include <vector>

#include <agrum/base/core/math/variableLog2ParamComplexity.h>
#include <agrum/base/stattests/cachedContingencyCounter.h>

namespace gum {

  namespace learning {


    /** @class KNML
     * @brief the class for computing the NML penalty used by MIIC
     * @ingroup learning_scores
     *
     */
    class KNML: public CachedContingencyCounter {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of
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
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      KNML(const DBRowGeneratorParser&                                 parser,
           const Prior&                                                prior,
           const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
           const Bijection< NodeId, std::size_t >&                     nodeId2columns
           = Bijection< NodeId, std::size_t >());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
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
      KNML(const DBRowGeneratorParser&             parser,
           const Prior&                            prior,
           const Bijection< NodeId, std::size_t >& nodeId2columns
           = Bijection< NodeId, std::size_t >());

      /// copy constructor
      KNML(const KNML& from);

      /// move constructor
      KNML(KNML&& from);

      /// virtual copy constructor
      [[nodiscard]] virtual KNML* clone() const;

      /// destructor
      ~KNML() override;

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      KNML& operator=(const KNML& from);

      /// move operator
      KNML& operator=(KNML&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the kNML penalty for a pair of nodes
      double score(NodeId var1, NodeId var2);

      /// returns the kNML penalty for a pair of nodes given conditioning nodes
      double score(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids);

      /// clears all the data structures from memory, including the C_n^r cache
      void clear() override;

      /// clears the current C_n^r cache
      void clearCache() override;

      /// turn on/off the use of the C_n^r cache
      void useCache(const bool on_off) override;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the CTable computation
      VariableLog2ParamComplexity _param_complexity_;

      /// computes the NML penalty for a given IdCondSet
      double _score_(const IdCondSet& idset);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/stattests/kNML_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_K_NML_H */
