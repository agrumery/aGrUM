/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the EdgeCode class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_EDGE_CODE_H
#define GUM_EDGE_CODE_H

#include <ostream>

#include <agrum/agrum.h>

#include <agrum/base/graphs/graphElements.h>

namespace gum {

  namespace prm {

    namespace gspan {

      /**
       * @class EdgeCode
       * @headerfile edgeCode.h <agrum/PRM/gspan/edgeCode.h>
       * @brief represent a DFS code used by gspan.
       *
       * A DFS code represents a pair of node and an edge between these two
       *nodes.
       */

      struct EdgeCode {
        /**
         * Default constructor.
         *
         * If flag is true then this EdgeCode is a forward edge,
         * otherwise it is a backward edge.
         *
         * @param i The DFS subscript of the first node in the code.
         * @param j The DFS subscript of the second node in the code.
         * @param l_i The label of the first node in the code.
         * @param l_ij The label of the edge in the code.
         * @param l_j The label of the second node in the code.
         */
        EdgeCode(NodeId i, NodeId j, Size l_i, Size l_ij, Size l_j);

        /**
         * Copy constructor.
         * @param source The copied EdgeCode.
         */
        EdgeCode(const EdgeCode& source);

        /// Destructor.
        ~EdgeCode();

        /// The DFS subscript of the first node in the code.
        NodeId i;

        /// The DFS subscript of the second node in the code.
        NodeId j;

        /// The label of the first node in the code.
        Size l_i;

        /// The label of the edge in the code.
        Size l_ij;

        /// The label of the second node in the code.
        Size l_j;

        /// The string version of this EdgeCode.
        std::string name;

        /**
         * Returns true if this EdgeCode is a forward edge.
         * @return Returns true if this EdgeCode is a forward edge.
         */
        bool isForward() const;

        /**
         * Returns true if this EdgeCode is a backward edge.
         * @return Returns true if this EdgeCode is a backward edge.
         */
        bool isBackward() const;

        /**
         * Copy operator.
         * @param source The copied EdgeCode.
         * @return Returns *this after the copy.
         */
        EdgeCode& operator=(const EdgeCode& source);

        /**
         * Equality operator.
         * @param code The code tested for equality with this.
         * @return Returns true if this and code are equal.
         */
        bool operator==(const EdgeCode& code) const;

        /**
         * Difference operator.
         * @param code The code tested for difference with this.
         * @return Returns true if this and code are different.
         */
        bool operator!=(const EdgeCode& code) const;

        /**
         * Lesser than operator.
         * @param code The code on which the test is made.
         * @return Returns true if this is lesser than code.
         */
        bool operator<(const EdgeCode& code) const;
      };

      /**
       * Print code in out.
       * @param out The stream in which code is printed.
       * @param code The printed EdgeCode.
       * @return Returns out after printing code in it.
       */
      std::ostream& operator<<(std::ostream& out, const EdgeCode& code);

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/PRM/gspan/edgeCode_inl.h>
#endif   // GUM_NO_INLINE

#endif   /* GUM_EDGE_CODE_H */
