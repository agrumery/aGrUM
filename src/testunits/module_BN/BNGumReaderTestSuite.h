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
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/GUM/BNGumReader.h>

#include <agrum/base/external/json/json.hpp>

using json = nlohmann::json;

namespace gum_tests {
  class [[maybe_unused]] BNGumReaderTestSuite: public CxxTest::TestSuite {
    const std::string jsondemo{R"(
{
  "nodes": [
    "A{yes|maybe|no}",
    "B[3,7]",
    "C[1.3,1.6,1.9,2.2,2.5]"
  ],
  "parents": {
    "A": [],
    "B": [
      "A",
      "C"
    ],
    "C": []
  },
  "cpt": {
    "A": [
      0.16257016307560487,
      0.22687476475247673,
      0.6105550721719185
    ],
    "B": [
      0.041281044698441745,
      0.15376048783477345,
      0.07692369182819567,
      0.356479962530545,
      0.3715548131080441,
      0.18589589055522782,
      0.2535589869525804,
      0.002668205696859853,
      0.25721770159130114,
      0.30065921520403077,
      0.024036035214958177,
      0.32247730515555506,
      0.1017324908413069,
      0.22054999277867035,
      0.3312041760095095,
      0.4117915588529451,
      0.12058045085755388,
      0.25265408228264985,
      0.05203968133013523,
      0.1629342266767159,
      0.20695693141435648,
      0.25726132473636226,
      0.26045352267605865,
      0.2560154733925188,
      0.01931274778070379,
      0.1823103393882961,
      0.36097761346630014,
      0.3733680644165876,
      0.033481293282911294,
      0.04986268944590483,
      0.2437076012575784,
      0.3109829855854673,
      0.2911149304701915,
      0.1422395085391709,
      0.011954974147592038,
      0.312799274766756,
      0.08781501619244818,
      0.3240593985895851,
      0.19367126238120058,
      0.08165504807000994,
      0.1532241482498576,
      0.35990624937523624,
      0.14418984513887345,
      0.154035714173722,
      0.1886440430623107,
      0.40369382696180434,
      0.17167775443901043,
      0.0014889575074050003,
      0.12925744673829392,
      0.2938820143534863,
      0.1957650015464493,
      0.26461026731115606,
      0.2763714355265347,
      0.03221617579304379,
      0.23103711982281613,
      0.20487169081258955,
      0.35729218755313674,
      0.10799371649568228,
      0.16630585336342132,
      0.16353655177517026
    ],
    "C": [
      0.018974912112612926,
      0.032630375972440354,
      0.3152219272008065,
      0.6331727847141403
    ]
  }
})"};

    /*,
      "properties": {
        "creation": "2025-07-29 09:31:09",
        "lastModification": "2025-07-29 09:31:09",
        "version": "aGrUM 2.2.0"
      }*/

    public:
    GUM_ACTIVE_TEST(FirstTest) {
      const auto ref = json::parse(jsondemo);
      const auto k   = json::parse(ref.dump());
      TS_ASSERT_EQUALS(ref, k)
    }

    GUM_ACTIVE_TEST(getNames) {
      const auto  ref = json::parse(jsondemo);
      const auto& k   = ref["nodes"];
      TS_ASSERT_EQUALS(k.size(), 3u);
      TS_ASSERT_EQUALS(k[0], "A{yes|maybe|no}");
      TS_ASSERT_EQUALS(k[1], "B[3,7]");
      TS_ASSERT_EQUALS(k[2], "C[1.3,1.6,1.9,2.2,2.5]");
    }

    GUM_ACTIVE_TEST(getParents) {
      const auto  ref = json::parse(jsondemo);
      const auto& k   = ref["parents"];
      TS_ASSERT_EQUALS(k.size(), 3u);
      TS_ASSERT_EQUALS(k["A"].size(), 0u);
      TS_ASSERT_EQUALS(k["B"].size(), 2u);
      TS_ASSERT_EQUALS(k["B"][0], "A");
      TS_ASSERT_EQUALS(k["B"][1], "C");
      TS_ASSERT_EQUALS(k["C"].size(), 0u);
    }

    GUM_ACTIVE_TEST(getCPTs) {
      const auto  ref = json::parse(jsondemo);
      const auto& k   = ref["cpt"];
      TS_ASSERT_EQUALS(k.size(), 3u);
      TS_ASSERT_EQUALS(k["A"].size(), 3u);
      TS_ASSERT_EQUALS(k["A"][0], 0.16257016307560487);
      TS_ASSERT_EQUALS(k["B"].size(), 60u);
      TS_ASSERT_EQUALS(k["B"][59], 0.16353655177517026);
      TS_ASSERT_EQUALS(k["C"].size(), 4u);
      TS_ASSERT_EQUALS(k["C"][0], 0.018974912112612926);
    }

    GUM_ACTIVE_TEST(testExistence) {
      const auto ref = json::parse(jsondemo);
      TS_ASSERT(ref.contains("nodes"));
      TS_ASSERT(ref.contains("parents"));
      TS_ASSERT(ref.contains("cpt"));
      TS_ASSERT(!ref.contains("utility"));
    }

    GUM_ACTIVE_TEST(BuildingBayesNetFromJson) {
      const auto              ref = json::parse(jsondemo);
      gum::BayesNet< double > bn;
      // iterate on nodes in json
      for (const auto& node: ref["nodes"]) {
        const auto n = bn.add(node.get< std::string >());
      }
      // iterate on parents in json
      for (const auto& parent: ref["parents"].items()) {
        const auto& nodeName = parent.key();
        for (const auto& p: parent.value()) {
          const auto& pName = p.get< std::string >();
          bn.addArc(pName, nodeName);
        }
      }
      // iterate on cpt in json
      for (const auto& cpt: ref["cpt"].items()) {
        const auto& nodeName = cpt.key();
        const auto& values   = cpt.value();
        bn.cpt(nodeName).fillWith(values.get< std::vector< double > >());
      }

      const std::string       filename = GET_RESSOURCES_PATH("jsonGum/minimal.gum");
      gum::BayesNet< double > bn2;

      auto reader = gum::BNGumReader< double >(&bn2, filename);

      TS_ASSERT_EQUALS(reader.proceed(), 0u);
      TS_ASSERT_EQUALS(bn2.size(), 3u);
      TS_ASSERT_EQUALS(bn2.variable(0).toFast(), "A{yes|maybe|no}");
      TS_ASSERT_EQUALS(bn2.variable(1).toFast(), "B[3,7]");
      TS_ASSERT_EQUALS(bn2.variable(2).toFast(), "C[1.3,1.6,1.9,2.2,2.5]");
      TS_ASSERT_EQUALS(bn2.parents(0).size(), 0u);
      TS_ASSERT_EQUALS(bn2.parents(1).size(), 2u);
      TS_ASSERT_EQUALS(bn2.parents(2).size(), 0u);
      TS_ASSERT(bn2.existsArc("A", "B"));
      TS_ASSERT(bn2.existsArc("C", "B"));

      TS_ASSERT_EQUALS(bn2, bn);

      TS_ASSERT_EQUALS(bn.properties().size(), 0u);
      TS_ASSERT_EQUALS(bn2.properties().size(), 3u);
    }
  };
}   // namespace gum_tests
