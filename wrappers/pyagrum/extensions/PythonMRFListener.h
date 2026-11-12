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
#include <string>

#include <agrum/base/graphicalModels/UGmodel.h>
#include <agrum/base/graphs/parts/listeners/undiGraphListener.h>

#include "helpersListener.h"

class PythonMRFListener: public gum::UndiGraphListener {
  private:
  PyObject* _pyWhenNodeAdded_   = nullptr;
  PyObject* _pyWhenNodeDeleted_ = nullptr;
  PyObject* _pyWhenEdgeAdded_   = nullptr;
  PyObject* _pyWhenEdgeDeleted_ = nullptr;

  const gum::VariableNodeMap* _map_;

  public:
  PythonMRFListener(const gum::UGmodel* model, const gum::VariableNodeMap* vnm) :
      gum::UndiGraphListener(&(model->internalGraph())), _map_(vnm) {}

  // the callbacks are owned references: a copy would release them twice
  PythonMRFListener(const PythonMRFListener&)            = delete;
  PythonMRFListener& operator=(const PythonMRFListener&) = delete;

  ~PythonMRFListener() {
    Py_XDECREF(_pyWhenEdgeAdded_);
    Py_XDECREF(_pyWhenEdgeDeleted_);
    Py_XDECREF(_pyWhenNodeAdded_);
    Py_XDECREF(_pyWhenNodeDeleted_);
  }

  virtual void whenNodeAdded(const void* source, gum::NodeId id) {
    // we could check if source==_digraph !!!
    if (_pyWhenNodeAdded_) {
      PyAgrumHelper::callListener(
          _pyWhenNodeAdded_,
          Py_BuildValue("(ns)", static_cast< Py_ssize_t >(id), (*_map_)[id].name().c_str()));
    }
  }

  virtual void whenNodeDeleted(const void*, gum::NodeId id) {
    // we could check if source==_digraph !!!
    if (_pyWhenNodeDeleted_) {
      PyAgrumHelper::callListener(_pyWhenNodeDeleted_,
                                  Py_BuildValue("(n)", static_cast< Py_ssize_t >(id)));
    }
  }

  virtual void whenEdgeAdded(const void*, gum::NodeId n1, gum::NodeId n2) {
    // we could check if source==_digraph !!!
    if (_pyWhenEdgeAdded_) {
      PyAgrumHelper::callListener(
          _pyWhenEdgeAdded_,
          Py_BuildValue("(nn)", static_cast< Py_ssize_t >(n1), static_cast< Py_ssize_t >(n2)));
    }
  }

  virtual void whenEdgeDeleted(const void*, gum::NodeId n1, gum::NodeId n2) {
    // we could check if source==_digraph !!!
    if (_pyWhenEdgeDeleted_) {
      PyAgrumHelper::callListener(
          _pyWhenEdgeDeleted_,
          Py_BuildValue("(nn)", static_cast< Py_ssize_t >(n1), static_cast< Py_ssize_t >(n2)));
    }
  }

  void setWhenEdgeAdded(PyObject* pyfunc) {
    PyAgrumHelper::setListenerCallback(_pyWhenEdgeAdded_, pyfunc);
  }

  void setWhenEdgeDeleted(PyObject* pyfunc) {
    PyAgrumHelper::setListenerCallback(_pyWhenEdgeDeleted_, pyfunc);
  }

  void setWhenNodeAdded(PyObject* pyfunc) {
    PyAgrumHelper::setListenerCallback(_pyWhenNodeAdded_, pyfunc);
  }

  void setWhenNodeDeleted(PyObject* pyfunc) {
    PyAgrumHelper::setListenerCallback(_pyWhenNodeDeleted_, pyfunc);
  }
};
