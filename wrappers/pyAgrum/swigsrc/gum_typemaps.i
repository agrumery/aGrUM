/**
 *
 *  Copyright 2005-2024 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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
