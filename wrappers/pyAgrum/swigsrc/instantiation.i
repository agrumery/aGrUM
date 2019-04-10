%extend gum::Instantiation {
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
      GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary");
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
        GUM_ERROR(gum::InvalidArgument, "A key is not a string");
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
  }
};
