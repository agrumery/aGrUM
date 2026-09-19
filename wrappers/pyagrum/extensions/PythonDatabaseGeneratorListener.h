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

#include <Python.h>

#include <agrum/base/core/progressNotification.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>

class PythonDatabaseGeneratorListener: public gum::ProgressListener {
  private:
  PyObject* _pyWhenProgress_;
  PyObject* _pyWhenStop_;

  void _checkCallable_(PyObject* pyfunc) {
    if (!PyCallable_Check(pyfunc)) { PyErr_SetString(PyExc_TypeError, "Need a callable object!"); }
  }

  public:
  explicit PythonDatabaseGeneratorListener(gum::learning::BNDatabaseGenerator< double >& notif) :
      gum::ProgressListener(notif) {
    _pyWhenProgress_ = _pyWhenStop_ = (PyObject*)0;
  }

  ~PythonDatabaseGeneratorListener() {
    if (_pyWhenProgress_) Py_DECREF(_pyWhenProgress_);

    if (_pyWhenStop_) Py_DECREF(_pyWhenStop_);
  }

  void whenProgress(const void* src, const gum::Size step, const double duration) {
    if (_pyWhenProgress_) {
      PyObject* arglist = Py_BuildValue("(ld)", step, duration);
      PyObject_Call(_pyWhenProgress_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  void whenStop(const void* src, std::string_view message) {
    if (_pyWhenStop_) {
      PyObject* arglist = Py_BuildValue("(s)", message.data());
      PyObject_Call(_pyWhenStop_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  void setWhenProgress(PyObject* pyfunc) {
    _checkCallable_(pyfunc);

    if (_pyWhenProgress_) Py_DECREF(_pyWhenProgress_);

    _pyWhenProgress_ = pyfunc;
    Py_INCREF(pyfunc);
  }

  void setWhenStop(PyObject* pyfunc) {
    _checkCallable_(pyfunc);

    if (_pyWhenStop_) Py_DECREF(_pyWhenStop_);

    _pyWhenStop_ = pyfunc;
    Py_INCREF(pyfunc);
  }
};
