/**
*
*  Copyright 2005-2023 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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
// defines some macro for MeekRules classe
%extend gum::MeekRules {
  gum::PDAG orientToPDAG(gum::MixedGraph graph, PyObject* latentCouples=nullptr) {
    std::vector< gum::Arc > _latentCouples_;
    if (latentCouples!=nullptr) {
      _latentCouples_=PyAgrumHelper::populateArcVectFromPyList(latentCouples);
    }

    return self->orientToPDAG(graph,_latentCouples_);
  }

  gum::DAG orientToDAG(gum::MixedGraph graph, PyObject* latentCouples=nullptr) {
    std::vector< gum::Arc > _latentCouples_;
    if (latentCouples!=nullptr) {
      _latentCouples_=PyAgrumHelper::populateArcVectFromPyList(latentCouples);
    }

    return self->orientToDAG(graph,_latentCouples_);
  }

  void orientAllEdges(gum::MixedGraph& graph, PyObject* latentCouples=nullptr) {
    std::vector< gum::Arc > _latentCouples_;
    if (latentCouples!=nullptr) {
      _latentCouples_=PyAgrumHelper::populateArcVectFromPyList(latentCouples);
    }

    self->orientAllEdges(graph,_latentCouples_);
  }

  void propagatesOrientations(gum::MixedGraph& graph, PyObject* latentCouples=nullptr) {
    std::vector< gum::Arc > _latentCouples_;
    if (latentCouples!=nullptr) {
      _latentCouples_=PyAgrumHelper::populateArcVectFromPyList(latentCouples);
    }

    self->propagatesOrientations(graph,_latentCouples_);
  }

}
%ignore gum::MeekRules::orientToPDAG(gum::MixedGraph graph, std::vector< Arc >& _latentCouples_);
%ignore gum::MeekRules::orientToDAG(gum::MixedGraph graph, std::vector< Arc >& _latentCouples_);
%ignore gum::MeekRules::orientAllEdges(gum::MixedGraph& graph, std::vector< Arc >& _latentCouples_);
%ignore gum::MeekRules::propagatesOrientations(gum::MixedGraph& graph, std::vector< Arc >& _latentCouples_);
