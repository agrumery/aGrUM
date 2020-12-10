/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * (@AMU) info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class BayesNetIOTestSuite: public CxxTest::TestSuite {
    public:
    void testNetImportExport() {
      std::string filebif = GET_RESSOURCES_PATH("bif/alarm.bif");
      std::string filenet = GET_RESSOURCES_PATH("net/alarm.net");

      gum::BayesNet< double >  bifbn;
      gum::BIFReader< double > bifreader(&bifbn, filebif);
      bifreader.proceed();

      gum::BayesNet< double >  netbn;
      gum::NetReader< double > netreader(&netbn, filenet);
      netreader.proceed();

      TS_ASSERT_EQUALS(netbn.size(), bifbn.size());
      TS_ASSERT_EQUALS(netbn.sizeArcs(), bifbn.sizeArcs());
      TS_ASSERT_EQUALS(netbn.toString(), bifbn.toString());
      for (const auto n: bifbn.nodes()) {
        const gum::Potential< double > p(bifbn.cpt(n));
        p.fillWith(netbn.cpt(bifbn.variable(n).name()));
        double err = (bifbn.cpt(n) - p).abs().max();
        TS_ASSERT_LESS_THAN_EQUALS(err, 1e-6);
      }

      gum::NetWriter< double > netwriter;
      std::string netwritefile = GET_RESSOURCES_PATH("outputs/alarm_writer.net");
      TS_GUM_ASSERT_THROWS_NOTHING(netwriter.write(netwritefile, netbn));

      gum::BayesNet< double >  netbn2;
      gum::NetReader< double > netreader2(&netbn2, netwritefile);
      netreader2.proceed();

      TS_ASSERT_EQUALS(netbn.size(), netbn2.size());
      TS_ASSERT_EQUALS(netbn.sizeArcs(), netbn2.sizeArcs());
      TS_ASSERT_EQUALS(netbn.toString(), netbn2.toString());
      for (const auto n: netbn2.nodes()) {
        const gum::Potential< double > p(netbn2.cpt(n));
        p.fillWith(netbn.cpt(netbn2.variable(n).name()));
        double err = (netbn2.cpt(n) - p).abs().max();
        TS_ASSERT_LESS_THAN(err, 1e-6);
      }

      TS_ASSERT_EQUALS(bifbn.size(), netbn2.size());
      TS_ASSERT_EQUALS(bifbn.sizeArcs(), netbn2.sizeArcs());
      TS_ASSERT_EQUALS(bifbn.toString(), netbn2.toString());
      for (const auto n: netbn2.nodes()) {
        const gum::Potential< double > p(netbn2.cpt(n));
        p.fillWith(bifbn.cpt(netbn2.variable(n).name()));
        double err = (netbn2.cpt(n) - p).abs().max();
        TS_ASSERT_LESS_THAN(err, 1e-6);
      }
    }
  };
}   // namespace gum_tests
