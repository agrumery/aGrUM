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

// Pre-template directives for gum::KTBN. Must be %include'd BEFORE
// aGrUM_wrap_KTBN.i's %include of KTBN.h / %template (KTBN), since SWIG only
// applies a %typemap to declarations parsed after it (same ordering rule as
// %rename/%ignore elsewhere in the codebase).

// KTBN::nodes()/parents()/children() return std::vector<std::pair<std::string,int>>
// by value -- a (base,slice) pair vector with no generic SWIG typemap (unlike
// vector<double>/<string> in gum_typemaps.i). Delegates to the shared
// PyAgrumHelper converters (extensions/helpersKTBN.h) so KTBN::arcs() below
// can reuse PyTupleFromBaseSlice() for each endpoint.
%typemap(out) std::vector<std::pair<std::string,int>> {
  $result = PyAgrumHelper::PyTupleFromBaseSliceVector($1);
}

// KTBN::arcs() returns std::vector<std::pair<std::pair<std::string,int>,
// std::pair<std::string,int>>> by value -- likewise has no generic typemap.
%typemap(out) std::vector<std::pair<std::pair<std::string,int>,std::pair<std::string,int>>> {
  $result = PyAgrumHelper::PyTupleFromArcVector($1);
}
