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
// instantiation of gum::KTBN<double> in aGrUM_wrap_KTBN.i. SWIG does not
// apply %extend declared before %template to the instantiated class; it
// must be declared here, after %template (KTBN) has been processed.

// ---------------------------------------------------------------------------
// KTBN<double>::fillCPT
//
// The C++ class exposes two fillCPT() overloads, neither directly bindable:
//   - fillCPT(base, slice, const std::map<std::pair<std::string,int>,
//     KTBNModality>&, const std::vector<double>&)
//   - fillCPT(node_name, const std::map<std::variant<std::string,
//     std::pair<std::string,int>>, KTBNModality>&, const std::vector<double>&)
// KTBNModality (implicit int-or-str) is never exposed to Python: both
// dispatchers below build it from a plain Python dict {key: int_or_str},
// via the shared helpers in extensions/helpersKTBN.h. `distribution` is left
// as a plain `const std::vector<double>&` -- gum_typemaps.i already maps a
// Python sequence[float] to it, no dispatch needed there.
// ---------------------------------------------------------------------------
%extend gum::KTBN<double> {
  void fillCPT(std::string base, int slice, PyObject* parents, const std::vector<double>& distribution) const {
    std::map<std::pair<std::string,int>, gum::KTBNModality> m;
    PyAgrumHelper::populateKTBNParentPairModalityMapFromPyDict(m, parents);
    self->fillCPT(base, slice, m, distribution);
  }

  void fillCPT(std::string node_name, PyObject* parents, const std::vector<double>& distribution) const {
    std::map<std::variant<std::string,std::pair<std::string,int>>, gum::KTBNModality> m;
    PyAgrumHelper::populateKTBNParentModalityMapFromPyDict(m, parents);
    self->fillCPT(node_name, m, distribution);
  }
}
