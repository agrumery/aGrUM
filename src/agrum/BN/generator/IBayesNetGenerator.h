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
 * @brief Interface-like class for generating Bayesian networks.
 *
 * @author Christophe GONZALES(_at_AMU), Pierre-Henri WUILLEMIN(_at_LIP6), Lionel TORTI
 * and Ariele-Paolo MAESANO
 */
#ifndef GUM_I_BAYES_NET_GENERATOR_H
#define GUM_I_BAYES_NET_GENERATOR_H

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

namespace gum {

  /**
   * @class IBayesNetGenerator
   * @headerfile IBayesNetGenerator.h <agrum/BN/generator/IBayesNetGenerator.h>
   * @brief Class for generating Bayesian networks.
   * @ingroup bn_generator
   *
   * This class is the abstract class for randomly generating a bayesian
   * network given three parameters: the number of nodes the wanted maximum
   * number of arcs and the maximum number of modality for each node.
   *
   * @warning  Be Careful when entering the parameters, high Values may cause
   * the density of the Bayesian network to be too high resulting in the
   * failure of most of the inference Methods.
   */
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  class IBayesNetGenerator: public ICPTGenerator< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param maxArcs The number of maximum number of arcs imposed on the
     * generator
     * @param maxModality Each DRV has from 2 to maxModality modalities
     * @throws OperationNotAllowed if the number of maximum arcs does not allow
     * the
     * generation of a connexe graph maxArcs < nbrNodes -1, is too big maxArcs >
     * nbrNodes *(nbrNodes -1) /2 and if the maximum of modality is lower than
     * 2.
     */
    IBayesNetGenerator(Size nbrNodes, Size maxArcs, Size maxModality);

    /**
     * Destructor.
     */

    virtual ~IBayesNetGenerator();
    /// @}

    // ############################################################################
    /// @name BN generation methods
    // ############################################################################
    /// @{

    /**
     * Virtual function that Generates a Bayesian networks.
     * @param bayesNet Bayesian network to be completed after initialisation
     * @return null but modify inputed empty Bayesian network
     */

    virtual void generateBN(BayesNet< GUM_SCALAR >& bayesNet) = 0;

    void fromDAG(BayesNet< GUM_SCALAR >& bayesNet);
    /**
     * function that insert random values in the CPT of each nodes according to
     * the
     * @a CPTGenerator.
     * @return null but modify inputed empty Bayesian network
     */

    void fillCPT(BayesNet< GUM_SCALAR >& bn) const;

    /// @}
    // ===========================================================================
    /// @name Variable manipulation methods.
    // ===========================================================================
    /// @{
    /**
     * @name Getters
     **/

    /**
     * Return a constant reference to the number of nodes imposed on the
     * IBayesNetGenerator.
     */
    Size nbrNodes() const;

    /**
     * Return a constant reference to the maximum number of arcs imposed on the
     * IBayesNetGenerator
     */
    Size maxArcs() const;

    /**
     * Return a constant reference to the maximum modality imposed on the
     * IBayesNetGenerator
     */
    Size maxModality() const;

    /**
     * @name Setters
     **/

    //      void setcptGenerator(CPTGenerator * cptGenerator);
    /**
     * Modifies the value of the number of nodes imposed on the BayesGenerator
     */
    void setNbrNodes(Size nbrNodes);

    /**
     * Modifies the value of the number of nodes imposed on the BayesGenerator
     */
    void setMaxArcs(Size maxArcs);

    /**
     * Modifies the value of the number of nodes imposed on the BayesGenerator
     */
    void setMaxModality(Size maxModality);

    /// @}

    protected:
    // The Conditional Probability Table generator
    //  CPTGenerator * cptGenerator_;
    Size nbrNodes_;
    Size maxArcs_;
    Size maxModality_;
    DAG  dag_;
  };

} /* namespace gum */

#include <agrum/BN/generator/IBayesNetGenerator_tpl.h>

#endif /* GUM_I_BAYES_NET_GENERATOR_H */
