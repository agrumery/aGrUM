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








// Sentinel typedefs: used as %extend return types so that passForPyType.py
// can map each sentinel name to the correct Python type annotation.
// All are aliases for PyObject at the C level — no runtime overhead.
// Declared in a shared header (not inlined here) so split submodules can
// %{ #include %} it too: %import does not carry raw %{ %} code blocks
// across modules, only typemap/type declarations (the plain `typedef`
// list right below, kept for SWIG's own type system).
%{
#include "extensions/pyagrumSentinelTypes.h"
%}
typedef PyObject PyAgrumSetOfArc;
typedef PyObject PyAgrumSetOfEdge;
typedef PyObject PyAgrumSetOfInt;
typedef PyObject PyAgrumSetOfStr;
typedef PyObject PyAgrumListOfSetOfInt;
typedef PyObject PyAgrumListOfInt;
typedef PyObject PyAgrumListOfStr;
typedef PyObject PyAgrumListOfArc;
typedef PyObject PyAgrumListOfTensor;
typedef PyObject PyAgrumList;
typedef PyObject PyAgrumTupleFF;
typedef PyObject PyAgrumTupleOfInt;
typedef PyObject PyAgrumDictSF;
typedef PyObject PyAgrumDictState;
typedef PyObject PyAgrumInstDict;
typedef PyObject PyAgrumBool;
typedef PyObject PyAgrumFloat;
typedef PyObject PyAgrumArgMinMax;
typedef PyObject PyAgrumStr;
typedef PyObject PyAgrumNdArray;
typedef PyObject PyAgrumNone;
typedef PyObject PyAgrumDict;

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
  // "N" takes the references, and Py_BuildValue fails if one of them is nullptr
  $result = Py_BuildValue("(NNN)",
    SWIG_NewPointerObj(_ci,   $descriptor(gum::CausalImpact<double>*), SWIG_POINTER_OWN),
    SWIG_NewPointerObj(_tens, $descriptor(gum::Tensor<double>*),       SWIG_POINTER_OWN),
    PyAgrumHelper::PyStr(std::get<2>(_tup)));
  if (!$result) SWIG_fail;
}

%typemap(out) gum::Set<gum::Instantiation> {
  $result = PyAgrumHelper::PySeqFromSetOfInstantiation($1);
  if (!$result) SWIG_fail;
}

// (mean, variance) of the influence diagram inferences only: any other pair of doubles stays a tuple
%typemap(out) std::pair<double,double> MEU, std::pair<double,double> meanVar {
  $result=PyAgrumHelper::PyDictFromPairMeanVar($1) ;
  if (!$result) SWIG_fail;
}

%typemap(out) const gum::Sequence<gum::NodeId>& {
  $result = PyAgrumHelper::PyListFrom(*$1, PyAgrumHelper::PyIntFromNodeId);
  if (!$result) SWIG_fail;
}

%typemap(out) gum::Sequence<gum::NodeId> {
  const gum::Sequence<gum::NodeId>& _seq = $1;   // $1 may be a SwigValueWrapper
  $result = PyAgrumHelper::PyListFrom(_seq, PyAgrumHelper::PyIntFromNodeId);
  if (!$result) SWIG_fail;
}

// for gum::IMarkovRandomField::factors
%typemap(out) const gum::FactorTable<double> & {
  $result = PyAgrumHelper::PyListFrom(*$1, [](const auto& kv) { return PyAgrumHelper::PySetFromNodeSet(kv.first); });
  if (!$result) SWIG_fail;
}

// gum::NodeSet -> Python set[int]
%typemap(out) gum::NodeSet {
  $result = PyAgrumHelper::PySetFromNodeSet($1);
  if (!$result) SWIG_fail;
}
%typemap(out) const gum::NodeSet& {
  $result = PyAgrumHelper::PySetFromNodeSet(*$1);
  if (!$result) SWIG_fail;
}
// gum::NodeGraphPart -> Python set[int]
%typemap(out) const gum::NodeGraphPart& {
  $result = PyAgrumHelper::PySetFromNodeSet(*$1);
  if (!$result) SWIG_fail;
}

// std::optional<gum::NodeId> -> Python int or None
%typemap(out) std::optional< gum::NodeId >, std::optional< std::size_t > {
  if ($1.has_value()) {
    $result = PyLong_FromSize_t((size_t)$1.value());
  } else {
    $result = Py_None;
    Py_INCREF(Py_None);
  }
}

// gum::VariableSet -> Python set[DiscreteVariable] (DiscreteVariable is hashable: see variables.i)
// The variables belong to the model: the proxies do not own them (as for variable()).
%typemap(out) gum::VariableSet {
  const gum::VariableSet& _vs = $1;   // $1 may be a SwigValueWrapper
  $result = PyAgrumHelper::PySetFrom(_vs, [](const gum::DiscreteVariable* var) {
    return SWIG_NewPointerObj(SWIG_as_voidptr(var), $descriptor(gum::DiscreteVariable*), 0);
  });
  if (!$result) SWIG_fail;
}

// std::optional<gum::NodeSet> -> Python set[int] or None
%typemap(out) std::optional< gum::NodeSet > {
  if ($1.has_value()) {
    $result = PyAgrumHelper::PySetFromNodeSet($1.value());
    if (!$result) SWIG_fail;
  } else {
    $result = Py_None;
    Py_INCREF(Py_None);
  }
}

// Python set/frozenset/list/tuple[int] -> gum::NodeSet (e.g. for excluded_nodes)
%typemap(in) const gum::NodeSet& (gum::NodeSet _ns_temp) {
  PyObject* _iter = PyObject_GetIter($input);
  if (!_iter) {
    PyErr_SetString(PyExc_TypeError, "expected an iterable of int (node IDs)");
    SWIG_fail;
  }
  PyObject* _item;
  while ((_item = PyIter_Next(_iter)) != nullptr) {
    long _val = PyLong_AsLong(_item);
    Py_DECREF(_item);
    if (_val == -1 && PyErr_Occurred()) { Py_DECREF(_iter); SWIG_fail; }
    _ns_temp.insert(static_cast< gum::NodeId >(_val));
  }
  Py_DECREF(_iter);
  if (PyErr_Occurred()) SWIG_fail;
  $1 = &_ns_temp;
}
// Typecheck for const gum::NodeSet&.
//
// Accepts Python set/frozenset whose elements are all integers (NodeIds).
// Lists and tuples are intentionally excluded so that they fall through to
// PyObject* %extend dispatchers (e.g. MarkovRandomField::addFactor) which
// preserve insertion order for list arguments.
//
// Rationale for set-only: a Python set is semantically unordered — it maps
// naturally to gum::NodeSet. A Python list implies ordering; callers that
// pass a list expect order-preserving behaviour (handled by %extend PyObject*).
//
// Rejects as soon as a string element is found, so that string-based
// calling conventions fall through to the appropriate overload:
//   - set of strings  → PyObject* %extend dispatcher (e.g. MarkovRandomField)
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const gum::NodeSet& {
  if (PySet_Check($input) || PyFrozenSet_Check($input)) {
    $1 = 1;
    PyObject* _tc_iter = PyObject_GetIter($input);
    if (_tc_iter) {
      PyObject* _tc_item;
      while ($1 && (_tc_item = PyIter_Next(_tc_iter)) != nullptr) {
        if (PyUnicode_Check(_tc_item)) { $1 = 0; }
        Py_DECREF(_tc_item);
      }
      Py_DECREF(_tc_iter);
    }
  } else {
    $1 = 0;
  }
}

// gum::Set<gum::NodeSet> (joint targets) -> Python list[set[int]]
%typemap(out) const gum::Set< gum::NodeSet >& {
  $result = PyAgrumHelper::PyListOfSetsFromSetOfNodeSets(*$1);
  if (!$result) SWIG_fail;
}

// gum::DoorCriteria::NodeSetVec, std::vector<gum::NodeSet> -> Python list[set[int]]
// SwigValueWrapper<T> only exposes operator T&&(), not T&. Move-construct a
// local lvalue so we can iterate safely.
%typemap(out) gum::DoorCriteria::NodeSetVec, std::vector< gum::NodeSet > {
  using _VecT = std::vector< gum::NodeSet >;
  _VecT _vec = std::move($1);
  $result = PyAgrumHelper::PyListFrom(_vec, [](const gum::NodeSet& ns) { return PyAgrumHelper::PySetFromNodeSet(ns); });
  if (!$result) SWIG_fail;
}

// gum::ArcSet -> Python set[tuple[int,int]]
%typemap(out) gum::ArcSet {
  $result = PyAgrumHelper::PySetFromArcSet($1);
  if (!$result) SWIG_fail;
}
%typemap(out) const gum::ArcSet& {
  $result = PyAgrumHelper::PySetFromArcSet(*$1);
  if (!$result) SWIG_fail;
}

// std::vector<gum::Arc> -> Python list[tuple[int,int]]
%typemap(out) std::vector<gum::Arc> {
  $result = PyAgrumHelper::PyListFromArcVect($1);
  if (!$result) SWIG_fail;
}
%typemap(out) const std::vector<gum::Arc>& {
  $result = PyAgrumHelper::PyListFromArcVect(*$1);
  if (!$result) SWIG_fail;
}

// gum::EdgeSet -> Python set[tuple[int,int]]
%typemap(out) gum::EdgeSet {
  $result = PyAgrumHelper::PySetFromEdgeSet($1);
  if (!$result) SWIG_fail;
}
%typemap(out) const gum::EdgeSet& {
  $result = PyAgrumHelper::PySetFromEdgeSet(*$1);
  if (!$result) SWIG_fail;
}

// std::vector<int> -> Python tuple[int]
// SwigValueWrapper<T> (by-value return) only exposes operator T&&(), not T&.
// Move-construct a local lvalue before iterating.
// Return tuple to match historic swig::from behaviour.
%typemap(out) std::vector<int> {
  std::vector<int> _v = std::move($1);
  $result = PyAgrumHelper::PyTupleFrom(_v, [](int x) { return PyLong_FromLong(x); });
  if (!$result) SWIG_fail;
}
%typemap(out) const std::vector<int>& {
  $result = PyAgrumHelper::PyTupleFrom(*$1, [](int x) { return PyLong_FromLong(x); });
  if (!$result) SWIG_fail;
}

// std::vector<std::pair<gum::Idx,gum::Idx>> -> Python tuple[tuple[int,int]]
// SWIG resolves gum::Idx -> std::size_t, so the typemap must use std::size_t
%typemap(out) std::vector< std::pair< std::size_t, std::size_t > > {
  std::vector< std::pair< std::size_t, std::size_t > > _v = std::move($1);
  $result = PyAgrumHelper::PyTupleFrom(_v, [](const std::pair< std::size_t, std::size_t >& p) {
    return Py_BuildValue("(NN)", PyLong_FromSize_t(p.first), PyLong_FromSize_t(p.second));
  });
  if (!$result) SWIG_fail;
}

// std::vector<double> -> Python tuple[float]
%typemap(out) std::vector<double> {
  std::vector<double> _v = std::move($1);
  $result = PyAgrumHelper::PyTupleFrom(_v, PyFloat_FromDouble);
  if (!$result) SWIG_fail;
}
%typemap(out) const std::vector<double>& {
  $result = PyAgrumHelper::PyTupleFrom(*$1, PyFloat_FromDouble);
  if (!$result) SWIG_fail;
}

// std::vector<std::string> -> Python tuple[str]
%typemap(out) std::vector<std::string> {
  std::vector<std::string> _v = std::move($1);
  $result = PyAgrumHelper::PyTupleFrom(_v, PyAgrumHelper::PyStr);
  if (!$result) SWIG_fail;
}
%typemap(out) const std::vector<std::string>& {
  $result = PyAgrumHelper::PyTupleFrom(*$1, PyAgrumHelper::PyStr);
  if (!$result) SWIG_fail;
}

// std::vector<int> <- Python sequence[int]
%typemap(in) const std::vector<int>& (std::vector<int> _tmp_vi) {
  if (!PySequence_Check($input) || PyUnicode_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "expected a sequence of ints");
    SWIG_fail;
  }
  Py_ssize_t _n = PySequence_Size($input);
  _tmp_vi.reserve(static_cast<std::size_t>(_n));
  for (Py_ssize_t _i = 0; _i < _n; ++_i) {
    PyObject* _item = PySequence_GetItem($input, _i);
    if (!_item) SWIG_fail;
    long _val = PyLong_AsLong(_item);
    Py_DECREF(_item);
    if (_val == -1 && PyErr_Occurred()) SWIG_fail;
    _tmp_vi.push_back(static_cast<int>(_val));
  }
  $1 = &_tmp_vi;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_INT32_ARRAY) const std::vector<int>& {
  $1 = (PySequence_Check($input) && !PyUnicode_Check($input)) ? 1 : 0;
}

// std::vector<double> <- Python sequence[float]
%typemap(in) const std::vector<double>& (std::vector<double> _tmp_vd) {
  if (!PySequence_Check($input) || PyUnicode_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "expected a sequence of floats");
    SWIG_fail;
  }
  Py_ssize_t _n = PySequence_Size($input);
  _tmp_vd.reserve(static_cast<std::size_t>(_n));
  for (Py_ssize_t _i = 0; _i < _n; ++_i) {
    PyObject* _item = PySequence_GetItem($input, _i);
    if (!_item) SWIG_fail;
    double _val = PyFloat_AsDouble(_item);
    Py_DECREF(_item);
    if (_val == -1.0 && PyErr_Occurred()) SWIG_fail;
    _tmp_vd.push_back(_val);
  }
  $1 = &_tmp_vd;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_DOUBLE_ARRAY) const std::vector<double>& {
  $1 = (PySequence_Check($input) && !PyUnicode_Check($input)) ? 1 : 0;
}

// std::vector<std::string> <- Python sequence[str]
%typemap(in) const std::vector<std::string>& (std::vector<std::string> _tmp_vs) {
  if (!PySequence_Check($input) || PyUnicode_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "expected a sequence of strings");
    SWIG_fail;
  }
  Py_ssize_t _n = PySequence_Size($input);
  _tmp_vs.reserve(static_cast<std::size_t>(_n));
  for (Py_ssize_t _i = 0; _i < _n; ++_i) {
    PyObject* _item = PySequence_GetItem($input, _i);
    if (!_item) SWIG_fail;
    if (!PyUnicode_Check(_item) && !PyBytes_Check(_item)) {
      // a non-string item would be read as "" (misleading NotFound "<>" later)
      PyErr_Format(PyExc_TypeError,
                   "expected a sequence of strings (item %zd is not a string)",
                   _i);
      Py_DECREF(_item);
      SWIG_fail;
    }
    _tmp_vs.push_back(PyAgrumHelper::stringFromPyObject(_item));
    Py_DECREF(_item);
  }
  $1 = &_tmp_vs;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_STRING_ARRAY) const std::vector<std::string>& {
  // Accept only sequences whose first element is a string.
  // This lets PyObject* %extend dispatchers handle int-element sequences.
  $1 = 0;
  if (PySequence_Check($input) && !PyUnicode_Check($input)) {
    Py_ssize_t _tc_sz = PySequence_Size($input);
    if (_tc_sz == 0) {
      $1 = 1;  // empty sequence: accept
    } else {
      PyObject* _tc_first = PySequence_GetItem($input, 0);
      if (_tc_first) {
        $1 = PyUnicode_Check(_tc_first) ? 1 : 0;
        Py_DECREF(_tc_first);
      }
    }
  }
}

// std::unordered_set<std::string> <- Python set[str]. Restricted to an
// actual `set`/`frozenset` (not any sequence): several call sites overload
// on this type vs. a sibling std::vector<std::string> at the same argument
// position (e.g. gum::learning::KTBNLearner's atemporalVars vs.
// missingSymbols constructors), and a Python `set` is never a
// PySequence_Check() sequence, so restricting the typecheck this way keeps
// the two overloads unambiguous -- callers select one or the other purely
// by passing a set literal vs. a list/tuple literal.
%typemap(in) const std::unordered_set<std::string>& (std::unordered_set<std::string> _tmp_uss) {
  if (!PyAnySet_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "expected a set of strings");
    SWIG_fail;
  }
  PyObject* _iter = PyObject_GetIter($input);
  if (!_iter) SWIG_fail;
  PyObject* _item;
  while ((_item = PyIter_Next(_iter)) != NULL) {
    _tmp_uss.insert(PyAgrumHelper::stringFromPyObject(_item));
    Py_DECREF(_item);
  }
  Py_DECREF(_iter);
  if (PyErr_Occurred()) SWIG_fail;
  $1 = &_tmp_uss;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_STRING_ARRAY) const std::unordered_set<std::string>& {
  $1 = PyAnySet_Check($input) ? 1 : 0;
}

// std::unordered_set<std::string> -> Python frozenset[str]
%typemap(out) const std::unordered_set<std::string>& {
  $result = PyAgrumHelper::PyFrozenSetFrom(*$1, PyAgrumHelper::PyStr);
  if (!$result) SWIG_fail;
}

// std::set<std::string> -> Python frozenset[str]
%typemap(out) const std::set<std::string>& {
  $result = PyAgrumHelper::PyFrozenSetFrom(*$1, PyAgrumHelper::PyStr);
  if (!$result) SWIG_fail;
}

// std::vector<gum::NodeId> -> Python list[int]
%typemap(out) std::vector<gum::NodeId> {
  $result = PyAgrumHelper::PyListFromNodeVect($1);
  if (!$result) SWIG_fail;
}
%typemap(out) const std::vector<gum::NodeId>& {
  $result = PyAgrumHelper::PyListFromNodeVect(*$1);
  if (!$result) SWIG_fail;
}

// std::optional<std::vector<gum::NodeId>> -> Python list[int] | None
// Note: SWIG expands the allocator in nested templates, so both forms must be covered.
%typemap(out) std::optional< std::vector< gum::NodeId > > {
  if ($1.has_value()) {
    $result = PyAgrumHelper::PyListFromNodeVect($1.value());
    if (!$result) SWIG_fail;
  } else {
    $result = Py_None;
    Py_INCREF(Py_None);
  }
}
%typemap(out) std::optional< std::vector< gum::NodeId,std::allocator< gum::NodeId > > > {
  if ($1.has_value()) {
    $result = PyAgrumHelper::PyListFromNodeVect($1.value());
    if (!$result) SWIG_fail;
  } else {
    $result = Py_None;
    Py_INCREF(Py_None);
  }
}

// Conversion C++ gum::Set<std::string> vers Python set[str]
%typemap(out) gum::Set<std::string> {
  $result = PyAgrumHelper::PySetFromStringSet($1);
  if (!$result) SWIG_fail;
}

%typemap(out) const gum::Set<std::string>& {
  $result = PyAgrumHelper::PySetFromStringSet(*$1);
  if (!$result) SWIG_fail;
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
  if (!$result) SWIG_fail;
}

%typemap(out) const gum::HashTable<std::string, std::string>& {
  $result = PyAgrumHelper::PyDictFromHashTableStrStr(*$1);
  if (!$result) SWIG_fail;
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) gum::HashTable<std::string, std::string>,
                                                        const gum::HashTable<std::string, std::string>& {
  $1 = PyDict_Check($input);
}

// SWIG does not follow transitive C++ #include directives, so it cannot resolve
// HashTable to gum::HashTable. Both qualified and unqualified forms are registered.
%apply gum::HashTable<std::string, std::string>        { HashTable<std::string, std::string> };
%apply const gum::HashTable<std::string, std::string>& { const HashTable<std::string, std::string>& };

// gum::NodeProperty<gum::NodeId> (= HashTable<NodeId,NodeId>) -> Python dict[int, int]
// Used by connectedComponents() on graph classes.
%typemap(out) gum::NodeProperty< gum::NodeId > {
  $result = PyAgrumHelper::PyDictFromNodePropertyNodeId($1);
  if (!$result) SWIG_fail;
}

%typemap(out) const gum::NodeProperty< gum::NodeId >& {
  $result = PyAgrumHelper::PyDictFromNodePropertyNodeId(*$1);
  if (!$result) SWIG_fail;
}

%apply gum::NodeProperty< gum::NodeId >        { NodeProperty< NodeId > };
%apply const gum::NodeProperty< gum::NodeId >& { const NodeProperty< NodeId >& };


// Python dict[int, int] -> const gum::NodeProperty<gum::Size>& domainSizes (node id -> domain size)
// Used by MeekRules::propagateToDAG(mg, domainSizes). Matched by the parameter name only, so that the
// other NodeProperty<Size> parameters keep their own conversions.
%typemap(in) const gum::NodeProperty< gum::Size >& domainSizes (gum::NodeProperty< gum::Size > _tmp_ds) {
  if (!PyDict_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "domainSizes must be a dict {node id: domain size}");
    SWIG_fail;
  }
  PyObject*  _key;
  PyObject*  _val;
  Py_ssize_t _pos = 0;
  while (PyDict_Next($input, &_pos, &_key, &_val)) {
    const std::size_t _id = PyLong_AsSize_t(_key);
    if (_id == static_cast< std::size_t >(-1) && PyErr_Occurred()) SWIG_fail;
    const std::size_t _ds = PyLong_AsSize_t(_val);
    if (_ds == static_cast< std::size_t >(-1) && PyErr_Occurred()) SWIG_fail;
    _tmp_ds.set(static_cast< gum::NodeId >(_id), static_cast< gum::Size >(_ds));
  }
  $1 = &_tmp_ds;
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const gum::NodeProperty< gum::Size >& domainSizes {
  $1 = PyDict_Check($input);
}

%apply const gum::NodeProperty< gum::Size >& domainSizes { const NodeProperty< Size >& domainSizes };
