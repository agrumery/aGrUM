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
 * @brief Lexical tools shared by the "fast" textual syntaxes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <algorithm>
#include <cctype>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/fastSyntax.h>

#include <agrum/base/core/utils_string.h>

namespace gum {

  namespace {

    bool isBlank(char c) { return std::isspace(static_cast< unsigned char >(c)) != 0; }

    /// the characters after which a quote begins a word (hence opens a quoted name)
    bool beginsAWordAfter(char c) {
      return isBlank(c) || c == ';' || c == '-' || c == '>' || c == '<' || c == '*' || c == '$';
    }

    /// the position just after the quoted name opened at position i of s (npos if not closed)
    std::size_t quotedEnd(std::string_view s, std::size_t i) {
      for (std::size_t j = i + 1; j < s.size(); ++j) {
        if (s[j] != '\'') continue;
        if (j + 1 < s.size() && s[j + 1] == '\'') {
          ++j;
          continue;
        }
        return j + 1;
      }
      return std::string_view::npos;
    }

    /**
     * the first position from `from` where atPos(i) holds at bracket depth 0 and outside any
     * quoted name (std::string_view::npos if none); lastQuoteEnd as in fast::detail::findTopLevel
     */
    template < typename AT_POS >
    std::size_t findIf(std::string_view s,
                       std::size_t      from,
                       bool             circleMarks,
                       std::size_t*     lastQuoteEnd,
                       AT_POS           atPos) {
      if (lastQuoteEnd != nullptr) *lastQuoteEnd = std::string_view::npos;
      int depth = 0;
      for (std::size_t i = from; i < s.size();) {
        const char c = s[i];
        if (depth == 0 && c == '\'' && fast::detail::opensQuote(s, i, circleMarks)) {
          // an unclosed quote hides the rest of s
          i = quotedEnd(s, i);
          if (i == std::string_view::npos) return std::string_view::npos;
          if (lastQuoteEnd != nullptr) *lastQuoteEnd = i;
          continue;
        }
        if (c == '[' || c == '{') {
          ++depth;
        } else if (c == ']' || c == '}') {
          if (depth > 0) --depth;
        } else if (depth == 0 && atPos(i)) {
          return i;
        }
        ++i;
      }
      return std::string_view::npos;
    }

  }   // namespace

  bool fastNameNeedsQuotes(std::string_view name) {
    if (name.empty() || isBlank(name.front()) || isBlank(name.back())) return true;
    if (name.front() == '\'' || name.front() == '*' || name.front() == '$') return true;
    if (name.back() == '+') return true;
    for (std::size_t i = 0; i < name.size(); ++i) {
      const char c = name[i];
      if (std::string_view("[]{};-<>\n\r").find(c) != std::string_view::npos) return true;
      if (c == '\'' && i > 0 && beginsAWordAfter(name[i - 1])) return true;
    }
    return false;
  }

  std::string quoteFastName(std::string_view name) {
    if (!fastNameNeedsQuotes(name)) return std::string(name);
    std::string quoted = "'";
    for (const char c: name) {
      quoted.push_back(c);
      if (c == '\'') quoted.push_back('\'');
    }
    quoted.push_back('\'');
    return quoted;
  }

  namespace fast::detail {

    bool opensQuote(std::string_view s, std::size_t i, bool circleMarks) {
      if (i >= s.size() || s[i] != '\'') return false;
      if (i == 0 || beginsAWordAfter(s[i - 1])) return true;
      return circleMarks && i >= 2 && s[i - 1] == 'o' && s[i - 2] == '-';
    }

    FastQuotedName readQuotedName(std::string_view s, std::size_t i) {
      const auto end = quotedEnd(s, i);
      if (end == std::string_view::npos)
        GUM_ERROR(InvalidArgument, "unclosed quote in '" << s << "'")
      if (end == i + 2) GUM_ERROR(InvalidArgument, "empty quoted name in '" << s << "'")
      std::string name;
      for (std::size_t j = i + 1; j + 1 < end; ++j) {
        name.push_back(s[j]);
        if (s[j] == '\'') ++j;   // a doubled quote stands for a quote
      }
      return {std::move(name), end};
    }

    std::size_t findTopLevel(std::string_view s,
                             std::size_t      from,
                             std::string_view chars,
                             bool             circleMarks,
                             std::size_t*     lastQuoteEnd) {
      return findIf(s, from, circleMarks, lastQuoteEnd, [&](std::size_t i) {
        return chars.find(s[i]) != std::string_view::npos;
      });
    }

    std::vector< std::string > splitTopLevel(std::string_view s, std::string_view sep) {
      if (sep.empty()) GUM_ERROR(InvalidArgument, "splitTopLevel: empty separator")
      std::vector< std::string > pieces;
      std::size_t                pos = 0;
      while (true) {
        const auto found = findIf(s, pos, false, nullptr, [&](std::size_t i) {
          return s.compare(i, sep.size(), sep) == 0;
        });
        if (found == std::string_view::npos) {
          pieces.emplace_back(s.substr(pos));
          return pieces;
        }
        pieces.emplace_back(s.substr(pos, found - pos));
        pos = found + sep.size();
      }
    }

    std::vector< std::string > splitChains(std::string_view s, bool circleMarks) {
      std::vector< std::string > chains;
      std::string                chain;
      auto                       endChain = [&]() {
        if (std::any_of(chain.begin(), chain.end(), [](char c) { return !isBlank(c); }))
          chains.push_back(std::move(chain));
        chain.clear();
      };
      int depth = 0;
      for (std::size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (depth == 0 && c == '\'' && opensQuote(s, i, circleMarks)) {
          // a quoted name is kept as is (an unclosed one runs to the end of s)
          const auto end = std::min(quotedEnd(s, i), s.size());
          chain.append(s.substr(i, end - i));
          i = end - 1;
          continue;
        }
        if (c == '[' || c == '{') {
          ++depth;
        } else if ((c == ']' || c == '}') && depth > 0) {
          --depth;
        }
        if (c == '\n' || c == '\r') {
          // inside brackets (the domain of a variable written on several lines), a line break is
          // removed
          if (depth == 0) endChain();
        } else if (c == ';' && depth == 0) {
          endChain();
        } else {
          chain.push_back(c);
        }
      }
      endChain();
      return chains;
    }

    std::optional< std::string > unquoteFastToken(std::string_view token) {
      if (token.empty() || token.front() != '\'') return std::nullopt;
      auto [name, end] = readQuotedName(token, 0);
      if (!trim_copy(token.substr(end)).empty())
        GUM_ERROR(InvalidArgument, "unexpected text after the quoted name in '" << token << "'")
      return name;
    }

  }   // namespace fast::detail

}   // namespace gum
