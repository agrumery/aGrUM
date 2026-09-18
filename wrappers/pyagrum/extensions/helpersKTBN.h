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

/**
 * @file
 * @brief C++ helper functions for the pyagrum.ktbn wrapper only. Split out of
 * extensions/helpers.h -- unlike that file, this one references
 * agrum/KTBN headers types (KTBNModality, KTBNInference::NodeKey), which live in
 * the KTBN module. Including it from the generic helpers.h (shared by every
 * pyAgrum SWIG module) would make every module, including core, require
 * KTBN's typeinfo to be linkable (same reasoning as extensions/helpersCM.h).
 *
 * @author Seth AGUILA & Anis KHACEF
 */
#pragma once

#include <Python.h>
#include <tuple>
#include <utility>
#include <vector>

#include <agrum/KTBN/inference/KTBNInference.h>
#include <agrum/KTBN/KTBN.h>
#include <agrum/KTBN/learning/KTBNAdaptiveLearner.h>
#include <agrum/KTBN/learning/KTBNLearner.h>

#ifndef PYAGRUM_HELPER_KTBN
#  define PYAGRUM_HELPER_KTBN

namespace PyAgrumHelper {

  // Builds a gum::KTBNModality from a Python int (modality index) or str
  // (modality label) -- the wrapper-side counterpart of KTBNModality's
  // implicit C++ constructors, since KTBNModality itself is never exposed
  // to Python (see markdown plan / class doc for the index-vs-label rule).
  gum::KTBNModality ktbnModalityFromPyObject(PyObject* o) {
    const std::string s = stringFromPyObject(o);
    if (s != "") return gum::KTBNModality(s);
    if (PyLong_Check(o)) return gum::KTBNModality(int(PyLong_AsLong(o)));
    GUM_ERROR(gum::InvalidArgument, "a KTBN modality value must be an int or a str");
  }

  // Fills the parents map of KTBN::fillCPT(node_name, parents, distribution)
  // from a Python dict. A key is either a str (an engine name such as "X[1]"
  // or "C") or a 2-tuple (base:str, slice:int); a value is passed through
  // ktbnModalityFromPyObject(). Both key spellings may be mixed in the same
  // dict -- this matches the variant-keyed C++ overload exactly.
  void populateKTBNParentModalityMapFromPyDict(
      std::map< std::variant< std::string, std::pair< std::string, int > >, gum::KTBNModality >& m,
      PyObject* dict) {
    if (!PyDict_Check(dict)) GUM_ERROR(gum::InvalidArgument, "parents must be a dict");
    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value)) {
      const gum::KTBNModality mod = ktbnModalityFromPyObject(value);
      if (PyTuple_Check(key) && PyTuple_Size(key) == 2) {
        const std::string base = stringFromPyObject(PyTuple_GetItem(key, 0));
        if (base == "")
          GUM_ERROR(gum::InvalidArgument, "a (base,slice) parent key's base must be a str");
        const long slice = PyLong_AsLong(PyTuple_GetItem(key, 1));
        m.insert_or_assign(std::pair< std::string, int >(base, int(slice)), mod);
      } else {
        const std::string name = stringFromPyObject(key);
        if (name == "")
          GUM_ERROR(gum::InvalidArgument, "a parents key must be a str or a (str,int) tuple");
        m.insert_or_assign(name, mod);
      }
    }
  }

  // Fills the parents map of KTBN::fillCPT(base, slice, parents, distribution)
  // from a Python dict. Unlike the node_name overload above, this C++ overload
  // takes a plain (base,slice)-keyed map, so every key must be a 2-tuple; a
  // bare engine-name string key is rejected here (use the node_name overload
  // instead, whose parents dict accepts both spellings).
  void populateKTBNParentPairModalityMapFromPyDict(
      std::map< std::pair< std::string, int >, gum::KTBNModality >& m,
      PyObject*                                                     dict) {
    if (!PyDict_Check(dict)) GUM_ERROR(gum::InvalidArgument, "parents must be a dict");
    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value)) {
      if (!(PyTuple_Check(key) && PyTuple_Size(key) == 2))
        GUM_ERROR(gum::InvalidArgument,
                  "a parents key must be a (str,int) tuple when the target node is given as "
                  "(base,slice)");
      const std::string base = stringFromPyObject(PyTuple_GetItem(key, 0));
      if (base == "")
        GUM_ERROR(gum::InvalidArgument, "a (base,slice) parent key's base must be a str");
      const long slice = PyLong_AsLong(PyTuple_GetItem(key, 1));
      m.insert_or_assign(std::pair< std::string, int >(base, int(slice)),
                         ktbnModalityFromPyObject(value));
    }
  }

  // Builds a KTBNInference<double>::NodeKey from a Python str (engine name)
  // or a 2-tuple (base:str, slice:int) -- shared by the single-node and
  // batch addIntervention()/addObservation() dispatchers below.
  gum::KTBNInference< double >::NodeKey nodeKeyFromPyObject(PyObject* key) {
    if (PyTuple_Check(key) && PyTuple_Size(key) == 2) {
      const std::string base = stringFromPyObject(PyTuple_GetItem(key, 0));
      if (base == "")
        GUM_ERROR(gum::InvalidArgument, "a (base,slice) node key's base must be a str");
      const long slice = PyLong_AsLong(PyTuple_GetItem(key, 1));
      return std::pair< std::string, int >(base, int(slice));
    }
    const std::string name = stringFromPyObject(key);
    if (name == "")
      GUM_ERROR(gum::InvalidArgument, "a node key must be a str or a (str,int) tuple");
    return name;
  }

  // Fills a batch addIntervention()/addObservation() vector from a Python
  // dict {node_key: value}. Shared by both methods since they take the same
  // std::vector<std::pair<NodeKey, KTBNModality>> shape.
  void populateKTBNNodeModalityVectorFromPyDict(
      std::vector< std::pair< gum::KTBNInference< double >::NodeKey, gum::KTBNModality > >& v,
      PyObject*                                                                             dict) {
    if (!PyDict_Check(dict)) GUM_ERROR(gum::InvalidArgument, "argument must be a dict");
    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value))
      v.emplace_back(nodeKeyFromPyObject(key), ktbnModalityFromPyObject(value));
  }

  // ---------------------------------------------------------------------
  // Output conversions: (base,slice) pairs, arcs, and misc introspection
  // vectors have no generic SWIG typemap (unlike vector<double>/<string>),
  // so KTBN's own %typemap(out) blocks (in swigsrc/ktbn.i and
  // swigsrc/ktbnLearner.i) delegate to these. All return a new Python tuple
  // reference, mirroring gum_typemaps.i's vector<double>/<string> style.
  // ---------------------------------------------------------------------

  // (base,slice) -> Python (str,int) tuple.
  PyObject* PyTupleFromBaseSlice(const std::pair< std::string, int >& p) {
    PyObject* t = PyTuple_New(2);
    PyTuple_SET_ITEM(t, 0, PyUnicode_FromString(p.first.c_str()));
    PyTuple_SET_ITEM(t, 1, PyLong_FromLong(p.second));
    return t;
  }

  // KTBN::nodes()/parents()/children() -> tuple[tuple[str,int], ...]
  PyObject* PyTupleFromBaseSliceVector(const std::vector< std::pair< std::string, int > >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i)
      PyTuple_SET_ITEM(t, i, PyTupleFromBaseSlice(v[std::size_t(i)]));
    return t;
  }

  // KTBN::arcs() -> tuple[tuple[tuple[str,int], tuple[str,int]], ...]
  PyObject* PyTupleFromArcVector(
      const std::vector<
          std::pair< std::pair< std::string, int >, std::pair< std::string, int > > >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i) {
      const auto& a   = v[std::size_t(i)];
      PyObject*   arc = PyTuple_New(2);
      PyTuple_SET_ITEM(arc, 0, PyTupleFromBaseSlice(a.first));
      PyTuple_SET_ITEM(arc, 1, PyTupleFromBaseSlice(a.second));
      PyTuple_SET_ITEM(t, i, arc);
    }
    return t;
  }

  // KTBNLearner/KTBNAdaptiveLearner::latentVariables() -> tuple[tuple[str,str], ...]
  PyObject* PyTupleFromStrStrVector(const std::vector< std::pair< std::string, std::string > >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i) {
      const auto& p = v[std::size_t(i)];
      PyObject*   e = PyTuple_New(2);
      PyTuple_SET_ITEM(e, 0, PyUnicode_FromString(p.first.c_str()));
      PyTuple_SET_ITEM(e, 1, PyUnicode_FromString(p.second.c_str()));
      PyTuple_SET_ITEM(t, i, e);
    }
    return t;
  }

  // KTBNAdaptiveLearner::scorePerCandidateK() -> tuple[tuple[int,float], ...]
  PyObject* PyTupleFromSizeDoubleVector(const std::vector< std::pair< gum::Size, double > >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i) {
      const auto& p = v[std::size_t(i)];
      PyObject*   e = PyTuple_New(2);
      PyTuple_SET_ITEM(e, 0, PyLong_FromUnsignedLong((unsigned long)p.first));
      PyTuple_SET_ITEM(e, 1, PyFloat_FromDouble(p.second));
      PyTuple_SET_ITEM(t, i, e);
    }
    return t;
  }

  // KTBNLearner/KTBNAdaptiveLearner::state() -> tuple[tuple[str,str,str], ...]
  PyObject* PyTupleFromStrStrStrVector(
      const std::vector< std::tuple< std::string, std::string, std::string > >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i) {
      const auto& e3 = v[std::size_t(i)];
      PyObject*   e  = PyTuple_New(3);
      PyTuple_SET_ITEM(e, 0, PyUnicode_FromString(std::get< 0 >(e3).c_str()));
      PyTuple_SET_ITEM(e, 1, PyUnicode_FromString(std::get< 1 >(e3).c_str()));
      PyTuple_SET_ITEM(e, 2, PyUnicode_FromString(std::get< 2 >(e3).c_str()));
      PyTuple_SET_ITEM(t, i, e);
    }
    return t;
  }

  // KTBNLearner::nbRows() -> tuple[int, ...]
  PyObject* PyTupleFromSizeVector(const std::vector< gum::Size >& v) {
    const Py_ssize_t n = static_cast< Py_ssize_t >(v.size());
    PyObject*        t = PyTuple_New(n);
    for (Py_ssize_t i = 0; i < n; ++i)
      PyTuple_SET_ITEM(t, i, PyLong_FromUnsignedLong((unsigned long)v[std::size_t(i)]));
    return t;
  }

}   // namespace PyAgrumHelper
#endif   // PYAGRUM_HELPER_KTBN
