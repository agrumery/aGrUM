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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief The BayesBall algorithm (as described by Schachter).
 *
 * @author Lionel TORTI & Christophe GONZALES(_at_AMU) and Pierre-Henri
 * WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BAYESBALLS_H
#define GUM_BAYESBALLS_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/BN/IBayesNet.h>

namespace gum {
  /**
   * @class BayesBall
   * @headerfile BayesBall.h <agrum/BN/inference/BayesBall.h>
   * @brief Implementation of Shachter's Bayes Balls algorithm.
   * @ingroup bn_inference
   *
   */
  class BayesBall {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    private:
    /// Default constructor.
    BayesBall();

    /// Destructor.
    ~BayesBall();

    /// @}

    public:
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief Fill the 'requisite' nodeset with the requisite nodes in dag
     * given a query and evidence.
     *
     * Requisite nodes are those that are d-connected to at least one of the
     * query nodes given a set of hard and soft evidence
     */
    static void requisiteNodes(const DAG&     dag,
                               const NodeSet& query,
                               const NodeSet& hardEvidence,
                               const NodeSet& softEvidence,
                               NodeSet&       requisite);

    /** @brief update a set of tensors, keeping only those d-connected with
     * query variables given evidence */
    template < typename GUM_SCALAR, class TABLE >
    static void relevantTensors(const IBayesNet< GUM_SCALAR >& bn,
                                const NodeSet&                 query,
                                const NodeSet&                 hardEvidence,
                                const NodeSet&                 softEvidence,
                                Set< const TABLE* >&           tensors);

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/BayesBall_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/BN/algorithms/BayesBall_tpl.h>

#endif /* GUM_BAYESBALLS_H */
