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

#include <iostream>
#include <sstream>
#include <string>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/GUM/GumIDReader.h>
#include <agrum/ID/io/GUM/GumIDWriter.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumIDReader
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct GumIDReaderTestSuite {
    public:
    static void testBuildingIDFromJson() {
      // Read minimal.id.jgum which has: chance A[2], C[2]; utility U[1]; decision D[2]
      // parents: A->{}, C->{A}, D->{}, U->{C,D}
      const std::string               filename = GET_RESSOURCES_PATH("jsonGum/minimal.id.jgum");
      gum::InfluenceDiagram< double > id;
      gum::GumIDReader< double >      reader(&id, filename);
      reader.proceed();
      CHECK_EQ(reader.count(), 0u);

      CHECK_EQ(id.size(), 4u);
      CHECK_EQ(id.chanceNodeSize(), 2u);
      CHECK_EQ(id.utilityNodeSize(), 1u);
      CHECK_EQ(id.decisionNodeSize(), 1u);

      CHECK(id.isChanceNode(id.idFromName("A")));
      CHECK(id.isChanceNode(id.idFromName("C")));
      CHECK(id.isUtilityNode(id.idFromName("U")));
      CHECK(id.isDecisionNode(id.idFromName("D")));

      CHECK_EQ(id.variable("A").domainSize(), 2u);
      CHECK_EQ(id.variable("C").domainSize(), 2u);
      CHECK_EQ(id.variable("D").domainSize(), 2u);
      CHECK_EQ(id.variable("U").domainSize(), 1u);

      CHECK_EQ(id.parents(id.idFromName("A")).size(), 0u);
      CHECK_EQ(id.parents(id.idFromName("D")).size(), 0u);
      CHECK(id.existsArc("A", "C"));
      CHECK(id.existsArc("C", "U"));
      CHECK(id.existsArc("D", "U"));

      // CPT values for A: [0.4, 0.6]
      const auto& cptA = id.cpt("A");
      CHECK_EQ(cptA.domainSize(), 2u);
      gum::Instantiation IA(cptA);
      IA.setFirst();
      CHECK_EQ(cptA[IA], doctest::Approx(0.4));
      ++IA;
      CHECK_EQ(cptA[IA], doctest::Approx(0.6));

      // utility values for U: [10.0, -5.0, 20.0, 5.0]
      const auto& utU = id.utility("U");
      CHECK_EQ(utU.domainSize(), 4u);
      gum::Instantiation IU(utU);
      IU.setFirst();
      CHECK_EQ(utU[IU], doctest::Approx(10.0));

      // properties from file
      CHECK(id.existsProperty("software"));
      CHECK_EQ(id.property("software"), "aGrUM test");
    }

    static void testProceedWithoutFilename() {
      gum::InfluenceDiagram< double > id;
      auto reader = gum::GumIDReader< double >(&id);
      CHECK_THROWS_AS(reader.proceed(), const gum::OperationNotAllowed&);
    }

    static void testProceedFromString() {
      // A -> C -> *D -> $U, A -> $U  (chance A, chance C, decision D, utility U)
      auto id = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
      gum::GumIDWriter< double > writer(false, 2);
      const std::string          str = writer.toString(id);

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(id2, id);
    }
  };

  GUM_TEST_ACTIF(BuildingIDFromJson)
  GUM_TEST_ACTIF(ProceedWithoutFilename)
  GUM_TEST_ACTIF(ProceedFromString)
}   // namespace gum_tests
