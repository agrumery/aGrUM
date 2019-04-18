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
