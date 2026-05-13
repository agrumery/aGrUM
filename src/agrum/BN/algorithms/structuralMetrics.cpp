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


#include <agrum/BN/algorithms/structuralMetrics.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  StructuralMetrics::StructuralMetrics() { GUM_CONSTRUCTOR(StructuralMetrics); }

  /// destructor
  StructuralMetrics::~StructuralMetrics() { GUM_DESTRUCTOR(StructuralMetrics); }

  void StructuralMetrics::compare(const DiGraph& ref, const DiGraph& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }
    _size_ = ref.size();
    // compute the orientation matrix
    // no edges so these stay null
    _true_edge_       = 0;
    _wrong_edge_arc_  = 0;
    _wrong_edge_none_ = 0;
    _wrong_arc_edge_  = 0;
    _wrong_none_edge_ = 0;
    // these will be filled
    _true_arc_        = 0;
    _true_none_       = 0;
    _misoriented_arc_ = 0;
    _wrong_arc_none_  = 0;
    _wrong_none_arc_  = 0;

    for (const Arc& arc: ref.arcs()) {
      if (test.existsArc(arc)) {
        ++_true_arc_;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++_misoriented_arc_;
      } else {
        ++_wrong_none_arc_;
      }
    }
    for (const Arc& arc: test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail())) { ++_wrong_arc_none_; }
    }
    // TN = #possible arcs - #existing arcs
    _true_none_ = ref.size() * (ref.size() - 1) - _true_arc_ - _misoriented_arc_ - _wrong_arc_none_
                - _wrong_none_arc_;
  }

  void StructuralMetrics::compare(const UndiGraph& ref, const UndiGraph& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }
    _size_ = ref.size();
    // compute the orientation matrix
    // no arcs so these stay null
    _true_arc_        = 0;
    _misoriented_arc_ = 0;
    _wrong_arc_none_  = 0;
    _wrong_none_arc_  = 0;
    _wrong_edge_arc_  = 0;
    _wrong_arc_edge_  = 0;
    // these will be filled
    _true_edge_       = 0;
    _true_none_       = 0;
    _wrong_edge_none_ = 0;
    _wrong_none_edge_ = 0;

    for (const Edge& edge: ref.edges()) {
      if (test.existsEdge(edge)) {
        ++_true_edge_;
      } else {
        ++_wrong_none_edge_;
      }
    }
    for (const Edge& edge: test.edges()) {
      if (!ref.existsEdge(edge)) { ++_wrong_edge_none_; }
    }
    // TN = #possible edges - #existing edges
    _true_none_
        = ref.size() * (ref.size() - 1) / 2 - _true_edge_ - _wrong_edge_none_ - _wrong_none_edge_;
  }

  void StructuralMetrics::compare(const PDAG& ref, const PDAG& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }
    _size_ = ref.size();

    // compute the orientation matrix
    _true_arc_        = 0;
    _true_edge_       = 0;
    _true_none_       = 0;
    _misoriented_arc_ = 0;
    _wrong_arc_edge_  = 0;
    _wrong_arc_none_  = 0;
    _wrong_edge_arc_  = 0;
    _wrong_edge_none_ = 0;
    _wrong_none_arc_  = 0;
    _wrong_none_edge_ = 0;

    for (const Arc& arc: ref.arcs()) {
      if (test.existsArc(arc)) {
        ++_true_arc_;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++_misoriented_arc_;
      } else if (test.existsEdge(arc.tail(), arc.head())) {
        ++_wrong_edge_arc_;
      } else {
        ++_wrong_none_arc_;
      }
    }
    for (const Edge& edge: ref.edges()) {
      if (test.existsEdge(edge)) {
        ++_true_edge_;
      } else if (test.existsArc(edge.first(), edge.second())
                 || test.existsArc(edge.second(), edge.first())) {
        ++_wrong_arc_edge_;
      } else {
        ++_wrong_none_edge_;
      }
    }
    for (const Arc& arc: test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail())
          && !ref.existsEdge(arc.tail(), arc.head())) {
        ++_wrong_arc_none_;
      }
    }
    for (const Edge& edge: test.edges()) {
      if (!ref.existsEdge(edge) && !ref.existsArc(edge.first(), edge.second())
          && !ref.existsArc(edge.second(), edge.first())) {
        ++_wrong_edge_none_;
      }
    }
    // TN = #possible edges - #existing edges
    _true_none_ = ref.size() * (ref.size() - 1) / 2 - _true_edge_ - _wrong_edge_none_
                - _wrong_none_edge_ - _true_arc_ - _misoriented_arc_ - _wrong_arc_none_
                - _wrong_none_arc_;
  }

  double StructuralMetrics::tp_skeleton() const {
    return _true_arc_ + _misoriented_arc_ + _true_edge_ + _wrong_edge_arc_ + _wrong_arc_edge_;
  }

  double StructuralMetrics::fp_skeleton() const { return _wrong_arc_none_ + _wrong_edge_none_; }

  double StructuralMetrics::fn_skeleton() const { return _wrong_none_arc_ + _wrong_none_edge_; }

  double StructuralMetrics::tn_skeleton() const {
    // Cannot simply return _true_none_: it counts ordered pairs in compare(DiGraph,DiGraph)
    // and unordered pairs in compare(UndiGraph/UndiGraph) and compare(PDAG/PDAG),
    // so units are inconsistent.
    return _size_ * (_size_ - 1) / 2.0 - tp_skeleton() - fp_skeleton() - fn_skeleton();
  }

  double StructuralMetrics::precision_skeleton() const {
    return tp_skeleton() / (tp_skeleton() + fp_skeleton());
  }

  double StructuralMetrics::recall_skeleton() const {
    return tp_skeleton() / (tp_skeleton() + fn_skeleton());
  }

  double StructuralMetrics::f_score_skeleton() const {
    const double p = precision_skeleton();
    const double r = recall_skeleton();
    return 2 * p * r / (p + r);
  }

  double StructuralMetrics::shd_skeleton() const { return fp_skeleton() + fn_skeleton(); }

  double StructuralMetrics::tp() const { return _true_arc_ + _true_edge_; }

  double StructuralMetrics::fp() const {
    return _wrong_edge_arc_ + _wrong_arc_edge_ + _wrong_arc_none_ + _wrong_edge_none_
         + _misoriented_arc_;
  }

  double StructuralMetrics::fn() const { return _wrong_none_arc_ + _wrong_none_edge_; }

  double StructuralMetrics::tn() const { return _true_none_; }

  double StructuralMetrics::precision() const { return tp() / (tp() + fp()); }

  double StructuralMetrics::recall() const { return tp() / (tp() + fn()); }

  double StructuralMetrics::f_score() const {
    const double p = precision();
    const double r = recall();
    return 2 * p * r / (p + r);
  }

  double StructuralMetrics::shd() const { return fp() + fn(); }

  double StructuralMetrics::sid(const DAG& ref, const DAG& test) const {
    // Validate node sets match.
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "test does not contain all nodes from ref")
      }
    }

    // Pre-compute (strict) descendants in ref. DE[i] does NOT include i itself.
    NodeProperty< NodeSet > DE;
    for (const NodeId i: ref.asNodeSet()) {
      DE.insert(i, ref.descendants(i));
    }

    // Working copy of ref for the back-door mutation (mutate / restore per pair).
    DAG G = ref;

    double errors = 0.0;

    for (const NodeId i: ref.asNodeSet()) {
      const NodeSet& paG = ref.parents(i);
      const NodeSet& paH = test.parents(i);

      if (paG == paH) continue;

      for (const NodeId j: ref.asNodeSet()) {
        if (j == i) continue;

        const bool ijGNull = !DE[i].contains(j);
        const bool ijHNull = paH.contains(j);

        // Case 1 : H wrongly predicts a null effect.
        if (!ijGNull && ijHNull) {
          errors += 1;
          continue;
        }
        // Case 2 : both predict null effect → OK.
        if (ijGNull && ijHNull) { continue; }

        // Part (2a) of Lemma 5 : no child c of i on a directed path to j
        //                       can be in paH nor have descendants in paH.
        bool violated_2a = false;
        for (const NodeId c: ref.children(i)) {
          if (c == j || DE[c].contains(j)) {
            // c is on a directed path i → ... → j (reflexive case if c == j)
            if (paH.contains(c)) {
              violated_2a = true;
              break;
            }
            for (const NodeId z: paH) {
              if (DE[c].contains(z)) {
                violated_2a = true;
                break;
              }
            }
            if (violated_2a) break;
          }
        }
        if (violated_2a) {
          errors += 1;
          continue;
        }

        // Part (2b) : test d-separation in the mutated G,
        //            with arcs i → c removed for every c on a directed path to j.
        std::vector< NodeId > arcs_removed;
        for (const NodeId c: ref.children(i)) {
          if (c == j || DE[c].contains(j)) {
            G.eraseArc(Arc(i, c));
            arcs_removed.push_back(c);
          }
        }

        if (!G.dSeparation(i, j, paH)) { errors += 1; }

        // Restore the removed arcs so that G is back to ref's structure.
        for (const NodeId c: arcs_removed) {
          G.addArc(i, c);
        }
      }
    }

    return errors;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
