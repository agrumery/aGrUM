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
#include <sys/stat.h>

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNWriter.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class O3prmBNWriterTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet< double >();

      gum::RangeVariable                 n1("1", "", 0, 1);
      gum::DiscretizedVariable< double > n2("2", "");
      n2.addTick(0.0).addTick(0.5).addTick(1.0);
      gum::LabelizedVariable n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);

      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);

      fill(*bn);
    }

    void tearDown() { delete bn; }

    void testConstuctor() {
      gum::O3prmBNWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::O3prmBNWriter< double >());
      delete writer;
    }

    void testWriter_ostream() {
      gum::O3prmBNWriter< double > writer;
      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testReadAfterWrite() {
      gum::O3prmBNWriter< double > writer;
      std::string file = GET_RESSOURCES_PATH("o3prm/BNO3PRMIO_file.o3prm");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));
      file = GET_RESSOURCES_PATH("o3prm/BNO3PRMIO_file.o3prm");
      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader(&bn2, file, "BayesNet");
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);

      for (auto node : bn->dag()) {
        auto        name = bn->variable(node).name();
        const auto& cpt_1 = bn->cpt(node);
        auto        i = gum::Instantiation(cpt_1);

        const auto& cpt_2 = bn2.cpt(name);
        auto        j = gum::Instantiation(cpt_2);
        TS_ASSERT_EQUALS(i.domainSize(), j.domainSize());
        if (i.domainSize() == j.domainSize()) {
          for (i.setFirst(); !i.end(); i.inc()) {
            for (auto i_var : i.variablesSequence()) {
              const auto& j_var = bn2.variable(i_var->name());
              j.chgVal(j_var, i.val(*i_var));
            }
            TS_ASSERT_DELTA(cpt_1[i], cpt_2[j], 1e-6);
          }
        }
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith({0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0});
      bn.cpt(i5).fillWith({0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                           0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                           0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0});
    }
  };
}   // namespace gum_tests
