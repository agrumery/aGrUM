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
 * @brief Completed partially directed acyclic graphs (essential graphs of DAGs)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_CPDAG_H
#define GUM_CPDAG_H

#include <agrum/base/graphs/chainGraph.h>
#include <agrum/base/graphs/immutableGraph.h>

namespace gum {

  /** @class CPDAG
   * @brief Completed PDAG: the representation of a Markov equivalence class of DAGs
   *
   * \ingroup graph_group
   *
   * Two DAGs are Markov equivalent iff they have the same skeleton and the same v-structures
   * (Verma and Pearl 1990). The completed PDAG (or essential graph) of a Markov equivalence class
   * has the skeleton of the class, an arc for each compelled arc (with the same orientation in all
   * the DAGs of the class) and an undirected edge for each reversible arc (Chickering 1995). It is
   * a chain graph whose chain components are chordal, without induced a->b-c, and whose arcs are
   * strongly protected (Andersson, Madigan and Perlman 1997, Theorem 4.1).
   *
   * The invariants of a CPDAG are global (adding an arc to a CPDAG gives in general a graph that is
   * no longer a CPDAG): this class is an immutable value, built by an algorithm (fromDAG) or by a
   * checked conversion (fromPDAG), which gives a read access to its chain graph.
   *
   * The separation of a CPDAG is the d-separation of any DAG of its class (they are Markov
   * equivalent): it is computed on a consistent extension, kept from the construction (see
   * ImmutableMixedGraph). It is also the c-separation (LWF) of its chain graph: a DAG and its
   * essential graph are Markov equivalent (Andersson, Madigan and Perlman 1997, Proposition 4.3)
   * and both separation criteria are complete (Bouckaert and Studený 1998); the LWF and AMP
   * interpretations coincide on a chain graph without induced a->b-c (Andersson, Madigan and
   * Perlman 1996, Theorem 4.2).
   *
   * @par References
   * - T. Verma, J. Pearl. Equivalence and Synthesis of Causal Models. Proceedings of the 6th
   *   Conference on Uncertainty in Artificial Intelligence (UAI 1990), pp. 220-227.
   *   arXiv:1304.1108.
   * - D. M. Chickering. A Transformational Characterization of Equivalent Bayesian Network
   *   Structures. Proceedings of the 11th Conference on Uncertainty in Artificial Intelligence
   *   (UAI 1995), pp. 87-98. arXiv:1302.4938.
   * - S. A. Andersson, D. Madigan, M. D. Perlman. An Alternative Markov Property for Chain Graphs.
   *   Proceedings of the 12th Conference on Uncertainty in Artificial Intelligence (UAI 1996),
   *   pp. 40-48. arXiv:1302.3553.
   * - S. A. Andersson, D. Madigan, M. D. Perlman. A characterization of Markov equivalence classes
   *   for acyclic digraphs. The Annals of Statistics 25(2):505-541, 1997.
   *   doi:10.1214/aos/1031833662.
   * - R. R. Bouckaert, M. Studený. On chain graph models for description of conditional
   *   independence structures. The Annals of Statistics 26(4):1434-1495, 1998.
   *   doi:10.1214/aos/1024691250.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   */
  class GUM_SHARED_PUBLIC CPDAG: public ImmutableMixedGraph< ChainGraph > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// the CPDAG of the Markov equivalence class of dag (Chickering 1995)
    static CPDAG fromDAG(const DAG& dag);

    /// checked conversion of a PDAG
    /** pdag is a CPDAG iff it has a consistent extension D such that the CPDAG of D is pdag
     * (Wienöbst et al. 2021, Observation 6.2). Time: see graph::consistentExtension and
     * graph::cpdag.
     * @throw InvalidArgument if pdag is not a CPDAG */
    static CPDAG fromPDAG(const PDAG& pdag);

    /// copy constructor
    CPDAG(const CPDAG& g);

    /// move constructor
    CPDAG(CPDAG&& g) noexcept;

    /// destructor
    ~CPDAG();

    /// copy operator
    CPDAG& operator=(const CPDAG& g);

    /// move operator
    CPDAG& operator=(CPDAG&& g) noexcept;

    /// @}

    /// true if dag belongs to the Markov equivalence class represented by the CPDAG
    bool contains(const DAG& dag) const;

    private:
    /// trusted constructor: g must be a CPDAG and extension one of its consistent extensions
    CPDAG(ChainGraph&& g, DAG&& extension);
  };

} /* namespace gum */

#endif /* GUM_CPDAG_H */
