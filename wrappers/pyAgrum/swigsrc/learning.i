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
%ignore gum::learning::BNLearner::setSliceOrder(const NodeProperty< unsigned int >& slice_order);
%ignore gum::learning::BNLearner::setSliceOrder(const std::vector< std::vector< std::string > >& slices);
%ignore gum::learning::BNLearner::useK2(const gum::Sequence< gum::NodeId >& order);
%ignore gum::learning::BNLearner::useK2(const std::vector< gum::NodeId >& order);
%ignore gum::learning::BNLearner::setForbiddenArcs(const gum::ArcSet& set);
%ignore gum::learning::BNLearner::addForbiddenArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::eraseForbiddenArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::setPossibleEdges(const gum::EdgeSet& set);
%ignore gum::learning::BNLearner::addPossibleEdge(const gum::Edge& edge);
%ignore gum::learning::BNLearner::erasePossibleEdge(const gum::Edge& edge);
%ignore gum::learning::BNLearner::setMandatoryArcs(const gum::ArcSet& set);
%ignore gum::learning::BNLearner::addMandatoryArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::eraseMandatoryArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::learnParameters(const gum::DAG& dag);
%ignore gum::learning::BNLearner::learnParameters(const gum::DAG& dag);

%extend gum::learning::BNLearner< double > {
  PyObject *chi2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->chi2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }
  PyObject *G2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->G2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }

  void setSliceOrder(PyObject * l) {
    gum::NodeProperty< gum::Size > ranks;

    if (PyList_Check(l) == 0) {
      PyErr_SetString(PyExc_TypeError,
                      "arg must be a sequence (of sequences of int or string)");
      return;
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* rows = PyList_GetItem(l, i);
      if (PyList_Check(rows) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "arg must be a sequence of sequences (of int or string)");
        return;
      }

      for (Py_ssize_t j = 0; j < PySequence_Size(rows); j++) {
        PyObject* row = PyList_GetItem(rows, j);
        if (PyInt_Check(row) != 0) {
          ranks.insert(PyInt_AsLong(row), i);
          continue;
        }
        std::string n=PyAgrumHelper::stringFromPyObject(row);
        if (n!="") {
          ranks.insert($self->idFromName(n),i);
          continue;
        }

        PyErr_SetString(PyExc_TypeError,
                        "arg must be a sequence of sequence of int or string");
        return;
      }
    }
    $self->setSliceOrder(ranks);
  }

  void useK2(PyObject * l) {
    std::vector< gum::NodeId > v;

    if (PyList_Check(l) == 0) {
      PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
      return;
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* row = PyList_GetItem(l, i);
      if (PyInt_Check(row) == 0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence of int");
        return;
      }
      v.push_back((gum::NodeId)PyInt_AsLong(row));
    }

    $self->useK2(v);
  }

  PyObject* latentVariables() {
    return PyAgrumHelper::PyListFromArcVect(self->latentVariables());
  }
}


%pythonappend gum::learning::BNLearner< double >::learnMixedStructure %{
        bn=BayesNet()
        for i in range(len(self.names())):
          bn.add(self.nameFromId(i),2)
        ge=EssentialGraph(bn,val)
        ge._bn=bn
        return ge
%}
