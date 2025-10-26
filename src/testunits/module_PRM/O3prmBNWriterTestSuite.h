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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNWriter.h>

#include <sys/stat.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class GUM_TEST_SUITE(O3prmBNWriter) {
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

    GUM_ACTIVE_TEST(Constuctor) {
      gum::O3prmBNWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::O3prmBNWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::O3prmBNWriter< double > writer;
      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn))
    }

    GUM_ACTIVE_TEST(ReadAfterWrite) {
      std::string                  rfile = GET_RESSOURCES_PATH("o3prm/alarm.o3prm");
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, rfile);
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(7));   // no system
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(37))
      TS_ASSERT_EQUALS(bn.property("name"), "alarm")


      gum::O3prmBNWriter< double > writer;
      std::string                  wfile = GET_RESSOURCES_PATH("outputs/alarm_written.o3prm");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(wfile, bn))


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(&bn2, wfile, "alarm");
      gum::Size                    res2 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res2 = reader2.proceed())
      TS_ASSERT_EQUALS(res2, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader2.warnings(), static_cast< gum::Size >(7));   // no system
      TS_ASSERT_EQUALS(bn2.size(), static_cast< gum::Size >(37))

      std::string nam;
      for (const auto& nod: bn.nodes()) {
        nam = bn.variable(nod).name();
        TS_ASSERT_EQUALS(bn.variable(nam).toString(), bn2.variable(nam).toString())
        const gum::Tensor< double > p(bn.cpt(nam));
        std::vector< std::string >  varmap;
        for (gum::Idx i = 0; i < p.nbrDim(); i++)
          varmap.push_back(p.variable(i).name());
        p.fillWith(bn2.cpt(nam), varmap);
        TS_ASSERT_LESS_THAN((p - bn.cpt(nam)).abs().max(), TS_GUM_SMALL_ERROR)
      }
    }

    GUM_ACTIVE_TEST(ReadAfterWriteRandom) {
      gum::BayesNet< double > bn = gum::BayesNet< double >::fastPrototype(
          "A[5]->B{yes|maybe|no}<-C[4];D[3,6]->E[1,2,3,4,5,6,7]->F<-G;F<-H");
      bn.setProperty("name", "random_written");

      gum::O3prmBNWriter< double > writer;
      std::string                  wfile = GET_RESSOURCES_PATH("outputs/random_written.o3prm");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(wfile, bn))


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(&bn2, wfile);
      gum::Size                    res2 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res2 = reader2.proceed())
      TS_ASSERT_EQUALS(res2, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(bn2.size(), static_cast< gum::Size >(8))

      std::string nam;
      for (const auto& nod: bn.nodes()) {
        nam = bn.variable(nod).name();
        TS_ASSERT_EQUALS(bn.variable(nam).toString(), bn2.variable(nam).toString())
        const gum::Tensor< double > p(bn.cpt(nam));
        std::vector< std::string >  varmap;
        for (gum::Idx i = 0; i < p.nbrDim(); i++)
          varmap.push_back(p.variable(i).name());
        p.fillWith(bn2.cpt(nam), varmap);
        TS_ASSERT_LESS_THAN((p - bn.cpt(nam)).abs().max(), TS_GUM_SMALL_ERROR)
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith({0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0});
      bn.cpt(i5).fillWith({0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0,
                           0.4, 0.6, 0.0, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0});
    }
  };
}   // namespace gum_tests
