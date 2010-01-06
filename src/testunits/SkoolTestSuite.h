/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class SkoolTestSuite: public CxxTest::TestSuite {
  public:
    void setUp() {
    }

    void tearDown() {
    }

    void testTypes() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/types.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_state").variable().domainSize(), 2);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_ink").variable().domainSize(), 2);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_degraded").variable().domainSize(), 3);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_bw_p").variable().domainSize(), 4);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_color_p").variable().domainSize(), 5);
      delete prm;
    }

    void testSubTypes() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/types.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT(prm->getType("agrum.test.t_state").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_state").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("agrum.test.t_degraded")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("agrum.test.t_degraded")));
      TS_ASSERT_EQUALS(prm->types().size(), 6);
      delete prm;
    }

};

} // namespace tests
} // namespace gum
// ============================================================================
