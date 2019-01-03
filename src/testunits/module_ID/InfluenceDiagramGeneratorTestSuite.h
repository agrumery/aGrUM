/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/generator/simpleUTGenerator.h>
#include <agrum/ID/influenceDiagram.h>

namespace gum_tests {

  class InfluenceDiagramGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletion_1() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::InfluenceDiagramGenerator< double >());
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletion_2() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen =
        new gum::SimpleCPTGenerator< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::InfluenceDiagramGenerator< double >(cptGen));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletion_3() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleUTGenerator* utGen = new gum::SimpleUTGenerator();
      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::InfluenceDiagramGenerator< double >(utGen));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletion_4() {
      gum::InfluenceDiagramGenerator< double >* gen = nullptr;

      gum::SimpleCPTGenerator< double >* cptGen =
        new gum::SimpleCPTGenerator< double >();
      gum::SimpleUTGenerator* utGen = new gum::SimpleUTGenerator();
      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::InfluenceDiagramGenerator< double >(cptGen, utGen));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationFloat_1() {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4));

      TS_ASSERT(id->decisionOrderExists());

      if (id != 0) delete id;
    }

    void testGenerationFloat_2() {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id =
        gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node : dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), gum::NotFound);
      }

      if (id != 0) delete id;
    }

    void testGenerationDouble_1() {
      gum::InfluenceDiagramGenerator< double > gen;
      gum::InfluenceDiagram< double >*         id = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4));

      TS_ASSERT(id->decisionOrderExists());

      if (id != 0) delete id;
    }

    void testGenerationDouble_2() {
      gum::InfluenceDiagramGenerator< double > gen;

      gum::InfluenceDiagram< double >* id =
        gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = id->dag();

      for (const auto node : dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), gum::NotFound);
      }

      if (id != 0) delete id;
    }
  };
}   // namespace gum_tests
