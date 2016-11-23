
%extend gum::LazyPropagation<double> {
    Potential<double> jointPosterior(PyObject *seq_of_ids) {
      if (PySequence_Check(seq_of_ids)==0) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence");
        return 0;
      }

      gum::NodeSet s;
      for(Py_ssize_t i=0;i<PySequence_Size(seq_of_ids);i++) {
        PyObject* o=PyList_GetItem(seq_of_ids, i);
        if (PyNumber_Check(o)==0) {
          PyErr_SetString(PyExc_TypeError, "arg must be a sequence of ids");
          return 0;
        }
        s<<gum::NodeId( PyInt_AsLong(o));
      }
      return self->jointPosterior(s);
    };
}

%define IMPROVE_INFERENCE_API(classname)
%feature("shadow") classname::setEvidence %{
def setEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))
    self.eraseAllEvidence()
    for k,v in evidces.items():
        self.addEvidence(k,v)
%}

%feature("shadow") classname::updateEvidence %{
def updateEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))

    for k,v in evidces.items():
        if self.hasEvidence(k):
            self.chgEvidence(k,v)
        else:
            self.addEvidence(k,v)
%}

%feature("shadow") classname::setTargets %{
def setTargets(self, targets):
    if not isinstance(targets, set):
        raise TypeError("setTargets parameter must be a set, not %s"%(type(targets)))

    self.eraseAllTargets()
    for k in targets:
        self.addTarget(k)
%}


// these void class extensions are rewritten by "shadow" declarations
%extend classname {
    void setEvidence(PyObject *evidces) {}
    void updateEvidence(PyObject *evidces) {}
    void setTargets(PyObject* targets) {}

    PyObject* hardEvidenceList() {
      PyObject* q = PyList_New( 0 );

      for ( auto node : self->hardEvidenceNodes() ) {
        PyList_Append( q, PyLong_FromUnsignedLong((unsigned long)node));
      }

      return q;
    }
    PyObject* softEvidenceList() {
      PyObject* q = PyList_New( 0 );

      for ( auto node : self->softEvidenceNodes() ) {
        PyList_Append( q, PyLong_FromUnsignedLong((unsigned long)node));
      }

      return q;
    }
    PyObject* targetList() {
      PyObject* q = PyList_New( 0 );

      for ( auto node : self->targets() ) {
        PyList_Append( q, PyLong_FromUnsignedLong((unsigned long)node));
      }

      return q;
    }
}
%enddef
IMPROVE_INFERENCE_API(gum::LazyPropagation<double>)
IMPROVE_INFERENCE_API(gum::GibbsInference<double>)
