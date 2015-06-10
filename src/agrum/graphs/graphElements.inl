/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
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
/** @file
 * @brief Inlined implementation of directed and undirected edges
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <iostream>
#include <algorithm>

#include <agrum/config.h>

// to facilitate parsing
#include <agrum/graphs/graphElements.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                GENERIC UNDIRECTED EDGES IMPLEMENTATION              === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor

  INLINE Edge::Edge(NodeId aN1, NodeId aN2)
      : n1(std::min(aN1, aN2)), n2(std::max(aN1, aN2)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Edge);
  }

  /// copy constructor

  INLINE Edge::Edge(const Edge &src) : n1(src.n1), n2(src.n2) {
    // for debugging purposes
    GUM_CONS_CPY(Edge);
  }

  /// copy operator

  INLINE Edge &Edge::operator=(const Edge &src) {
    // for debugging purposes
    GUM_OP_CPY(Edge);
    n1 = src.n1;
    n2 = src.n2;
    return *this;
  }

  /// destructor

  INLINE Edge::~Edge() {
    // for debugging purposes
    GUM_DESTRUCTOR(Edge);
  }

  /// returns a boolean indicating whether the edge is directed

  INLINE bool Edge::isDirected() const { return false; }

  /// returns an extremal node of an edge given the ID of the other one

  INLINE NodeId Edge::other(NodeId id) const {
    if (id == n1)
      return n2;
    else if (id == n2)
      return n1;
    else {
      GUM_ERROR(IdError, id << " does not belong to this edge");
    }
  }

  /// returns one extremal node ID (whichever one it is is unspecified)

  INLINE NodeId Edge::first() const { return n1; }

  /// returns the second extremal node

  INLINE NodeId Edge::second() const { return n2; }

  /// check if two undirected edges are equal

  INLINE bool Edge::operator==(const Edge &src) const {
    return ((n1 == src.n1) && (n2 == src.n2));
  }

  /// check if two undirected edges are different

  INLINE bool Edge::operator!=(const Edge &src) const {
    return ((n1 != src.n1) || (n2 != src.n2));
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GENERIC DIRECTED EDGES IMPLEMENTATION               === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor.

  INLINE Arc::Arc(NodeId tail, NodeId head) : n1(tail), n2(head) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Arc);
  }

  /// copy constructor

  INLINE Arc::Arc(const Arc &src) : n1(src.n1), n2(src.n2) {
    // for debugging purposes
    GUM_CONS_CPY(Arc);
  }

  /// copy operator

  INLINE Arc &Arc::operator=(const Arc &src) {
    // for debugging purposes
    GUM_OP_CPY(Arc);
    n1 = src.n1;
    n2 = src.n2;
    return *this;
  }

  /// destructor

  INLINE Arc::~Arc() {
    // for debugging purposes
    GUM_DESTRUCTOR(Arc);
  }

  /// returns the tail of the arc

  INLINE NodeId Arc::tail() const { return n1; }

  /// modifies the tail of the arc

  INLINE void Arc::__setTail(NodeId id) { n1 = id; }

  /// returns the head of the arc

  INLINE NodeId Arc::head() const { return n2; }

  /// modifies the head of the arc

  INLINE void Arc::__setHead(NodeId id) { n2 = id; }

  /// returns a boolean indicating whether the arc is directed

  INLINE bool Arc::isDirected() const { return true; }

  /// returns an extremal node of an edge given the ID of the other one

  INLINE NodeId Arc::other(NodeId id) const {
    if (id == n1)
      return n2;
    else if (id == n2)
      return n1;
    else {
      GUM_ERROR(IdError, id << " does not belong to this arc");
    }
  }

  /// returns one extremal node ID (whichever one it is is unspecified)

  INLINE NodeId Arc::first() const { return n1; }

  /// returns the second extremal node

  INLINE NodeId Arc::second() const { return n2; }

  /// check if two arcs are equal

  INLINE bool Arc::operator==(const Arc &src) const {
    return ((n1 == src.n1) && (n2 == src.n2));
  }

  /// check if two arcs are different

  INLINE bool Arc::operator!=(const Arc &src) const {
    return ((n1 != src.n1) || (n2 != src.n2));
  }

  /// reverses the direction of the arc

  INLINE void Arc::operator-() {
    NodeId n_temp = n1;
    n1 = n2;
    n2 = n_temp;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
