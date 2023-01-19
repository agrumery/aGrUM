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
%ignore gum::learning::BNLearner::setSliceOrder(const NodeProperty< unsigned int >& slice_order);
%ignore gum::learning::BNLearner::setSliceOrder(const std::vector< std::vector< std::string > >& slices);
%ignore gum::learning::BNLearner<double>::useK2(const gum::Sequence< gum::NodeId >& order);
%ignore gum::learning::BNLearner<double>::useK2(const std::vector< gum::NodeId >& order);
%ignore gum::learning::BNLearner::setForbiddenArcs(const gum::ArcSet& set) const;
%ignore gum::learning::BNLearner::addForbiddenArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::eraseForbiddenArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::setPossibleEdges(const gum::EdgeSet& set);
%ignore gum::learning::BNLearner::addPossibleEdge(const gum::Edge& edge);
%ignore gum::learning::BNLearner::erasePossibleEdge(const gum::Edge& edge);
%ignore gum::learning::BNLearner::setMandatoryArcs(const gum::ArcSet& set);
%ignore gum::learning::BNLearner::addMandatoryArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::eraseMandatoryArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::learnParameters(const gum::DAG& dag);
%ignore gum::learning::BNLearner::state() const;

%pythonprepend gum::learning::BNLearner<double>::BNLearner %{
  if not type(args[0]) is str:
    if hasattr(args[0],"to_csv"):
        import tempfile
        csvfile = tempfile.NamedTemporaryFile(delete=False)
        tmpfilename = csvfile.name
        csvfilename = tmpfilename + ".csv"
        csvfile.close()
        args[0].to_csv(csvfilename,na_rep="?",index=False)

        self.__init__(csvfilename,*args[1:])
        return

%}

%define SETPROP_THEN_RETURN_SELF(methodname)
%pythonappend gum::learning::BNLearner<double>::methodname %{
        return self
%}
%enddef

SETPROP_THEN_RETURN_SELF(setNumberOfThreads);

%extend gum::learning::BNLearner< double > {
  PyObject *chi2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->chi2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }
  PyObject *G2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->G2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }

  gum::learning::BNLearner< double >& setSliceOrder(PyObject * l) {
    gum::NodeProperty< gum::Size > ranks;

    if (PyList_Check(l) == 0) {
      PyErr_SetString(PyExc_TypeError,
                      "arg must be a sequence (of sequences of int or string)");
      return  *$self;
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* rows = PyList_GetItem(l, i);
      if (PyList_Check(rows) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "arg must be a sequence of sequences (of int or string)");
        return  *$self;
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
        return  *$self;
      }
    }
    $self->setSliceOrder(ranks);
    return  *$self;
  }

  gum::learning::BNLearner< double >& useK2(PyObject * l) {
    std::vector< gum::NodeId > v;

    if (PyList_Check(l) == 0) {
      PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
      return  *$self;
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* row = PyList_GetItem(l, i);
      if (PyInt_Check(row) == 0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence of int");
        return  *$self;
      }
      v.push_back((gum::NodeId)PyInt_AsLong(row));
    }

    $self->useK2(v);
    return *$self;
  }

  PyObject* latentVariables() {
    return PyAgrumHelper::PyListFromArcVect(self->latentVariables());
  }

  PyObject* state() {
    PyObject* res=PyDict_New();
    for(const auto& tuple: self->state()) {
      PyDict_SetItemString(res,
                           std::get<0>(tuple).c_str(),
                           Py_BuildValue("(ss)",std::get<1>(tuple).c_str(),std::get<2>(tuple).c_str())
                          );
    }

    return res;
  }

%pythoncode {
def pseudoCount(self,vars):
    """ access to pseudo-count (priors taken into account)

    Parameters
    ----------
    vars : list[str]
      a list of name of vars to add in the pseudo_count

    Returns
    -------
    a Potential containing this pseudo-counts
    """
    p=Potential()
    lv=list()
    for i in vars:
        if type(i) is str:
            name=i
        else:
            name=self.nameFromId(i)
        p.add(RangeVariable(name,name,0,self.domainSize(i)-1))
        lv.append(name)
    p.fillWith(self.rawPseudoCount(lv))
    return p

def fitParameters(self,bn):
  """
  Easy shortcut to LearnParameters method. fitParameters uses self to direcuptly populate the CPTs of bn.0

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a BN which will directly have its parameters learned.

  """
  if set(self.names())!=bn.names():
    raise Exception("Not the same variable names in the database and in the BN")

  d=DAG()
  for n in bn.names():
    d.addNodeWithId(self.idFromName(n))
  for i1,i2 in bn.arcs():
    d.addArc(self.idFromName(bn.variable(i1).name()),self.idFromName(bn.variable(i2).name()))
  tmp=self.learnParameters(d)
  for n in tmp.names():
    bn.cpt(n).fillWith(tmp.cpt(n))
  return self

def learnEssentialGraph(self):
  bn=BayesNet()
  for i in range(len(self.names())):
    bn.add(self.nameFromId(i),2)
  ge=EssentialGraph(bn,self.learnPDAG())
  ge._bn=bn
  return ge

  }
};
