
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
  INLINE BarrenNodesFinder::BarrenNodesFinder(const DAG* dag) : __dag(dag) {
    // for debugging purposes
    GUM_CONSTRUCTOR(BarrenNodesFinder);
  }


  /// copy constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(const BarrenNodesFinder& from) :
      __dag(from.__dag), __observed_nodes(from.__observed_nodes),
      __target_nodes(from.__target_nodes) {
    // for debugging purposes
    GUM_CONS_CPY(BarrenNodesFinder);
  }


  /// move constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder(BarrenNodesFinder&& from) noexcept :
      __dag(from.__dag), __observed_nodes(from.__observed_nodes),
      __target_nodes(from.__target_nodes) {
    // for debugging purposes
    GUM_CONS_MOV(BarrenNodesFinder);
  }


  /// destructor
  INLINE BarrenNodesFinder::~BarrenNodesFinder() {
    // for debugging purposes
    GUM_DESTRUCTOR(BarrenNodesFinder);
  }


  /// copy operator
  INLINE BarrenNodesFinder& BarrenNodesFinder::
                            operator=(const BarrenNodesFinder& from) {
    if (this != &from) {
      __dag = from.__dag;
      __observed_nodes = from.__observed_nodes;
      __target_nodes = from.__target_nodes;
    }
    return *this;
  }


  /// move operator
  INLINE BarrenNodesFinder& BarrenNodesFinder::
                            operator=(BarrenNodesFinder&& from) {
    if (this != &from) {
      __dag = from.__dag;
      __observed_nodes = from.__observed_nodes;
      __target_nodes = from.__target_nodes;
    }
    return *this;
  }


  /// sets a new DAG
  INLINE void BarrenNodesFinder::setDAG(const DAG* new_dag) { __dag = new_dag; }


  /// sets the observed nodes in the DAG
  INLINE void BarrenNodesFinder::setEvidence(const NodeSet* observed_nodes) {
    __observed_nodes = observed_nodes;
  }


  /// sets the set of target nodes we are interested in
  INLINE void BarrenNodesFinder::setTargets(const NodeSet* target_nodes) {
    __target_nodes = target_nodes;
  }


} /* namespace gum */
