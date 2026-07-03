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
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/structureUtils/graphChange.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/structureUtils/graphChange_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace learning {

    /// put the content of the GraphChange into a string
    std::string GraphChange::toString() const {
      switch (type()) {
        case GraphChangeType::ARC_ADDITION :
          return std::format("ArcAddition ( {} , {} )", node1(), node2());

        case GraphChangeType::ARC_DELETION :
          return std::format("ArcDeletion ( {} , {} )", node1(), node2());

        case GraphChangeType::ARC_REVERSAL :
          return std::format("ArcReversal ( {} , {} )", node1(), node2());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return std::format("ArcTriangleDeletion1 ( {} , {} , {} )", node1(), node2(), node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return std::format("ArcTriangleDeletion2 ( {} , {} , {} )", node1(), node2(), node3());

        case GraphChangeType::EDGE_ADDITION :
          return std::format("EdgeAddition ( {} , {} )", node1(), node2());

        case GraphChangeType::EDGE_DELETION :
          return std::format("EdgeDeletion ( {} , {} )", node1(), node2());
      }

      GUM_ERROR(OperationNotAllowed,
                "Graph modification " << (int)type() << " is not supported yet in method toString")
    }

    /// returns a string corresponding to the type of the change
    std::string GraphChange::typeAsString() const {
      switch (type()) {
        case GraphChangeType::ARC_ADDITION : return "ArcAddition";

        case GraphChangeType::ARC_DELETION : return "ArcDeletion";

        case GraphChangeType::ARC_REVERSAL : return "ArcReversal";

        case GraphChangeType::ARC_TRIANGLE_DELETION1 : return "ArcTriangleDeletion1";

        case GraphChangeType::ARC_TRIANGLE_DELETION2 : return "ArcTriangleDeletion2";

        case GraphChangeType::EDGE_ADDITION : return "EdgeAddition";

        case GraphChangeType::EDGE_DELETION : return "EdgeDeletion";
      }

      GUM_ERROR(OperationNotAllowed,
                "Graph modification " << (int)type()
                                      << " is not supported yet in method typeAsString")
    }

    /// a \c << operator for GraphChanges
    std::ostream& operator<<(std::ostream& stream, const GraphChange& change) {
      return stream << change.toString();
    }

  } /* namespace learning */

} /* namespace gum */
