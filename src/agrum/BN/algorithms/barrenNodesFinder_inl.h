/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


namespace gum {


  /// default constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(const DAG* dag) : dag__(dag) {
    // for debugging purposes
    GUM_CONSTRUCTOR(BarrenNodesFinder);
  }


  /// copy constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(const BarrenNodesFinder& from) :
      dag__(from.dag__), observed_nodes__(from.observed_nodes__),
      target_nodes__(from.target_nodes__) {
    // for debugging purposes
    GUM_CONS_CPY(BarrenNodesFinder);
  }


  /// move constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(BarrenNodesFinder&& from) noexcept :
      dag__(from.dag__), observed_nodes__(from.observed_nodes__),
      target_nodes__(from.target_nodes__) {
    // for debugging purposes
    GUM_CONS_MOV(BarrenNodesFinder);
  }


  /// destructor
  INLINE BarrenNodesFinder::~BarrenNodesFinder() {
    // for debugging purposes
    GUM_DESTRUCTOR(BarrenNodesFinder);
  }


  /// copy operator
  INLINE BarrenNodesFinder&
     BarrenNodesFinder::operator=(const BarrenNodesFinder& from) {
    if (this != &from) {
      dag__            = from.dag__;
      observed_nodes__ = from.observed_nodes__;
      target_nodes__   = from.target_nodes__;
    }
    return *this;
  }


  /// move operator
  INLINE BarrenNodesFinder&
     BarrenNodesFinder::operator=(BarrenNodesFinder&& from) {
    if (this != &from) {
      dag__            = from.dag__;
      observed_nodes__ = from.observed_nodes__;
      target_nodes__   = from.target_nodes__;
    }
    return *this;
  }


  /// sets a new DAG
  INLINE void BarrenNodesFinder::setDAG(const DAG* new_dag) { dag__ = new_dag; }


  /// sets the observed nodes in the DAG
  INLINE void BarrenNodesFinder::setEvidence(const NodeSet* observed_nodes) {
    observed_nodes__ = observed_nodes;
  }


  /// sets the set of target nodes we are interested in
  INLINE void BarrenNodesFinder::setTargets(const NodeSet* target_nodes) {
    target_nodes__ = target_nodes;
  }


} /* namespace gum */
