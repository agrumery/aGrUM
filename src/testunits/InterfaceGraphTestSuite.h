/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
#include <iostream>
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/core/exceptions.h>
// ============================================================================
#include <agrum/prm/gspan/interfaceGraph.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;
using namespace gum::prm::gspan;

class InterfaceGraphTestSuite: public CxxTest::TestSuite {
  private:
    PRM*  __prm;
    std::string source_dir;
    std::string ressource_dir;
    std::string printers;

  public:
    void setUp() {
      SkoolReader reader;
      reader.readFile("../../../src/testunits/ressources/skool/printers_systems.skool");
      __prm = reader.prm();
    }

    void tearDown() {
      delete __prm;
    }

    void testConstructorDestructor() {
      InterfaceGraph* ig = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new InterfaceGraph(__prm->getSystem("microSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testTopologie() {
      InterfaceGraph* ig = 0;
      System& m = __prm->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new InterfaceGraph(m));
      // Checking existing nodes
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("pow"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("r"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("p"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("c"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("e"))));
      TS_ASSERT_EQUALS(ig->graph().size(), (Size) 5);
      // Checking existing edges
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("p"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("c"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("e"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("p")), ig->id(m.get("c"))));
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (Size) 4);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testLabelsOnNodes() {
      InterfaceGraph* ig = 0;
      System& m = __prm->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new InterfaceGraph(m));
      // Testing power supply
      TS_ASSERT_EQUALS (ig->size(ig->node(ig->id(m.get("pow"))).l), (Size) 1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("r"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("p"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      // Testing rooms
      TS_ASSERT_EQUALS (ig->size(ig->node(ig->id(m.get("r"))).l), (Size) 1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l,
                        ig->node(ig->id(m.get("p"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      // Testing printers
      TS_ASSERT_EQUALS (ig->size(ig->node(ig->id(m.get("p"))).l), (Size) 1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("p"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testLabelsOnEdges() {
      InterfaceGraph* ig = 0;
      System& m = __prm->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new InterfaceGraph(m));
      // Test difference
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")),               ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("pow")),               ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")),               ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("p")),                 ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")),               ig->id(m.get("c"))).l,
                        ig->edge(ig->id(m.get("p")),                 ig->id(m.get("c"))).l);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }
};

} /* namespace test */
} /* namespace gum */
// ============================================================================
