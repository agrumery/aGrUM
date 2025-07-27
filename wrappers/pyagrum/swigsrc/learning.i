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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%ignore *::useK2(const gum::Sequence< gum::NodeId >& order);
%ignore *::useK2(const std::vector< gum::NodeId >& order);
%ignore *::setForbiddenArcs;
%ignore *::addForbiddenArc(const gum::Arc& arc);
%ignore *::eraseForbiddenArc(const gum::Arc& arc);
%ignore *::setPossibleEdges(const gum::EdgeSet& set);
%ignore *::addPossibleEdge(const gum::Edge& edge);
%ignore *::erasePossibleEdge(const gum::Edge& edge);
%ignore *::setMandatoryArcs;
%ignore *::addMandatoryArc(const gum::Arc& arc);
%ignore *::eraseMandatoryArc(const gum::Arc& arc);
%ignore gum::learning::BNLearner::state() const;
%ignore gum::learning::BNLearner::EM();
%ignore gum::learning::DAG2BNLearner::approximationScheme();



%pythoncode {
    import warnings
};

%pythonprepend gum::learning::BNLearner<double>::BNLearner %{
  if type(args[0]) is not str:
    if hasattr(args[0],"to_csv") or hasattr(args[0],"write_csv"):
        import tempfile
        csvfile = tempfile.NamedTemporaryFile(delete=False)
        tmpfilename = csvfile.name
        csvfilename = tmpfilename + ".csv"
        csvfile.close()

        if hasattr(args[0],"to_csv"):
          args[0].to_csv(csvfilename,na_rep="?",index=False)
        else:
          args[0].write_csv(csvfilename,null_value="?")

        self.__init__(csvfilename,*args[1:])
        return
    else:
      raise TypeError("first argument must be a string or a DataFrame")
%}

%define SETPROP_THEN_RETURN_SELF(methodname)
%pythonappend gum::learning::BNLearner<double>::methodname %{
        return self
%}
%enddef

SETPROP_THEN_RETURN_SELF(setNumberOfThreads);
SETPROP_THEN_RETURN_SELF(copyState);

%extend gum::learning::BNLearner< double > {
  PyObject *chi2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->chi2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }

  PyObject *G2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->G2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }

  bool _EM_warning() const {
    return $self->isUsingEM() &&
      ($self->EMStateApproximationScheme() == gum::learning::BNLearner< double >::ApproximationSchemeSTATE::Stopped);
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

    PyObject* pyval;
    for(const auto& tuple: self->state()) {
      pyval=Py_BuildValue("(ss)",std::get<1>(tuple).c_str(),std::get<2>(tuple).c_str());
      PyDict_SetItemString(res,std::get<0>(tuple).c_str(),pyval);
      Py_DecRef(pyval);
    }

    return res;
  }

  void setPossibleEdges(PyObject * soe) {
    gum::EdgeSet set;
    PyAgrumHelper::fillEdgeSetFromPyObject(set,soe);
    $self->setPossibleEdges(set);
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
    a Tensor containing this pseudo-counts
    """
    p=pyagrum.Tensor()
    lv=list()
    for i in vars:
        if type(i) is str:
            name=i
        else:
            name=self.nameFromId(i)
        p.add(pyagrum.RangeVariable(name,name,0,self.domainSize(i)-1))
        lv.append(name)
    p.fillWith(self.rawPseudoCount(lv))
    return p

def fitParameters(self,bn,take_into_account_score=True):
  if set(self.names())!=bn.names():
    raise Exception("Not the same variable names in the database and in the BN")

  tmp=self.learnParameters(bn,take_into_account_score)
  for n in tmp.names():
    bn.cpt(n).fillWith(tmp.cpt(n))
  return self

def learnEssentialGraph(self):
  """
  learn an essential graph from a file

  Returns
  -------
  pyagrum.EssentialGraph
    the learned essential graph
  """
  bn = BayesNet()
  for i in range(len(self.names())):
    bn.add(self.nameFromId(i),2)
  try:
    ge = EssentialGraph(bn,self.learnPDAG()) # for constraint-based methods
  except:
    bn = self.learnBN()
    ge = EssentialGraph(bn)  # for score-based methods

  ge._bn=bn

  return ge
  }
}


%pythonappend gum::learning::BNLearner<double>::learnParameters %{
  if self._EM_warning():
      warnings.warn("\nthe learnParameters's EM algorithm has completed prematurely due to a likelihood divergence\n", UserWarning)
%}
