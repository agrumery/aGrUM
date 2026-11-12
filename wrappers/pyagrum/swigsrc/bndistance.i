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







%define ADD_COMPUTE_TO_BNDISTANCE(classname)
%extend classname {
PyAgrumDict* compute(void) {
  // the values are read in this order (the first one may launch the computation)
  const double     klPQ         = self->klPQ();
  const Py_ssize_t errorPQ      = static_cast< Py_ssize_t >(self->errorPQ());
  const double     klQP         = self->klQP();
  const Py_ssize_t errorQP      = static_cast< Py_ssize_t >(self->errorQP());
  const double     hellinger    = self->hellinger();
  const double     bhattacharya = self->bhattacharya();
  const double     jsd          = self->jsd();
  return Py_BuildValue("{s:d,s:n,s:d,s:n,s:d,s:d,s:d}",
                       "klPQ", klPQ, "errorPQ", errorPQ, "klQP", klQP, "errorQP", errorQP,
                       "hellinger", hellinger, "bhattacharya", bhattacharya, "jensen-shannon", jsd);
}
}
%enddef

ADD_COMPUTE_TO_BNDISTANCE(gum::ExactBNdistance)
ADD_COMPUTE_TO_BNDISTANCE(gum::GibbsBNdistance)
ADD_COMPUTE_TO_BNDISTANCE(gum::MCBNDistance)

// a distance only references its two Bayesian networks: the Python distance keeps them alive (when built from
// another distance, it keeps the Bayesian networks of this one)
%define KEEP_BNS_OF_BNDISTANCE(classname)
%pythonappend gum::classname<double>::classname %{
  self._models = getattr(args[0], "_models", None) if len(args) == 1 else (args[0], args[1])
%}
%enddef

KEEP_BNS_OF_BNDISTANCE(ExactBNdistance)
KEEP_BNS_OF_BNDISTANCE(GibbsBNdistance)
KEEP_BNS_OF_BNDISTANCE(MCBNDistance)
