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
#pragma once


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/generator/layerGenerator.h>
#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class GUM_TEST_SUITE(InterfaceGraph) {
    private:
    gum::prm::PRM< double >* _prm_;
    std::string              source_dir;
    std::string              ressource_dir;
    std::string              printers;

    public:
    void setUp() {
      gum::prm::o3prm::O3prmReader< double > reader;
      reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm"));
      _prm_ = reader.prm();
    }

    void tearDown() { delete _prm_; }

    GUM_ACTIVE_TEST(ConstructorDestructor) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          ig = new gum::prm::gspan::InterfaceGraph< double >(_prm_->getSystem("microSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
    }

    GUM_ACTIVE_TEST(MicroTopologie) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Checking existing nodes
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("pow"))))
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("r"))))
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("p"))))
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("c"))))
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("e"))))
      TS_ASSERT_EQUALS(ig->graph().size(), static_cast< gum::Size >(5))
      // Checking existing edges
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("p"))))
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("c"))))
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("e"))))
      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("p")), ig->id(m.get("c"))))
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), static_cast< gum::Size >(4))
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
    }

    GUM_ACTIVE_TEST(MicroLabelsOnNodes) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Testing power supply
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("pow"))).l), static_cast< gum::Size >(1))
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l, ig->node(ig->id(m.get("r"))).l->l)
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l, ig->node(ig->id(m.get("p"))).l->l)
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("pow"))).l->l, ig->node(ig->id(m.get("c"))).l->l)
      // Testing rooms
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("r"))).l), static_cast< gum::Size >(1))
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l, ig->node(ig->id(m.get("p"))).l->l)
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("r"))).l->l, ig->node(ig->id(m.get("c"))).l->l)
      // Testing printers
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("p"))).l), static_cast< gum::Size >(1))
      TS_ASSERT_DIFFERS(ig->node(ig->id(m.get("p"))).l->l, ig->node(ig->id(m.get("c"))).l->l)
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
    }

    GUM_ACTIVE_TEST(MicroLabelsOnEdges) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("microSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Test difference
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("p"))).l,
                        ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l);
      TS_ASSERT_DIFFERS(ig->edge(ig->id(m.get("pow")), ig->id(m.get("c"))).l,
                        ig->edge(ig->id(m.get("p")), ig->id(m.get("c"))).l);
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
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

    GUM_ACTIVE_TEST(SmallTopologie) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Checking existing nodes
      int node_count = 0;
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("pow"))))
      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("r"))))
      node_count += 2;

      for (int i = 0; i < 2; ++i, ++node_count) {
        TS_ASSERT(ig->graph().existsNode(ig->id(m.get(getPrinter(i)))))
      }

      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("another_printer"))))

      ++node_count;

      for (int i = 0; i < 4; ++i, ++node_count) {
        TS_ASSERT(ig->graph().existsNode(ig->id(m.get(getComputer(i)))))
      }

      TS_ASSERT(ig->graph().existsNode(ig->id(m.get("another_computer"))))

      ++node_count;
      TS_ASSERT_EQUALS(ig->graph().size(), (gum::Size)node_count)
      // Checking existing edges from pow
      int edge_count = 0;

      for (int i = 0; i < 2; ++i, ++edge_count) {
        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get(getPrinter(i)))))
      }

      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("another_printer"))))

      ++edge_count;

      for (int i = 0; i < 4; ++i, ++edge_count) {
        TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get(getComputer(i)))))
      }

      TS_ASSERT(ig->graph().existsEdge(ig->id(m.get("pow")), ig->id(m.get("another_computer"))))

      ++edge_count;
      // Checking existing edges from printers

      for (int i = 0; i < 2; ++i, ++edge_count) {
        for (int j = 0; j < 4; ++j, ++edge_count) {
          TS_ASSERT(
              ig->graph().existsEdge(ig->id(m.get(getPrinter(i))), ig->id(m.get(getComputer(j)))));
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
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (gum::Size)edge_count)
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
    }

    GUM_ACTIVE_TEST(SmallLabelsOnNodes) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Testing each labels size (the number of nodes with the given label)
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("pow"))).l), static_cast< gum::Size >(1))
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("r"))).l), static_cast< gum::Size >(1))
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("another_printer"))).l),
                       static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(ig->size(ig->node(ig->id(m.get("another_computer"))).l),
                       static_cast< gum::Size >(5))
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
    }

    GUM_ACTIVE_TEST(SmallLabelsOnEdges) {
      gum::prm::gspan::InterfaceGraph< double >* ig = 0;
      gum::prm::PRMSystem< double >&             m  = _prm_->getSystem("smallSys");
      TS_GUM_ASSERT_THROWS_NOTHING(ig = new gum::prm::gspan::InterfaceGraph< double >(m))
      // Test difference
      int edge_count = 0;
      TS_ASSERT_EQUALS(ig->size(ig->edge(ig->id(m.get("pow")), ig->id(m.get("another_printer"))).l),
                       static_cast< gum::Size >(3));
      edge_count += 3;
      TS_ASSERT_EQUALS(
          ig->size(ig->edge(ig->id(m.get("pow")), ig->id(m.get("another_computer"))).l),
          static_cast< gum::Size >(5));
      edge_count += 5;
      TS_ASSERT_EQUALS(
          ig->size(ig->edge(ig->id(m.get("another_printer")), ig->id(m.get("another_computer"))).l),
          static_cast< gum::Size >(15));
      edge_count += 15;
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(), (gum::Size)edge_count)
      TS_GUM_ASSERT_THROWS_NOTHING(delete ig)
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

    GUM_ACTIVE_TEST(WithLayerGeneration) {
      std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
      generateLayer(layers, 10);
      gum::prm::LayerGenerator< double > generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM< double >*       prm = generator.generate();
      gum::prm::PRMSystem< double >& sys = prm->getSystem((**(prm->systems().begin())).name());
      gum::prm::gspan::InterfaceGraph< double >* g = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(g = new gum::prm::gspan::InterfaceGraph< double >(sys))

      if (g != nullptr) delete g;

      delete prm;
    }
  };

} /* namespace gum_tests */
