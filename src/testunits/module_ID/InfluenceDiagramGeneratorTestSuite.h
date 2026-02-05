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
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/generator/simpleUTGenerator.h>
#include <agrum/ID/influenceDiagram.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  InfluenceDiagramGenerator
#define GUM_CURRENT_MODULE ID

namespace gum_tests {

  struct InfluenceDiagramGeneratorTestSuite {
    public:
    static void testCreationDeletion_1() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >());
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }   // namespace gum_tests

    static void testCreationDeletion_2() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen = new gum::SimpleCPTGenerator< double >();
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >(cptGen));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testCreationDeletion_3() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleUTGenerator* utGen = new gum::SimpleUTGenerator();
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::InfluenceDiagramGenerator< double >(utGen));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testCreationDeletion_4() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen = new gum::SimpleCPTGenerator< double >();
      gum::SimpleUTGenerator*            utGen  = new gum::SimpleUTGenerator();
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::InfluenceDiagramGenerator< double >(cptGen, utGen));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testGenerationFloat_1() {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4));

      CHECK(id->decisionOrderExists());

      if (id != 0) delete id;
    }

    static void testGenerationFloat_2() {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node: dag.nodes()) {
        CHECK_THROWS_AS(dag.directedPath(node, node), const gum::NotFound&);
      }

      if (id != 0) delete id;
    }

    static void testGenerationDouble_1() {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4));

      CHECK(id->decisionOrderExists());

      if (id != 0) delete id;
    }

    static void testGenerationDouble_2() {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node: dag.nodes()) {
        CHECK_THROWS_AS(dag.directedPath(node, node), const gum::NotFound&);
      }

      if (id != 0) delete id;
    }
  };

  GUM_TEST_ACTIF(CreationDeletion_1)
  GUM_TEST_ACTIF(CreationDeletion_2)
  GUM_TEST_ACTIF(CreationDeletion_3)
  GUM_TEST_ACTIF(CreationDeletion_4)
  GUM_TEST_ACTIF(GenerationFloat_1)
  GUM_TEST_ACTIF(GenerationFloat_2)
  GUM_TEST_ACTIF(GenerationDouble_1)
  GUM_TEST_ACTIF(GenerationDouble_2)
}   // namespace gum_tests
