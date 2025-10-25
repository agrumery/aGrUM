/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef AGRUM_AGRUMAPPROXIMATIONUTILS_H_H
#define AGRUM_AGRUMAPPROXIMATIONUTILS_H_H

template < typename GUM_SCALAR >
void unsharpen(const gum::BayesNet< GUM_SCALAR >& bn) {
  for (const auto nod: bn.nodes().asNodeSet()) {
    bn.cpt(nod).translate(1);
    bn.cpt(nod).normalizeAsCPT();
  }
}

template < typename GUM_SCALAR >
bool compareInference(const std::string&                       file,
                      int                                      line,
                      const gum::BayesNet< GUM_SCALAR >&       bn,
                      gum::LazyPropagation< GUM_SCALAR >&      lazy,
                      gum::ApproximateInference< GUM_SCALAR >& inf,
                      double                                   errmax = 5e-2) {
  GUM_SCALAR  err    = static_cast< GUM_SCALAR >(0);
  std::string argstr = "";

  for (const auto& node: bn.nodes()) {
    if (!inf.BN().dag().exists(node)) continue;
    GUM_SCALAR e = (inf.posterior(node) - lazy.posterior(node)).abs().max();

    if (e > err) {
      err    = e;
      argstr = bn.variable(node).name() + " (err=" + std::to_string(err) + ") : \n";
      argstr += "  lazy : " + lazy.posterior(node).toString() + "\n";
      argstr += "  inf : " + inf.posterior(node).toString() + " \n";
    }
  }
  /*if (err > errmax) {
     _GUM_PRINT(file, line, argstr);
     _GUM_PRINT(file, line, inf.messageApproximationScheme());
  }*/
  return err <= errmax;
}

#define GUM_APPROX_TEST_BEGIN_ITERATION \
  bool res = false;                     \
  (void)res;                            \
  for (int ii = 0; ii < 10; ii++) {
#define GUM_APPROX_TEST_END_ITERATION(ERRMAX)                        \
  if (compareInference(__FILE__, __LINE__, bn, lazy, inf, ERRMAX)) { \
    res = true;                                                      \
    break;                                                           \
  }                                                                  \
  std::cout << "![" << __LINE__ << "]" << std::flush;                \
  if (ii == 10) TS_FAIL("even with 10 tries.");                      \
  }
#endif   // AGRUM_AGRUMAPPROXIMATIONUTILS_H_H
