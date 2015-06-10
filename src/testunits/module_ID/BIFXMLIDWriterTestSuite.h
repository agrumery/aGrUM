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
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>

// The graph used for the tests:
//           D1
//           |   D1 -> C1
//           C1
//          /  \  C1 -> C2 & C1 -> U1
//        C2   U1
//       /  \ /  C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |  D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /  C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /  C5 -> U2 & D4 -> U2
//          U2

namespace gum_tests {

  class BIFXMLIDWriterTestSuite : public CxxTest::TestSuite {
    private:
    void fillTopo(gum::InfluenceDiagram<double> &infDiag,
                  gum::List<gum::NodeId> &idList) {
      try {
        idList.insert(infDiag.addDecisionNode(*decisionVar1)); // 0
        idList.insert(infDiag.addDecisionNode(*decisionVar2)); // 1
        idList.insert(infDiag.addDecisionNode(*decisionVar3)); // 2
        idList.insert(infDiag.addDecisionNode(*decisionVar4)); // 3
        idList.insert(infDiag.addChanceNode(*chanceVar1));     // 4
        idList.insert(infDiag.addChanceNode(*chanceVar2));     // 5
        idList.insert(infDiag.addChanceNode(*chanceVar3));     // 6
        idList.insert(infDiag.addChanceNode(*chanceVar4));     // 7
        idList.insert(infDiag.addChanceNode(*chanceVar5));     // 8
        idList.insert(infDiag.addUtilityNode(*utilityVar1));   // 9
        idList.insert(infDiag.addUtilityNode(*utilityVar2));   // 10

        infDiag.addArc(idList[0], idList[4]);
        infDiag.addArc(idList[4], idList[9]);
        infDiag.addArc(idList[4], idList[5]);
        infDiag.addArc(idList[5], idList[1]);
        infDiag.addArc(idList[5], idList[2]);
        infDiag.addArc(idList[1], idList[9]);
        infDiag.addArc(idList[1], idList[7]);
        infDiag.addArc(idList[2], idList[6]);
        infDiag.addArc(idList[6], idList[8]);
        infDiag.addArc(idList[7], idList[8]);
        infDiag.addArc(idList[8], idList[10]);
        infDiag.addArc(idList[3], idList[10]);

      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill(gum::InfluenceDiagram<double> &infDiag,
              gum::List<gum::NodeId> &idList) {
      fillTopo(infDiag, idList);

      try {
        const gum::Potential<double> &p1 = infDiag.cpt(idList[4]);
        {
          // FILLING PARAMS
          const double t[4] = {0.2, 0.8, 0.5, 0.5};
          int n = 4;
          const std::vector<double> v(t, t + n);
          p1.fillWith(v);
        }

        const gum::Potential<double> &p2 = infDiag.cpt(idList[5]);
        {
          // FILLING PARAMS
          const double t[4] = {0.1, 0.9, 0.9, 0.1};
          int n = 4;
          const std::vector<double> v(t, t + n);
          p2.fillWith(v);
        }

        const gum::Potential<double> &p3 = infDiag.cpt(idList[6]);
        {
          // FILLING PARAMS
          const double t[4] = {0.35, 0.65, 0.19, 0.81};
          int n = 4;
          const std::vector<double> v(t, t + n);
          p3.fillWith(v);
        }

        const gum::Potential<double> &p4 = infDiag.cpt(idList[7]);
        {
          // FILLING PARAMS
          const double t[4] = {0.4, 0.6, 0.5, 0.5};
          int n = 4;
          const std::vector<double> v(t, t + n);
          p4.fillWith(v);
        }

        const gum::Potential<double> &p5 = infDiag.cpt(idList[8]);
        {
          // FILLING PARAMS
          const double t[8] = {0.4, 0.6, 0.8, 0.2, 0.4, 0.6, 0.3, 0.7};
          int n = 8;
          const std::vector<double> v(t, t + n);
          p5.fillWith(v);
        }

        const gum::UtilityTable<double> &u1 = infDiag.utility(idList[9]);
        {
          // FILLING PARAMS
          const double t[4] = {42, 69, 666, 84};
          int n = 4;
          const std::vector<double> v(t, t + n);
          u1.fillWith(v);
        }

        const gum::UtilityTable<double> &u2 = infDiag.utility(idList[10]);
        {
          // FILLING PARAMS
          const double t[4] = {42, -69, 666, 84};
          int n = 4;
          const std::vector<double> v(t, t + n);
          u2.fillWith(v);
        }
      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    gum::InfluenceDiagram<double> *id;
    gum::List<gum::NodeId> *idListPtr;
    gum::LabelizedVariable *decisionVar1, *decisionVar2, *decisionVar3,
        *decisionVar4;
    gum::LabelizedVariable *chanceVar1, *chanceVar2, *chanceVar3, *chanceVar4,
        *chanceVar5;
    gum::LabelizedVariable *utilityVar1, *utilityVar2;

    void setUp() {
      id = new gum::InfluenceDiagram<double>();
      idListPtr = new gum::List<gum::NodeId>();

      decisionVar1 = new gum::LabelizedVariable("decisionVar1", "D1", 2);
      decisionVar2 = new gum::LabelizedVariable("decisionVar2", "D2", 2);
      decisionVar3 = new gum::LabelizedVariable("decisionVar3", "D3", 2);
      decisionVar4 = new gum::LabelizedVariable("decisionVar4", "D4", 2);
      chanceVar1 = new gum::LabelizedVariable("chanceVar1", "C1", 2);
      chanceVar2 = new gum::LabelizedVariable("chanceVar2", "C2", 2);
      chanceVar3 = new gum::LabelizedVariable("chanceVar3", "C3", 2);
      chanceVar4 = new gum::LabelizedVariable("chanceVar4", "C4", 2);
      chanceVar5 = new gum::LabelizedVariable("chanceVar5", "C5", 2);
      utilityVar1 = new gum::LabelizedVariable("utilityVar1", "U1", 1);
      utilityVar2 = new gum::LabelizedVariable("utilityVar2", "U2", 1);

      fill(*id, *idListPtr);
    }

    void tearDown() {
      delete id;
      delete idListPtr;
      delete decisionVar1;
      delete decisionVar2;
      delete decisionVar3;
      delete decisionVar4;
      delete chanceVar1;
      delete chanceVar2;
      delete chanceVar3;
      delete chanceVar4;
      delete chanceVar5;
      delete utilityVar1;
      delete utilityVar2;
    }

    void testConstuctor() {
      gum::BIFXMLIDWriter<double> *writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::BIFXMLIDWriter<double>());
      delete writer;
    }

    void testWriter_ostream() {
      gum::BIFXMLIDWriter<double> writer;
      std::stringstream s;

      // Uncomment this to check the ouput
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(s /*cerr*/, *id));
    }

    void testWriter_file() {
      gum::BIFXMLIDWriter<double> writer;
      std::string file = GET_PATH_STR("IDBIFXMLIO_file.xml");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *id));

      // For comparison with what readers will return
      std::string dotfile = GET_PATH_STR("IDToDotWriter.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      output << id->toDot();
      output.flush();
      output.close();

      if (output.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }
    }
  };
}
