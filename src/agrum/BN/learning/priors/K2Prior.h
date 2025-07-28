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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief the internal prior for the K2 score = Laplace Prior
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_K2_H
#define GUM_LEARNING_PRIOR_K2_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/learning/priors/smoothingPrior.h>

namespace gum {

  namespace learning {

    /** @class K2Prior
     * @brief the internal prior for the K2 score = Laplace Prior
     * @headerfile K2Prior.h <agrum/base/database/K2Prior.h>
     * @ingroup learning_priors
     *
     * K2 is a BD score with a Laplace prior (i.e., a smoothing of 1).
     *
     * It is important to note that, to be meaningful a structure + parameter
     * learning requires that the same priors are taken into account during
     * structure learning and parameter learning.
     */
    class K2Prior: public SmoothingPrior {
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
      explicit K2Prior(const DatabaseTable&                    database,
                       const Bijection< NodeId, std::size_t >& nodeId2columns
                       = Bijection< NodeId, std::size_t >());

      /// copy constructor
      K2Prior(const K2Prior& from);

      /// move constructor
      K2Prior(K2Prior&& from);

      /// virtual copy constructor
      virtual K2Prior* clone() const;

      /// destructor
      virtual ~K2Prior();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      K2Prior& operator=(const K2Prior& from);

      /// move operator
      K2Prior& operator=(K2Prior&& from);

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
#  include <agrum/BN/learning/priors/K2Prior_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_K2_H */
