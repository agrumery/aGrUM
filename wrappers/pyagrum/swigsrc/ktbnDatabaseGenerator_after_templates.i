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

// This file contains %extend blocks that must appear AFTER the %template
// instantiation of gum::learning::KTBNDatabaseGenerator<double> in
// aGrUM_wrap_KTBN.i.

// ---------------------------------------------------------------------------
// KTBNDatabaseGenerator<double>::drawSamples
//
// The C++ class exposes two overloads:
//   - drawSamples(Size nbSamples, Size nbTimeSlices, dirPath, csvBaseName,
//     mode, useLabels, csvSeparator)  -- every trajectory shares one horizon.
//     Wraps automatically: every argument type already has a typemap (Size,
//     std::string_view, the nested VarOrderMode enum, bool, std::string), so
//     it needs no %extend and is left to the plain %include'd declaration.
//   - drawSamples(const std::vector<Size>& nbTimeSlices, dirPath, csvBaseName,
//     mode, useLabels, csvSeparator)  -- one horizon per trajectory.
//     std::vector<gum::Size> has no input typemap in gum_typemaps.i (only
//     vector<int>/<double>/<string>/<NodeId> do, and even those only as
//     *output* typemaps for NodeId), so this overload is rebuilt below taking
//     a plain PyObject* (any Python sequence of int).
//
// The two forms differ in arity (7 vs 6 parameters), so SWIG's normal
// overload resolution (by argument count, then by typecheck precedence)
// disambiguates them with no further help: a call passing two leading ints
// matches the native 7-arg overload; a call passing one leading sequence
// falls through PyObject*'s low-precedence typecheck to the 6-arg dispatcher
// below (same reasoning as KTBN::fillCPT's PyObject*-vs-vector<double>
// disambiguation).
// ---------------------------------------------------------------------------
%extend gum::learning::KTBNDatabaseGenerator<double> {
  std::vector<double> drawSamples(
      PyObject* nbTimeSlices,
      std::string_view dirPath,
      std::string_view csvBaseName,
      VarOrderMode mode = VarOrderMode::RANDOM,
      bool useLabels = true,
      std::string csvSeparator = ",") {
    std::vector<gum::Size> v;
    PyObject* iter = PyObject_GetIter(nbTimeSlices);
    if (!iter) GUM_ERROR(gum::InvalidArgument, "nbTimeSlices must be a sequence of int");
    PyObject* item;
    while ((item = PyIter_Next(iter)) != nullptr) {
      if (!PyLong_Check(item)) {
        Py_DECREF(item);
        Py_DECREF(iter);
        GUM_ERROR(gum::InvalidArgument, "nbTimeSlices must be a sequence of int");
      }
      v.push_back(gum::Size(PyLong_AsLong(item)));
      Py_DECREF(item);
    }
    Py_DECREF(iter);
    if (PyErr_Occurred()) GUM_ERROR(gum::InvalidArgument, "nbTimeSlices must be a sequence of int");
    return self->drawSamples(v, dirPath, csvBaseName, mode, useLabels, csvSeparator);
  }
}
