
%extend gum::LazyPropagation<double> {
    const std::string junctionTreeToDot() {
      return self->joinTree()->toDot();
    }

    Potential<double>*  posterior(PyObject *seq_of_ids) {
      if (PySequence_Check(seq_of_ids)==0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
        return 0;
      }

      gum::NodeSet s;
      for(Py_ssize_t i=0;i<PySequence_Size(seq_of_ids);i++) {
        PyObject* o=PyList_GetItem(seq_of_ids, i);
        /*if (PyNumber_Check(o)==0) {
          PyErr_SetString(PyExc_TypeError, "arg must be a sequence of ids");
          return 0;
        }*/
        std::cout<<"   "<<i<<" : "<<PyInt_AsLong(o)<<std::endl;
        s<<gum::NodeId( PyInt_AsLong(o));
      }
      std::cout<<s<<std::endl;
      return self->jointPosterior(s);
    };
}

