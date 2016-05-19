%ignore *::insertEvidence;
%ignore *::addHardEvidence;
%ignore *::addHardEvidence;

%pythonappend gum::BayesNetInference::posterior %{
        val.__fill_distrib__()
%}


// copy: M indicates the modifications
%feature("shadow") gum::BayesNetInference<double>::setEvidence %{
def setEvidence(self, evidces):
    import numpy as np
    
    bn = self.bn()
    if isinstance(evidces, dict):
        # set evidences
        self.list_pot = []
        try:
          items=evidces.iteritems()
        except AttributeError:
          items=evidces.items()

        for var_name, evidce in items:
            pot = Potential_double()

            if isinstance(var_name, int):
                var = bn.variable(var_name)
            elif isinstance(var_name, str):
                var = bn.variableFromName(var_name)
            else:
                raise TypeError('values of the dict must be int or string')

            pot.add(var)
            if isinstance(evidce, (int, float, str,np.int64)):
                pot[:] = 0
                # determine the var type
                try:
                    cast_var = var.toLabelizedVar()
                    if isinstance(evidce, (int,np.int64)):
                        index = int(evidce)
                    elif isinstance(evidce, (str)):
                        index = cast_var[evidce]
                    else:
                        raise TypeError('values of the dict must be integer or string')
                except RuntimeError:
                    try:
                        cast_var = var.toRangeVar()
                        if isinstance(evidce, (int,np.int64)):
                            index = cast_var[str(int(evidce))]
                        elif isinstance(evidce, str):
                            index = cast_var[evidce]
                        else:
                            raise TypeError('values of the dict must be integer or string')
                    except RuntimeError:
                        cast_var = var.toDiscretizedVar()
                        if isinstance(evidce, float):
                            index = cast_var.index(evidce)
                        elif isinstance(evidce, str):
                            index = cast_var.index(float(evidce))
                        else:
                            raise TypeError('values of the dict must be float or string')
                pot[index] = 1
            elif isinstance(evidce, (list, tuple)):
                pot[:] = evidce
            else:
                raise TypeError('dict values must be number, string or sequence')
            self.list_pot.append(pot)
    else:
        try:
            l=list()
            for p in evidces:
                if not isinstance(p,Potential):
                    raise TypeError('setEvidence parameter must be an iterable of Potentials')
                l.append(p)
            self.list_pot=l
        except TypeError:
            raise TypeError("setEvidence parameter must be a dict or an iterable of Potentials, not %s"%(type(evidces)))

    self.eraseAllEvidence()
    self._setEvidence(self.list_pot)
%}


// these void class extensions are rewritten by "shadow" declarations
%extend gum::BayesNetInference<double> {
    void setEvidence(PyObject *evidces) {}

    // evidences is a python list of potentials*
    void _setEvidence(PyObject *evidences) {
      if (PySequence_Check(evidences)==0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
        return;
      }
      gum::List<const gum::Potential<double>* > l;

      for(Py_ssize_t i=0;i<PySequence_Size(evidences);i++) {
        PyObject* pot=PyList_GetItem(evidences, i);

	void *argp1 = 0 ;
	int res1 = 0 ;
	res1 = SWIG_ConvertPtr(pot, &argp1,SWIGTYPE_p_gum__PotentialT_double_t, 0 |  0 );
	if (!SWIG_IsOK(res1)) {
	  PyErr_SetString(PyExc_TypeError,"Elements of arg must be Potentials");
	  return;
	}
	gum::Potential< double > *arg1 =  reinterpret_cast< gum::Potential< double > * >(argp1);
	l.push_front(arg1);
      }
      self->insertEvidence(l);
    }
}


%extend gum::LazyPropagation<double> {
    const std::string junctionTreeToDot() {
      return self->junctionTree()->toDot();
    }

    Potential<double>*  joint(PyObject *seq_of_ids) {
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
      return self->joint(s);
    };
}

