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

#include <agrum/FMDP/planning/IOperatorStrategy.h>

namespace gum {

  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      IOperatorStrategy< GUM_ELEMENT >::frameTransition_(const DiscreteVariable* x,
                                                         const DiscreteVariable* xp) {
    MultiDimFunctionGraph< GUM_ELEMENT >* transition = this->getFunctionInstance();
    transition->add(*x);
    transition->add(*xp);
    auto*      manager = transition->manager();
    const auto root    = manager->addInternalNode(x);
    manager->setRootNode(root);
    const auto zero = manager->addTerminalNode(GUM_ELEMENT(0));
    const auto one  = manager->addTerminalNode(GUM_ELEMENT(1));
    for (Idx i = 0; i < x->domainSize(); ++i) {
      const auto node = manager->addInternalNode(xp);
      manager->setSon(root, i, node);
      for (Idx ip = 0; ip < xp->domainSize(); ++ip)
        manager->setSon(node, ip, (i == ip) ? one : zero);
    }
    return transition;
  }

  template < typename GUM_ELEMENT >
  void IOperatorStrategy< GUM_ELEMENT >::deleteFunctionGraph_(
      const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
      const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
      Idx                                         del) {
    if (del == 1 || del == 3) delete f1;
    if (del >= 2) delete f2;
  }

  template < typename GUM_ELEMENT >
  void IOperatorStrategy< GUM_ELEMENT >::deleteFunctionGraph_(
      const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f1,
      const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f2,
      Idx                                                                                  del) {
    if (del == 1 || del == 3) delete f1;
    if (del >= 2) delete f2;
  }

}   // namespace gum
