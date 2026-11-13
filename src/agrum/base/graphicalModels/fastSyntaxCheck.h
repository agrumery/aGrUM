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
 * @brief Checks a fast description for every fast syntax (graphs and graphical models).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_FAST_SYNTAX_CHECK_H
#define GUM_FAST_SYNTAX_CHECK_H

#include <array>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include <agrum/agrum.h>

namespace gum {

  /// the fast syntaxes: the kinds of objects built from a fast description
  enum class FastSyntax : std::uint8_t {
    UndiGraph,          ///< fastGraph< UndiGraph >: edges only
    DiGraph,            ///< fastGraph< DiGraph > (and DAG): arcs only
    MixedGraph,         ///< fastGraph< MixedGraph > (and PDAG, ChainGraph): arcs and edges
    MarkedGraph,        ///< fastMarkedGraph< MarkedGraph > (and PAG): marked edges
    BayesNet,           ///< BayesNet::fastPrototype
    InfluenceDiagram,   ///< InfluenceDiagram::fastPrototype
    MarkovRandomField   ///< MarkovRandomField::fastPrototype
  };

  /// the name of a fast syntax ("UndiGraph", "BayesNet"...)
  GUM_SHARED_PUBLIC std::string toString(FastSyntax syntax);

  /// writes the name of a fast syntax
  GUM_SHARED_PUBLIC std::ostream& operator<<(std::ostream& stream, FastSyntax syntax);

  /**
   * @class FastSyntaxCheck
   * @brief The verdict on a fast description for every fast syntax.
   *
   * A description may be valid for several syntaxes ("A->B" is a DiGraph, a MixedGraph, a
   * MarkedGraph, a BayesNet and an InfluenceDiagram). A description is valid for a syntax if the
   * corresponding fast function accepts it, apart from the semantic constraints checked when the
   * object is built: acyclicity (DAG, PDAG, ChainGraph, BayesNet, InfluenceDiagram), the children
   * of a utility node, the constraints of a PAG.
   *
   * A syntax is checked the first time it is asked for (a graph is built, a model is only
   * analysed).
   */
  class GUM_SHARED_PUBLIC FastSyntaxCheck {
    public:
    /**
     * @brief Checks a description.
     * @param description the fast description.
     * @param defaultDomain the domain of the variables without one, for the graphical models.
     * @throw InvalidArgument if defaultDomain is not a valid domain (see fastVariable()).
     */
    explicit FastSyntaxCheck(std::string_view description, std::string_view defaultDomain = "[2]");

    /// the checked description
    const std::string& description() const;

    /// whether the description is valid for this syntax
    /** @throw OutOfBounds if syntax is not a FastSyntax */
    bool isValid(FastSyntax syntax) const;

    /// why the description is not valid for this syntax (empty if it is valid)
    /** @throw OutOfBounds if syntax is not a FastSyntax */
    const std::string& error(FastSyntax syntax) const;

    /// the syntaxes for which the description is valid, in the order of FastSyntax
    std::vector< FastSyntax > validSyntaxes() const;

    private:
    static constexpr std::size_t _nbSyntaxes_
        = static_cast< std::size_t >(FastSyntax::MarkovRandomField) + 1;

    std::string _description_;
    std::string _defaultDomain_;
    /// the error of each syntax once checked (empty if valid), std::nullopt before
    mutable std::array< std::optional< std::string >, _nbSyntaxes_ > _errors_;

    /// the error of syntax (empty if valid), checked if needed
    const std::string& _check_(FastSyntax syntax) const;
  };

  /// checks a fast description for every fast syntax (see FastSyntaxCheck)
  GUM_SHARED_PUBLIC FastSyntaxCheck checkFastSyntax(std::string_view description,
                                                    std::string_view defaultDomain = "[2]");

}   // namespace gum

#endif   // GUM_FAST_SYNTAX_CHECK_H
