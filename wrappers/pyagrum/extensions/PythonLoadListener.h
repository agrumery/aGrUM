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
#include <string>

#include <agrum/base/core/signal/listener.h>

#include "helpersListener.h"

class PythonLoadListener: public gum::Listener {
  private:
  PyObject* _whenLoading_ = nullptr;

  public:
  void whenLoading(const void* buffer, int percent) {
    if (_whenLoading_) {
      PyAgrumHelper::callListener(_whenLoading_, Py_BuildValue("(i)", percent));
    }
  }

  /// keeps its own reference to l
  /// @throw gum::InvalidArgument if l is not callable
  void setPythonListener(PyObject* l) { PyAgrumHelper::setListenerCallback(_whenLoading_, l); }

  PythonLoadListener() = default;

  PythonLoadListener(const PythonLoadListener& from) :
      gum::Listener(from), _whenLoading_(from._whenLoading_) {
    Py_XINCREF(_whenLoading_);
  }

  PythonLoadListener& operator=(const PythonLoadListener&) = delete;

  ~PythonLoadListener() { Py_XDECREF(_whenLoading_); }
};

/// fills py_listener with l (a callable or a sequence of callables) and returns the number of
/// listeners
/// @throw gum::InvalidArgument if a listener is not callable or if the sequence cannot be read
int _fillLoadListeners_(std::vector< PythonLoadListener >& py_listener, PyObject* l) {
  if (!l || l == Py_None) return 0;

  if (!PySequence_Check(l)) {
    py_listener.resize(1);
    py_listener[0].setPythonListener(l);
    return 1;
  }

  const Py_ssize_t l_size = PySequence_Size(l);
  if (l_size < 0) {
    PyErr_Clear();
    GUM_ERROR(gum::InvalidArgument, "Cannot read the size of the sequence of listeners")
  }
  py_listener.resize(static_cast< std::size_t >(l_size));
  for (Py_ssize_t i = 0; i < l_size; i++) {
    PyObject* item = PySequence_GetItem(l, i);   // new reference
    if (item == nullptr) {
      PyErr_Clear();
      GUM_ERROR(gum::InvalidArgument, "Cannot read the listener #" << i)
    }
    const bool callable = PyCallable_Check(item);
    if (callable) py_listener[i].setPythonListener(item);
    Py_DECREF(item);
    if (!callable) GUM_ERROR(gum::InvalidArgument, "The listener #" << i << " is not callable")
  }
  return static_cast< int >(l_size);
}
