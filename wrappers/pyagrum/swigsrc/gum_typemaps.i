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

// std::tuple<CausalImpact<double>, Tensor<double>, std::string>
// returned by gum::causalImpact() → Python tuple (ci, pot, expl).
// SWIG wraps the return value in SwigValueWrapper<T>, which is not directly
// accessible via std::get. Binding it to a typed reference invokes
// SwigValueWrapper::operator T&() and gives a proper tuple reference.
// $descriptor() is resolved at SWIG code-gen time, so forward-declaration order
// does not matter here.
%typemap(out) std::tuple<gum::CausalImpact<double>, gum::Tensor<double>, std::string> {
  // SwigValueWrapper<T> only exposes operator T&&(), not operator T&().
  // Move-construct a local tuple to get an lvalue we can work with.
  using _TupleT = std::tuple<gum::CausalImpact<double>, gum::Tensor<double>, std::string>;
  _TupleT _tup  = std::move($1);
  auto* _ci   = new gum::CausalImpact<double>(std::move(std::get<0>(_tup)));
  auto* _tens = new gum::Tensor<double>(std::move(std::get<1>(_tup)));
  $result = PyTuple_New(3);
  PyTuple_SET_ITEM($result, 0,
    SWIG_NewPointerObj(_ci,   $descriptor(gum::CausalImpact<double>*), SWIG_POINTER_OWN));
  PyTuple_SET_ITEM($result, 1,
    SWIG_NewPointerObj(_tens, $descriptor(gum::Tensor<double>*),       SWIG_POINTER_OWN));
  PyTuple_SET_ITEM($result, 2, PyUnicode_FromString(std::get<2>(_tup).c_str()));
}

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

// gum::NodeSet -> Python set[int]
%typemap(out) gum::NodeSet {
  $result = PyAgrumHelper::PySetFromNodeSet($1);
}
%typemap(out) const gum::NodeSet& {
  $result = PyAgrumHelper::PySetFromNodeSet(*$1);
}

// gum::ArcSet -> Python set[tuple[int,int]]
%typemap(out) gum::ArcSet {
  $result = PyAgrumHelper::PySetFromArcSet($1);
}
%typemap(out) const gum::ArcSet& {
  $result = PyAgrumHelper::PySetFromArcSet(*$1);
}

// gum::EdgeSet -> Python set[tuple[int,int]]
%typemap(out) gum::EdgeSet {
  $result = PyAgrumHelper::PySetFromEdgeSet($1);
}
%typemap(out) const gum::EdgeSet& {
  $result = PyAgrumHelper::PySetFromEdgeSet(*$1);
}

// std::vector<gum::NodeId> -> Python list[int]
%typemap(out) std::vector<gum::NodeId> {
  $result = PyAgrumHelper::PyListFromNodeVect($1);
}
%typemap(out) const std::vector<gum::NodeId>& {
  $result = PyAgrumHelper::PyListFromNodeVect(*$1);
}

// Conversion C++ gum::Set<std::string> vers Python set[str]
%typemap(out) gum::Set<std::string> {
  $result = PyAgrumHelper::PySetFromStringSet($1);
}

%typemap(out) const gum::Set<std::string>& {
  $result = PyAgrumHelper::PySetFromStringSet(*$1);
}

// Conversion Python set[str] vers C++ gum::Set<std::string>
%typemap(in) gum::Set<std::string> (gum::Set<std::string> temp) {
  PyAgrumHelper::populateStringSetFromPySequence(temp, $input);
  $1 = temp;
}

%typemap(in) const gum::Set<std::string>& (gum::Set<std::string> temp) {
  PyAgrumHelper::populateStringSetFromPySequence(temp, $input);
  $1 = &temp;
}

// Typecheck pour la surcharge (permet d'accepter set, list ou str comme argument)
%typemap(typecheck, precedence=SWIG_TYPECHECK_STRING_ARRAY) gum::Set<std::string>, const gum::Set<std::string>& {
  $1 = PySet_Check($input) || PyList_Check($input) || PyUnicode_Check($input) || PyBytes_Check($input);
}

// gum::HashTable<std::string, std::string> <-> Python dict[str, str|int]
// Both gum:: and non-qualified forms are needed: SWIG sometimes drops the namespace
// prefix when resolving types declared inside the gum namespace.
%typemap(in) gum::HashTable<std::string, std::string> (gum::HashTable<std::string, std::string> temp) {
  PyAgrumHelper::populateHashTableStrStrFromPyDict(temp, $input);
  $1 = temp;
}

%typemap(in) const gum::HashTable<std::string, std::string>& (gum::HashTable<std::string, std::string> temp) {
  PyAgrumHelper::populateHashTableStrStrFromPyDict(temp, $input);
  $1 = &temp;
}

%typemap(out) gum::HashTable<std::string, std::string> {
  $result = PyAgrumHelper::PyDictFromHashTableStrStr($1);
}

%typemap(out) const gum::HashTable<std::string, std::string>& {
  $result = PyAgrumHelper::PyDictFromHashTableStrStr(*$1);
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) gum::HashTable<std::string, std::string>,
                                                        const gum::HashTable<std::string, std::string>& {
  $1 = PyDict_Check($input);
}

// SWIG does not follow transitive C++ #include directives, so it cannot resolve
// HashTable to gum::HashTable. Both qualified and unqualified forms are registered.
%apply gum::HashTable<std::string, std::string>        { HashTable<std::string, std::string> };
%apply const gum::HashTable<std::string, std::string>& { const HashTable<std::string, std::string>& };

