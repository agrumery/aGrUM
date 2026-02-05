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
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesNetIO
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct BayesNetIOTestSuite {
    public:
    static void testNetImportExport() {
      std::string filebif = GET_RESSOURCES_PATH("bif/alarm.bif");
      std::string filenet = GET_RESSOURCES_PATH("net/alarm.net");

      gum::BayesNet< double >  bifbn;
      gum::BIFReader< double > bifreader(&bifbn, filebif);
      bifreader.proceed();

      gum::BayesNet< double >  netbn;
      gum::NetReader< double > netreader(&netbn, filenet);
      netreader.proceed();

      CHECK((netbn.size()) == (bifbn.size()));
      CHECK((netbn.sizeArcs()) == (bifbn.sizeArcs()));
      CHECK((netbn.toString()) == (bifbn.toString()));
      for (const auto n: bifbn.nodes()) {
        const gum::Tensor< double > p(bifbn.cpt(n));
        p.fillWith(netbn.cpt(bifbn.variable(n).name()));
        double err = (bifbn.cpt(n) - p).abs().max();
        CHECK((err) <= (1e-6));
      }

      gum::NetWriter< double > netwriter;
      std::string              netwritefile = GET_RESSOURCES_PATH("outputs/alarm_writer.net");
      GUM_CHECK_ASSERT_THROWS_NOTHING(netwriter.write(netwritefile, netbn));

      gum::BayesNet< double >  netbn2;
      gum::NetReader< double > netreader2(&netbn2, netwritefile);
      netreader2.proceed();

      CHECK((netbn.size()) == (netbn2.size()));
      CHECK((netbn.sizeArcs()) == (netbn2.sizeArcs()));
      CHECK((netbn.toString()) == (netbn2.toString()));
      for (const auto n: netbn2.nodes()) {
        const gum::Tensor< double > p(netbn2.cpt(n));
        p.fillWith(netbn.cpt(netbn2.variable(n).name()));
        double err = (netbn2.cpt(n) - p).abs().max();
        CHECK((err) < (1e-6));
      }

      CHECK((bifbn.size()) == (netbn2.size()));
      CHECK((bifbn.sizeArcs()) == (netbn2.sizeArcs()));
      CHECK((bifbn.toString()) == (netbn2.toString()));
      for (const auto n: netbn2.nodes()) {
        const gum::Tensor< double > p(netbn2.cpt(n));
        p.fillWith(bifbn.cpt(netbn2.variable(n).name()));
        double err = (netbn2.cpt(n) - p).abs().max();
        CHECK((err) < (1e-6));
      }
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(NetImportExport)
}   // namespace gum_tests
