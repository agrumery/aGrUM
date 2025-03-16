/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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
  INLINE BarrenNodesFinder::~BarrenNodesFinder() {   // for debugging purposes
    GUM_DESTRUCTOR(BarrenNodesFinder)
  }

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
