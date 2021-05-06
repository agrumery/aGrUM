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
%ignore gum::Instantiation::actAsSlave;
%ignore gum::Instantiation::addWithMaster;
%ignore gum::Instantiation::eraseWithMaster;
%ignore gum::Instantiation::forgetMaster;
%ignore gum::Instantiation::isMaster;
%ignore gum::Instantiation::isSlave;
%ignore gum::Instantiation::synchronizeWithMaster;
%ignore gum::Instantiation::valFromPtr;

%extend gum::Instantiation {
  void setMutable() {
    self->forgetMaster();
  }

  bool isMutable() {
    return self->isSlave();
  }
  
  PyObject* todict(bool withLabels=false) const {
    auto res=PyDict_New();
    for(gum::Idx i=0;i<self->nbrDim();i++) {
      auto key=PyString_FromString(self->variable(i).name().c_str());
      PyObject* val;
      if (withLabels) {
        val=PyString_FromString(self->variable(i).label(self->val(i)).c_str());
      } else {
        val=PyLong_FromUnsignedLong(self->val(i));
      }
      PyDict_SetItem(res,key,val);
    }
    return res;
  }


  void fromdict(PyObject* dict) {
    if (!PyDict_Check(dict)) {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary")
    }

    gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
    for (gum::Idx i = 0; i < self->nbrDim(); i++)
      namesToVars.insert(self->variable(i).name(), &(self->variable(i)));

    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value)) {
      std::string name = PyAgrumHelper::stringFromPyObject(key);
      gum::Idx    v;
      if (name == "") {
        GUM_ERROR(gum::InvalidArgument, "A key is not a string")
      }
      if (namesToVars.exists(name)) {
        if (!PyInt_Check(value)) {
          std::string label = PyAgrumHelper::stringFromPyObject(value);
          if (label == "") {
            GUM_ERROR(gum::InvalidArgument,
                      "A value is neither an int or a string");
          }
          v = namesToVars[name]->index(label);  // may throw gum::OutOfBounds
        } else {
          v = gum::Idx(PyInt_AsLong(value));
        }
        if (v >= namesToVars[name]->domainSize()) {
          GUM_ERROR(gum::InvalidArgument,
                    "The value " << v << " is not in the domain of " << name);
        }
        self->chgVal(namesToVars[name], v);
      }
    }
  }

  %pythoncode {
  def __setitem__(self,key,item):
    self.chgVal(key,item)

  def __getitem__(self,key):
    return self.val(self.variable(key))

  def addVarsFromModel(self,model,names):
    r"""
    From a graphical model, add all the variable whose names are in the iterable

    Parameters
    ----------
    model : pyAgrum.GraphicalModel
    a (discrete) graphical model such as Bayesian network, Markov network, Influence Diagram, etc.

    names : iterable of strings
    a list/set/etc of names of variables (as string)

    Returns
    -------
    pyAgrum.Instantiation
    the current instantiation (self) in order to chain methods.
    """
    for name in names:
      self.add(model.variable(name))
    return self

  }
};

%pythonappend gum::Instantiation::add %{
  return self
%}

