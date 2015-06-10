/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr *
 *                                                                                                                      *
 *   This program is free software; you can redistribute it and/or modify           *
 *   it under the terms of the GNU General Public License as published by       *
 *   the Free Software Foundation; either version 2 of the License, or              *
 *   (at your option) any later version. *
 *                                                                                                                      *
 *   This program is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details. *
 *                                                                                                                      *
 *   You should have received a copy of the GNU General Public License          *
 *   along with this program; if not, write to the *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                      *
 ********************************************************************************/

#include <cmath>

#include <agrum/multidim/patterns/DDUtility/o4DDContext.h>

namespace gum {

  // ********************************************************************************************************************
  // First 140 prime numbers...
  // see e.g.
  // http://en.wikipedia.org/wiki/List_of_prime_numbers#The_first_500_prime_numbers
  // if more is needed...
  // Don't forget to change __nbLogPrime to match number of log2 of prime numbers  in
  // that table.

  const gum::Idx O4DDContext::__nbLogPrime = 140;

  const double O4DDContext::__logPrime[] = {
      log2(2),   log2(3),   log2(5),   log2(7),   log2(11),  log2(13),  log2(17),
      log2(19),  log2(23),  log2(29),  log2(31),  log2(37),  log2(41),  log2(43),
      log2(47),  log2(53),  log2(59),  log2(61),  log2(67),  log2(71),  log2(73),
      log2(79),  log2(83),  log2(89),  log2(97),  log2(101), log2(103), log2(107),
      log2(109), log2(113), log2(127), log2(131), log2(137), log2(139), log2(149),
      log2(151), log2(157), log2(163), log2(167), log2(173), log2(179), log2(181),
      log2(191), log2(193), log2(197), log2(199), log2(211), log2(223), log2(227),
      log2(229), log2(233), log2(239), log2(241), log2(251), log2(257), log2(263),
      log2(269), log2(271), log2(277), log2(281), log2(283), log2(293), log2(307),
      log2(311), log2(313), log2(317), log2(331), log2(337), log2(347), log2(349),
      log2(353), log2(359), log2(367), log2(373), log2(379), log2(383), log2(389),
      log2(397), log2(401), log2(409), log2(419), log2(421), log2(431), log2(433),
      log2(439), log2(443), log2(449), log2(457), log2(461), log2(463), log2(467),
      log2(479), log2(487), log2(491), log2(499), log2(503), log2(509), log2(521),
      log2(523), log2(541), log2(547), log2(557), log2(563), log2(569), log2(571),
      log2(577), log2(587), log2(593), log2(599), log2(601), log2(607), log2(613),
      log2(617), log2(619), log2(631), log2(641), log2(643), log2(647), log2(653),
      log2(659), log2(661), log2(673), log2(677), log2(683), log2(691), log2(701),
      log2(709), log2(719), log2(727), log2(733), log2(739), log2(743), log2(751),
      log2(757), log2(761), log2(769), log2(773), log2(787), log2(797), log2(809)};
  // ********************************************************************************************************************

  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                         Constructors, Destructors */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Default constructor.

  O4DDContext::O4DDContext() {
    __DD1ExploredNode = 0;
    __DD1PrimeLog = __logPrime[__nbLogPrime - 1];
    __DD2ExploredNode = 0;
    __DD2PrimeLog = __logPrime[__nbLogPrime - 2];
  }

  // Default constructor.

  O4DDContext::~O4DDContext() {}

  //

  std::string O4DDContext::toString() const {
    std::stringstream ret;
    ret << " DD1 Current Node : " << __DD1ExploredNode << " - Log " << __DD1PrimeLog
        << " | DD2 Current Node : " << __DD2ExploredNode << " - Log "
        << __DD2PrimeLog << " | ";

    for (const auto var : __varSeq)
      ret << "\t Variable retrograde : " << var->name() << " -> Current Modality : "
          << __retrogradeVarInstantiation[__varSeq.pos(var)] << " | ";

    return ret.str();
  }
}

#ifdef GUM_NO_INLINE
#include <agrum/multidim/patterns/DDUtility/o4DDContext.inl>
#endif
