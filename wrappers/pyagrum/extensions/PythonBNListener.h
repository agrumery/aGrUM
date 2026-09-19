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
 * @brief This file contains definition for a BN listener in python
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <Python.h>

#include <agrum/base/graphs/parts/listeners/diGraphListener.h>
#include <agrum/BN/BayesNet.h>

class PythonBNListener: public gum::DiGraphListener {
  private:
  PyObject* _pyWhenNodeAdded_;
  PyObject* _pyWhenNodeDeleted_;
  PyObject* _pyWhenArcAdded_;
  PyObject* _pyWhenArcDeleted_;

  const gum::VariableNodeMap* _map_;

  void _checkCallable_(PyObject* pyfunc) {
    if (!PyCallable_Check(pyfunc)) { PyErr_SetString(PyExc_TypeError, "Need a callable object!"); }
  }

  public:
  PythonBNListener(const gum::BayesNet< double >* bn, const gum::VariableNodeMap* vnm) :
      gum::DiGraphListener(&(bn->internalDag())), _map_(vnm) {
    _pyWhenArcAdded_ = _pyWhenArcDeleted_ = (PyObject*)0;
    _pyWhenNodeAdded_ = _pyWhenNodeDeleted_ = (PyObject*)0;
  }

  ~PythonBNListener() {
    if (_pyWhenArcAdded_) Py_DECREF(_pyWhenArcAdded_);

    if (_pyWhenArcDeleted_) Py_DECREF(_pyWhenArcDeleted_);

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

  virtual void whenArcAdded(const void*, gum::NodeId src, gum::NodeId dst) {
    // we could check if source==_digraph !!!
    if (_pyWhenArcAdded_) {
      PyObject* arglist = Py_BuildValue("(ll)", src, dst);
      PyObject_Call(_pyWhenArcAdded_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  virtual void whenArcDeleted(const void*, gum::NodeId src, gum::NodeId dst) {
    // we could check if source==_digraph !!!
    if (_pyWhenArcDeleted_) {
      PyObject* arglist = Py_BuildValue("(ll)", src, dst);
      PyObject_Call(_pyWhenArcDeleted_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  void setWhenArcAdded(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenArcAdded_ = pyfunc;
    Py_INCREF(pyfunc);
  }

  void setWhenArcDeleted(PyObject* pyfunc) {
    _checkCallable_(pyfunc);
    _pyWhenArcDeleted_ = pyfunc;
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
