/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/BN/algorithms/structuralComparator.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  StructuralComparator::StructuralComparator() { GUM_CONSTRUCTOR(StructuralComparator); }

  /// destructor
  StructuralComparator::~StructuralComparator() { GUM_DESTRUCTOR(StructuralComparator); }

  void StructuralComparator::compare(const DiGraph& ref, const DiGraph& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }
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

  void StructuralComparator::compare(const UndiGraph& ref, const UndiGraph& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }
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

  void StructuralComparator::compare(const MixedGraph& ref, const MixedGraph& test) {
    if (ref.size() != test.size()) { GUM_ERROR(OperationNotAllowed, "Graphs of different sizes") }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref")
      }
    }

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

  double StructuralComparator::precision_skeleton() const {
    double tp, fp, precision;
    tp        = _true_arc_ + _misoriented_arc_ + _true_edge_ + _wrong_edge_arc_ + _wrong_arc_edge_;
    fp        = _wrong_arc_none_ + _wrong_edge_none_;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall_skeleton() const {
    double tp, fn, recall;
    tp     = _true_arc_ + _misoriented_arc_ + _true_edge_ + _wrong_edge_arc_ + _wrong_arc_edge_;
    fn     = _wrong_none_arc_ + _wrong_none_edge_;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score_skeleton() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = _true_arc_ + _misoriented_arc_ + _true_edge_ + _wrong_edge_arc_ + _wrong_arc_edge_;
    fp = _wrong_arc_none_ + _wrong_edge_none_;
    fn = _wrong_none_arc_ + _wrong_none_edge_;

    precision = tp / (tp + fp);
    recall    = tp / (tp + fn);
    f_score   = 2 * precision * recall / (precision + recall);
    return f_score;
  }

  double StructuralComparator::precision() const {
    double tp, fp, precision;
    tp = _true_arc_ + _true_edge_;
    fp = _wrong_edge_arc_ + _wrong_arc_edge_ + _wrong_arc_none_ + _wrong_edge_none_
       + _misoriented_arc_;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall() const {
    double tp, fn, recall;
    tp     = _true_arc_ + _true_edge_;
    fn     = _wrong_none_arc_ + _wrong_none_edge_;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = _true_arc_ + _true_edge_;
    fp = _wrong_edge_arc_ + _wrong_arc_edge_ + _wrong_arc_none_ + _wrong_edge_none_
       + _misoriented_arc_;
    fn = _wrong_none_arc_ + _wrong_none_edge_;

    precision = tp / (tp + fp);
    recall    = tp / (tp + fn);

    f_score = 2 * precision * recall / (precision + recall);
    return f_score;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
