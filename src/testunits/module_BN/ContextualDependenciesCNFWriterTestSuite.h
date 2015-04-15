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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/cnf/ContextualDependenciesCNFWriter.h>
#include <agrum/core/algorithms/approximationPolicy/linearApproximationPolicy.h>
#include <agrum/core/algorithms/approximationPolicy/exactPolicy.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class ContextualDependenciesCNFWriterTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<double> *bn;
    gum::Id i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<double>();

      gum::LabelizedVariable n1("n1", "", 2), n2("n2", "", 2), n3("n3", "", 2);
      gum::LabelizedVariable n4("n4", "", 2), n5("n5", "", 2);

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
      gum::ContextualDependenciesCNFWriter<double> *writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          writer = new gum::ContextualDependenciesCNFWriter<double>());
      delete writer;
    }

    void testConstuctor_With_Aproximation() {
      typedef gum::ContextualDependenciesCNFWriter<double, gum::ExactPolicy> typCNF;
      typCNF *writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new typCNF());
      //   writer->setEpsilon( 0.2 );
      //         writer->setLowLimit ( 0 );
      //         writer->setHighLimit ( 0.5 );

      delete writer;
    }

    void testWriter_ostream() {
      gum::ContextualDependenciesCNFWriter<double> writer;
      std::stringstream sstream;
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(sstream, *bn));
    }

    void testWriter_ostream_With_Approximation() {
      gum::ContextualDependenciesCNFWriter<double, gum::LinearApproximationPolicy>
          writer;
      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);

      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testWriter_string() {
      gum::ContextualDependenciesCNFWriter<double> writer;
      std::string file =
          GET_PATH_STR("ContextualDependenciesCNFWriter_TestFile.cnf");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));

      file = GET_PATH_STR("ContextualDependenciesCNFWriter_RO_TestFile.cnf");

      try {
        writer.write(file, *bn);
        // TS_ASSERT(false);
      } catch (gum::IOError &e) {
        TS_ASSERT(true);
      }
    }

    void testWriter_string_With_Approximation() {
      gum::ContextualDependenciesCNFWriter<double, gum::LinearApproximationPolicy>
          writer;

      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);
      std::string file =
          GET_PATH_STR("ContextualDependenciesCNFWriter_TestFile_Approximation.cnf");

      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));

      file = GET_PATH_STR(
          "ContextualDependenciesCNFWriter_RO_TestFile_Approximation.cnf");

      try {
        writer.write(file, *bn);
        // TS_ASSERT(false);
      } catch (gum::IOError &e) {
        TS_ASSERT(true);
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet<double> &bn) {
      const gum::Potential<double> &p1 = bn.cpt(i1);
      {
        // FILLING PARAMS
        const double t[2] = {0.5, 0.5};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<double> &p2 = bn.cpt(i2);
      {
        // FILLING PARAMS
        const double t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<double> &p3 = bn.cpt(i3);
      {
        // FILLING PARAMS
        const double t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<double> v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential<double> &p4 = bn.cpt(i4);
      {
        // FILLING PARAMS
        const double t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<double> v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential<double> &p5 = bn.cpt(i5);
      {
        // FILLING PARAMS
        const double t[16] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
                              0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
        int n = 16;
        const std::vector<double> v(t, t + n);
        p5.fillWith(v);
      }
    }
  };
}
