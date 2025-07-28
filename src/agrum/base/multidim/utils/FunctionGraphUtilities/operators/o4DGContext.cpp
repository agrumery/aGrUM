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
      = {log2(2),   log2(3),   log2(5),   log2(7),   log2(11),  log2(13),  log2(17),  log2(19),
         log2(23),  log2(29),  log2(31),  log2(37),  log2(41),  log2(43),  log2(47),  log2(53),
         log2(59),  log2(61),  log2(67),  log2(71),  log2(73),  log2(79),  log2(83),  log2(89),
         log2(97),  log2(101), log2(103), log2(107), log2(109), log2(113), log2(127), log2(131),
         log2(137), log2(139), log2(149), log2(151), log2(157), log2(163), log2(167), log2(173),
         log2(179), log2(181), log2(191), log2(193), log2(197), log2(199), log2(211), log2(223),
         log2(227), log2(229), log2(233), log2(239), log2(241), log2(251), log2(257), log2(263),
         log2(269), log2(271), log2(277), log2(281), log2(283), log2(293), log2(307), log2(311),
         log2(313), log2(317), log2(331), log2(337), log2(347), log2(349), log2(353), log2(359),
         log2(367), log2(373), log2(379), log2(383), log2(389), log2(397), log2(401), log2(409),
         log2(419), log2(421), log2(431), log2(433), log2(439), log2(443), log2(449), log2(457),
         log2(461), log2(463), log2(467), log2(479), log2(487), log2(491), log2(499), log2(503),
         log2(509), log2(521), log2(523), log2(541), log2(547), log2(557), log2(563), log2(569),
         log2(571), log2(577), log2(587), log2(593), log2(599), log2(601), log2(607), log2(613),
         log2(617), log2(619), log2(631), log2(641), log2(643), log2(647), log2(653), log2(659),
         log2(661), log2(673), log2(677), log2(683), log2(691), log2(701), log2(709), log2(719),
         log2(727), log2(733), log2(739), log2(743), log2(751), log2(757), log2(761), log2(769),
         log2(773), log2(787), log2(797), log2(809)};

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
