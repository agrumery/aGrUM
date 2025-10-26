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

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

namespace gum_tests {

  class GUM_TEST_SUITE(SimpleCPTGenerator) {
    public:
    // Binary variables
    gum::LabelizedVariable* binVar1;
    gum::LabelizedVariable* binVar2;
    gum::LabelizedVariable* binVar3;
    gum::LabelizedVariable* binVar4;

    gum::Tensor< double >* rootPot1F;
    gum::Tensor< double >* rootPot1D;

    gum::Tensor< double >* pot1F;
    gum::Tensor< double >* pot1D;

    gum::Tensor< double >* pot2F;
    gum::Tensor< double >* pot2D;

    // n-ary variables
    gum::LabelizedVariable* nVar1;
    gum::LabelizedVariable* nVar2;
    gum::LabelizedVariable* nVar3;
    gum::LabelizedVariable* nVar4;
    gum::LabelizedVariable* nVar5;

    gum::Tensor< double >* pot3F;
    gum::Tensor< double >* pot3D;

    void setUp() {
      binVar1 = new gum::LabelizedVariable("binVar1", "Binary Variable number 1");
      binVar2 = new gum::LabelizedVariable("binVar2", "Binary Variable number 2");
      binVar3 = new gum::LabelizedVariable("binVar3", "Binary Variable number 3");
      binVar4 = new gum::LabelizedVariable("binVar4", "Binary Variable number 4");

      rootPot1F = new gum::Tensor< double >();
      rootPot1F->add(*binVar1);

      rootPot1D = new gum::Tensor< double >();
      rootPot1D->add(*binVar1);

      pot1F = new gum::Tensor< double >();
      pot1F->add(*binVar1);
      pot1F->add(*binVar2);

      pot1D = new gum::Tensor< double >();
      pot1D->add(*binVar1);
      pot1D->add(*binVar2);

      pot2F = new gum::Tensor< double >();
      pot2F->add(*binVar1);
      pot2F->add(*binVar2);
      pot2F->add(*binVar3);
      pot2F->add(*binVar4);

      pot2D = new gum::Tensor< double >();
      pot2D->add(*binVar1);
      pot2D->add(*binVar2);
      pot2D->add(*binVar3);
      pot2D->add(*binVar4);

      nVar1 = new gum::LabelizedVariable("nVar1", "A discrete variable", 2);
      nVar2 = new gum::LabelizedVariable("nVar2", "A discrete variable", 3);
      nVar3 = new gum::LabelizedVariable("nVar3", "A discrete variable", 4);
      nVar4 = new gum::LabelizedVariable("nVar4", "A discrete variable", 5);
      nVar5 = new gum::LabelizedVariable("nVar5", "A discrete variable", 10);

      pot3F = new gum::Tensor< double >();
      pot3F->add(*nVar1);
      pot3F->add(*nVar2);
      pot3F->add(*nVar3);
      pot3F->add(*nVar4);
      pot3F->add(*nVar5);

      pot3D = new gum::Tensor< double >();
      pot3D->add(*nVar1);
      pot3D->add(*nVar2);
      pot3D->add(*nVar3);
      pot3D->add(*nVar4);
      pot3D->add(*nVar5);
    }

    void tearDown() {
      delete binVar1;
      delete binVar2;
      delete binVar3;
      delete binVar4;

      delete rootPot1F;
      delete rootPot1D;

      delete pot1F;
      delete pot1D;

      delete pot2F;
      delete pot2D;

      delete nVar1;
      delete nVar2;
      delete nVar3;
      delete nVar4;
      delete nVar5;

      delete pot3F;
      delete pot3D;
    }

    GUM_ACTIVE_TEST(CreationDeletion) {
      gum::SimpleCPTGenerator< double >* aCptGen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(aCptGen = new gum::SimpleCPTGenerator< double >())
      TS_GUM_ASSERT_THROWS_NOTHING(delete aCptGen)
    }

    // Test the generator on root variables
    GUM_ACTIVE_TEST(RootVariablesFloat) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(rootPot1F->pos(*binVar1), *rootPot1F))

      gum::Instantiation inst(*rootPot1F);
      double             sum = static_cast< float >(0);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        TS_ASSERT(((*rootPot1F)[inst] >= static_cast< float >(0))
                  && ((*rootPot1F)[inst] <= static_cast< float >(1)))
        sum += (*rootPot1F)[inst];
      }

      TS_ASSERT_DELTA(sum, static_cast< float >(1), 0.0001)
    }

    GUM_ACTIVE_TEST(RootVariablesDouble) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(rootPot1F->pos(*binVar1), *rootPot1D))

      gum::Instantiation inst(*rootPot1D);
      double             sum = static_cast< double >(0);

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        TS_ASSERT(((*rootPot1D)[inst] >= static_cast< double >(0))
                  && ((*rootPot1D)[inst] <= static_cast< double >(1)))
        sum += (*rootPot1D)[inst];
      }

      TS_ASSERT_DELTA(sum, static_cast< double >(1), 0.0001)
    }

    // Testing with one parent - float version
    GUM_ACTIVE_TEST(BinaryVariableFloat_1) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(pot1F->pos(*binVar1), *pot1F))

      gum::Instantiation inst(*pot1F);
      gum::Instantiation instVar1;
      instVar1.add(*binVar1);

      // std::cout << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";
      // std::cout << std::endl << *pot1F << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot1F)[inst] >= static_cast< float >(0))
                    && ((*pot1F)[inst] <= static_cast< float >(1)))
          sum += (*pot1F)[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< float >(1), 0.0001)

        inst.unsetEnd();
      }
    }

    // Testing with many parents - float version
    GUM_ACTIVE_TEST(BinaryVariableFloat_2) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(pot2F->pos(*binVar3), *pot2F))

      gum::Instantiation inst(*pot2F);
      gum::Instantiation instVar1;
      instVar1.add(*binVar3);

      // std::cout << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";
      // std::cout << std::endl << *pot2F << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot2F)[inst] >= static_cast< float >(0))
                    && ((*pot2F)[inst] <= static_cast< float >(1)))
          sum += (*pot2F)[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< float >(1), 0.0001)

        inst.unsetEnd();
      }
    }

    // Testing with one parent - double version
    GUM_ACTIVE_TEST(BinaryVariableDouble_1) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(pot1D->pos(*binVar1), *pot1D))

      gum::Instantiation inst(*pot1D);
      gum::Instantiation instVar1;
      instVar1.add(*binVar1);

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot1D)[inst] >= 0) && ((*pot1D)[inst] <= 1))
          sum += (*pot1D)[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< double >(1), 0.0001)

        inst.unsetEnd();
      }
    }

    // Testing with many parents - double version
    GUM_ACTIVE_TEST(BinaryVariableDouble_2) {
      gum::SimpleCPTGenerator< double > aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(aCPTGen.generateCPT(pot2D->pos(*binVar4), *pot2D))

      gum::Instantiation inst(*pot2D);
      gum::Instantiation instVar1;
      instVar1.add(*binVar4);

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot2D)[inst] >= 0) && ((*pot2D)[inst] <= 1))
          sum += (*pot2D)[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< double >(1), 0.0001)

        inst.unsetEnd();
      }
    }

    // Testing with no parents - float version
    GUM_ACTIVE_TEST(NAryVariableFloat_1) {
      gum::SimpleCPTGenerator< double > cptGen;

      for (int i = 2; i < 100; ++i) {
        gum::LabelizedVariable aVar("aVar", "A discrete variable", i);
        gum::Tensor< double >  aPot;
        aPot.add(aVar);
        TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(aPot.pos(aVar), aPot))

        gum::Instantiation inst(aPot);
        double             sum = static_cast< double >(0);

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          sum += aPot[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< float >(1), 0.0001)
      }
    }

    // Testing with 4 parents, with domain size > 2
    GUM_ACTIVE_TEST(NAryVariableFloat_2) {
      gum::SimpleCPTGenerator< double > cptGen;
      TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(pot3F->pos(*nVar3), *pot3F))
      gum::Instantiation instVar3;
      instVar3.add(*nVar3);
      gum::Instantiation inst(*pot3F);

      for (inst.setFirstOut(instVar3); !inst.end(); inst.incOut(instVar3)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar3); !inst.end(); inst.incIn(instVar3)) {
          sum += (*pot3F)[inst];
        }

        TS_ASSERT_DELTA(static_cast< double >(1), sum, 0.0001)

        inst.unsetEnd();
      }
    }

    // Testing with no parents - double version
    GUM_ACTIVE_TEST(NAryVariableDouble_1) {
      gum::SimpleCPTGenerator< double > cptGen;

      for (int i = 2; i < 100; ++i) {
        gum::LabelizedVariable aVar("aVar", "A discrete variable", i);
        gum::Tensor< double >  aPot;
        aPot.add(aVar);
        TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(aPot.pos(aVar), aPot))

        gum::Instantiation inst(aPot);
        double             sum = static_cast< double >(0);

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          sum += aPot[inst];
        }

        TS_ASSERT_DELTA(sum, static_cast< double >(1), 0.0001)
      }
    }

    // Testing with 4 parents, with domain size > 2
    GUM_ACTIVE_TEST(NAryVariableDouble_2) {
      gum::SimpleCPTGenerator< double > cptGen;
      TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(pot3D->pos(*nVar3), *pot3D))

      gum::Instantiation instVar3;
      instVar3.add(*nVar3);
      gum::Instantiation inst(*pot3D);

      for (inst.setFirstOut(instVar3); !inst.end(); inst.incOut(instVar3)) {
        double sum = static_cast< double >(0);

        for (inst.setFirstIn(instVar3); !inst.end(); inst.incIn(instVar3)) {
          sum += (*pot3D)[inst];
        }

        TS_ASSERT_DELTA(static_cast< double >(1), sum, 0.0001)

        inst.unsetEnd();
      }
    }
  };
}   // namespace gum_tests
