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
 * @brief Maximally oriented PDAGs (CPDAGs with background knowledge)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MPDAG_H
#define GUM_MPDAG_H

#include <agrum/base/graphs/CPDAG.h>

namespace gum {

  /** @class MPDAG
   * @brief Maximally oriented PDAG: a CPDAG refined by background knowledge
   *
   * \ingroup graph_group
   *
   * A PDAG G represents the set [G] of its consistent extensions: the DAGs with the same skeleton,
   * the same v-structures and containing its arcs. The maximal orientation (the MPDAG) of an
   * extendable PDAG orients every edge that has the same orientation in all the DAGs of [G]; it is
   * the closure of the PDAG under the four Meek rules (Meek 1995). A CPDAG with some imposed arcs
   * (background knowledge) gives an MPDAG (Perković, Kalisch and Maathuis 2017). An MPDAG is not
   * always a chain graph: it may contain partially directed cycles (Wienöbst et al. 2021, Fig. 1).
   *
   * The invariants of an MPDAG are global: this class is an immutable value, built by the Meek
   * closure of an extendable PDAG or by a checked conversion, which gives a read access to its
   * PDAG.
   *
   * The DAGs of [G] are Markov equivalent (same skeleton and v-structures, Verma and Pearl 1990):
   * the separation of an MPDAG is the d-separation of any of them. It is computed on a consistent
   * extension, kept from the construction (see ImmutableMixedGraph).
   *
   * @par References
   * - C. Meek. Causal Inference and Causal Explanation with Background Knowledge. Proceedings of
   *   the 11th Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410.
   *   arXiv:1302.4972.
   * - T. Verma, J. Pearl. Equivalence and Synthesis of Causal Models. Proceedings of the 6th
   *   Conference on Uncertainty in Artificial Intelligence (UAI 1990), pp. 220-227.
   *   arXiv:1304.1108.
   * - E. Perković, M. Kalisch, M. H. Maathuis. Interpreting and using CPDAGs with background
   *   knowledge. Proceedings of the 33rd Conference on Uncertainty in Artificial Intelligence
   *   (UAI 2017), PMLR R15:461-470. arXiv:1707.02171.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   */
  class GUM_SHARED_PUBLIC MPDAG: public ImmutableMixedGraph< PDAG > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// the maximal orientation of an extendable PDAG (its closure under the Meek rules)
    /** Computed by graph::meekClosure (see its time complexity).
     * @throw InvalidArgument if pdag is not extendable */
    static MPDAG meekClosure(const PDAG& pdag);

    /// the MPDAG of a CPDAG with some arcs imposed by background knowledge
    /** @param cpdag the Markov equivalence class
     * @param arcs the imposed arcs: each one must orient an edge of cpdag or be an arc of cpdag
     * @throw InvalidArc if an arc of arcs is not an orientation of an edge of cpdag
     * @throw InvalidArgument if no DAG of the Markov equivalence class contains these arcs (in
     * particular if they form a directed cycle) */
    static MPDAG fromCPDAG(const CPDAG& cpdag, const ArcSet& arcs);

    /// checked conversion of a PDAG
    /** @throw InvalidArgument if pdag is not extendable or not closed under the Meek rules */
    static MPDAG fromPDAG(const PDAG& pdag);

    /// copy constructor
    MPDAG(const MPDAG& g);

    /// move constructor
    MPDAG(MPDAG&& g);

    /// destructor
    ~MPDAG();

    /// copy operator
    MPDAG& operator=(const MPDAG& g);

    /// move operator
    MPDAG& operator=(MPDAG&& g);

    /// @}

    // ############################################################################
    /// @name Represented DAGs
    // ############################################################################
    /// @{

    /// true if dag is represented by the MPDAG (dag is a consistent extension of the MPDAG)
    bool contains(const DAG& dag) const;

    /// the CPDAG of the Markov equivalence class of the represented DAGs
    CPDAG cpdag() const;

    /// @}

    private:
    /// trusted constructor: g must be an MPDAG and extension one of its consistent extensions
    MPDAG(PDAG&& g, DAG&& extension);
  };

} /* namespace gum */

#endif /* GUM_MPDAG_H */
