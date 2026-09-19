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
 * @brief Template implementation of the IKTBNLearner class.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#pragma once

#include <agrum/KTBN/learning/IKTBNLearner.h>

namespace gum {

  namespace learning {

    template < GUM_Numeric GUM_SCALAR >
    std::string IKTBNLearner< GUM_SCALAR >::_encode_(std::string_view base, int slice) const {
      if (slice == KTBN< GUM_SCALAR >::ATEMPORAL) return std::string{base};
      return std::string{base} + '[' + std::to_string(slice) + ']';
    }

    template < GUM_Numeric GUM_SCALAR >
    std::pair< std::string, int >
        IKTBNLearner< GUM_SCALAR >::_determineNode_(const std::string& name) const {
      // check atemporal set first: a name registered as atemporal always maps
      // to ATEMPORAL, even if it syntactically looks like "base[t]"
      if (_atemporalVarNames_().contains(name)) return {name, KTBN< GUM_SCALAR >::ATEMPORAL};

      // syntactic parse: look for a trailing "[digits]" suffix
      const std::size_t bracketPos = name.rfind('[');
      if (bracketPos == std::string::npos) return {name, KTBN< GUM_SCALAR >::ATEMPORAL};

      const std::string_view bracketContent{name.c_str() + bracketPos + 1,
                                            name.size() - bracketPos - 1};
      if (bracketContent.empty() || bracketContent.back() != ']')
        return {name, KTBN< GUM_SCALAR >::ATEMPORAL};

      const std::string_view digits = bracketContent.substr(0, bracketContent.size() - 1);
      if (digits.empty()) return {name, KTBN< GUM_SCALAR >::ATEMPORAL};
      for (const char c: digits)
        if (std::isdigit(static_cast< unsigned char >(c)) == 0)
          return {name, KTBN< GUM_SCALAR >::ATEMPORAL};

      int slice{};
      try {
        slice = std::stoi(std::string{digits});
      } catch (const std::out_of_range&) {
        GUM_ERROR(InvalidArgument,
                  "Node name '" << name << "' has a slice index too large to represent as int.")
      }
      return {name.substr(0, bracketPos), slice};
    }

    template < GUM_Numeric GUM_SCALAR >
    void IKTBNLearner< GUM_SCALAR >::_checkArcTemporallyFeasible_(std::string_view tail,
                                                                  std::string_view head,
                                                                  std::string_view action) const {
      const int tailSlice = _determineNode_(std::string{tail}).second;
      const int headSlice = _determineNode_(std::string{head}).second;
      if (tailSlice != KTBN< GUM_SCALAR >::ATEMPORAL && headSlice == KTBN< GUM_SCALAR >::ATEMPORAL)
        GUM_ERROR(InvalidArgument,
                  "cannot " << action << " " << tail << " -> " << head
                            << ": a temporal variable can never be a parent of an atemporal one; "
                               "this constraint is part of the k-TBN definition")
      if (headSlice < tailSlice)
        GUM_ERROR(InvalidArgument,
                  "cannot " << action << " " << tail << " -> " << head
                            << ": its head is at an earlier time slice than its tail, which "
                               "violates temporal causality")
    }

    template < GUM_Numeric GUM_SCALAR >
    std::unordered_set< std::string > IKTBNLearner< GUM_SCALAR >::_scanConstantColumns_(
        std::string_view                  dirPath,
        std::string_view                  csvBaseName,
        Size                              nbSamples,
        const std::vector< std::string >& missingSymbols) {
      namespace fs = std::filesystem;
      const fs::path                          dir{dirPath};
      const std::string                       stem{csvBaseName};
      const std::unordered_set< std::string > missing(missingSymbols.begin(), missingSymbols.end());

      std::vector< std::string > header;       // captured from trajectory 1
      std::vector< std::size_t > candidates;   // column indices not yet falsified; shrinks only
      std::vector< std::string > firstSeen;   // per column: first non-missing value THIS trajectory
      std::vector< bool >
          haveFirstSeen;   // per column: whether firstSeen[c] is set THIS trajectory

      for (Size i = 0; i < nbSamples; ++i) {
        // once every column has been falsified, nothing left to test can
        // ever become atemporal again, so remaining trajectories are never
        // even opened. Not part of the for-condition: `candidates` does not
        // exist yet before trajectory 1 populates it.
        if (i > 0 && candidates.empty()) break;

        const fs::path file = dir / (stem + std::to_string(i + 1) + ".csv");
        std::ifstream  is(file, std::ifstream::in);
        if (!is.is_open()) GUM_ERROR(IOError, "Cannot open " << file.string());

        CSVParser parser(is, file.string());
        parser.next();
        const auto& raw = parser.current();

        if (i == 0) {
          header.assign(raw.begin(), raw.end());
          candidates.resize(header.size());
          std::iota(candidates.begin(), candidates.end(), std::size_t{0});
        } else {
          bool same = (raw.size() == header.size());
          for (std::size_t c = 0; same && c < header.size(); ++c)
            same = (raw[c] == header[c]);
          if (!same)
            GUM_ERROR(InvalidArgument,
                      "Header of " << file.string() << " differs from trajectory 1");
        }

        haveFirstSeen.assign(header.size(), false);
        firstSeen.assign(header.size(), {});

        while (parser.next()) {
          const auto& tokens = parser.current();
          if (tokens.size() != header.size())
            GUM_ERROR(InvalidArgument,
                      "Trajectory " << (i + 1) << ", row " << parser.nbLine() << ": expected "
                                    << header.size() << " columns, got " << tokens.size());
          // iterate only the still-live candidates, swap-erasing any just
          // falsified so later rows (and later trajectories) never revisit it
          for (std::size_t idx = 0; idx < candidates.size();) {
            const std::size_t c = candidates[idx];
            if (missing.contains(tokens[c])) {
              ++idx;   // uninformative row for this column, still a candidate
              continue;
            }
            if (!haveFirstSeen[c]) {
              firstSeen[c]     = tokens[c];
              haveFirstSeen[c] = true;
              ++idx;
            } else if (tokens[c] != firstSeen[c]) {
              candidates[idx] = candidates.back();   // falsified: drop, O(1)
              candidates.pop_back();
            } else {
              ++idx;
            }
          }
          if (candidates.empty()) break;   // nothing left to test in this file either
        }
      }

      std::unordered_set< std::string > atemporalVars;
      for (const std::size_t c: candidates)
        atemporalVars.insert(header[c]);
      return atemporalVars;
    }

    template < GUM_Numeric GUM_SCALAR >
    void IKTBNLearner< GUM_SCALAR >::_checkBaseIsTemporal_(std::string_view base,
                                                           std::string_view context) const {
      const std::string b{base};
      if (!_isKnownBase_(b))
        GUM_ERROR(InvalidArgument,
                  "unknown base variable '" << base
                                            << "': it is not one of this learner's variables")
      if (_atemporalVarNames_().contains(b))
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << base << "' cannot appear in " << context
                                         << ": it has no per-slice instance")
    }

    template < GUM_Numeric GUM_SCALAR >
    void IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(Size order, std::string_view label) {
      if (order < 2)
        GUM_ERROR(InvalidArgument,
                  "a k-TBN learner requires "
                      << label << " >= 2: k=1 is a static Bayesian network, use BNLearner instead")
    }

  } /* namespace learning */
} /* namespace gum */
