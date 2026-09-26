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

    // -------------------------------------------------------------------------
    // Constructors, destructors and assignment operators of GraphChange and of
    // its subclasses are defined out-of-line (not INLINE) on purpose: under
    // MSVC, a non-template dllexport'ed class has its inline-defined special
    // members promoted to strong/eager symbol emission (instead of COMDAT),
    // which causes LNK2005 duplicate-definition errors once several
    // translation units include the inline body.
    // -------------------------------------------------------------------------

    /// default constructor
    GraphChange::GraphChange(GraphChangeType type,
                              NodeId          node1,
                              NodeId          node2,
                              NodeId          node3) noexcept : type_{type} {
      nodes_[0] = LearnNodeId(node1);
      nodes_[1] = LearnNodeId(node2);
      nodes_[2] = LearnNodeId(node3);
      GUM_CONSTRUCTOR(GraphChange);
    }

    /// copy constructor
    GraphChange::GraphChange(const GraphChange& from) noexcept {
      std::memcpy(nodes_, from.nodes_, 4 * sizeof(LearnNodeId));
      GUM_CONS_CPY(GraphChange);
    }

    /// move constructor
    GraphChange::GraphChange(GraphChange&& from) noexcept {
      std::memcpy(nodes_, from.nodes_, 4 * sizeof(LearnNodeId));
      GUM_CONS_MOV(GraphChange);
    }

    /// destructor
    GraphChange::~GraphChange() noexcept { GUM_DESTRUCTOR(GraphChange); }

    /// copy operator
    GraphChange& GraphChange::operator=(const GraphChange& from) noexcept {
      if (this != &from) { std::memcpy(nodes_, from.nodes_, 4 * sizeof(LearnNodeId)); }
      return *this;
    }

    /// move operator
    GraphChange& GraphChange::operator=(GraphChange&& from) noexcept {
      if (this != &from) { std::memcpy(nodes_, from.nodes_, 4 * sizeof(LearnNodeId)); }
      return *this;
    }

    /// default constructor
    ArcAddition::ArcAddition(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_ADDITION, node1, node2) {}

    /// copy constructor
    ArcAddition::ArcAddition(const ArcAddition& from) noexcept : GraphChange(from) {}

    /// move constructor
    ArcAddition::ArcAddition(ArcAddition&& from) noexcept : GraphChange(std::move(from)) {}

    /// destructor
    ArcAddition::~ArcAddition() noexcept {}

    /// copy operator
    ArcAddition& ArcAddition::operator=(const ArcAddition& from) noexcept = default;

    /// move operator
    ArcAddition& ArcAddition::operator=(ArcAddition&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    ArcDeletion::ArcDeletion(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_DELETION, node1, node2) {}

    /// copy constructor
    ArcDeletion::ArcDeletion(const ArcDeletion& from) noexcept : GraphChange(from) {}

    /// move constructor
    ArcDeletion::ArcDeletion(ArcDeletion&& from) noexcept : GraphChange(std::move(from)) {}

    /// destructor
    ArcDeletion::~ArcDeletion() noexcept {}

    /// copy operator
    ArcDeletion& ArcDeletion::operator=(const ArcDeletion& from) noexcept = default;

    /// move operator
    ArcDeletion& ArcDeletion::operator=(ArcDeletion&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    ArcReversal::ArcReversal(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_REVERSAL, node1, node2) {}

    /// copy constructor
    ArcReversal::ArcReversal(const ArcReversal& from) noexcept : GraphChange(from) {}

    /// move constructor
    ArcReversal::ArcReversal(ArcReversal&& from) noexcept : GraphChange(std::move(from)) {}

    /// destructor
    ArcReversal::~ArcReversal() noexcept {}

    /// copy operator
    ArcReversal& ArcReversal::operator=(const ArcReversal& from) noexcept = default;

    /// move operator
    ArcReversal& ArcReversal::operator=(ArcReversal&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    ArcTriangleDeletion1::ArcTriangleDeletion1(NodeId node1, NodeId node2, NodeId node3) noexcept :
        GraphChange(GraphChangeType::ARC_TRIANGLE_DELETION1, node1, node2, node3) {}

    /// copy constructor
    ArcTriangleDeletion1::ArcTriangleDeletion1(const ArcTriangleDeletion1& from) noexcept :
        GraphChange(from) {}

    /// move constructor
    ArcTriangleDeletion1::ArcTriangleDeletion1(ArcTriangleDeletion1&& from) noexcept :
        GraphChange(std::move(from)) {}

    /// destructor
    ArcTriangleDeletion1::~ArcTriangleDeletion1() noexcept {}

    /// copy operator
    ArcTriangleDeletion1&
        ArcTriangleDeletion1::operator=(const ArcTriangleDeletion1& from) noexcept = default;

    /// move operator
    ArcTriangleDeletion1& ArcTriangleDeletion1::operator=(ArcTriangleDeletion1&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    ArcTriangleDeletion2::ArcTriangleDeletion2(NodeId node1, NodeId node2, NodeId node3) noexcept :
        GraphChange(GraphChangeType::ARC_TRIANGLE_DELETION2, node1, node2, node3) {}

    /// copy constructor
    ArcTriangleDeletion2::ArcTriangleDeletion2(const ArcTriangleDeletion2& from) noexcept :
        GraphChange(from) {}

    /// move constructor
    ArcTriangleDeletion2::ArcTriangleDeletion2(ArcTriangleDeletion2&& from) noexcept :
        GraphChange(std::move(from)) {}

    /// destructor
    ArcTriangleDeletion2::~ArcTriangleDeletion2() noexcept {}

    /// copy operator
    ArcTriangleDeletion2&
        ArcTriangleDeletion2::operator=(const ArcTriangleDeletion2& from) noexcept = default;

    /// move operator
    ArcTriangleDeletion2& ArcTriangleDeletion2::operator=(ArcTriangleDeletion2&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    EdgeAddition::EdgeAddition(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::EDGE_ADDITION,
                    std::min(node1, node2),
                    std::max(node1, node2)) {}

    /// copy constructor
    EdgeAddition::EdgeAddition(const EdgeAddition& from) noexcept : GraphChange(from) {}

    /// move constructor
    EdgeAddition::EdgeAddition(EdgeAddition&& from) noexcept : GraphChange(std::move(from)) {}

    /// destructor
    EdgeAddition::~EdgeAddition() noexcept {}

    /// copy operator
    EdgeAddition& EdgeAddition::operator=(const EdgeAddition& from) noexcept = default;

    /// move operator
    EdgeAddition& EdgeAddition::operator=(EdgeAddition&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// default constructor
    EdgeDeletion::EdgeDeletion(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::EDGE_DELETION,
                    std::min(node1, node2),
                    std::max(node1, node2)) {}

    /// copy constructor
    EdgeDeletion::EdgeDeletion(const EdgeDeletion& from) noexcept : GraphChange(from) {}

    /// move constructor
    EdgeDeletion::EdgeDeletion(EdgeDeletion&& from) noexcept : GraphChange(std::move(from)) {}

    /// destructor
    EdgeDeletion::~EdgeDeletion() noexcept {}

    /// copy operator
    EdgeDeletion& EdgeDeletion::operator=(const EdgeDeletion& from) noexcept = default;

    /// move operator
    EdgeDeletion& EdgeDeletion::operator=(EdgeDeletion&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

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
