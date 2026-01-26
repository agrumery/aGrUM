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

#pragma once


/** @file
 * @brief Source implementation of SimpleCPTDisturber.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 *
 */
#include <agrum/BN/generator/simpleCPTDisturber.h>

namespace gum {

  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTDisturber< GUM_SCALAR >::SimpleCPTDisturber() : ICPTDisturber< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(SimpleCPTDisturber);
  }

  // Destructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTDisturber< GUM_SCALAR >::~SimpleCPTDisturber() {
    GUM_DESTRUCTOR(SimpleCPTDisturber);
  }

  // Generates a CPT using GUM_SCALAR.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.

  template < typename GUM_SCALAR >
  void SimpleCPTDisturber< GUM_SCALAR >::disturbReducCPT(NodeId                  node,
                                                         BayesNet< GUM_SCALAR >& bayesNet,
                                                         Tensor< GUM_SCALAR >&   src,
                                                         Tensor< GUM_SCALAR >&   marg) {
    Instantiation i_src(src);
    Instantiation i_dest(bayesNet.cpt(node));
    Instantiation i_marg(marg);

    for (i_dest.setFirst(); !i_dest.end(); ++i_dest) {
      GUM_SCALAR potval = 0;

      i_src.setVals(i_dest);
      for (i_marg.setFirst(); !i_marg.end(); ++i_marg) {
        i_src.setVals(i_marg);
        potval += src.get(i_src) * marg.get(i_marg);
      }

      bayesNet.cpt(node).set(i_dest, potval);
    }

    bayesNet.cpt(node).normalizeAsCPT();
  }

  template < typename GUM_SCALAR >
  void SimpleCPTDisturber< GUM_SCALAR >::disturbAugmCPT(NodeId                  node,
                                                        BayesNet< GUM_SCALAR >& bayesNet,
                                                        Tensor< GUM_SCALAR >&   src,
                                                        GUM_SCALAR              variation) {
    Instantiation i(bayesNet.cpt(node));

    for (i.setFirst(); !i.end(); ++i)
      bayesNet.cpt(node).set(i, src.get(i) + GUM_SCALAR(randomProba()) * variation);

    bayesNet.cpt(node).normalizeAsCPT();
  }

} /* namespace gum */
