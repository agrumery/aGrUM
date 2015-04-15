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
#include <agrum/BN/generator/simpleCPTGenerator.h>

namespace gum_tests {

  class SimpleCPTGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    // Binary variables
    gum::LabelizedVariable *binVar1;
    gum::LabelizedVariable *binVar2;
    gum::LabelizedVariable *binVar3;
    gum::LabelizedVariable *binVar4;

    gum::Potential<float> *rootPot1F;
    gum::Potential<double> *rootPot1D;

    gum::Potential<float> *pot1F;
    gum::Potential<double> *pot1D;

    gum::Potential<float> *pot2F;
    gum::Potential<double> *pot2D;

    // n-ary variables
    gum::LabelizedVariable *nVar1;
    gum::LabelizedVariable *nVar2;
    gum::LabelizedVariable *nVar3;
    gum::LabelizedVariable *nVar4;
    gum::LabelizedVariable *nVar5;

    gum::Potential<float> *pot3F;
    gum::Potential<double> *pot3D;

    void setUp() {
      binVar1 = new gum::LabelizedVariable("binVar1", "Binary Variable number 1");
      binVar2 = new gum::LabelizedVariable("binVar2", "Binary Variable number 2");
      binVar3 = new gum::LabelizedVariable("binVar3", "Binary Variable number 3");
      binVar4 = new gum::LabelizedVariable("binVar4", "Binary Variable number 4");

      rootPot1F = new gum::Potential<float>();
      rootPot1F->add(*binVar1);

      rootPot1D = new gum::Potential<double>();
      rootPot1D->add(*binVar1);

      pot1F = new gum::Potential<float>();
      pot1F->add(*binVar1);
      pot1F->add(*binVar2);

      pot1D = new gum::Potential<double>();
      pot1D->add(*binVar1);
      pot1D->add(*binVar2);

      pot2F = new gum::Potential<float>();
      pot2F->add(*binVar1);
      pot2F->add(*binVar2);
      pot2F->add(*binVar3);
      pot2F->add(*binVar4);

      pot2D = new gum::Potential<double>();
      pot2D->add(*binVar1);
      pot2D->add(*binVar2);
      pot2D->add(*binVar3);
      pot2D->add(*binVar4);

      nVar1 = new gum::LabelizedVariable("nVar1", "A discrete variable", 2);
      nVar2 = new gum::LabelizedVariable("nVar1", "A discrete variable", 3);
      nVar3 = new gum::LabelizedVariable("nVar1", "A discrete variable", 4);
      nVar4 = new gum::LabelizedVariable("nVar1", "A discrete variable", 5);
      nVar5 = new gum::LabelizedVariable("nVar1", "A discrete variable", 10);

      pot3F = new gum::Potential<float>();
      pot3F->add(*nVar1);
      pot3F->add(*nVar2);
      pot3F->add(*nVar3);
      pot3F->add(*nVar4);
      pot3F->add(*nVar5);

      pot3D = new gum::Potential<double>();
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

    void testCreationDeletion() {
      gum::SimpleCPTGenerator<float> *aCptGen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(aCptGen = new gum::SimpleCPTGenerator<float>());
      TS_GUM_ASSERT_THROWS_NOTHING(delete aCptGen);
    }

    // Test the generator on root variables
    void testRootVariablesFloat() {
      gum::SimpleCPTGenerator<float> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(rootPot1F->pos(*binVar1), *rootPot1F));

      gum::Instantiation inst(*rootPot1F);
      float sum = (float)0;

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        TS_ASSERT(((*rootPot1F)[inst] >= (float)0) &&
                  ((*rootPot1F)[inst] <= (float)1));
        sum += (*rootPot1F)[inst];
      }

      TS_ASSERT_DELTA(sum, (float)1, 0.0001);
    }

    void testRootVariablesDouble() {
      gum::SimpleCPTGenerator<double> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(rootPot1F->pos(*binVar1), *rootPot1D));

      gum::Instantiation inst(*rootPot1D);
      double sum = (double)0;

      for (inst.setFirst(); !inst.end(); inst.inc()) {
        TS_ASSERT(((*rootPot1D)[inst] >= (double)0) &&
                  ((*rootPot1D)[inst] <= (double)1));
        sum += (*rootPot1D)[inst];
      }

      TS_ASSERT_DELTA(sum, (double)1, 0.0001);
    }

    // Testing with one parent - float version
    void testBinaryVariableFloat_1() {
      gum::SimpleCPTGenerator<float> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(pot1F->pos(*binVar1), *pot1F));

      gum::Instantiation inst(*pot1F);
      gum::Instantiation instVar1;
      instVar1.add(*binVar1);

      // std::cout << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";
      // std::cout << std::endl << *pot1F << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        float sum = (float)0;

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot1F)[inst] >= (float)0) && ((*pot1F)[inst] <= (float)1));
          sum += (*pot1F)[inst];
        }

        TS_ASSERT_DELTA(sum, (float)1, 0.0001);

        inst.unsetEnd();
      }
    }

    // Testing with many parents - float version
    void testBinaryVariableFloat_2() {
      gum::SimpleCPTGenerator<float> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(pot2F->pos(*binVar3), *pot2F));

      gum::Instantiation inst(*pot2F);
      gum::Instantiation instVar1;
      instVar1.add(*binVar3);

      // std::cout << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";
      // std::cout << std::endl << *pot2F << std::endl;
      // for (int i = 0; i < 80; ++i) std::cout << "#";

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        float sum = (float)0;

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot2F)[inst] >= (float)0) && ((*pot2F)[inst] <= (float)1));
          sum += (*pot2F)[inst];
        }

        TS_ASSERT_DELTA(sum, (float)1, 0.0001);

        inst.unsetEnd();
      }
    }

    // Testing with one parent - double version
    void testBinaryVariableDouble_1() {
      gum::SimpleCPTGenerator<double> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(pot1D->pos(*binVar1), *pot1D));

      gum::Instantiation inst(*pot1D);
      gum::Instantiation instVar1;
      instVar1.add(*binVar1);

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = (double)0;

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot1D)[inst] >= 0) && ((*pot1D)[inst] <= 1));
          sum += (*pot1D)[inst];
        }

        TS_ASSERT_DELTA(sum, (double)1, 0.0001);

        inst.unsetEnd();
      }
    }

    // Testing with many parents - double version
    void testBinaryVariableDouble_2() {
      gum::SimpleCPTGenerator<double> aCPTGen;
      TS_GUM_ASSERT_THROWS_NOTHING(
          aCPTGen.generateCPT(pot2D->pos(*binVar4), *pot2D));

      gum::Instantiation inst(*pot2D);
      gum::Instantiation instVar1;
      instVar1.add(*binVar4);

      for (inst.setFirstOut(instVar1); !inst.end(); inst.incOut(instVar1)) {
        double sum = (double)0;

        for (inst.setFirstIn(instVar1); !inst.end(); inst.incIn(instVar1)) {
          TS_ASSERT(((*pot2D)[inst] >= 0) && ((*pot2D)[inst] <= 1));
          sum += (*pot2D)[inst];
        }

        TS_ASSERT_DELTA(sum, (double)1, 0.0001);

        inst.unsetEnd();
      }
    }

    // Testing with no parents - float version
    void testNAryVariableFloat_1() {
      gum::SimpleCPTGenerator<float> cptGen;

      for (int i = 2; i < 100; ++i) {
        gum::LabelizedVariable aVar("aVar", "A discrete variable", i);
        gum::Potential<float> aPot;
        aPot.add(aVar);
        TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(aPot.pos(aVar), aPot));

        gum::Instantiation inst(aPot);
        float sum = (float)0;

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          sum += aPot[inst];
        }

        TS_ASSERT_DELTA(sum, (float)1, 0.0001);
      }
    }

    // Testing with 4 parents, with domain size > 2
    void testNAryVariableFloat_2() {
      gum::SimpleCPTGenerator<float> cptGen;
      TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(pot3F->pos(*nVar3), *pot3F));
      gum::Instantiation instVar3;
      instVar3.add(*nVar3);
      gum::Instantiation inst(*pot3F);

      for (inst.setFirstOut(instVar3); !inst.end(); inst.incOut(instVar3)) {
        float sum = (float)0;

        for (inst.setFirstIn(instVar3); !inst.end(); inst.incIn(instVar3)) {
          sum += (*pot3F)[inst];
        }

        TS_ASSERT_DELTA((float)1, sum, 0.0001);

        inst.unsetEnd();
      }
    }

    // Testing with no parents - double version
    void testNAryVariableDouble_1() {
      gum::SimpleCPTGenerator<double> cptGen;

      for (int i = 2; i < 100; ++i) {
        gum::LabelizedVariable aVar("aVar", "A discrete variable", i);
        gum::Potential<double> aPot;
        aPot.add(aVar);
        TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(aPot.pos(aVar), aPot));

        gum::Instantiation inst(aPot);
        double sum = (double)0;

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          sum += aPot[inst];
        }

        TS_ASSERT_DELTA(sum, (double)1, 0.0001);
      }
    }

    // Testing with 4 parents, with domain size > 2
    void testNAryVariableDouble_2() {
      gum::SimpleCPTGenerator<double> cptGen;
      TS_GUM_ASSERT_THROWS_NOTHING(cptGen.generateCPT(pot3D->pos(*nVar3), *pot3D));

      gum::Instantiation instVar3;
      instVar3.add(*nVar3);
      gum::Instantiation inst(*pot3D);

      for (inst.setFirstOut(instVar3); !inst.end(); inst.incOut(instVar3)) {
        double sum = (double)0;

        for (inst.setFirstIn(instVar3); !inst.end(); inst.incIn(instVar3)) {
          sum += (*pot3D)[inst];
        }

        TS_ASSERT_DELTA((double)1, sum, 0.0001);

        inst.unsetEnd();
      }
    }
  };
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
