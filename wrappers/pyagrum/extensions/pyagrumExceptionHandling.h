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
 * @brief gum:: -> Python exception translation used by the global
 * %exception SWIG feature declared in exceptions.i. Shared via #include
 * (not SWIG %import, which does not carry raw %{ %} code blocks across
 * modules) by every generated SWIG module (core pyagrum + each split
 * submodule): %import does propagate the %exception feature itself, so
 * every module's wrapper call sites call SetPythonizeAgrumException(), but
 * each module's own translation unit still needs this definition.
 *
 * The gum::Exception -> Python exception type mapping needs no per-class
 * catch here: every gum::Exception subclass overrides pythonClassName_()
 * (see GUM_MAKE_ERROR in exceptions.h) with its own exact C++ class name,
 * which is enough to look up the matching SWIG-wrapped Python type.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>
#include <string>

#include <agrum/agrum.h>

static void SetPythonizeAgrumException() {
  try {
    throw;
  } catch (std::bad_cast&) {
    PyErr_SetString(PyExc_RuntimeError, "C++ Bad Cast");
  } catch (gum::SyntaxError& e) {
    PyErr_SetString(PyExc_SyntaxError, e.errorContent().c_str());
    PyErr_SyntaxLocationEx(e.filename().c_str(), e.line(), e.col());
  } catch (gum::Exception& e) {
    const std::string swigTypeName = std::string("gum::") + e.pythonClassName_() + " *";
    PyErr_SetString(SWIG_Python_ExceptionType(SWIG_TypeQuery(swigTypeName.c_str())), e.what());
  } catch (std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); }
}
