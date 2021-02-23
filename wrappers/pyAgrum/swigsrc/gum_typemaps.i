/* Convert from C --> Python */
%typemap(out) gum::Set<gum::Instantiation> {
  $result = PyAgrumHelper::PySeqFromSetOfInstantiation($1);
}

%typemap(out) std::pair<double,double> {
  $result=PyAgrumHelper::PyDictFromPairMeanVar($1) ;
}

%typemap(out) const gum::Sequence<gum::NodeId>& {
  PyObject *q=PyList_New(0);
  for(auto i : *$1) {
    PyList_Append(q,PyInt_FromLong(i));
  }
  $result=q;
}

// for gum::IMarkovNet::factors
%typemap(out) const gum::FactorTable<double> & {
  $result = PyList_New(0);

  for (auto kv : *$1) {
    PyList_Append($result, PyAgrumHelper::PySetFromNodeSet(kv.first));
  }
}