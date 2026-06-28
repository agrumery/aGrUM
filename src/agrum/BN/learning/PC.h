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
 * @brief PC (Peter-Clark) constraint-based structure learning algorithm.
 *
 * Learns a PDAG (CPDAG) from data via conditional independence tests.
 * Three phases: skeleton discovery, v-structure orientation, Meek rules.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PC_H
#define GUM_LEARNING_PC_H

#include <agrum/BN/learning/CIBasedLearning.h>

namespace gum {

  namespace learning {

    /**
     * @class PC
     * @brief PC (Peter-Clark) constraint-based structure learning algorithm.
     *
     * Inherits skeleton discovery, v-structure orientation, and all CI-test
     * state from CIBasedLearning.  PC only adds learnMixedStructure(), which
     * calls orientUnshieldedColliders_() then applies Meek rules.
     *
     * The independence test is injected via setIndependenceTest() (inherited)
     * and is non-owning: the caller is responsible for the test object's
     * lifetime.
     *
     * @ingroup learning_group
     */
    class PC: public CIBasedLearning {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      PC();
      PC(const PC&);
      PC(PC&&) noexcept;
      ~PC() override;

      PC& operator=(const PC&);
      PC& operator=(PC&&) noexcept;

      /// @}

      // ##########################################################################
      /// @name Learning
      // ##########################################################################
      /// @{

      /// learnSkeleton()  — inherited from CIBasedLearning
      /// setIndependenceTest(), setAlpha(), setStable(), setMaxCondSetSize(),
      /// setUCPriority(), alpha(), ucPriority() — all inherited from CIBasedLearning

      MixedGraph learnMixedStructure(MixedGraph graph) override;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_PC_H */
