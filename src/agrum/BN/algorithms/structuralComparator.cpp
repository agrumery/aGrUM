
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
  StructuralComparator::StructuralComparator() {
    GUM_CONSTRUCTOR(StructuralComparator);
  }

  /// destructor
  StructuralComparator::~StructuralComparator() {
    GUM_DESTRUCTOR(StructuralComparator);
  }

  void StructuralComparator::compare(const DiGraph& ref, const DiGraph& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }
    // compute the orientation matrix
    // no edges so these stay null
    true_edge__ = 0;
    wrong_edge_arc__ = 0;
    wrong_edge_none__ = 0;
    wrong_arc_edge__ = 0;
    wrong_none_edge__ = 0;
    // these will be filled
    true_arc__ = 0;
    true_none__ = 0;
    misoriented_arc__ = 0;
    wrong_arc_none__ = 0;
    wrong_none_arc__ = 0;

    for (const Arc& arc: ref.arcs()) {
      if (test.existsArc(arc)) {
        ++true_arc__;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++misoriented_arc__;
      } else {
        ++wrong_none_arc__;
      }
    }
    for (const Arc& arc: test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail())) {
        ++wrong_arc_none__;
      }
    }
    // TN = #possible arcs - #existing arcs
    true_none__ = ref.size() * (ref.size() - 1) - true_arc__ - misoriented_arc__
                  - wrong_arc_none__ - wrong_none_arc__;
  }

  void StructuralComparator::compare(const UndiGraph& ref, const UndiGraph& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }
    // compute the orientation matrix
    // no arcs so these stay null
    true_arc__ = 0;
    misoriented_arc__ = 0;
    wrong_arc_none__ = 0;
    wrong_none_arc__ = 0;
    wrong_edge_arc__ = 0;
    wrong_arc_edge__ = 0;
    // these will be filled
    true_edge__ = 0;
    true_none__ = 0;
    wrong_edge_none__ = 0;
    wrong_none_edge__ = 0;

    for (const Edge& edge: ref.edges()) {
      if (test.existsEdge(edge)) {
        ++true_edge__;
      } else {
        ++wrong_none_edge__;
      }
    }
    for (const Edge& edge: test.edges()) {
      if (!ref.existsEdge(edge)) { ++wrong_edge_none__; }
    }
    // TN = #possible edges - #existing edges
    true_none__ = ref.size() * (ref.size() - 1) / 2 - true_edge__
                  - wrong_edge_none__ - wrong_none_edge__;
  }

  void StructuralComparator::compare(const MixedGraph& ref,
                                     const MixedGraph& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node: ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }

    // compute the orientation matrix
    true_arc__ = 0;
    true_edge__ = 0;
    true_none__ = 0;
    misoriented_arc__ = 0;
    wrong_arc_edge__ = 0;
    wrong_arc_none__ = 0;
    wrong_edge_arc__ = 0;
    wrong_edge_none__ = 0;
    wrong_none_arc__ = 0;
    wrong_none_edge__ = 0;

    for (const Arc& arc: ref.arcs()) {
      if (test.existsArc(arc)) {
        ++true_arc__;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++misoriented_arc__;
      } else if (test.existsEdge(arc.tail(), arc.head())) {
        ++wrong_edge_arc__;
      } else {
        ++wrong_none_arc__;
      }
    }
    for (const Edge& edge: ref.edges()) {
      if (test.existsEdge(edge)) {
        ++true_edge__;
      } else if (test.existsArc(edge.first(), edge.second())
                 || test.existsArc(edge.second(), edge.first())) {
        ++wrong_arc_edge__;
      } else {
        ++wrong_none_edge__;
      }
    }
    for (const Arc& arc: test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail())
          && !ref.existsEdge(arc.tail(), arc.head())) {
        ++wrong_arc_none__;
      }
    }
    for (const Edge& edge: test.edges()) {
      if (!ref.existsEdge(edge) && !ref.existsArc(edge.first(), edge.second())
          && !ref.existsArc(edge.second(), edge.first())) {
        ++wrong_edge_none__;
      }
    }
    // TN = #possible edges - #existing edges
    true_none__ = ref.size() * (ref.size() - 1) / 2 - true_edge__
                  - wrong_edge_none__ - wrong_none_edge__ - true_arc__
                  - misoriented_arc__ - wrong_arc_none__ - wrong_none_arc__;
  }

  double StructuralComparator::precision_skeleton() const {
    double tp, fp, precision;
    tp = true_arc__ + misoriented_arc__ + true_edge__ + wrong_edge_arc__
         + wrong_arc_edge__;
    fp = wrong_arc_none__ + wrong_edge_none__;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall_skeleton() const {
    double tp, fn, recall;
    tp = true_arc__ + misoriented_arc__ + true_edge__ + wrong_edge_arc__
         + wrong_arc_edge__;
    fn = wrong_none_arc__ + wrong_none_edge__;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score_skeleton() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = true_arc__ + misoriented_arc__ + true_edge__ + wrong_edge_arc__
         + wrong_arc_edge__;
    fp = wrong_arc_none__ + wrong_edge_none__;
    fn = wrong_none_arc__ + wrong_none_edge__;

    precision = tp / (tp + fp);
    recall = tp / (tp + fn);
    f_score = 2 * precision * recall / (precision + recall);
    return f_score;
  }

  double StructuralComparator::precision() const {
    double tp, fp, precision;
    tp = true_arc__ + true_edge__;
    fp = wrong_edge_arc__ + wrong_arc_edge__ + wrong_arc_none__ + wrong_edge_none__
         + misoriented_arc__;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall() const {
    double tp, fn, recall;
    tp = true_arc__ + true_edge__;
    fn = wrong_none_arc__ + wrong_none_edge__;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = true_arc__ + true_edge__;
    fp = wrong_edge_arc__ + wrong_arc_edge__ + wrong_arc_none__ + wrong_edge_none__
         + misoriented_arc__;
    fn = wrong_none_arc__ + wrong_none_edge__;

    precision = tp / (tp + fp);
    recall = tp / (tp + fn);

    f_score = 2 * precision * recall / (precision + recall);
    return f_score;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
