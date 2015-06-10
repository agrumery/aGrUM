/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/generator/layerGenerator.h>

namespace gum_tests {

  class InterfaceGraphTestSuite : public CxxTest::TestSuite {

    private:
    gum::prm::PRM<double> *__prm;
    std::string source_dir;
    std::string ressource_dir;
    std::string printers;

    public:
    void setUp() {
      gum::prm::o3prm::O3prmReader<double> reader;
      reader.readFile(
          "../../../src/testunits/ressources/o3prm/printers_systems.o3prm");
      __prm = reader.prm();
    }

    void tearDown() { delete __prm; }

    void testConstructorDestructor() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph<double>(
                                       __prm->system("microSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testMicroTopologie() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Checking existing nodes
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("pow"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("r"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("p"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("c"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("e"))));
      TS_ASSERT_EQUALS(ig->graph().size(), (gum::Size)5);
      // Checking existing edges
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("p"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("c"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("e"))));
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("p")), ig->id(m.get("c"))));
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (gum::Size)4);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testMicroLabelsOnNodes() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Testing power supply
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("pow"))).l), (gum::Size)1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("r"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("p"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      // Testing rooms
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("r"))).l), (gum::Size)1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l,
                        ig->node(ig->id(m.get("p"))).l->l);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      // Testing printers
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("p"))).l), (gum::Size)1);
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("p"))).l->l,
                        ig->node(ig->id(m.get("c"))).l->l);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testMicroLabelsOnEdges() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Test difference
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l,
                        ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    std::string getPrinter(int i) const {
      std::stringstream sBuff;
      sBuff << "printers[" << i << "]";
      return sBuff.str();
    }

    std::string getComputer(int i) const {
      std::stringstream sBuff;
      sBuff << "computers[" << i << "]";
      return sBuff.str();
    }

    void testSmallTopologie() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Checking existing nodes
      int node_count = 0;
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("pow"))));
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("r"))));
      node_count += 2;

      for (int i = 0; i < 2; ++i, ++node_count) {
        TS_ASSERT(ig->graph().existsNode(ig->id(m.get(getPrinter(i)))));
      }

      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("another_printer"))));

      ++node_count;

      for (int i = 0; i < 4; ++i, ++node_count) {
        TS_ASSERT(ig->graph().existsNode(ig->id(m.get(getComputer(i)))));
      }

      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("another_computer"))));

      ++node_count;
      TS_ASSERT_EQUALS(ig->graph().size(), (gum::Size)node_count);
      // Checking existing edges from pow
      int edge_count = 0;

      for (int i = 0; i < 2; ++i, ++edge_count) {
        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")),
                                         ig->id(m.get(getPrinter(i)))));
      }

      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")),
                                       ig->id(m.get("another_printer"))));

      ++edge_count;

      for (int i = 0; i < 4; ++i, ++edge_count) {
        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")),
                                         ig->id(m.get(getComputer(i)))));
      }

      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")),
                                       ig->id(m.get("another_computer"))));

      ++edge_count;
      // Checking existing edges from printers

      for (int i = 0; i < 2; ++i, ++edge_count) {
        for (int j = 0; j < 4; ++j, ++edge_count) {
          TS_ASSERT(ig->graph().existsEdge(ig->id(m.get(getPrinter(i))),
                                           ig->id(m.get(getComputer(j)))));
        }

        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get(getPrinter(i))),
                                         ig->id(m.get("another_computer"))));
      }

      for (int i = 0; i < 4; ++i, ++edge_count) {
        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("another_printer")),
                                         ig->id(m.get(getComputer(i)))));
      }

      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("another_printer")),
                                       ig->id(m.get("another_computer"))));

      ++edge_count;
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (gum::Size)edge_count);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testSmallLabelsOnNodes() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Testing each labels size (the number of nodes with the given label)
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("pow"))).l), (gum::Size)1);
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("r"))).l), (gum::Size)1);
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("another_printer"))).l),
                       (gum::Size)3);
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("another_computer"))).l),
                       (gum::Size)5);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void testSmallLabelsOnEdges() {
      gum::prm::gspan::InterfaceGraph<double> *ig = 0;
      gum::prm::System<double> &m = __prm->system("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph<double>(m));
      // Test difference
      int edge_count = 0;
      TS_ASSERT_EQUALS(ig->size(ig->edge(ig->id(m.get("pow")),
                                         ig->id(m.get("another_printer"))).l),
                       (gum::Size)3);
      edge_count += 3;
      TS_ASSERT_EQUALS(ig->size(ig->edge(ig->id(m.get("pow")),
                                         ig->id(m.get("another_computer"))).l),
                       (gum::Size)5);
      edge_count += 5;
      TS_ASSERT_EQUALS(ig->size(ig->edge(ig->id(m.get("another_printer")),
                                         ig->id(m.get("another_computer"))).l),
                       (gum::Size)15);
      edge_count += 15;
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (gum::Size)edge_count);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig);
    }

    void generateLayer(std::vector<gum::prm::LayerGenerator<double>::LayerData> &v,
                       size_t layer_count) {
      for (size_t lvl = 0; lvl < layer_count; ++lvl) {
        v.push_back(gum::prm::LayerGenerator<double>::LayerData());
        v[lvl].a = 30;
        v[lvl].g = 2;
        v[lvl].c = 1;
        v[lvl].o = 10;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

    void testWithLayerGeneration() {
      std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
      generateLayer(layers, 10);
      gum::prm::LayerGenerator<double> generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM<double> *prm = generator.generate();
      gum::prm::System<double> &sys =
          prm->system((**(prm->systems().begin())).name());
      gum::prm::gspan::InterfaceGraph<double> *g = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          g = new gum::prm::gspan::InterfaceGraph<double>(sys));

      if (g != nullptr)
        delete g;

      delete prm;
    }
  };

} /* namespace gum_tests */
