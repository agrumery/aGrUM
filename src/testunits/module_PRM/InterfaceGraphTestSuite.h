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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/generator/layerGenerator.h>
#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  InterfaceGraph
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct InterfaceGraphTestSuite {
    private:
    gum::prm::PRM< double >* _prm_;
    std::string              source_dir;
    std::string              ressource_dir;
    std::string              printers;

    public:
    static void testConstructorDestructor() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph< double >(_prm_->getSystem("microSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    InterfaceGraphTestSuite() {
      gum::prm::o3prm::O3prmReader< double > reader;

      reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm"));

      _prm_ = reader.prm();
    }

    ~InterfaceGraphTestSuite() { delete _prm_; }

    static void testMicroTopologie() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Checking existing nodes
      CHECK(ig->graph().existsNode(ig->id(m.get("pow"))));
      CHECK(ig->graph().existsNode(ig->id(m.get("r"))));
      CHECK(ig->graph().existsNode(ig->id(m.get("p"))));
      CHECK(ig->graph().existsNode(ig->id(m.get("c"))));
      CHECK(ig->graph().existsNode(ig->id(m.get("e"))));
      CHECK((ig->graph().size()) == (static_cast< gum::Size >(5)));
      // Checking existing edges
      CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("p"))));
      CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("c"))));
      CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("e"))));
      CHECK(ig->graph().existsEdge(ig->id(m.get("p")), ig->id(m.get("c"))));
      CHECK((ig->graph().sizeEdges()) == (static_cast< gum::Size >(4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    static void testMicroLabelsOnNodes() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Testing power supply
      CHECK((ig->size(ig->node(ig->id(m.get("pow"))).l)) == (static_cast< gum::Size >(1)));
      CHECK((ig->node(ig->id(m.get("pow"))).l->l) != (ig->node(ig->id(m.get("r"))).l->l));
      CHECK((ig->node(ig->id(m.get("pow"))).l->l) != (ig->node(ig->id(m.get("p"))).l->l));
      CHECK((ig->node(ig->id(m.get("pow"))).l->l) != (ig->node(ig->id(m.get("c"))).l->l));
      // Testing rooms
      CHECK((ig->size(ig->node(ig->id(m.get("r"))).l)) == (static_cast< gum::Size >(1)));
      CHECK((ig->node(ig->id(m.get("r"))).l->l) != (ig->node(ig->id(m.get("p"))).l->l));
      CHECK((ig->node(ig->id(m.get("r"))).l->l) != (ig->node(ig->id(m.get("c"))).l->l));
      // Testing printers
      CHECK((ig->size(ig->node(ig->id(m.get("p"))).l)) == (static_cast< gum::Size >(1)));
      CHECK((ig->node(ig->id(m.get("p"))).l->l) != (ig->node(ig->id(m.get("c"))).l->l));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    static void testMicroLabelsOnEdges() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Test difference
      CHECK((ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l)
            != (ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l));
      CHECK((ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l)
            != (ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l));
      CHECK((ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l)
            != (ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
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

    static void testSmallTopologie() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Checking existing nodes
      int node_count = 0;
      CHECK(ig->graph().existsNode(ig->id(m.get("pow"))));
      CHECK(ig->graph().existsNode(ig->id(m.get("r"))));
      node_count += 2;

      for (int i = 0; i < 2; ++i, ++node_count) {
        CHECK(ig->graph().existsNode(ig->id(m.get(getPrinter(i)))));
      }

      CHECK(ig->graph().existsNode(ig->id(m.get("another_printer"))));

      ++node_count;

      for (int i = 0; i < 4; ++i, ++node_count) {
        CHECK(ig->graph().existsNode(ig->id(m.get(getComputer(i)))));
      }

      CHECK(ig->graph().existsNode(ig->id(m.get("another_computer"))));

      ++node_count;
      CHECK((ig->graph().size()) == ((gum::Size)node_count));
      // Checking existing edges from pow
      int edge_count = 0;

      for (int i = 0; i < 2; ++i, ++edge_count) {
        CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get(getPrinter(i)))));
      }

      CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("another_printer"))));

      ++edge_count;

      for (int i = 0; i < 4; ++i, ++edge_count) {
        CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get(getComputer(i)))));
      }

      CHECK(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("another_computer"))));

      ++edge_count;
      // Checking existing edges from printers

      for (int i = 0; i < 2; ++i, ++edge_count) {
        for (int j = 0; j < 4; ++j, ++edge_count) {
          CHECK(
              ig->graph().existsEdge(ig->id(m.get(getPrinter(i))), ig->id(m.get(getComputer(j)))));
        }

        CHECK(ig->graph().existsEdge(ig->id(m.get(getPrinter(i))),
                                     ig->id(m.get("another_computer"))));
      }

      for (int i = 0; i < 4; ++i, ++edge_count) {
        CHECK(ig->graph().existsEdge(ig->id(m.get("another_printer")),
                                     ig->id(m.get(getComputer(i)))));
      }

      CHECK(ig->graph().existsEdge(ig->id(m.get("another_printer")),
                                   ig->id(m.get("another_computer"))));

      ++edge_count;
      CHECK((ig->graph().sizeEdges()) == ((gum::Size)edge_count));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    static void testSmallLabelsOnNodes() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Testing each labels size (the number of nodes with the given label)
      CHECK((ig->size(ig->node(ig->id(m.get("pow"))).l)) == (static_cast< gum::Size >(1)));
      CHECK((ig->size(ig->node(ig->id(m.get("r"))).l)) == (static_cast< gum::Size >(1)));
      CHECK((ig->size(ig->node(ig->id(m.get("another_printer"))).l))
            == (static_cast< gum::Size >(3)));
      CHECK((ig->size(ig->node(ig->id(m.get("another_computer"))).l))
            == (static_cast< gum::Size >(5)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    static void testSmallLabelsOnEdges() {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      GUM_CHECK_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m));
      // Test difference
      int edge_count = 0;
      CHECK((ig->size(ig->edge(ig->id(m.get("pow")), ig->id(m.get("another_printer"))).l))
            == (static_cast< gum::Size >(3)));
      edge_count += 3;
      CHECK((ig->size(ig->edge(ig->id(m.get("pow")), ig->id(m.get("another_computer"))).l))
            == (static_cast< gum::Size >(5)));
      edge_count += 5;
      CHECK((ig->size(
                ig->edge(ig->id(m.get("another_printer")), ig->id(m.get("another_computer"))).l))
            == (static_cast< gum::Size >(15)));
      edge_count += 15;
      CHECK((ig->graph().sizeEdges()) == ((gum::Size)edge_count));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete ig);
    }

    void generateLayer(std::vector< gum::prm::LayerGenerator< double >::LayerData >& v,
                       size_t                                                        layer_count) {
      for (size_t lvl = 0; lvl < layer_count; ++lvl) {
        v.push_back(gum::prm::LayerGenerator< double >::LayerData());
        v[lvl].a              = 30;
        v[lvl].g              = 2;
        v[lvl].c              = 1;
        v[lvl].o              = 10;
        v[lvl].inner_density  = 0.2f;
        v[lvl].outter_density = 0.05f;
      }
    }

    static void testWithLayerGeneration() {
      std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
      generateLayer(layers, 10);
      gum::prm::LayerGenerator< double > generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM< double >*       prm = generator.generate();
      gum::prm::PRMSystem< double >& sys = prm->getSystem((**(prm->systems().begin())).name());
      gum::prm::gspan::InterfaceGraph< double >* g = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(g = new gum::prm::gspan::InterfaceGraph< double >(sys));

      if (g != nullptr) delete g;

      delete prm;
    }
  };

  GUM_TEST_ACTIF(ConstructorDestructor)
  GUM_TEST_ACTIF(MicroTopologie)
  GUM_TEST_ACTIF(MicroLabelsOnNodes)
  GUM_TEST_ACTIF(MicroLabelsOnEdges)
  GUM_TEST_ACTIF(SmallTopologie)
  GUM_TEST_ACTIF(SmallLabelsOnNodes)
  GUM_TEST_ACTIF(SmallLabelsOnEdges)
  GUM_TEST_ACTIF(WithLayerGeneration)

} /* namespace gum_tests */
