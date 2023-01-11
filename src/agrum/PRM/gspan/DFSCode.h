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
 * @brief Headers of a DFSCode.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <ostream>
#include <vector>

#include <agrum/PRM/gspan/edgeCode.h>

#ifndef GUM_DFS_CODE_H
#  define GUM_DFS_CODE_H

namespace gum {
  namespace prm {
    namespace gspan {

      /**
       * @class DFSCode
       * @headerfile DFSCode.h <agrum/PRM/gspan/DFSCode.h>
       * @brief Reprensent a Depth First Search coding of a graph.
       *
       * A DFSCode is composed of EdgeCode. Each EdgeCode is either
       * a forward edge or a backward edge.
       *
       * Regarding memory allocation EdgeCode are shared between related
       * DFSCode, so delete DFSCode in a bottom up fashion.
       */
      class DFSCode {
        public:
        /**
         * Returns true of e2 is a valid neighbor for e1 (i.e. it respect the
         * neighborhood restriction) if e1 precedes e2 in a DFSCode.
         *
         * @param e1 An EdgeCode.
         * @param e2 Another EdgeCode.
         * @return Returns true of e2 is a valid neighbor for e1 (i.e. it
         *respect
         *         the neighborhood restriction) if e1 precedes e2 in a DFSCode.
         */
        static bool validNeighbors(EdgeCode* e1, EdgeCode* e2);

        /**
         * Default constructor.
         * Create an empty DFSCode.
         */
        DFSCode();

        /**
         * @brief Copy constructor.
         *
         * Proceeds with a deep copy.
         */
        DFSCode(const DFSCode& source);

        /**
         * @brief Destructor.
         * This will delete all children of this DFSCode, with their respective
         * EdgeCode.
         */
        ~DFSCode();

        /**
         * The vector containing the EdgeCode composing this DFSCode.
         */
        std::vector< EdgeCode* > codes;

        /**
         * @brief Copy operator.
         *
         * Proceeds with a deep copy.
         */
        DFSCode& operator=(const DFSCode& source);

        /**
         * Equality operator.
         * @param code The code tested for equality with this.
         * @return Returns true if this and code are equal.
         */
        bool operator==(const DFSCode& code) const;

        /**
         * Difference operator.
         * @param code The code tested for difference with this.
         * @return Returns true if this and code are different.
         */
        bool operator!=(const DFSCode& code) const;

        /**
         * Lesser than operator.
         * @param code The code on which the test is made.
         * @return Returns true if this is lesser than code.
         */
        bool operator<(const DFSCode& code) const;

        /**
         * Lesser or equal than operator.
         * @param code The code on which the test is made.
         * @return Returns true if this is lesser than code.
         */
        bool operator<=(const DFSCode& code) const;

        /// Code alias.
        using iterator = std::vector< EdgeCode* >::iterator;

        /// Code alias.
        using const_iterator = std::vector< EdgeCode* >::const_iterator;
      };

      /**
       * Print code in out.
       * @param out The stream in which code is printed.
       * @param code The printed DFSCode.
       * @return Returns out after printing code in it.
       */
      std::ostream& operator<<(std::ostream& out, const DFSCode& code);

      inline bool DFSCode::validNeighbors(EdgeCode* e1, EdgeCode* e2) {
        if (e1->isBackward()) {
          if (e2->isForward()) {
            return (e2->i <= e1->i) && (e2->j = (e1->i + 1));
          } else {
            return (e2->i == e1->i) && (e1->j < e2->j);
          }
        } else {
          // e1 is a forward edge
          if (e2->isForward()) {
            return (e2->i <= e1->j) && (e2->j == (e1->j + 1));
          } else {
            return (e2->i == e1->j) && (e2->j < e1->i);
          }
        }
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#  ifndef GUM_NO_INLINE
#    include <agrum/PRM/gspan/DFSCode_inl.h>
#  endif   // GUM_NO_INLINE

#endif /* GUM_DFS_CODE_H */
