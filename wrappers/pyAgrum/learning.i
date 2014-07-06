%ignore  gum::learning::BNLearner::setSliceOrder( const NodeProperty<unsigned int>& slice_order );
%ignore  gum::learning::BNLearner::useK2( const Sequence<NodeId>& order);
%ignore  gum::learning::BNLearner::useK2( const std::vector<NodeId>& order );

%extend gum::learning::BNLearner {
  gum::BayesNet<double> learnBN(const std::string filename) {
    return self->learnBN<double>(filename);
  }

  void setSliceOrder(PyObject *l) {
    NodeProperty<unsigned int> ranks;

    if (PyList_Check(l)==0) {
      PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
      return;
    }
    
    for(Py_ssize_t i=0;i<PySequence_Size(l);i++) {
      PyObject* rows=PyList_GetItem(l, i);
      if (PyList_Check(rows)==0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence of sequence");
        return;
      }

      for(Py_ssize_t j=0;j<PySequence_Size(rows);j++) {
        PyObject* row=PyList_GetItem(rows, j);
        if (PyInt_Check(row)==0) {
          PyErr_SetString(PyExc_TypeError, "arg must be a sequence of sequence of int");
          return;
        }
        ranks.insert(PyInt_AsLong(row),i);
      }
    }
    $self->setSliceOrder(ranks);
  }

  void useK2(PyObject *l) {
    std::vector<unsigned int> v;

    if (PyList_Check(l)==0) {
      PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
      return;
    }

    for(Py_ssize_t i=0;i<PySequence_Size(l);i++) {
      PyObject* row=PyList_GetItem(l, i);
      if (PyInt_Check(row)==0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence of int");
        return;
      }
      v.push_back((unsigned int)PyInt_AsLong(row));
    }

    $self->useK2(v);
  }
}

