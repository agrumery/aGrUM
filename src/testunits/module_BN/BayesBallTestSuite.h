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
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/inference/BayesBall.h>

namespace gum_tests {

  class BayesBallTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreation() {
      gum::BayesBall *balls = 0;
      TS_ASSERT_THROWS_NOTHING(balls = new gum::BayesBall());

      TS_ASSERT_THROWS_NOTHING(if (balls != 0) delete balls);
    }

    void testRequisiteNodes() {
      gum::BayesBall balls;
      gum::SimpleBayesNetGenerator<float> gen(50, 200, 2);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      gum::Set<gum::NodeId> requisite;

      gum::Set<gum::NodeId> query, hardEvidence, softEvidence;
      gum::Sequence<gum::NodeId> nodes_seq;

      for (const auto node : bn->nodes())
        nodes_seq.insert(node);

      for (gum::Idx i = 0; i < 5; ++i)
        hardEvidence.insert(nodes_seq.atPos(i));

      for (gum::Idx j = 24; j > 19; --j)
        query.insert(nodes_seq.atPos(j));

      TS_ASSERT_THROWS_NOTHING(
                               balls.requisiteNodes(bn->dag(), query, hardEvidence, softEvidence, requisite));

      TS_ASSERT(requisite.size() >= 5);

      if (bn != 0)
        delete bn;
    }
  };
}
