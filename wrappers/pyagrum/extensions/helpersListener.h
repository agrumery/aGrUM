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
 * @brief C++ helper functions for the Python listeners (PythonBNListener,
 * PythonApproximationListener...)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>

#include <agrum/base/core/exceptions.h>

namespace PyAgrumHelper {

  /**
   * Calls the Python callback of a listener with the tuple args (stolen reference, may be nullptr
   * if its construction failed). An exception raised by the callback cannot go through the C++ code
   * which notified the listener: it is reported as unraisable (sys.unraisablehook) and cleared, so
   * that no Python exception stays set.
   */
  inline void callListener(PyObject* callback, PyObject* args) {
    if (args == nullptr) {
      PyErr_WriteUnraisable(callback);
      return;
    }
    PyObject* result = PyObject_Call(callback, args, nullptr);
    if (result == nullptr) PyErr_WriteUnraisable(callback);
    Py_XDECREF(result);
    Py_DECREF(args);
  }

  /**
   * Replaces the callback stored in slot by pyfunc.
   * @throw gum::InvalidArgument if pyfunc is not callable (slot is then unchanged)
   */
  inline void setListenerCallback(PyObject*& slot, PyObject* pyfunc) {
    if (!PyCallable_Check(pyfunc)) { GUM_ERROR(gum::InvalidArgument, "Need a callable object!") }
    Py_INCREF(pyfunc);
    Py_XDECREF(slot);
    slot = pyfunc;
  }
}   // namespace PyAgrumHelper
