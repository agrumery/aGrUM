%extend gum::ExactBNdistance {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyDict_SetItemString(q,"klPQ",PyFloat_FromDouble(self->klPQ()));
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));
  PyDict_SetItemString(q,"jensen-shannon",PyFloat_FromDouble(self->jsd()));

  return q;
}

}

%extend gum::GibbsBNdistance {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyDict_SetItemString(q,"klPQ",PyFloat_FromDouble(self->klPQ()));
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));
  PyDict_SetItemString(q,"jensen-shannon",PyFloat_FromDouble(self->jsd()));

  return q;
}

}