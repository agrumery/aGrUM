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
 * @brief Lexical tools shared by the "fast" textual syntaxes (fastGraph, fastMarkedGraph,
 * fastVariable, fastPrototype of BayesNet, InfluenceDiagram and MarkovRandomField).
 *
 * A node or variable name may be written between single quotes ('Visit Asia'). In such a quoted
 * name, every character is literal ('-', ';', '[', '<', blanks, line breaks...), and a doubled
 * quote stands for a quote ('l''eau' is the name l'eau). Labels between '{' and '}' are never
 * quoted: a quote inside brackets is an ordinary character ({l'eau|x}).
 *
 * At bracket depth 0, a quote opens a quoted name only if it is the first character of the text or
 * if it follows a blank, ';', '-', '>', '<', '*' or '$' (the beginning of a name): a quote inside
 * a word (O'Brien) is an ordinary character.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_FAST_SYNTAX_H
#define GUM_FAST_SYNTAX_H

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <agrum/agrum.h>

namespace gum {

  /**
   * @brief Whether the name of a variable must be quoted to be read back as is by fastVariable()
   * and by the fastPrototype() of the graphical models.
   *
   * True if the name begins or ends with a blank, begins with a quote, '*' or '$', contains one of
   * "[]{};-<>" or a line break, ends with '+', or contains a quote at the beginning of a word
   * (after a blank or an operator). Blanks inside a name do not need quotes there. The node names
   * of fastGraph() are stricter: an unquoted one contains no blank and is a NodeId if it is made of
   * digits.
   */
  GUM_SHARED_PUBLIC bool fastNameNeedsQuotes(std::string_view name);

  /**
   * @brief The name as written in a fast syntax: between single quotes (with its quotes doubled)
   * if fastNameNeedsQuotes(), as is otherwise.
   *
   * Used by DiscreteVariable::toFast(), hence by the GUM and BIFXML writers. A name cannot be empty
   * (fastVariable() refuses it).
   * @warning aGrUM/pyAgrum 3.3.2 or older read a quoted name with the quotes in the name (e.g. a
   * file written with the variable 'X-ray'[3], or with the variables of a KTBN, named X[0]...).
   */
  GUM_SHARED_PUBLIC std::string quoteFastName(std::string_view name);

#ifndef SWIG
  namespace fast::detail {

    /// whether position i of s, a quote at bracket depth 0, opens a quoted name; with circleMarks
    /// (marked graphs), a quote right after "-o" opens one too (the circle mark of "A <-o'B'")
    GUM_SHARED_PUBLIC bool opensQuote(std::string_view s, std::size_t i, bool circleMarks = false);

    /// a quoted name and the position just after its closing quote
    struct FastQuotedName {
      std::string name;
      std::size_t end;
    };

    /**
     * @brief Reads the quoted name which begins at position i of s (a quote).
     * @throw InvalidArgument if the quote is not closed or if the quoted name is empty.
     */
    GUM_SHARED_PUBLIC FastQuotedName readQuotedName(std::string_view s, std::size_t i);

    /**
     * @brief The first position, from @p from, of a character of @p chars at bracket depth 0 and
     * outside any quoted name.
     *
     * @param lastQuoteEnd if not null, set to the position just after the last quoted name closed
     *        before the returned position (std::string_view::npos if none).
     * @return the position, std::string_view::npos if none (an unclosed quote hides the end of s).
     */
    GUM_SHARED_PUBLIC std::size_t findTopLevel(std::string_view s,
                                               std::size_t      from,
                                               std::string_view chars,
                                               bool             circleMarks  = false,
                                               std::size_t*     lastQuoteEnd = nullptr);

    /// splits s on sep found at bracket depth 0 and outside any quoted name (empty pieces kept)
    GUM_SHARED_PUBLIC std::vector< std::string > splitTopLevel(std::string_view s,
                                                               std::string_view sep);

    /// splits s on ';' and line breaks at bracket depth 0 and outside any quoted name (see
    /// opensQuote() for circleMarks); a line break inside brackets is removed, inside a quoted name
    /// it is kept; the blank pieces are dropped
    GUM_SHARED_PUBLIC std::vector< std::string > splitChains(std::string_view s,
                                                             bool             circleMarks = false);

    /**
     * @brief The name denoted by a trimmed token if it is quoted, std::nullopt otherwise.
     * @throw InvalidArgument if the quote is not closed, if the quoted name is empty or if
     *        anything but blanks follows it (see readQuotedName()).
     */
    GUM_SHARED_PUBLIC std::optional< std::string > unquoteFastToken(std::string_view token);

  }   // namespace fast::detail
#endif   // SWIG

}   // namespace gum

#endif   // GUM_FAST_SYNTAX_H
