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
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/generator/simpleUTGenerator.h>
#include <agrum/ID/influenceDiagram.h>

namespace gum_tests {

  class GUM_TEST_SUITE(InfluenceDiagramGenerator) {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(CreationDeletion_1) {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >())
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(CreationDeletion_2) {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen = new gum::SimpleCPTGenerator< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >(cptGen))
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletion_3) {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleUTGenerator* utGen = new gum::SimpleUTGenerator();
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >(utGen))
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletion_4) {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen = new gum::SimpleCPTGenerator< double >();
      gum::SimpleUTGenerator*            utGen  = new gum::SimpleUTGenerator();
      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::InfluenceDiagramGenerator< double >(cptGen, utGen));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(GenerationFloat_1) {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4))

      TS_ASSERT(id->decisionOrderExists())

      if (id != 0) delete id;
    }

    GUM_ACTIVE_TEST(GenerationFloat_2) {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node: dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), const gum::NotFound&)
      }

      if (id != 0) delete id;
    }

    GUM_ACTIVE_TEST(GenerationDouble_1) {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4))

      TS_ASSERT(id->decisionOrderExists())

      if (id != 0) delete id;
    }

    GUM_ACTIVE_TEST(GenerationDouble_2) {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node: dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), const gum::NotFound&)
      }

      if (id != 0) delete id;
    }
  };
}   // namespace gum_tests
