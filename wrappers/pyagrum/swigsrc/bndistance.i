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







 %extend gum::ExactBNdistance {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyObject* pyval;

  pyval=PyFloat_FromDouble(self->klPQ());
  PyDict_SetItemString(q,"klPQ",pyval);
  Py_DecRef(pyval);

  pyval=PyInt_FromLong(self->errorPQ());
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->klQP());
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  Py_DecRef(pyval);

  pyval=PyInt_FromLong(self->errorQP());
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->hellinger());
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->bhattacharya());
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->jsd());
  PyDict_SetItemString(q,"jensen-shannon",PyFloat_FromDouble(self->jsd()));
  Py_DecRef(pyval);

  return q;
}

}

%extend gum::GibbsBNdistance {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyObject* pyval;

  pyval=PyFloat_FromDouble(self->klPQ());
  PyDict_SetItemString(q,"klPQ",pyval);
  Py_DecRef(pyval);

  pyval=PyInt_FromLong(self->errorPQ());
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->klQP());
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  Py_DecRef(pyval);

  pyval=PyInt_FromLong(self->errorQP());
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->hellinger());
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->bhattacharya());
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));
  Py_DecRef(pyval);

  pyval=PyFloat_FromDouble(self->jsd());
  PyDict_SetItemString(q,"jensen-shannon",PyFloat_FromDouble(self->jsd()));
  Py_DecRef(pyval);

  return q;
}

}
