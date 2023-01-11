/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Inline implementation of the DFSCode class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {
    namespace gspan {

      INLINE
      DFSCode::DFSCode() {
        GUM_CONSTRUCTOR(DFSCode);
        ;
      }

      INLINE
      DFSCode::DFSCode(const DFSCode& source) {
        GUM_CONS_CPY(DFSCode);

        for (const auto code: source.codes)
          codes.push_back(new EdgeCode(*code));
      }

      INLINE
      DFSCode::~DFSCode() {
        GUM_DESTRUCTOR(DFSCode);

        for (const auto item: codes)
          delete item;
      }

      INLINE
      DFSCode& DFSCode::operator=(const DFSCode& source) {
        for (const auto item: codes)
          delete item;

        for (const auto srcitem: source.codes)
          codes.push_back(new EdgeCode(*srcitem));

        return *this;
      }

      INLINE
      bool DFSCode::operator==(const DFSCode& from) const {
        if (codes.size() == from.codes.size()) {
          for (size_t idx = 0; idx < codes.size(); ++idx) {
            if ((*codes[idx]) != (*codes[idx])) { return false; }
          }

          return true;
        } else {
          return false;
        }
      }

      INLINE
      bool DFSCode::operator!=(const DFSCode& from) const {
        if (codes.size() == from.codes.size()) {
          for (size_t idx = 0; idx < codes.size(); ++idx) {
            if ((*codes[idx]) != (*codes[idx])) { return true; }
          }

          return false;
        } else {
          return true;
        }
      }

      INLINE
      bool DFSCode::operator<(const DFSCode& from) const {
        DFSCode::const_iterator iter = codes.begin();
        DFSCode::const_iterator jter = from.codes.begin();

        for (; (iter != codes.end()) && (jter != from.codes.end()); ++iter, ++jter) {
          if ((**iter) != (**jter)) {
            EdgeCode& alpha = **iter;
            EdgeCode& beta  = **jter;

            if (alpha.isBackward()) {
              if (beta.isForward()) {
                return true;
              } else if (alpha.j < beta.j) {
                // beta is a backward edge
                return true;
              } else if ((alpha.j == beta.j) && (alpha.l_ij < beta.l_ij)) {
                return true;
              }

              return false;
            } else {
              // alpha is a forward edge
              if (beta.isBackward()) {
                return false;
              } else if (beta.i < alpha.i) {
                // Beta is a forward edge
                return true;
              } else if (beta.i == alpha.i) {
                if (alpha.l_i < beta.l_i) {
                  return true;
                } else if (alpha.l_i == beta.l_i) {
                  if (alpha.l_ij < beta.l_ij) {
                    return true;
                  } else if (alpha.l_ij == beta.l_ij) {
                    return alpha.l_j < beta.l_j;
                  }
                }
              }

              return false;
            }

            return (**iter) < (**jter);
          }
        }

        return false;
      }

      INLINE
      bool DFSCode::operator<=(const DFSCode& from) const {
        DFSCode::const_iterator iter = codes.begin();
        DFSCode::const_iterator jter = from.codes.begin();

        for (; (iter != codes.end()) && (jter != from.codes.end()); ++iter, ++jter) {
          if ((**iter) != (**jter)) { return (**iter) < (**jter); }
        }

        return codes.size() <= from.codes.size();
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
