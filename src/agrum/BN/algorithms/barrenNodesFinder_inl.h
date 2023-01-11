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


namespace gum {


  /// default constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(const DAG* dag) :
      _dag_(dag) {   // for debugging purposes
    GUM_CONSTRUCTOR(BarrenNodesFinder);
  }


  /// copy constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(const BarrenNodesFinder& from) :
      _dag_(from._dag_), _observed_nodes_(from._observed_nodes_),
      _target_nodes_(from._target_nodes_) {   // for debugging purposes
    GUM_CONS_CPY(BarrenNodesFinder);
  }


  /// move constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(BarrenNodesFinder&& from) noexcept :
      _dag_(from._dag_), _observed_nodes_(from._observed_nodes_),
      _target_nodes_(from._target_nodes_) {
    // for debugging purposes
    GUM_CONS_MOV(BarrenNodesFinder);
  }


  /// destructor
  INLINE BarrenNodesFinder::~BarrenNodesFinder(){// for debugging purposes
                                                 GUM_DESTRUCTOR(BarrenNodesFinder)}


  /// copy operator
  INLINE BarrenNodesFinder& BarrenNodesFinder::operator=(const BarrenNodesFinder& from) {
    if (this != &from) {
      _dag_            = from._dag_;
      _observed_nodes_ = from._observed_nodes_;
      _target_nodes_   = from._target_nodes_;
    }
    return *this;
  }


  /// move operator
  INLINE BarrenNodesFinder& BarrenNodesFinder::operator=(BarrenNodesFinder&& from) {
    if (this != &from) {
      _dag_            = from._dag_;
      _observed_nodes_ = from._observed_nodes_;
      _target_nodes_   = from._target_nodes_;
    }
    return *this;
  }


  /// sets a new DAG
  INLINE void BarrenNodesFinder::setDAG(const DAG* new_dag) { _dag_ = new_dag; }


  /// sets the observed nodes in the DAG
  INLINE void BarrenNodesFinder::setEvidence(const NodeSet* observed_nodes) {
    _observed_nodes_ = observed_nodes;
  }


  /// sets the set of target nodes we are interested in
  INLINE void BarrenNodesFinder::setTargets(const NodeSet* target_nodes) {
    _target_nodes_ = target_nodes;
  }


} /* namespace gum */
