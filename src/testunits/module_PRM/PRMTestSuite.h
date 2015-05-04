/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/VEWithBB.h>

#include <agrum/PRM/instanceBayesNet.h>
#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/inference/SVED.h>
#include <agrum/PRM/inference/structuredBayesBall.h>
#include <agrum/PRM/inference/structuredInference.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

#define GET_PATH_STR_O3PRM(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/o3prm/" #x
namespace gum_tests {

  class PRMTestSuite : public CxxTest::TestSuite {
    private:
    gum::prm::PRM<double> *prm;
    gum::prm::PRM<double> *small;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile(GET_PATH_STR_O3PRM(inference.o3prm));
        prm = reader.prm();
      }
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile(GET_PATH_STR_O3PRM(printers_systems.o3prm));
        small = reader.prm();
      }
    }

    void tearDown() {
      delete prm;
      delete small;
    }

    void testCreation() {
      gum::prm::ClassBayesNet<double> *c = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("SafeComputer"));
      TS_GUM_ASSERT_THROWS_NOTHING(
          c = new gum::prm::ClassBayesNet<double>(prm->getClass("SafeComputer")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete c);
      gum::prm::InstanceBayesNet<double> *inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::prm::InstanceBayesNet<double>(
                                       prm->system("aSys").get("c1")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete inst);
    }

    void testClassAccess() {
      gum::prm::Class<double> &c = prm->getClass("SafeComputer");
      gum::prm::ClassBayesNet<double> *bn = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn = new gum::prm::ClassBayesNet<double>(prm->getClass("SafeComputer")));
      gum::Size elts = c.attributes().size() + c.aggregates().size();
      TS_ASSERT_EQUALS(bn->size(), elts);

      for (const auto attr : c.attributes()) {
        gum::NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(attr->cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName(attr->safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS(attr->cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS(attr->cpf().domainSize(), bn->cpt(id).domainSize());
      }

      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testInstanceAccess() {
      gum::prm::InstanceBayesNet<double> *bn = 0;
      gum::prm::Instance<double> &i = prm->system("aSys").get("c1");
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new gum::prm::InstanceBayesNet<double>(i));
      TS_ASSERT_EQUALS(bn->size(), i.size());

      for (auto attr = i.begin(); attr != i.end(); ++attr) {
        gum::NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((*(attr.val())).cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id =
                                         bn->idFromName((*(attr.val())).safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS((*(attr.val())).cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS((*(attr.val())).cpf().domainSize(),
                         bn->cpt(id).domainSize());
      }

      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testGroundedBN() {
      gum::prm::System<double> &sys = prm->system("aSys");
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
      int count = 0;

      for (auto iter = sys.begin(); iter != sys.end(); ++iter) {
        for (auto jter = (*(iter.val())).begin(); jter != (*(iter.val())).end();
             ++jter) {
          ++count;
        }
      }

      int wount = 0;

      for (const auto node : bn.nodes()) {
        wount++;
        std::string var = bn.variable(node).name();
        size_t pos = var.find_first_of('.');
        gum::prm::Instance<double> &instance = sys.get(var.substr(0, pos));
        gum::prm::Attribute<double> &attr = instance.get(var.substr(pos + 1));
        TS_ASSERT_DIFFERS(bn.cpt(node).nbrDim(), (gum::Size)0);

        if (gum::prm::ClassElement<double>::isAggregate(
                instance.type().get(attr.id()))) {
          TS_ASSERT_DIFFERS(attr.cpf().nbrDim(), (gum::Size)1);
        }
      }

      TS_ASSERT_EQUALS(count, wount);

      for (const auto node : bn.nodes()) {
        const gum::DiscreteVariable *var = &(bn.variable(node));

        for (const auto node2 : bn.nodes())
          if (node != node2) {
            TS_ASSERT_DIFFERS(var, &(bn.variable(node2)));
          }
      }
    }

    void testCPF() {
      gum::prm::System<double> &sys = prm->system("aSys");

      for (gum::prm::System<double>::iterator iter = sys.begin(); iter != sys.end();
           ++iter) {
        for (gum::prm::Instance<double>::iterator jter = (*(iter.val())).begin();
             jter != (*(iter.val())).end(); ++jter) {
          gum::Instantiation i((*(jter.val())).cpf()), var;
          var.add((*(jter.val())).type().variable());

          for (i.setFirstOut(var); not i.end(); i.incOut(var)) {
            double f = 0.0;

            for (i.setFirstIn(var); not i.end(); i.incIn(var)) {
              f += (*(jter.val())).cpf().get(i);
            }

            TS_ASSERT_DELTA(f, 1.0, 1e-7);
            i.unsetOverflow();
          }
        }
      }
    }

    void testNormalisedCPT() {
      gum::prm::System<double> &sys = prm->system("aSys");
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));

      for (const auto node : bn.nodes()) {
        const gum::Potential<double> &cpt = bn.cpt(node);
        gum::Instantiation i(cpt), j;
        j.add(bn.variable(node));

        for (i.setFirstOut(j); not i.end(); i.incOut(j)) {
          double sum = 0.0;

          for (i.setFirstIn(j); not i.end(); i.incIn(j))
            sum += cpt.get(i);

          TS_ASSERT_DELTA(sum, 1.0, 1e-7);
          i.unsetOverflow();
        }
      }
    }
  };

} // namespace gum_tests
