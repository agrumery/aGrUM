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


// Conversions for the free functions of gum::graph wrapped in aGrUM_wrap_graphAlgorithms.i, used by
// the pure Python package pyagrum.graph. The input typemaps are matched by the parameter name only,
// so that the other parameters of the same types keep their own conversions: these names are only
// used by the declarations of aGrUM_wrap_graphAlgorithms.i (BNLearner.setTotalOrder has its own
// parameter `order`).

// std::optional<gum::DAG> / std::optional<gum::PDAG> -> a new graph or None
// The type is looked up at run time: a $descriptor would add the graph types to the type table of
// every module importing pyagrum.i, including the ones which do not include their headers (prm).
%define GUM_OPTIONAL_GRAPH_OUT(classname)
%typemap(out) std::optional< classname > {
  // $1 may be a SwigValueWrapper, whose operator& gives the address of the optional
  std::optional< classname >* _opt = &$1;
  if (_opt->has_value()) {
    static swig_type_info* _type = SWIG_TypeQuery(#classname " *");
    $result = SWIG_NewPointerObj(new classname(std::move(**_opt)), _type, SWIG_POINTER_OWN);
  } else {
    $result = Py_None;
    Py_INCREF(Py_None);
  }
}
%enddef
GUM_OPTIONAL_GRAPH_OUT(gum::DAG)
GUM_OPTIONAL_GRAPH_OUT(gum::PDAG)

// Python iterable of int -> const std::vector<gum::NodeId>& visitFirst (lexBFS)
%typemap(in) const std::vector< gum::NodeId >& visitFirst (std::vector< gum::NodeId > _tmp_first) {
  PyObject* _iter = PyObject_GetIter($input);
  if (_iter == nullptr) SWIG_fail;
  while (PyObject* _item = PyIter_Next(_iter)) {
    const std::size_t _id = PyLong_AsSize_t(_item);
    Py_DECREF(_item);
    if (_id == static_cast< std::size_t >(-1) && PyErr_Occurred()) {
      Py_DECREF(_iter);
      SWIG_fail;
    }
    _tmp_first.push_back(static_cast< gum::NodeId >(_id));
  }
  Py_DECREF(_iter);
  if (PyErr_Occurred()) SWIG_fail;
  $1 = &_tmp_first;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const std::vector< gum::NodeId >& visitFirst {
  $1 = PyList_Check($input) || PyTuple_Check($input);
}

// Python iterable of int -> const gum::Sequence<gum::NodeId>& eliminationOrder (isPerfectEliminationOrdering)
%typemap(in) const gum::Sequence< gum::NodeId >& eliminationOrder (gum::Sequence< gum::NodeId > _tmp_order) {
  PyObject* _iter = PyObject_GetIter($input);
  if (_iter == nullptr) SWIG_fail;
  while (PyObject* _item = PyIter_Next(_iter)) {
    const std::size_t _id = PyLong_AsSize_t(_item);
    Py_DECREF(_item);
    if (_id == static_cast< std::size_t >(-1) && PyErr_Occurred()) {
      Py_DECREF(_iter);
      SWIG_fail;
    }
    try {
      _tmp_order.insert(static_cast< gum::NodeId >(_id));
    } catch (const gum::DuplicateElement&) {
      Py_DECREF(_iter);
      PyErr_SetString(PyExc_ValueError, "the ordering contains a node twice");
      SWIG_fail;
    }
  }
  Py_DECREF(_iter);
  if (PyErr_Occurred()) SWIG_fail;
  $1 = &_tmp_order;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const gum::Sequence< gum::NodeId >& eliminationOrder {
  $1 = PyList_Check($input) || PyTuple_Check($input);
}
