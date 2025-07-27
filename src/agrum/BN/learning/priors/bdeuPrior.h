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
 * @brief the internal prior for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PRIOR_BDEU_H
#define GUM_LEARNING_PRIOR_BDEU_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/learning/priors/prior.h>

namespace gum::learning {

  /** @class BDeuPrior
   * @brief the internal prior for the BDeu score (N' / (r_i * q_i)
   * @headerfile bdeuPrior.h <agrum/base/database/bdeuPrior.h>
   * @ingroup learning_priors
   *
   * BDeu is a BD score with a N'/(r_i * q_i) prior, where N' is an
   * effective sample size and r_i is the domain size of the target variable
   * and q_i is the domain size of the Cartesian product of its parents.
   *
   * It is important to note that, to be meaningful a structure + parameter
   * learning requires that the same priors are taken into account during
   * structure learning and parameter learning.
   */
  class BDeuPrior: public Prior {
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
    explicit BDeuPrior(const DatabaseTable&                    database,
                       const Bijection< NodeId, std::size_t >& nodeId2columns
                       = Bijection< NodeId, std::size_t >());

    /// copy constructor
    BDeuPrior(const BDeuPrior& from);

    /// move constructor
    BDeuPrior(BDeuPrior&& from) noexcept;

    /// virtual copy constructor
    BDeuPrior* clone() const override;

    /// destructor
    virtual ~BDeuPrior();

    /// @}


    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    BDeuPrior& operator=(const BDeuPrior& from);

    /// move operator
    BDeuPrior& operator=(BDeuPrior&& from) noexcept;

    /// @}


    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// sets the effective sample size N' (alias of setEffectiveSampleSize ())
    void setWeight(double weight) final;

    /// sets the effective sample size N'
    void setEffectiveSampleSize(double weight);

    /// returns the type of the prior
    PriorType getType() const final;

    /// indicates whether the prior is tensorly informative
    /** Basically, only the NoPrior is uninformative. However, it may happen
     * that, under some circumstances, an prior, which is usually not equal
     * to the NoPrior, becomes equal to it (e.g., when the weight is equal
     * to zero). In this case, if the prior can detect this case, it shall
     * inform the classes that use it that it is temporarily uninformative.
     * These classes will then be able to speed-up their code by avoiding to
     * take into account the prior in their computations. */
    bool isInformative() const final;

    /// adds the prior to a counting vector corresponding to the idset
    /** adds the prior to an already created counting vector defined over
     * the union of the variables on both the left and right hand side of the
     * conditioning bar of the idset.
     * @warning the method assumes that the size of the vector is exactly
     * the domain size of the joint variables set. */
    void addJointPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

    /** @brief adds the prior to a counting vector defined over the right
     * hand side of the idset
     *
     * @warning the method assumes that the size of the vector is exactly
     * the domain size of the joint RHS variables of the idset. */
    void addConditioningPseudoCount(const IdCondSet& idset, std::vector< double >& counts) final;

    /// @}
  };

}   // namespace gum::learning

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/priors/bdeuPrior_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PRIOR_BDEU_H */
