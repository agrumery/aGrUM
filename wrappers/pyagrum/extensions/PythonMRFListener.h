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
 * @brief This file contains definition for a listener of the structure of an undirected graphical
 * model (MRF) in python
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <Python.h>

#include <agrum/base/graphicalModels/UGmodel.h>
#include <agrum/base/graphs/parts/listeners/undiGraphListener.h>

class PythonMRFListener: public gum::UndiGraphListener {
  private:
  PyObject* _pyWhenNodeAdded_;
  PyObject* _pyWhenNodeDeleted_;
  PyObject* _pyWhenEdgeAdded_;
  PyObject* _pyWhenEdgeDeleted_;

  const gum::VariableNodeMap* _map_;

  void _checkCallable_(PyObject* pyfunc) {
    if (!PyCallable_Check(pyfunc)) { PyErr_SetString(PyExc_TypeError, "Need a callable object!"); }
  }

  public:
  PythonMRFListener(const gum::UGmodel* model, const gum::VariableNodeMap* vnm) :
      gum::UndiGraphListener(&(model->internalGraph())), _map_(vnm) {
    _pyWhenEdgeAdded_ = _pyWhenEdgeDeleted_ = (PyObject*)0;
    _pyWhenNodeAdded_ = _pyWhenNodeDeleted_ = (PyObject*)0;
  }

  ~PythonMRFListener() {
    if (_pyWhenEdgeAdded_) Py_DECREF(_pyWhenEdgeAdded_);

    if (_pyWhenEdgeDeleted_) Py_DECREF(_pyWhenEdgeDeleted_);

    if (_pyWhenNodeAdded_) Py_DECREF(_pyWhenNodeAdded_);

    if (_pyWhenNodeDeleted_) Py_DECREF(_pyWhenNodeDeleted_);
  }

  virtual void whenNodeAdded(const void* source, gum::NodeId id) {
    // we could check if source==_digraph !!!
    if (_pyWhenNodeAdded_) {
      PyObject* arglist = Py_BuildValue("(ls)", id, (*_map_)[id].name().c_str());
      PyObject_Call(_pyWhenNodeAdded_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  virtual void whenNodeDeleted(const void*, gum::NodeId id) {
    // we could check if source==_digraph !!!
    if (_pyWhenNodeDeleted_) {
      PyObject* arglist = Py_BuildValue("(l)", id);
      PyObject_Call(_pyWhenNodeDeleted_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  virtual void whenEdgeAdded(const void*, gum::NodeId n1, gum::NodeId n2) {
    // we could check if source==_digraph !!!
    if (_pyWhenEdgeAdded_) {
      PyObject* arglist = Py_BuildValue("(ll)", n1, n2);
      PyObject_Call(_pyWhenEdgeAdded_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  virtual void whenEdgeDeleted(const void*, gum::NodeId n1, gum::NodeId n2) {
    // we could check if source==_digraph !!!
    if (_pyWhenEdgeDeleted_) {
      PyObject* arglist = Py_BuildValue("(ll)", n1, n2);
      PyObject_Call(_pyWhenEdgeDeleted_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  void setWhenEdgeAdded(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenEdgeAdded_ = pyfunc;
    Py_INCREF(pyfunc);
  }

  void setWhenEdgeDeleted(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenEdgeDeleted_ = pyfunc;
    Py_INCREF(pyfunc);
  }

  void setWhenNodeAdded(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenNodeAdded_ = pyfunc;
    Py_INCREF(pyfunc);
  }

  void setWhenNodeDeleted(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenNodeDeleted_ = pyfunc;
    Py_INCREF(pyfunc);
  }
};
