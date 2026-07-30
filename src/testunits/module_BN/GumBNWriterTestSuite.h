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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/base/core/timer.h>
#include <agrum/base/multidim/aggregators/amplitude.h>
#include <agrum/base/multidim/aggregators/and.h>
#include <agrum/base/multidim/aggregators/count.h>
#include <agrum/base/multidim/aggregators/exists.h>
#include <agrum/base/multidim/aggregators/forall.h>
#include <agrum/base/multidim/aggregators/max.h>
#include <agrum/base/multidim/aggregators/median.h>
#include <agrum/base/multidim/aggregators/min.h>
#include <agrum/base/multidim/aggregators/or.h>
#include <agrum/base/multidim/aggregators/sum.h>
#include <agrum/base/multidim/ICIModels/multiDimLogit.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/GUM/GumBNReader.h>
#include <agrum/BN/io/GUM/GumBNWriter.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {
  struct GumBNWriterTestSuite {
    protected:
    static void _simpleTextForWriter_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bgum")
                                 : GET_RESSOURCES_PATH("outputs/test.jgum");

      GUM_CHECK_ASSERT_THROWS_NOTHING({
        gum::GumBNWriter< double > writer(isbinary);
        writer.write(path, bn);
      });

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(bn2, bn);
    }

    static void _checkMetaData_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bgum")
                                 : GET_RESSOURCES_PATH("outputs/test.jgum");

      CHECK_NOTHROW({
        gum::GumBNWriter< double > writer(isbinary, 2);
        writer.write(path, bn);
      });
      {
        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(bn2, bn);

        CHECK(bn2.existsProperty("software"));
        CHECK_EQ(bn2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(bn2.existsProperty("creation"));
        CHECK(bn2.existsProperty("lastModification"));
        // creation == lastModification since file was just created
        CHECK_EQ(bn2.property("creation"), bn2.property("lastModification"));
      }

      // slight delay to ensure timestamps differ on second write
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      {
        CHECK_NOTHROW({
          gum::GumBNWriter< double > writer(isbinary, 2);
          writer.write(path, bn);   // overwrite the file
        });

        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(bn2, bn);

        CHECK(bn2.existsProperty("software"));
        CHECK_EQ(bn2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(bn2.existsProperty("creation"));
        CHECK(bn2.existsProperty("lastModification"));
        // creation != lastModification because of the sleep above
        CHECK_NE(bn2.property("creation"), bn2.property("lastModification"));
      }
    }

    static void _withBigFiles_(bool isbinary, int indent = 0) {
      const auto src    = GET_RESSOURCES_PATH("bifxml/Diabetes.bifxml");
      const auto dstxml = GET_RESSOURCES_PATH("outputs/Diabetes.bifxml");
      const auto dst = isbinary ? GET_RESSOURCES_PATH("outputs/Diabetes.bgum")
                                : ((indent < 0) ? GET_RESSOURCES_PATH("outputs/Diabetes_comp.jgum")
                                                : GET_RESSOURCES_PATH("outputs/Diabetes.jgum"));

      gum::BayesNet< double > bn;

      gum::BIFXMLBNReader< double > reader(&bn, src);
      CHECK_EQ(reader.proceed(), 0u);

      gum::GumBNWriter< double > writer(isbinary, indent);
      writer.write(dst, bn);

      gum::BayesNet< double > bn2;
      auto                    reader2 = gum::GumBNReader< double >(&bn2, dst, isbinary);
      CHECK_EQ(reader2.proceed(), 0u);

      CHECK_EQ(bn2, bn);
    }

    static void _aggregatorAndICIRoundtrip_(bool isbinary) {
      // fast-syntax reparses purely-numeric labels ("0","1") as an integer/range
      // variable rather than a LabelizedVariable (see BayesNet::fastPrototype doc);
      // use non-numeric labels so the round-trip preserves the variable type.
      const std::vector< std::string > labels{"no", "yes"};
      gum::BayesNet< double >          bn;

      gum::LabelizedVariable p1("P1", "", labels);
      gum::LabelizedVariable p2("P2", "", labels);
      const gum::NodeId      idP1 = bn.add(p1);
      const gum::NodeId      idP2 = bn.add(p2);

      gum::LabelizedVariable vAnd("And", "", labels);
      gum::LabelizedVariable vOr("Or", "", labels);
      gum::LabelizedVariable vAmplitude("Amplitude", "", labels);
      gum::LabelizedVariable vCount("Count", "", labels);
      gum::LabelizedVariable vExists("Exists", "", labels);
      gum::LabelizedVariable vForall("Forall", "", labels);
      gum::LabelizedVariable vMax("Max", "", labels);
      gum::LabelizedVariable vMedian("Median", "", labels);
      gum::LabelizedVariable vMin("Min", "", labels);
      gum::LabelizedVariable vSum("Sum", "", labels);
      gum::LabelizedVariable vNoisyORCompound("NoisyORCompound", "", labels);
      gum::LabelizedVariable vNoisyORNet("NoisyORNet", "", labels);
      gum::LabelizedVariable vNoisyAND("NoisyAND", "", labels);
      gum::LabelizedVariable vLogit("Logit", "", labels);

      const gum::NodeId idAnd             = bn.addAND(vAnd);
      const gum::NodeId idOr              = bn.addOR(vOr);
      const gum::NodeId idAmplitude       = bn.addAMPLITUDE(vAmplitude);
      const gum::NodeId idCount           = bn.addCOUNT(vCount, 1);
      const gum::NodeId idExists          = bn.addEXISTS(vExists, 1);
      const gum::NodeId idForall          = bn.addFORALL(vForall, 1);
      const gum::NodeId idMax             = bn.addMAX(vMax);
      const gum::NodeId idMedian          = bn.addMEDIAN(vMedian);
      const gum::NodeId idMin             = bn.addMIN(vMin);
      const gum::NodeId idSum             = bn.addSUM(vSum);
      const gum::NodeId idNoisyORCompound = bn.addNoisyORCompound(vNoisyORCompound, 0.1);
      const gum::NodeId idNoisyORNet      = bn.addNoisyORNet(vNoisyORNet, 0.1);
      const gum::NodeId idNoisyAND        = bn.addNoisyAND(vNoisyAND, 0.1);
      const gum::NodeId idLogit           = bn.addLogit(vLogit, 0.1);

      for (const gum::NodeId id:
           {idAnd, idOr, idAmplitude, idCount, idExists, idForall, idMax, idMedian, idMin, idSum}) {
        bn.addArc(idP1, id);
        bn.addArc(idP2, id);
      }
      for (const gum::NodeId id: {idNoisyORCompound, idNoisyORNet, idNoisyAND, idLogit}) {
        bn.addWeightedArc(idP1, id, 0.3);
        bn.addWeightedArc(idP2, id, 0.6);
      }

      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/aggregators_ici.bgum")
                                 : GET_RESSOURCES_PATH("outputs/aggregators_ici.jgum");
      gum::GumBNWriter< double > writer(isbinary);
      writer.write(path, bn);

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(bn2, bn);

      // operator== only compares CPT values, not the underlying implementation type:
      // check explicitly that each node kept its symbolic (non-dense) type after round-trip.
      CHECK(dynamic_cast< const gum::aggregator::And< double >* >(bn2.cpt("And").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Or< double >* >(bn2.cpt("Or").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Amplitude< double >* >(
                bn2.cpt("Amplitude").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Count< double >* >(bn2.cpt("Count").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Exists< double >* >(bn2.cpt("Exists").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Forall< double >* >(bn2.cpt("Forall").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Max< double >* >(bn2.cpt("Max").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Median< double >* >(bn2.cpt("Median").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Min< double >* >(bn2.cpt("Min").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::aggregator::Sum< double >* >(bn2.cpt("Sum").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::MultiDimNoisyORCompound< double >* >(
                bn2.cpt("NoisyORCompound").content())
            != nullptr);
      CHECK(
          dynamic_cast< const gum::MultiDimNoisyORNet< double >* >(bn2.cpt("NoisyORNet").content())
          != nullptr);
      CHECK(dynamic_cast< const gum::MultiDimNoisyAND< double >* >(bn2.cpt("NoisyAND").content())
            != nullptr);
      CHECK(dynamic_cast< const gum::MultiDimLogit< double >* >(bn2.cpt("Logit").content())
            != nullptr);
    }

    static void _aggregatorCompactness_() {
      // a dense encoding of a 4-parent, 10-modality aggregator CPT would need 10^4
      // doubles (~80KB in JSON); the compact "kind":"aggregator" encoding keeps the
      // whole file well under 1KB.
      gum::BayesNet< double > bn;
      gum::LabelizedVariable  p1("P1", "", 10);
      gum::LabelizedVariable  p2("P2", "", 10);
      gum::LabelizedVariable  p3("P3", "", 10);
      gum::LabelizedVariable  p4("P4", "", 10);
      gum::LabelizedVariable  vSum("Sum", "", 10);

      const gum::NodeId idP1  = bn.add(p1);
      const gum::NodeId idP2  = bn.add(p2);
      const gum::NodeId idP3  = bn.add(p3);
      const gum::NodeId idP4  = bn.add(p4);
      const gum::NodeId idSum = bn.addSUM(vSum);
      bn.addArc(idP1, idSum);
      bn.addArc(idP2, idSum);
      bn.addArc(idP3, idSum);
      bn.addArc(idP4, idSum);

      const auto                 path = GET_RESSOURCES_PATH("outputs/aggregator_compact.jgum");
      gum::GumBNWriter< double > writer(false);
      writer.write(path, bn);

      std::ifstream file(path, std::ios::binary | std::ios::ate);
      CHECK(file.is_open());
      const auto fileSize = static_cast< uint64_t >(file.tellg());
      file.close();

      CHECK(fileSize < 1000u);
    }

    public:
  };

  GUM_TEST(SimpleTestForWriter) {
    _simpleTextForWriter_(false);
    _simpleTextForWriter_(true);
  }

  GUM_TEST(CheckMetaData) {
    _checkMetaData_(false);
    _checkMetaData_(true);
  }

  GUM_TEST(WithBigFiles) {
    _withBigFiles_(false, -1);
    _withBigFiles_(false, 2);
    _withBigFiles_(true);
  }

  GUM_TEST(AggregatorsAndICIModelsRoundtrip) {
    _aggregatorAndICIRoundtrip_(false);
    _aggregatorAndICIRoundtrip_(true);
  }

  GUM_TEST(AggregatorCompactEncoding) { _aggregatorCompactness_(); }

  GUM_TEST(BinaryFileIntegrity) {
    // Verify the binary file layout: [8-byte LE uint64 payload size][payload].
    // If the file is opened in text mode on Windows, the CRLF translation corrupts
    // the binary payload and the size prefix no longer matches the actual data.
    auto       bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
    const auto path = GET_RESSOURCES_PATH("outputs/test_integrity.bgum");

    gum::GumBNWriter< double > writer(true);
    writer.write(path, bn);

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    CHECK(file.is_open());
    const auto fileSize = static_cast< uint64_t >(file.tellg());
    file.seekg(0);
    uint64_t payloadSize = 0;
    file.read(reinterpret_cast< char* >(&payloadSize), sizeof(payloadSize));
    file.close();

    // Exactly 8 bytes of prefix + payloadSize bytes of msgpack data
    CHECK_EQ(fileSize, payloadSize + 8u);
  }

  GUM_TEST(SingleVariable) {
    // BN with a single node and no arcs — "parents" section must still exist in JSON
    auto                       bn = gum::BayesNet< double >::fastPrototype("A{Yes|No}");
    gum::GumBNWriter< double > writer(false, 2);
    const std::string          str = writer.toString(bn);

    gum::BayesNet< double > bn2;
    auto                    reader = gum::GumBNReader< double >(&bn2);
    CHECK_EQ(reader.proceedFromString(str), 0u);
    CHECK_EQ(bn2, bn);
    CHECK_EQ(bn2.size(), 1u);
  }

  GUM_TEST(EmptyBN) {
    // BN with no nodes — "nodes", "parents", "cpt" sections must still exist in JSON
    gum::BayesNet< double >    bn;
    gum::GumBNWriter< double > writer(false, 2);
    const std::string          str = writer.toString(bn);

    gum::BayesNet< double > bn2;
    auto                    reader = gum::GumBNReader< double >(&bn2);
    CHECK_EQ(reader.proceedFromString(str), 0u);
    CHECK_EQ(bn2.size(), 0u);
  }

  GUM_TEST(ToString) {
    auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
    gum::GumBNWriter< double > writer(false, 2);
    std::string                str = writer.toString(bn);

    std::string   tempFileName = gum_tests::getTempFilePath();
    std::ofstream tempFile(tempFileName.c_str(), std::ios_base::trunc);
    tempFile << str;
    tempFile.close();

    gum::BayesNet< double > bn2;
    auto                    reader = gum::GumBNReader< double >(&bn2, tempFileName, false);
    CHECK_EQ(reader.proceed(), 0u);
    CHECK_EQ(bn2, bn);
  }
}   // namespace gum_tests
