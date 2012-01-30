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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/VEWithBB.h>
// ============================================================================
#include <agrum/prm/instanceBayesNet.h>
#include <agrum/prm/classBayesNet.h>
#include <agrum/prm/groundedInference.h>
#include <agrum/prm/SVE.h>
#include <agrum/prm/SVED.h>
#include <agrum/prm/structuredBayesBall.h>
#include <agrum/prm/structuredInference.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

#include "testsuite_utils.h"
namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class PRMTestSuite: public CxxTest::TestSuite {
  private:
    PRM* prm;
    PRM* small;

  public:
    void setUp() {
      {
        SkoolReader reader;
        reader.readFile(GET_PATH_STR(skool/inference.skool));
        prm = reader.prm();
      }
      {
        SkoolReader reader;
        reader.readFile(GET_PATH_STR(skool/printers_systems.skool));
        small = reader.prm();
      }
    }

    void tearDown() {
      delete prm;
      delete small;
    }

    void testCreation() {
      ClassBayesNet* c = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(c = new ClassBayesNet(prm->getClass("SafeComputer")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete c);
      InstanceBayesNet* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new InstanceBayesNet(prm->system("aSys").get("c1")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete inst);
    }

    void testClassAccess() {
      Class& c = prm->getClass("SafeComputer");
      ClassBayesNet* bn = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new ClassBayesNet(prm->getClass("SafeComputer")));
      Size elts = c.attributes().size() + c.aggregates().size();
      TS_ASSERT_EQUALS(bn->size(), elts);
      for (Set<Attribute*>::iterator attr = c.attributes().begin(); attr != c.attributes().end(); ++attr) {
        NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
      }
      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testInstanceAccess() {
      InstanceBayesNet* bn = 0;
      Instance& i = prm->system("aSys").get("c1");
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new InstanceBayesNet(i));
      TS_ASSERT_EQUALS(bn->size(), i.size());
      for (Instance::iterator attr = i.begin(); attr != i.end(); ++attr) {
        NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
      }
      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testGroundedBN() {
      System& sys = prm->system("aSys");
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
      int count = 0;
      for (System::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
        for (Instance::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
          ++count;
        }
      }
      int wount = 0;
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        wount++;
        std::string var = bn.variable(*node).name();
        size_t pos = var.find_first_of('.');
        Instance& instance = sys.get(var.substr(0, pos));
        Attribute& attr = instance.get(var.substr(pos +1));
        TS_ASSERT_DIFFERS(bn.cpt(*node).nbrDim(), (Size)0);
        if (ClassElement::isAggregate(instance.type().get(attr.id()))) {
          TS_ASSERT_DIFFERS(attr.cpf().nbrDim(), (Size)1);
        }
      }
      TS_ASSERT_EQUALS(count, wount);
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        const DiscreteVariable* var = &(bn.variable(*node));
        for (DAG::NodeIterator mode = bn.dag().beginNodes(); mode != bn.dag().endNodes(); ++mode) {
          if ((*node) != (*mode)) {
            TS_ASSERT_DIFFERS(var, &(bn.variable(*mode)));
          }
        }
      }
    }

    void testCPF() {
      System& sys = prm->system("aSys");
      for (System::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
        for (Instance::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
          Instantiation i((**jter).cpf()), var;
          var.add((**jter).type().variable());
          for (i.setFirstOut(var); not i.end(); i.incOut(var)) {
            prm_float f = 0.0;
            for (i.setFirstIn(var); not i.end(); i.incIn(var)) {
              f += (**jter).cpf().get(i);
            }
            TS_ASSERT_DELTA(f, 1.0, 1e-9);
            i.unsetOverflow();
          }
        }
      }
    }

    void testNormalisedCPT() {
      System& sys = prm->system("aSys");
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        const Potential<prm_float>& cpt = bn.cpt(*node);
        Instantiation i(cpt), j;
        j.add(bn.variable(*node));
        for (i.setFirstOut(j); not i.end(); i.incOut(j)) {
          prm_float sum = 0.0;
          for (i.setFirstIn(j); not i.end(); i.incIn(j))
            sum += cpt.get(i);
          TS_ASSERT_DELTA(sum, 1.0, 1e-9);
          i.unsetOverflow();
        }
      }
    }
};

} // namespace tests
} // namespace gum
// ============================================================================
