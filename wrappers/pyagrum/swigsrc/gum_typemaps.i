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








/* Convert from C --> Python */
%typemap(out) gum::Set<gum::Instantiation> {
  $result = PyAgrumHelper::PySeqFromSetOfInstantiation($1);
}

%typemap(out) std::pair<double,double> {
  $result=PyAgrumHelper::PyDictFromPairMeanVar($1) ;
}

%typemap(out) const gum::Sequence<gum::NodeId>& {
  PyObject *q=PyList_New(0);

  PyObject* pyval;
  for(auto i : *$1) {
    pyval=PyInt_FromLong(i);
    PyList_Append(q,pyval);
    Py_DecRef(pyval);
  }
  $result=q;
}

%typemap(out) gum::Sequence<gum::NodeId> {
  PyObject *q=PyList_New(0);

  PyObject *pyval;
  for(auto i : static_cast<gum::Sequence<gum::NodeId>>($1)) {
    pyval=PyInt_FromLong(i);
    PyList_Append(q,pyval);
    Py_DecRef(pyval);
  }
  $result=q;
}

// for gum::IMarkovRandomField::factors
%typemap(out) const gum::FactorTable<double> & {
  $result = PyList_New(0);

  PyObject* pyval;
  for (auto kv : *$1) {
    pyval=PyAgrumHelper::PySetFromNodeSet(kv.first);
    PyList_Append($result, pyval);
    Py_DecRef(pyval);
  }
}


// for gum::IMarkovRandomField::factors
%typemap(out) const gum::FactorTable<double> & {
  $result = PyList_New(0);

  PyObject* pyval;
  for (auto kv : *$1) {
    pyval=PyAgrumHelper::PySetFromNodeSet(kv.first);
    PyList_Append($result, pyval);
    Py_DecRef(pyval);
  }
}
