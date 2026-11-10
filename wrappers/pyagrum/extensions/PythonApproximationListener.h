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
#include <string>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>

#include "helpersListener.h"

class PythonApproximationListener: public gum::ApproximationSchemeListener {
  private:
  PyObject* _pyWhenProgress_;
  PyObject* _pyWhenStop_;

  public:
  PythonApproximationListener(gum::IApproximationSchemeConfiguration& algo) :
      gum::ApproximationSchemeListener(algo) {
    _pyWhenProgress_ = _pyWhenStop_ = (PyObject*)0;
  }

  ~PythonApproximationListener() {
    if (_pyWhenProgress_) Py_DECREF(_pyWhenProgress_);

    if (_pyWhenStop_) Py_DECREF(_pyWhenStop_);
  }

  void whenProgress(const void*     src,
                    const gum::Size step,
                    const double    error,
                    const double    duration) {
    if (_pyWhenProgress_) {
      PyAgrumHelper::callListener(
          _pyWhenProgress_,
          Py_BuildValue("(ndd)", static_cast< Py_ssize_t >(step), error, duration));
    }
  }

  void whenStop(const void* src, std::string_view message) {
    if (_pyWhenStop_) {
      PyAgrumHelper::callListener(_pyWhenStop_, Py_BuildValue("(s)", std::string(message).c_str()));
    }
  }

  void setWhenProgress(PyObject* pyfunc) {
    PyAgrumHelper::setListenerCallback(_pyWhenProgress_, pyfunc);
  }

  void setWhenStop(PyObject* pyfunc) { PyAgrumHelper::setListenerCallback(_pyWhenStop_, pyfunc); }
};
