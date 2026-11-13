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

#include <cctype>

#include <agrum/base/core/fastSyntax.h>
#include <agrum/base/graphs/fastMarkedGraph.h>

#include <agrum/base/core/utils_string.h>

namespace gum {

  namespace graph::detail {

    namespace {

      /// true if position i of chain is a space or out of the chain
      bool fastGraphIsBlank(std::string_view chain, std::size_t i) {
        return i >= chain.size() || std::isspace(static_cast< unsigned char >(chain[i]));
      }

      /// position of the first non-blank character of chain from i (chain.size() if none)
      std::size_t fastGraphSkipBlanks(std::string_view chain, std::size_t i) {
        while (i < chain.size() && fastGraphIsBlank(chain, i)) {
          ++i;
        }
        return i;
      }

    }   // namespace

    std::vector< FastMarkedToken > fastMarkedGraphTokenize(std::string_view chain) {
      std::vector< FastMarkedToken > tokens;
      std::size_t                    pos            = 0;
      EdgeMark                       markAtPrevious = EdgeMark::Tail;
      EdgeMark                       markAtNext     = EdgeMark::Tail;
      while (pos <= chain.size()) {
        // a quote right after "-o" opens a quoted name: the circle mark of "A <-o'B'"
        std::size_t lastQuoteEnd = std::string_view::npos;
        const auto  dashPos      = fast::detail::findTopLevel(chain, pos, "-", true, &lastQuoteEnd);
        if (dashPos == std::string_view::npos) {
          tokens.push_back({trim_copy(chain.substr(pos)), markAtPrevious, markAtNext});
          break;
        }

        std::size_t opStart = dashPos;
        EdgeMark    left    = EdgeMark::Tail;
        if (dashPos > pos && chain[dashPos - 1] == '<') {
          left    = EdgeMark::Arrowhead;
          opStart = dashPos - 1;
        } else if (dashPos > pos + 1 && chain[dashPos - 1] == 'o'
                   && ((fastGraphIsBlank(chain, dashPos - 2)
                        && fastGraphSkipBlanks(chain, pos) < dashPos - 1)
                       || lastQuoteEnd == dashPos - 1)) {
          // a circle needs the name of its node before it: separated by a blank, or quoted
          // ("'A'o-")
          left    = EdgeMark::Circle;
          opStart = dashPos - 1;
        }
        std::size_t opEnd = dashPos + 1;
        EdgeMark    right = EdgeMark::Tail;
        if (opEnd < chain.size() && chain[opEnd] == '>') {
          right = EdgeMark::Arrowhead;
          ++opEnd;
        } else if (opEnd < chain.size() && chain[opEnd] == 'o'
                   && fastGraphIsBlank(chain, opEnd + 1)) {
          // a circle needs the name of its node after it, not an operator nor the end of chain
          const auto next = fastGraphSkipBlanks(chain, opEnd + 1);
          if (next < chain.size() && chain[next] != '<' && chain[next] != '-') {
            right = EdgeMark::Circle;
            ++opEnd;
          }
        } else if (opEnd + 1 < chain.size() && chain[opEnd] == 'o' && chain[opEnd + 1] == '\'') {
          // a circle followed by a quoted name ("-o'B'")
          right = EdgeMark::Circle;
          ++opEnd;
        }

        tokens.push_back({trim_copy(chain.substr(pos, opStart - pos)), markAtPrevious, markAtNext});
        markAtPrevious = left;
        markAtNext     = right;
        pos            = opEnd;
      }
      return tokens;
    }

  }   // namespace graph::detail

}   // namespace gum
