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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>

#include <agrum/base/core/math/math_utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5
namespace gum_tests {
  class [[maybe_unused]] DSLReaderTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constuctor) {
      std::string             file = GET_RESSOURCES_PATH("DSL/DSLReader_file1.txt");
      gum::BayesNet< double > net;

      gum::DSLReader< double >* reader = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::DSLReader< double >(&net, file))
      TS_GUM_ASSERT_THROWS_NOTHING(delete reader)
    }

    GUM_ACTIVE_TEST(Read_file1) {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file1.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(net->empty())
        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file2_float) {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file2.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT_EQUALS(net->size(), (gum::Size)2)
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        TS_ASSERT_EQUALS(var_1.name(), "n1")
        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2)
        const gum::Tensor< double >& proba_1 = net->cpt(node_1);
        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2)
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        TS_ASSERT(std::abs((proba_1[inst_1] - 0.2f)) < 0.001f)
        inst_1.setLast();
        TS_ASSERT(std::abs((proba_1[inst_1] - 0.8f)) < 0.001f)

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "n2")
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2)
        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2)
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        TS_ASSERT(std::abs((proba_2[inst_2] - 0.3f)) < 0.001f)
        inst_2.setLast();
        TS_ASSERT(std::abs((proba_2[inst_2] - 0.7f)) < 0.001f)

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file2_double) {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file2.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT_EQUALS(net->size(), (gum::Size)2)
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        TS_ASSERT_EQUALS(var_1.name(), "n1")
        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2)
        const gum::Tensor< double >& proba_1 = net->cpt(node_1);
        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2)
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        TS_ASSERT(std::abs((proba_1[inst_1] - 0.2)) < 0.001)
        inst_1.setLast();
        TS_ASSERT(std::abs((proba_1[inst_1] - 0.8)) < 0.001)

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "n2")
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2)
        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2)
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        TS_ASSERT(std::abs((proba_2[inst_2] - 0.3)) < 0.001)
        inst_2.setLast();
        TS_ASSERT(std::abs((proba_2[inst_2] - 0.7)) < 0.001)

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file3) {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file3.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      TS_ASSERT_EQUALS(net->size(), (gum::Size)6)
      TS_ASSERT_DIFFERS(net, nullptr)
      gum::DSLWriter< double > writer;
      std::string              file2 = GET_RESSOURCES_PATH("outputs/DSLWriter_TestFile3.txt");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file2, *net))

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Read_file_completeDSL) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Ling.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      reader.trace(true);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      TS_ASSERT_EQUALS(net->size(), (gum::Size)13)
      TS_ASSERT_DIFFERS(net, nullptr)
      gum::DSLWriter< double > writer;
      std::string              file2 = GET_RESSOURCES_PATH("outputs/DSLWriter_Ling.txt");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file2, *net))

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Alarm) {
      std::string              file = GET_RESSOURCES_PATH("DSL/alarm.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      gum::HashTable< std::string, gum::NodeId > idMap;

      for (const auto node: net->nodes())
        idMap.insert(net->variable(node).name(), node);

      // The node wich we'll test
      TS_ASSERT(idMap.exists("HISTORY"))
      // It's parent
      TS_ASSERT(idMap.exists("LVFAILURE"))

      if (idMap.exists("HISTORY") && idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable& history = net->variable(idMap["HISTORY"]);
        TS_ASSERT_EQUALS(history.domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(history.label(0), "TRUE")
        TS_ASSERT_EQUALS(history.label(1), "FALSE")
        TS_ASSERT(net->dag().existsArc(idMap["LVFAILURE"], idMap["HISTORY"]))
        const gum::Tensor< double >& historyCPT = net->cpt(idMap["HISTORY"]);
        TS_ASSERT_EQUALS(historyCPT.domainSize(), (gum::Size)4)
        TS_ASSERT(historyCPT.contains(net->variable(idMap["HISTORY"])))
        TS_ASSERT(historyCPT.contains(net->variable(idMap["LVFAILURE"])))
        gum::Instantiation historyInst(historyCPT);
        // (TRUE | TRUE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        TS_ASSERT(std::abs(historyCPT[historyInst] - 0.9f) < 0.0001f)
        // (FALSE | TRUE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        TS_ASSERT(std::abs(historyCPT[historyInst] - 0.1f) < 0.0001f)
        // (TRUE | FALSE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        TS_ASSERT(std::abs(historyCPT[historyInst] - 0.01f) < 0.0001f)
        // (FALSE | FALSE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        TS_ASSERT(std::abs(historyCPT[historyInst] - 0.99f) < 0.0001f)
      }

      // The node wich we'll test
      TS_ASSERT(idMap.exists("ERRLOWOUTPUT"))
      // It's Children
      TS_ASSERT(idMap.exists("HRBP"))

      if (idMap.exists("ERRLOWOUTPUT") && idMap.exists("HRBP")) {
        const gum::DiscreteVariable& errlowoutput = net->variable(idMap["ERRLOWOUTPUT"]);
        TS_ASSERT_EQUALS(errlowoutput.domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(errlowoutput.label(0), "TRUE")
        TS_ASSERT_EQUALS(errlowoutput.label(1), "FALSE")
        TS_ASSERT(net->dag().existsArc(idMap["ERRLOWOUTPUT"], idMap["HRBP"]))
        const gum::Tensor< double >& errlowoutputCPT = net->cpt(idMap["ERRLOWOUTPUT"]);
        TS_ASSERT_EQUALS(errlowoutputCPT.domainSize(), (gum::Size)2)
        TS_ASSERT(errlowoutputCPT.contains(errlowoutput))
        gum::Instantiation errlowoutputInst(errlowoutputCPT);
        errlowoutputInst.chgVal(errlowoutput, 0);
        TS_ASSERT(std::abs(errlowoutputCPT[errlowoutputInst] - 0.05f) < 0.001f)
        errlowoutputInst.chgVal(errlowoutput, 1);
        TS_ASSERT(std::abs(errlowoutputCPT[errlowoutputInst] - 0.95f) < 0.001f)
      }

      // The nide wich we'll test
      TS_ASSERT(idMap.exists("LVEDVOLUME"))
      // It's parents
      TS_ASSERT(idMap.exists("HYPOVOLEMIA"))
      TS_ASSERT(idMap.exists("LVFAILURE"))

      if (idMap.exists("LVEDVOLUME") && idMap.exists("HYPOVOLEMIA") && idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable& lvedvolume  = net->variable(idMap["LVEDVOLUME"]);
        const gum::DiscreteVariable& hypovolemia = net->variable(idMap["HYPOVOLEMIA"]);
        const gum::DiscreteVariable& lvfailure   = net->variable(idMap["LVFAILURE"]);
        // checking label order
        TS_ASSERT_EQUALS(lvedvolume.label(0), "LOW")
        TS_ASSERT_EQUALS(lvedvolume.label(1), "NORMAL")
        TS_ASSERT_EQUALS(lvedvolume.label(2), "HIGH")
        TS_ASSERT_EQUALS(hypovolemia.label(0), "TRUE")
        TS_ASSERT_EQUALS(hypovolemia.label(1), "FALSE")
        TS_ASSERT_EQUALS(lvfailure.label(0), "TRUE")
        TS_ASSERT_EQUALS(lvfailure.label(1), "FALSE")
        const gum::Tensor< double >& cpt = net->cpt(idMap["LVEDVOLUME"]);
        gum::Instantiation           inst(cpt);
        gum::Instantiation           var_inst;
        var_inst << lvedvolume;
        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 0);
        float  array_1[] = {0.95f, 0.04f, 0.01f};
        size_t i         = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_1[i], 0.001f)
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 0);
        float array_2[] = {0.98f, 0.01f, 0.01f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_2[i], 0.001f)
        }

        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 1);
        float array_3[] = {0.01f, 0.09f, 0.9f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_3[i], 0.001f)
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 1);
        float array_4[] = {0.05f, 0.9f, 0.05f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_4[i], 0.001f)
        }
      }

      delete net;
    }

    GUM_ACTIVE_TEST(Unexisting) {
      std::string              file = "Schmurtz";
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gum::DSLReader< double > reader(net, file))
      gum::DSLReader< double > reader(net, file);
      TS_ASSERT_THROWS(reader.proceed(), const gum::IOError&)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Barley) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Barley.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Carpo) {
      std::string              file = GET_RESSOURCES_PATH("DSL/carpo.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Diabetes) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Diabetes.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Hailfinder) {
      std::string              file = GET_RESSOURCES_PATH("DSL/hailfinder.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Insurance) {
      std::string              file = GET_RESSOURCES_PATH("DSL/insurance.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Link) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Link.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    // Mildew is not readable (because of the VERY LARGE FLOAT_LIST in foto_4
    // (at
    // least))...
    void Mildew() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Mildew.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Munin1) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Munin1.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Pigs) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Pigs.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }

    GUM_ACTIVE_TEST(Water) {
      std::string              file = GET_RESSOURCES_PATH("DSL/Water.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      if (net) delete net;
    }
  };
}   // namespace gum_tests
