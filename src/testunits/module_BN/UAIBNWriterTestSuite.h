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
#include <agrum/BN/io/UAI/UAIBNReader.h>
#include <agrum/BN/io/UAI/UAIBNWriter.h>

#include <sys/stat.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  UAIBNWriter
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 4
//          4_/             1 -> 3
//                          1 -> 4

namespace gum_tests {

  struct UAIBNWriterTestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    UAIBNWriterTestSuite() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("0", "", 2), n2("1", "", 2), n3("2", "", 2);

      gum::LabelizedVariable n4("3", "", 2), n5("4", "", 3);

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

    ~UAIBNWriterTestSuite() { delete bn; }

    static void testConstructor() {
      gum::UAIBNWriter< double >* writer = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer = new gum::UAIBNWriter< double >());
      delete writer;
    }

    void testWriter_ostream() const {
      gum::UAIBNWriter< double > writer;
      std::string                file = GET_RESSOURCES_PATH("outputs/uaibn_generated.uai");
      // Uncomment this to check the ouput
      writer.write(file, *bn);

      gum::BayesNet< double >    net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK((*bn) == (net));
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) const {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith({0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0});
      bn.cpt(i5).fillWith({0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0,
                           0.4, 0.6, 0.0, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0});
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Writer_ostream)
}   // namespace gum_tests
