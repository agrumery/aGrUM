/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

// Renamed so that the public methods (added in %pythoncode below) keep named arguments and default their
// optional parameters to None: the library then uses the keys of the section "learning" of
// pyagrum.config (see the section learning of the configuration), while an explicit value is used as is.
%rename(_useEM) gum::learning::BNLearner::useEM;
%rename(_useEMWithRateCriterion) gum::learning::BNLearner::useEMWithRateCriterion;
%rename(_useEMWithDiffCriterion) gum::learning::BNLearner::useEMWithDiffCriterion;
%rename(_useBDeuPrior) gum::learning::BNLearner::useBDeuPrior;
%rename(_useSmoothingPrior) gum::learning::BNLearner::useSmoothingPrior;
%rename(_useDirichletPrior) gum::learning::BNLearner::useDirichletPrior;
%rename(_useLocalSearchWithTabuList) gum::learning::BNLearner::useLocalSearchWithTabuList;

%define SETPROP_THEN_RETURN_SELF(methodname)
%pythonappend gum::learning::BNLearner<double>::methodname %{
        return self
%}
%enddef

SETPROP_THEN_RETURN_SELF(setNumberOfThreads);
SETPROP_THEN_RETURN_SELF(copyState);
SETPROP_THEN_RETURN_SELF(useFCI);
SETPROP_THEN_RETURN_SELF(useFCIChi2Test);
SETPROP_THEN_RETURN_SELF(useFCIG2Test);
SETPROP_THEN_RETURN_SELF(setFCIAlpha);
SETPROP_THEN_RETURN_SELF(setFCIMaxPathLength);
SETPROP_THEN_RETURN_SELF(setFCIExhaustiveSepSet);

%extend gum::learning::BNLearner< double > {
  PyAgrumTupleFF *chi2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
    std::pair<double,double> res=$self->chi2(var1,var2,knw);
    return Py_BuildValue("(dd)",res.first,res.second);
  }

  PyAgrumTupleFF *G2(const std::string& var1,const std::string& var2,const std::vector<std::string>& knw={}) {
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
                      "arg must be a list (of lists of int or string)");
      throw PythonErrorAlreadySet();
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* rows = PyList_GetItem(l, i);
      if (PyList_Check(rows) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "arg must be a list of lists (of int or string)");
        throw PythonErrorAlreadySet();
      }

      for (Py_ssize_t j = 0; j < PySequence_Size(rows); j++) {
        PyObject* row = PyList_GetItem(rows, j);
        if (PyLong_Check(row) != 0) {
          ranks.insert(PyLong_AsLong(row), i);
          continue;
        }
        std::string n=PyAgrumHelper::stringFromPyObject(row);
        if (n!="") {
          ranks.insert($self->idFromName(n),i);
          continue;
        }

        PyErr_SetString(PyExc_TypeError,
                        "arg must be a list of lists of int or string");
        throw PythonErrorAlreadySet();
      }
    }
    $self->setSliceOrder(ranks);
    return  *$self;
  }

  gum::learning::BNLearner< double >& useK2(PyObject * l) {
    std::vector< gum::NodeId > v;

    if (PyList_Check(l) == 0) {
      PyErr_SetString(PyExc_TypeError, "arg must be a list");
      throw PythonErrorAlreadySet();
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(l); i++) {
      PyObject* row = PyList_GetItem(l, i);
      if (PyLong_Check(row) == 0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a list of int");
        throw PythonErrorAlreadySet();
      }
      v.push_back((gum::NodeId)PyLong_AsLong(row));
    }

    $self->useK2(v);
    return *$self;
  }

  std::vector<gum::Arc> latentVariables() {
    return self->latentVariables();
  }

  PyAgrumDictState* state() {
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

def fitParameters(self, bn, take_into_account_score=True):
  """
  Fit the parameters of a BayesNet in place using this learner's database.

  Parameters
  ----------
  bn : pyagrum.BayesNet
      the network whose CPTs will be filled (structure must match the database variables)
  take_into_account_score : bool
      if True, include the implicit prior of the structure-learning score (e.g. K2) in parameter learning

  Returns
  -------
  pyagrum.BNLearner
      self
  """
  if not set(self.names()).issuperset(bn.names()):
    raise Exception(f"Some variables are in the BN but not in the data : {bn.names()-set(self.names())}")

  tmp=self.learnParameters(bn,take_into_account_score)
  for n in tmp.names():
    bn.cpt(bn.idFromName(n)).fillWith(tmp.cpt(n))
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

def useEM(self, epsilon: float, noise: float | None = None) -> "pyagrum.BNLearner":
  """
  Sets whether we use EM for parameter learning or not, depending on the value of epsilon.

  When epsilon is equal to 0.0, EM is forbidden, else EM is used for parameter learning
  whenever the database contains missing values. In this case, its stopping criterion
  is a threshold on the log-likelihood evolution rate, i.e., if llc and llo refer to
  the log-likelihoods at the current and previous EM steps respectively, EM will stop
  when (llc - llo) / llc drops below epsilon. If you wish to be more specific on which
  stopping criterion to use, you may prefer exploiting methods `useEMWithRateCriterion()`
  or `useEMWithDiffCriterion()`.

  Parameters
  ----------
  epsilon : float
    if epsilon>0 then EM is used and stops whenever the relative difference between two
    consecutive log-likelihoods (log-likelihood evolution rate) drops below epsilon.

    if epsilon=0.0 then EM is not used. But if you wish to forbid the use of EM, prefer
    executing Method `forbidEM()` rather than useEM(0.0) as it is more unequivocal.
  noise : float, optional
    During EM's initialization, the CPTs are randomly perturbed using the following formula:
    new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
    to interval [0,1]. By default, noise is given by `pyagrum.config['learning', 'emNoise']` (0.1).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `epsilon` is strictly negative or if `noise` does not belong to interval [0,1].
  """
  if noise is None:
    self._useEM(epsilon)
  else:
    self._useEM(epsilon, noise)
  return self

def useEMWithRateCriterion(self, epsilon: float, noise: float | None = None) -> "pyagrum.BNLearner":
  """
  Enforces that EM with the log-likelihood min evolution rate stopping criterion will be
  used for parameter learning when the dataset contains missing values.

  Parameters
  ----------
  epsilon : float
    epsilon sets the approximation stopping criterion: EM stops whenever the absolute
    value of the relative difference between two consecutive log-likelihoods drops below
    epsilon. Note that epsilon should be strictly positive.
  noise : float, optional
    During EM's initialization, the CPTs are randomly perturbed using the following formula:
    new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
    to interval [0,1]. By default, noise is given by `pyagrum.config['learning', 'emNoise']` (0.1).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `epsilon` is not strictly positive or if `noise` does not belong to interval [0,1].
  """
  if noise is None:
    self._useEMWithRateCriterion(epsilon)
  else:
    self._useEMWithRateCriterion(epsilon, noise)
  return self

def useEMWithDiffCriterion(self, epsilon: float, noise: float | None = None) -> "pyagrum.BNLearner":
  """
  Enforces that EM with the log-likelihood min difference criterion will be used for
  parameter learning whenever the dataset contains missing values.

  Parameters
  ----------
  epsilon : float
    epsilon sets the approximation stopping criterion: EM stops whenever the
    difference between two consecutive log-likelihoods drops below
    epsilon. Note that epsilon should be strictly positive.
  noise : float, optional
    During EM's initialization, the CPTs are randomly perturbed using the following formula:
    new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
    to interval [0,1]. By default, noise is given by `pyagrum.config['learning', 'emNoise']` (0.1).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `epsilon` is not strictly positive or if `noise` does not belong to interval [0,1].
  """
  if noise is None:
    self._useEMWithDiffCriterion(epsilon)
  else:
    self._useEMWithDiffCriterion(epsilon, noise)
  return self

def useBDeuPrior(self, weight: float | None = None) -> "pyagrum.BNLearner":
  """
  The BDeu prior adds weight to all the cells of the counting tables.
  In other words, it adds weight rows in the database with equally probable
  values.

  Parameters
  ----------
  weight : float, optional
    the prior weight, which must not be negative. By default, it is `pyagrum.config['learning', 'priorWeight']` (1.0).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `weight` is negative.
  """
  if weight is None:
    self._useBDeuPrior()
  else:
    self._useBDeuPrior(weight)
  return self

def useSmoothingPrior(self, weight: float | None = None) -> "pyagrum.BNLearner":
  """
  Use the prior smoothing.

  Parameters
  ----------
  weight : float, optional
    the weight of the smoothing, which must not be negative. By default, it is
    `pyagrum.config['learning', 'priorWeight']` (1.0).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `weight` is negative.
  """
  if weight is None:
    self._useSmoothingPrior()
  else:
    self._useSmoothingPrior(weight)
  return self

def useDirichletPrior(self, source, weight: float | None = None) -> "pyagrum.BNLearner":
  """
  Use the Dirichlet prior.

  Parameters
  ----------
  source : str|pyagrum.BayesNet
    the Dirichlet related source (filename of a database or a Bayesian network)
  weight : float, optional
    the weight of the prior (the 'size' of the corresponding 'virtual database'), which must not be negative.
    By default, it is `pyagrum.config['learning', 'priorWeight']` (1.0).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.

  Raises
  ------
  pyagrum.OutOfBounds
    if `weight` is negative.
  """
  if weight is None:
    self._useDirichletPrior(source)
  else:
    self._useDirichletPrior(source, weight)
  return self

def useLocalSearchWithTabuList(self, tabu_size: int | None = None, nb_decrease: int | None = None) -> "pyagrum.BNLearner":
  """
  Indicate that we wish to use a local search with tabu list

  Parameters
  ----------
  tabu_size : int, optional
    The size of the tabu list. By default, it is `pyagrum.config['learning', 'tabuListSize']` (100).
  nb_decrease : int, optional
    The max number of changes decreasing the score consecutively that we allow to apply.
    By default, it is `pyagrum.config['learning', 'tabuNbDecrease']` (2).

  Returns
  -------
  pyagrum.BNLearner
    the BNLearner itself, so that we can chain useXXX() methods.
  """
  if tabu_size is None:
    if nb_decrease is None:
      self._useLocalSearchWithTabuList()
    else:
      self._useLocalSearchWithTabuList(pyagrum.config.typed["learning", "tabuListSize"], nb_decrease)
  elif nb_decrease is None:
    self._useLocalSearchWithTabuList(tabu_size)
  else:
    self._useLocalSearchWithTabuList(tabu_size, nb_decrease)
  return self
  }
}


%pythonappend gum::learning::BNLearner<double>::learnParameters %{
  if self._EM_warning():
      import warnings
      warnings.warn("\nthe learnParameters's EM algorithm has completed prematurely due to a likelihood divergence\n", UserWarning)
%}
