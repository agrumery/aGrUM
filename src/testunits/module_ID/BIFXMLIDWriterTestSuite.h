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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>
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
  class [[maybe_unused]] BIFXMLIDWriterTestSuite: public CxxTest::TestSuite {
    private:
    void fillTopo(gum::InfluenceDiagram< double >& infDiag, gum::List< gum::NodeId >& idList) {
      try {
        idList.insert(infDiag.addDecisionNode(*decisionVar1)); // 0
        idList.insert(infDiag.addDecisionNode(*decisionVar2)); // 1
        idList.insert(infDiag.addDecisionNode(*decisionVar3)); // 2
        idList.insert(infDiag.addDecisionNode(*decisionVar4)); // 3
        idList.insert(infDiag.addChanceNode(*chanceVar1)); // 4
        idList.insert(infDiag.addChanceNode(*chanceVar2)); // 5
        idList.insert(infDiag.addChanceNode(*chanceVar3)); // 6
        idList.insert(infDiag.addChanceNode(*chanceVar4)); // 7
        idList.insert(infDiag.addChanceNode(*chanceVar5)); // 8
        idList.insert(infDiag.addUtilityNode(*utilityVar1)); // 9
        idList.insert(infDiag.addUtilityNode(*utilityVar2)); // 10

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
      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill(gum::InfluenceDiagram< double >& infDiag, gum::List< gum::NodeId >& idList) {
      fillTopo(infDiag, idList);

      try {
        infDiag.cpt(idList[4]).populate({0.2, 0.8, 0.5, 0.5});
        infDiag.cpt(idList[5]).populate({0.1, 0.9, 0.9, 0.1});
        infDiag.cpt(idList[6]).populate({0.35, 0.65, 0.19, 0.81});
        infDiag.cpt(idList[7]).populate({0.4, 0.6, 0.5, 0.5});
        infDiag.cpt(idList[8]).populate({0.4, 0.6, 0.8, 0.2, 0.4, 0.6, 0.3, 0.7});

        infDiag.utility(idList[9]).populate({42, 69, 666, 84});
        infDiag.utility(idList[10]).populate({42, -69, 666, 84});
      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    gum::InfluenceDiagram< double >* id;
    gum::List< gum::NodeId >*        idListPtr;
    gum::LabelizedVariable *         decisionVar1, *decisionVar2, *decisionVar3, *decisionVar4;
    gum::LabelizedVariable *         chanceVar1, *chanceVar2, *chanceVar3, *chanceVar4, *chanceVar5;
    gum::LabelizedVariable *         utilityVar1, *utilityVar2;

    void setUp() {
      id        = new gum::InfluenceDiagram< double >();
      idListPtr = new gum::List< gum::NodeId >();

      decisionVar1 = new gum::LabelizedVariable("decisionVar1", "D1", 2);
      decisionVar2 = new gum::LabelizedVariable("decisionVar2", "D2", 2);
      decisionVar3 = new gum::LabelizedVariable("decisionVar3", "D3", 2);
      decisionVar4 = new gum::LabelizedVariable("decisionVar4", "D4", 2);
      chanceVar1   = new gum::LabelizedVariable("chanceVar1", "C1", 2);
      chanceVar2   = new gum::LabelizedVariable("chanceVar2", "C2", 2);
      chanceVar3   = new gum::LabelizedVariable("chanceVar3", "C3", 2);
      chanceVar4   = new gum::LabelizedVariable("chanceVar4", "C4", 2);
      chanceVar5   = new gum::LabelizedVariable("chanceVar5", "C5", 2);
      utilityVar1  = new gum::LabelizedVariable("utilityVar1", "U1", 1);
      utilityVar2  = new gum::LabelizedVariable("utilityVar2", "U2", 1);

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

    GUM_ACTIVE_TEST(Constuctor) {
      gum::BIFXMLIDWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::BIFXMLIDWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::BIFXMLIDWriter< double > writer;
      std::stringstream             s;

      // Uncomment this to check the ouput
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(s /*cerr*/, *id))
    }

    GUM_ACTIVE_TEST(Writer_file) {
      gum::BIFXMLIDWriter< double > writer;
      std::string                   file = GET_RESSOURCES_PATH("outputs/IDBIFXMLIO_file.xml");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *id))

      // For comparison with what readers will return
      std::string   dotfile = GET_RESSOURCES_PATH("outputs/IDToDotWriter.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

      output << id->toDot();
      output.flush();
      output.close();

      if (output.fail()) { GUM_ERROR(gum::IOError, "Writing in the ostream failed.") }
    }

    GUM_ACTIVE_TEST(GenerationReadWrite) {
      gum::InfluenceDiagramGenerator< double > gen;

      for (int i = 0; i < 5; i++) {
        gum::InfluenceDiagram< double >* net = gen.generateID(25, 0.3f, 0.3f, 0.1f, 4);

        gum::BIFXMLIDWriter< double > writer;
        std::string                   file = GET_RESSOURCES_PATH("outputs/random.xml");
        writer.write(file, *net);

        gum::InfluenceDiagram< double > net2;
        gum::BIFXMLIDReader< double >   reader(&net2, file);
        reader.proceed();

        for (const auto n: net->nodes()) {
          if (net->isChanceNode(n)) {
            const std::string&    name = net->variable(n).name();
            gum::Tensor< double > p(net->cpt(name));
            p.fillWith(net2.cpt(name));
            if (net->cpt(name) != p) { TS_ASSERT(false) }
          }
          if (net->isUtilityNode(n)) {
            const std::string&    name = net->variable(n).name();
            gum::Tensor< double > p(net->utility(name));
            p.fillWith(net2.utility(name));
            if (net->utility(name) != p) { TS_ASSERT(false) }
          }
        }

        delete net;
      }
    }


    GUM_ACTIVE_TEST(GenerationReadWrite2) {
      auto net = gum::InfluenceDiagram< double >::fastPrototype(
          "A[5]->B+[0,10,5]->C{0|1|20|300}->D{0.5|1.99|2|3.14}");

      gum::BIFXMLIDWriter< double > writer;
      std::string                   file = GET_RESSOURCES_PATH("outputs/random.xml");
      writer.write(file, net);

      gum::InfluenceDiagram< double > net2;
      gum::BIFXMLIDReader< double >   reader(&net2, file);
      reader.proceed();

      for (const auto n: net.nodes()) {
        const std::string& name = net.variable(n).name();
        TS_ASSERT_EQUALS(net.variable(name).toFast(), net2.variable(name).toFast());
        if (net.isChanceNode(n)) {
          TS_GUM_TENSOR_DELTA_WITH_TRANSLATION(net.cpt(name), net2.cpt(name), 0.0001);
        }
        if (net.isUtilityNode(n)) {
          TS_GUM_TENSOR_DELTA_WITH_TRANSLATION(net.utility(name), net2.utility(name), 0.0001);
        }
      }
    }
  };
} // namespace gum_tests
