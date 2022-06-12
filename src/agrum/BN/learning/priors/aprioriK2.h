/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @brief the internal apriori for the K2 score = Laplace Prior
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_K2_H
#define GUM_LEARNING_PRIOR_K2_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/learning/priors/aprioriSmoothing.h>

namespace gum {

  namespace learning {

    /** @class AprioriK2
     * @brief the internal apriori for the K2 score = Laplace Prior
     * @headerfile aprioriK2.h <agrum/tools/database/aprioriK2.h>
     * @ingroup learning_apriori
     *
     * K2 is a BD score with a Laplace apriori (i.e., a smoothing of 1).
     *
     * It is important to note that, to be meaningful a structure + parameter
     * learning requires that the same priors are taken into account during
     * structure learning and parameter learning.
     */
    class AprioriK2: public AprioriSmoothing {
      public:

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param database the database from which learning is performed. This is
       * useful to get access to the random variables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable.
       * This enables estimating from a database in which variable A corresponds
       * to the 2nd column the parameters of a BN in which variable A has a
       * NodeId of 5. An empty nodeId2Columns bijection means that the mapping
       * is an identity, i.e., the value of a NodeId is equal to the index of
       * the column in the DatabaseTable.
       */
      AprioriK2(const DatabaseTable&                    database,
                const Bijection< NodeId, std::size_t >& nodeId2columns
                = Bijection< NodeId, std::size_t >());

      /// copy constructor
      AprioriK2(const AprioriK2& from);

      /// move constructor
      AprioriK2(AprioriK2&& from);

      /// virtual copy constructor
      virtual AprioriK2* clone() const;

      /// destructor
      virtual ~AprioriK2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      AprioriK2& operator=(const AprioriK2& from);

      /// move operator
      AprioriK2& operator=(AprioriK2&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// dummy set weight function: in K2, weights are always equal to 1
      virtual void setWeight(const double weight) final;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/priors/aprioriK2_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_K2_H */
