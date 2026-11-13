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
 * @brief Analysis, without construction, of the fast description of a graphical model (the
 * dotlike argument of BayesNet::fastPrototype, InfluenceDiagram::fastPrototype and
 * MarkovRandomField::fastPrototype).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_FAST_MODEL_H
#define GUM_FAST_MODEL_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/graphElements.h>

#ifndef SWIG
namespace gum::fast::detail {

  /// the kind of graphical model described by a fast description
  enum class FastModelType : std::uint8_t { BayesNet, InfluenceDiagram, MarkovRandomField };

  /// the kind of a node of an influence diagram (always Chance for the other models)
  enum class FastNodeKind : std::uint8_t { Chance, Decision, Utility };

  /// a node of a fast description: the fastVariable() description of its variable (without the
  /// '*' or '$' prefix of an influence diagram), the default domain to read it with and its kind
  struct FastModelNode {
    std::string  description;
    std::string  defaultDomain;
    FastNodeKind kind;
  };

  /// the nodes (in the order of their first occurrence), the arcs (BayesNet, InfluenceDiagram)
  /// and the factors (MarkovRandomField) of a fast description, nodes given by their index
  struct FastModel {
    std::vector< FastModelNode >                         nodes;
    std::vector< std::pair< std::size_t, std::size_t > > arcs;
    std::vector< std::vector< std::size_t > >            factors;
  };

  /**
   * @brief Analyses the fast description of a graphical model without building it.
   *
   * A node is identified by the name of its variable: its first occurrence gives its variable and
   * its kind ("D->X;*D->Y": D is a chance node), every occurrence must be a valid fastVariable()
   * description. Chains are separated by ';' or line breaks; BayesNet and InfluenceDiagram chains
   * use "->" and "<-", MarkovRandomField chains list the variables of a factor separated by "--".
   * Names may be quoted (see fastSyntax.h); an unquoted one contains no '<' nor '>' outside its
   * domain (a misplaced operator: "A<" in "A<->B", "A->B" in a Markov random field).
   *
   * The semantic constraints (acyclicity, children of a utility node...) are not checked: they
   * are checked when the model is built.
   *
   * @param desc the description.
   * @param type the kind of model.
   * @param defaultDomain the domain of the variables without one (e.g. "[2]").
   * @throw InvalidArgument if the description is malformed (an operator without a node token, a
   *        misplaced '<' or '>', a malformed variable description).
   * @throw InvalidEdge if a '-' (edge) is used in a BayesNet or an InfluenceDiagram.
   */
  GUM_SHARED_PUBLIC FastModel parseFastModel(std::string_view desc,
                                             FastModelType    type,
                                             std::string_view defaultDomain);

  /**
   * @brief The fast description of a directed graphical model (BayesNet, InfluenceDiagram), read
   * back by its fastPrototype().
   *
   * Each arc gives a chain "tail->head", then each isolated node a chain of its own; a node is
   * written in full (@p prefix, then DiscreteVariable::toFast()) at its first occurrence, by its
   * name (quoted if needed, see quoteFastName()) afterwards.
   *
   * @param model the model.
   * @param prefix `std::string prefix(NodeId)`: the prefix of a node ("*" or "$" for an
   *        influence diagram, "" otherwise).
   */
  template < typename MODEL, typename PREFIX >
  std::string directedModelToFast(const MODEL& model, PREFIX prefix);

}   // namespace gum::fast::detail
#endif   // SWIG

#ifndef SWIG
#  include <agrum/base/graphicalModels/fastModel_tpl.h>
#endif

#endif   // GUM_FAST_MODEL_H
