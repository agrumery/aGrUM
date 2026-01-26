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
 * @brief Class used to manipulate o4DGContext in Function Graph Operations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/o4DGContext.h>

namespace gum {

  // First 140 prime numbers...
  // see e.g.
  // http://en.wikipedia.org/wiki/List_of_prime_numbers#The_first_500_prime_numbers
  // if more is needed...
  // Don't forget to change  _nbLogPrime_ to match number of log2 of prime
  // numbers  in that table.
  const Idx O4DGContext::_nbLogPrime_ = 140;

  const double O4DGContext::_logPrime_[]
      = {std::log2(2.0),   std::log2(3.0),   std::log2(5.0),   std::log2(7.0),   std::log2(11.0),
         std::log2(13.0),  std::log2(17.0),  std::log2(19.0),  std::log2(23.0),  std::log2(29.0),
         std::log2(31.0),  std::log2(37.0),  std::log2(41.0),  std::log2(43.0),  std::log2(47.0),
         std::log2(53.0),  std::log2(59.0),  std::log2(61.0),  std::log2(67.0),  std::log2(71.0),
         std::log2(73.0),  std::log2(79.0),  std::log2(83.0),  std::log2(89.0),  std::log2(97.0),
         std::log2(101.0), std::log2(103.0), std::log2(107.0), std::log2(109.0), std::log2(113.0),
         std::log2(127.0), std::log2(131.0), std::log2(137.0), std::log2(139.0), std::log2(149.0),
         std::log2(151.0), std::log2(157.0), std::log2(163.0), std::log2(167.0), std::log2(173.0),
         std::log2(179.0), std::log2(181.0), std::log2(191.0), std::log2(193.0), std::log2(197.0),
         std::log2(199.0), std::log2(211.0), std::log2(223.0), std::log2(227.0), std::log2(229.0),
         std::log2(233.0), std::log2(239.0), std::log2(241.0), std::log2(251.0), std::log2(257.0),
         std::log2(263.0), std::log2(269.0), std::log2(271.0), std::log2(277.0), std::log2(281.0),
         std::log2(283.0), std::log2(293.0), std::log2(307.0), std::log2(311.0), std::log2(313.0),
         std::log2(317.0), std::log2(331.0), std::log2(337.0), std::log2(347.0), std::log2(349.0),
         std::log2(353.0), std::log2(359.0), std::log2(367.0), std::log2(373.0), std::log2(379.0),
         std::log2(383.0), std::log2(389.0), std::log2(397.0), std::log2(401.0), std::log2(409.0),
         std::log2(419.0), std::log2(421.0), std::log2(431.0), std::log2(433.0), std::log2(439.0),
         std::log2(443.0), std::log2(449.0), std::log2(457.0), std::log2(461.0), std::log2(463.0),
         std::log2(467.0), std::log2(479.0), std::log2(487.0), std::log2(491.0), std::log2(499.0),
         std::log2(503.0), std::log2(509.0), std::log2(521.0), std::log2(523.0), std::log2(541.0),
         std::log2(547.0), std::log2(557.0), std::log2(563.0), std::log2(569.0), std::log2(571.0),
         std::log2(577.0), std::log2(587.0), std::log2(593.0), std::log2(599.0), std::log2(601.0),
         std::log2(607.0), std::log2(613.0), std::log2(617.0), std::log2(619.0), std::log2(631.0),
         std::log2(641.0), std::log2(643.0), std::log2(647.0), std::log2(653.0), std::log2(659.0),
         std::log2(661.0), std::log2(673.0), std::log2(677.0), std::log2(683.0), std::log2(691.0),
         std::log2(701.0), std::log2(709.0), std::log2(719.0), std::log2(727.0), std::log2(733.0),
         std::log2(739.0), std::log2(743.0), std::log2(751.0), std::log2(757.0), std::log2(761.0),
         std::log2(769.0), std::log2(773.0), std::log2(787.0), std::log2(797.0), std::log2(809.0)};

  const Idx O4DGContext::_offsetv_ = O4DGContext::_nbLogPrime_ - 3;
  const Idx O4DGContext::_offset1_ = O4DGContext::_nbLogPrime_ - 1;
  const Idx O4DGContext::_offset2_ = O4DGContext::_nbLogPrime_ - 2;

  // Default constructor.
  O4DGContext::O4DGContext(Idx* varInst, Idx nbVar) :
      _DG1ExploredNode_(0), _DG2ExploredNode_(0), _key_(0) {
    GUM_CONSTRUCTOR(O4DGContext);
    _varInstantiation_ = varInst;
    _nbVar_            = nbVar;
  }

  // Default Destructor.
  O4DGContext::~O4DGContext() {
    GUM_DESTRUCTOR(O4DGContext);
    ;
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/o4DGContext_inl.h>
#endif
