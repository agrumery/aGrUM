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
 * @brief This file contains definition for a "loading action" listener in python
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <Python.h>

#include <agrum/base/core/signal/listener.h>

class PythonLoadListener: public gum::Listener {
  private:
  PyObject* _whenLoading_;

  public:
  void whenLoading(const void* buffer, int percent) {
    if (_whenLoading_) {
      PyObject* arglist = Py_BuildValue("(i)", percent);
      PyObject_Call(_whenLoading_, arglist, NULL);
      Py_DECREF(arglist);
    }
  }

  bool setPythonListener(PyObject* l) {
    if (!PyCallable_Check(l)) {
      return false;
    } else {
      _whenLoading_ = l;
      Py_INCREF(l);
      return true;
    }
  }

  PythonLoadListener() { _whenLoading_ = (PyObject*)0; }

  ~PythonLoadListener() {
    if (_whenLoading_) Py_DECREF(_whenLoading_);
  }
};

int _fillLoadListeners_(std::vector< PythonLoadListener >& py_listener, PyObject* l) {
  if (!l) return 0;

  if (l == Py_None) return 0;

  int       l_size = 0;
  PyObject* item;

  if (PySequence_Check(l)) {
    l_size = (int)PySequence_Size(l);
    py_listener.resize(l_size);

    for (int i = 0; i < l_size; i++) {
      item = PySequence_GetItem(l, i);

      if (!py_listener[i].setPythonListener(item)) return 0;
    }
  } else {
    l_size = 1;
    py_listener.resize(l_size);

    if (!py_listener[0].setPythonListener(l)) return 0;
  }

  return l_size;
}
