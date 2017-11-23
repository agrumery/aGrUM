/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
    for (const NodeId node : ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }
    // compute the orientation matrix
    // no edges so these stay null
    __true_edge = 0;
    __wrong_edge_arc = 0;
    __wrong_edge_none = 0;
    __wrong_arc_edge = 0;
    __wrong_none_edge = 0;
    // these will be filled
    __true_arc = 0;
    __true_none = 0;
    __misoriented_arc = 0;
    __wrong_arc_none = 0;
    __wrong_none_arc = 0;

    for (const Arc& arc : ref.arcs()) {
      if (test.existsArc(arc)) {
        ++__true_arc;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++__misoriented_arc;
      } else {
        ++__wrong_none_arc;
      }
    }
    for (const Arc& arc : test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail())) {
        ++__wrong_arc_none;
      }
    }
    // TN = #possible arcs - #existing arcs
    __true_none = ref.size() * (ref.size() - 1) - __true_arc - __misoriented_arc -
                  __wrong_arc_none - __wrong_none_arc;
  }

  void StructuralComparator::compare(const UndiGraph& ref, const UndiGraph& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node : ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }
    // compute the orientation matrix
    // no arcs so these stay null
    __true_arc = 0;
    __misoriented_arc = 0;
    __wrong_arc_none = 0;
    __wrong_none_arc = 0;
    __wrong_edge_arc = 0;
    __wrong_arc_edge = 0;
    // these will be filled
    __true_edge = 0;
    __true_none = 0;
    __wrong_edge_none = 0;
    __wrong_none_edge = 0;

    for (const Edge& edge : ref.edges()) {
      if (test.existsEdge(edge)) {
        ++__true_edge;
      } else {
        ++__wrong_none_edge;
      }
    }
    for (const Edge& edge : test.edges()) {
      if (!ref.existsEdge(edge)) {
        ++__wrong_edge_none;
      }
    }
    // TN = #possible edges - #existing edges
    __true_none = ref.size() * (ref.size() - 1) / 2 - __true_edge -
                  __wrong_edge_none - __wrong_none_edge;
  }

  void StructuralComparator::compare(const MixedGraph& ref,
                                     const MixedGraph& test) {
    if (ref.size() != test.size()) {
      GUM_ERROR(OperationNotAllowed, "Graphs of different sizes");
    }
    for (const NodeId node : ref.asNodeSet()) {
      if (!test.existsNode(node)) {
        GUM_ERROR(InvalidNode, "Test doesn't contain all nodes from ref");
      }
    }

    // compute the orientation matrix
    __true_arc = 0;
    __true_edge = 0;
    __true_none = 0;
    __misoriented_arc = 0;
    __wrong_arc_edge = 0;
    __wrong_arc_none = 0;
    __wrong_edge_arc = 0;
    __wrong_edge_none = 0;
    __wrong_none_arc = 0;
    __wrong_none_edge = 0;

    for (const Arc& arc : ref.arcs()) {
      if (test.existsArc(arc)) {
        ++__true_arc;
      } else if (test.existsArc(arc.head(), arc.tail())) {
        ++__misoriented_arc;
      } else if (test.existsEdge(arc.tail(), arc.head())) {
        ++__wrong_edge_arc;
      } else {
        ++__wrong_none_arc;
      }
    }
    for (const Edge& edge : ref.edges()) {
      if (test.existsEdge(edge)) {
        ++__true_edge;
      } else if (test.existsArc(edge.first(), edge.second()) ||
                 test.existsArc(edge.second(), edge.first())) {
        ++__wrong_arc_edge;
      } else {
        ++__wrong_none_edge;
      }
    }
    for (const Arc& arc : test.arcs()) {
      if (!ref.existsArc(arc) && !ref.existsArc(arc.head(), arc.tail()) &&
          !ref.existsEdge(arc.tail(), arc.head())) {
        ++__wrong_arc_none;
      }
    }
    for (const Edge& edge : test.edges()) {
      if (!ref.existsEdge(edge) && !ref.existsArc(edge.first(), edge.second()) &&
          !ref.existsArc(edge.second(), edge.first())) {
        ++__wrong_edge_none;
      }
    }
    // TN = #possible edges - #existing edges
    __true_none = ref.size() * (ref.size() - 1) / 2 - __true_edge -
                  __wrong_edge_none - __wrong_none_edge - __true_arc -
                  __misoriented_arc - __wrong_arc_none - __wrong_none_arc;
  }

  double StructuralComparator::precision_skeleton() const {
    double tp, fp, precision;
    tp = __true_arc + __misoriented_arc + __true_edge + __wrong_edge_arc +
         __wrong_arc_edge;
    fp = __wrong_arc_none + __wrong_edge_none;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall_skeleton() const {
    double tp, fn, recall;
    tp = __true_arc + __misoriented_arc + __true_edge + __wrong_edge_arc +
         __wrong_arc_edge;
    fn = __wrong_none_arc + __wrong_none_edge;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score_skeleton() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = __true_arc + __misoriented_arc + __true_edge + __wrong_edge_arc +
         __wrong_arc_edge;
    fp = __wrong_arc_none + __wrong_edge_none;
    fn = __wrong_none_arc + __wrong_none_edge;

    precision = tp / (tp + fp);
    recall = tp / (tp + fn);
    f_score = 2 * precision * recall / (precision + recall);
    return f_score;
  }

  double StructuralComparator::precision() const {
    double tp, fp, precision;
    tp = __true_arc + __true_edge;
    fp = __wrong_edge_arc + __wrong_arc_edge + __wrong_arc_none +
         __wrong_edge_none + __misoriented_arc;
    precision = tp / (tp + fp);
    return precision;
  }

  double StructuralComparator::recall() const {
    double tp, fn, recall;
    tp = __true_arc + __true_edge;
    fn = __wrong_none_arc + __wrong_none_edge;
    recall = tp / (tp + fn);
    return recall;
  }

  double StructuralComparator::f_score() const {
    double tp, fp, fn, precision, recall, f_score;
    tp = __true_arc + __true_edge;
    fp = __wrong_edge_arc + __wrong_arc_edge + __wrong_arc_none +
         __wrong_edge_none + __misoriented_arc;
    fn = __wrong_none_arc + __wrong_none_edge;

    precision = tp / (tp + fp);
    recall = tp / (tp + fn);

    f_score = 2 * precision * recall / (precision + recall);
    return f_score;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
