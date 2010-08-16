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
#include <cxxtest/TestSuite.h>
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/core/exceptions.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
// ============================================================================
#include <agrum/prm/gspan.h>
// ============================================================================
#include <agrum/prm/PRMFactory.h>
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================
namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::gspan;
using namespace gum::prm::skool;


class GSpanTestSuite: public CxxTest::TestSuite {
  private:
    SkoolReader* __driver;
    std::string dot_dir;

    InterfaceGraph* ig;

  public:
    void setUp() {
      __driver  = new SkoolReader();
      //dot_dir = "/testunits/dot/";
      __driver->readFile("../../../src/testunits/ressources/skool/specialprinters.skool");
      ig = new InterfaceGraph(__driver->prm()->getSystem("m"));
    }

    void tearDown() {
      delete ig;
      if (__driver->prm() != 0) delete __driver->prm();
      delete __driver;
    }

    void testInterfaceGraph() {
      TS_ASSERT_EQUALS(ig->graph().size(), (Size) 1 + 5*2 + 4*3 + 4*3 + 5 + 3 + 4);
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (Size) (5*2 + 3*4 + 4*3) + 5 + 3*3 + 4*2);
    }

    void testTree() {
      GSpan* gspan = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gspan = new GSpan(*(__driver->prm()), __driver->prm()->getSystem("m"), 2, 5));
      if (gspan != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(gspan->discoverPatterns());
        TS_GUM_ASSERT_THROWS_NOTHING(delete gspan);
      }
      //   // DFSTree& tree = gspan->tree();
      //   // std::vector<NodeId> stack;
      //   // int count = 0;
      //   // for(std::list<NodeId>::iterator root = tree.roots().begin(); root != tree.roots().end(); ++root) {
      //   //   stack.push_back(*root);
      //   //   ++count;
      //   // }
      //   // while (not stack.empty()) {
      //   //   NodeId plop = stack.back();
      //   //   stack.pop_back();
      //   //   for (std::list<NodeId>::iterator child = tree.children(tree.pattern(plop)).begin();
      //   //        child != tree.children(tree.pattern(plop)).end(); ++child) {
      //   //     stack.push_back(*child);
      //   //     ++count;
      //   //   }
      //   // }
      //   // std::list<NodeId> queue = gspan->tree().roots();
      //   // size_t name = 1;
      //   // std::ofstream output;
      //   // while (not queue.empty()) {
      //   //   std::stringstream filename;
      //   //   filename << ++name << ".dot";
      //   //   std::string file = source_dir + dot_dir + filename.str();
      //   //   output.open(file.c_str(), std::ios::out & std::ios::trunc);
      //   //   output << gspan->tree().pattern(queue.front()).toDot(name);
      //   //   output.close();
      //   //   std::list<NodeId>& children = gspan->tree().children(gspan->tree().pattern(queue.front()));
      //   //   for (std::list<NodeId>::iterator child = children.begin(); child != children.end(); ++child) {
      //   //     queue.push_back(*child);
      //   //   }
      //   //   queue.pop_front();
      //   // }
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("pow"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("s1_p1[0]"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("v1_p1[0]"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("x1_p[0]"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("s1_c1"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("v1_c1"))).l->id);
      //   // GUM_TRACE_VAR(ig->node(&(__factory->prm()->getModel("m").get("x1_c1"))).l->id);
      //   // for (std::vector<Pattern*>::iterator p = gspan->patterns().begin(); p != gspan->patterns().end(); ++p) {
      //   //   GUM_TRACE_VAR((**p).code());
      //   //   GUM_TRACE_VAR(gspan->tree().score(**p));
      //   //   GUM_TRACE_VAR(gspan->tree().iso_graph(**p).size());
      //   //   GUM_TRACE_VAR(gspan->tree().max_indep_set(**p).size());
      //   // }
      //   // GUM_TRACE_VAR(gspan->patterns().front()->code());
      //   // GUM_TRACE_VAR(gspan->tree().score(*(gspan->patterns().front())));
      //   // GUM_TRACE_VAR(gspan->tree().iso_graph(*(gspan->patterns().front())).size());
      //   // GUM_TRACE_VAR(gspan->tree().sub_patterns(*(gspan->patterns().front())).size());
      //   // GUM_TRACE_VAR(gspan->tree().sub_patterns_map(*(gspan->patterns().front())).size());
      //   //GUM_TRACE_VAR(gspan->patterns().back()->code());
      //   //GUM_TRACE_VAR(gspan->tree().score(*(gspan->patterns().back())));
      //   // GUM_TRACE(count);
      //   // GUM_TRACE_VAR(gspan->matches().size());
      //   // GUM_TRACE_VAR(gspan->patterns_cost());
      //   // GUM_TRACE_VAR(gspan->sve_cost());
      //   // BayesNet<prm_float>* bn = 0;
      //   // TS_GUM_ASSERT_THROWS_NOTHING(bn = &(gspan->baseBN(*(gspan->patterns().front()))));
      //   // CPF<prm_float> m;
      //   // TS_GUM_ASSERT_THROWS_NOTHING(gspan->generateBN(3));
      //   // TS_GUM_ASSERT_THROWS_NOTHING(gspan->infer(3));
      //   //GUM_TRACE_VAR(bn.toDot());
      //   //BIFWriter<prm_float> writer;
      //   //TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

};
// ============================================================================
} // tests
} // gum

