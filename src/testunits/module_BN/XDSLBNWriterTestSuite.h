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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/XDSL/XDSLBNReader.h>
#include <agrum/BN/io/XDSL/XDSLBNWriter.h>

#include <sys/stat.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  XDSLBNWriter
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct XDSLBNWriterTestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    XDSLBNWriterTestSuite() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2);
      gum::LabelizedVariable n2("2", "", 2);
      gum::LabelizedVariable n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2);
      gum::LabelizedVariable n5("5", "", 3);

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

      _fill_(*bn);
    }

    ~XDSLBNWriterTestSuite() { delete bn; }

    static void testConstuctor() {
      gum::XDSLBNWriter< double >* writer = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer = new gum::XDSLBNWriter< double >());
      delete writer;
    }

    void testWriter_ostream() const {
      gum::XDSLBNWriter< double > writer;
      // Uncomment this to check the output
      std::string outfile = GET_RESSOURCES_PATH("outputs/benefits_out.xdsl");
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(outfile, *bn));
    }

    void testRead_file2() const {
      std::string                 outfile = GET_RESSOURCES_PATH("outputs/benefits_out.xdsl");
      gum::XDSLBNWriter< double > writer;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(outfile, *bn));

      gum::BayesNet< double >     net;
      gum::XDSLBNReader< double > reader(&net, outfile);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      for (auto i: bn->nodes()) {
        CHECK((bn->cpt(i).toString()) == (net.cpt(bn->variable(i).name()).toString()));
      }
    }

    private:
    // Builds a BN to test the inference
    void _fill_(gum::BayesNet< double >& bn) const {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith(   // clang-format off
             {0.4, 0.6,
              0.5, 0.5,
              0.5, 0.5,
              1.0, 0.0} );   // clang-format on
      bn.cpt(i5).fillWith(                      // clang-format off
             {0.3, 0.6, 0.1,
              0.5, 0.5, 0.0,
              0.5, 0.5, 0.0,
              1.0, 0.0, 0.0,
              0.4, 0.6, 0.0,
              0.5, 0.5, 0.0,
              0.5, 0.5, 0.0,
              0.0, 0.0, 1.0} );
      // clang-format on
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Writer_ostream)
  GUM_TEST_ACTIF(Read_file2)
}   // namespace gum_tests
