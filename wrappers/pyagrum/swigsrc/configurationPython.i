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

/*
 * Python-specific part of the gum::Configuration binding (the generic part is in
 * wrappers/swig/configuration.i): the two methods below only convert between C++ and Python
 * objects, the logic stays in gum::Configuration.
 */

%{
#include <type_traits>
#include <variant>
%}

%extend gum::Configuration {
  // the value of section.key converted by the C++ configuration according to the declared type of
  // the key: a bool, an int, a float or a str
  PyObject* getValue(std::string_view section, std::string_view key) const {
    return std::visit(
        [](const auto& value) -> PyObject* {
          using T = std::decay_t< decltype(value) >;
          if constexpr (std::is_same_v< T, bool >) return PyBool_FromLong(value ? 1 : 0);
          else if constexpr (std::is_same_v< T, std::int64_t >) return PyLong_FromLongLong(value);
          else if constexpr (std::is_same_v< T, double >) return PyFloat_FromDouble(value);
          else return PyUnicode_FromString(value.c_str());
        },
        self->getValue(section, key));
  }

  // registers a Python callable (without argument) as a hook of the configuration. The hook is
  // called with the GIL; an exception raised by the callable is propagated to whoever changed the
  // configuration. The callable is kept until the end of the process (the configuration is a
  // singleton, no Python object may be released from a static destructor).
  void addPythonHook(PyObject* callable) {
    if (!PyCallable_Check(callable)) {
      PyErr_SetString(PyExc_TypeError, "Need a callable object!");
      throw PythonErrorAlreadySet();
    }
    Py_INCREF(callable);
    self->addHook([callable](const gum::Configuration&) {
      const PyGILState_STATE gil    = PyGILState_Ensure();
      PyObject*              result = PyObject_CallNoArgs(callable);
      const bool             failed = (result == nullptr);
      Py_XDECREF(result);
      PyGILState_Release(gil);
      if (failed) throw PythonErrorAlreadySet();
    });
  }
}
