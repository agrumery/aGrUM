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
#include <agrum/BN/io/DSL/DSLWriter.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DSLWriter
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct DSLWriterTestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    DSLWriterTestSuite() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);

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

    ~DSLWriterTestSuite() { delete bn; }

    static void testConstuctor() {
      gum::DSLWriter< double >* writer = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer = new gum::DSLWriter< double >());
      delete writer;
    }

    static void testWriter_ostream() {
      gum::DSLWriter< double > writer;
      // Uncomment this to check the output
      // GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testWriter_string() const {
      gum::DSLWriter< double > writer;
      std::string              file = GET_RESSOURCES_PATH("outputs/DSLWriter_TestFile.txt");
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file, *bn));
    }

    void testSyntaxicError() {
      gum::DSLWriter< double > writer;
      {
        CHECK(!writer.isModificationAllowed());
        std::string file = GET_RESSOURCES_PATH("outputs/shouldNotBeWrittenDSL.txt");
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World !->c");
          CHECK_THROWS_AS(writer.write(file, bn), gum::FatalError&);
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World->c");
          CHECK_THROWS_AS(writer.write(file, bn), gum::FatalError&);
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->HelloWorld!->c");
          CHECK_THROWS_AS(writer.write(file, bn), gum::FatalError&);
        }
      }
      {
        writer.setAllowModification(true);
        CHECK(writer.isModificationAllowed());
        std::string file = GET_RESSOURCES_PATH("outputs/shouldBeWrittenDSL.txt");
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World !->c");
          GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file, bn));
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World->c");
          GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file, bn));
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->HelloWorld!->c");
          GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file, bn));
        }
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) const {
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
               0.0, 0.0, 1.0} );  // clang-format
                                                                      // on
    }
  };

GUM_TEST_ACTIF(Constuctor)
GUM_TEST_ACTIF(Writer_ostream)
GUM_TEST_ACTIF(Writer_string)
GUM_TEST_ACTIF(SyntaxicError)
}
