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
#include <agrum/BN/inference/valueElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
// ============================================================================
#include <agrum/prm/instanceBayesNet.h>
#include <agrum/prm/classBayesNet.h>
#include <agrum/prm/groundedInference.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class GroundedBNTestSuite: public CxxTest::TestSuite {
  private:
    PRM* prm;

  public:
    void setUp() {
      SkoolReader reader;
      reader.readFile("../../../src/testunits/ressources/skool/inference.skool");
      prm = reader.prm();
      //std::cerr << std::endl;
    }

    void tearDown() {
      delete prm;
    }

//    void testCreation() {
//      ClassBayesNet* c = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(c = new ClassBayesNet(prm->getClass("SafeComputer")));
//      TS_GUM_ASSERT_THROWS_NOTHING(delete c);
//      InstanceBayesNet* inst = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(inst = new InstanceBayesNet(prm->getSystem("aSys").get("c1")));
//      TS_GUM_ASSERT_THROWS_NOTHING(delete inst);
//      SystemBayesNet* sys = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(sys = new SystemBayesNet(prm->getSystem("aSys")));
//      TS_GUM_ASSERT_THROWS_NOTHING(delete sys);
//    }
//
//    void testClassAccess() {
//      Class& c = prm->getClass("SafeComputer");
//      ClassBayesNet* bn = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(bn = new ClassBayesNet(prm->getClass("SafeComputer")));
//      Size elts = c.attributes().size() + c.aggregates().size();
//      TS_ASSERT_EQUALS(bn->size(), elts);
//      for (Set<Attribute*>::iterator attr = c.attributes().begin(); attr != c.attributes().end(); ++attr) {
//        NodeId id = 0;
//        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
//        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
//        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
//        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
//        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
//      }
//      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
//    }
//
//    void testInstanceAccess() {
//      InstanceBayesNet* bn = 0;
//      Instance& i = prm->getSystem("aSys").get("c1");
//      TS_GUM_ASSERT_THROWS_NOTHING(bn = new InstanceBayesNet(i));
//      TS_ASSERT_EQUALS(bn->size(), i.size());
//      for (Instance::iterator attr = i.begin(); attr != i.end(); ++attr) {
//        NodeId id = 0;
//        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
//        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
//        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
//        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
//        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
//      }
//      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
//    }
//
//    void testSystemAccess() {
//      SystemBayesNet* bn = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(bn = new SystemBayesNet(prm->getSystem("aSys")));
//      System& sys = prm->getSystem("aSys");
//      for (DAG::NodeIterator node = bn->dag().beginNodes(); node != bn->dag().endNodes(); ++node) {
//        std::string name = bn->variableNodeMap().name(*node);
//        Attribute* attr = 0;
//        // GUM_TRACE_VAR(name.substr(0, name.find_first_of('.')));
//        // GUM_TRACE_VAR(name.substr(name.find_first_of('.') + 1));
//        TS_GUM_ASSERT_THROWS_NOTHING(attr = &(sys.get(name.substr(0, name.find_first_of('.'))).get(name.substr(name.find_first_of('.') + 1))));
//        TS_GUM_ASSERT_THROWS_NOTHING(attr->cpf());
//        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(*node));
//        TS_ASSERT_EQUALS(attr->cpf().nbrDim(), bn->cpt(*node).nbrDim());
//        TS_ASSERT_EQUALS(attr->cpf().domainSize(), bn->cpt(*node).domainSize());
//      }
//      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
//    }
//
//    void testInferenceCreation() {
//      GroundedInference* inf = 0;
//      ValueElimination<prm_float>* lazy = 0;
//      SystemBayesNet* bn = 0;
//      TS_GUM_ASSERT_THROWS_NOTHING(bn = new SystemBayesNet(prm->getSystem("aSys")));
//      TS_GUM_ASSERT_THROWS_NOTHING(lazy = new ValueElimination<prm_float>(*bn));
//      TS_GUM_ASSERT_THROWS_NOTHING(inf = new GroundedInference(*prm, prm->getSystem("aSys")));
//      TS_GUM_ASSERT_THROWS_NOTHING(inf->setBNInference(lazy));
//      delete inf;
//      delete bn;
//    }

    void testGroundedBN() {
      System& sys = prm->getSystem("aSys");
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
        if (bn.cpt(*node).nbrDim() == 0) {
          std::stringstream sBuff;
          sBuff << instance.name() << "." << attr.safeName();
          GUM_TRACE(sBuff.str());
        }
        if (ClassElement::isAggregate(instance.type().get(attr.id()))) {
          if (attr.cpf().nbrDim() == 1) {
            std::stringstream sBuff;
            sBuff << instance.name() << "." << attr.safeName();
            GUM_TRACE(sBuff.str());
          }
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

    // void testInference() {
    //   GroundedInference* inf = 0;
    //   LazyPropagation<prm_float>* lazy = 0;
    //   BayesNet<prm_float> bn;
    //   BayesNetFactory<prm_float> bn_factory(&bn);
    //   TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
    //   TS_GUM_ASSERT_THROWS_NOTHING(lazy = new LazyPropagation<prm_float>(bn));
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf = new GroundedInference(*prm, prm->getSystem("aSys")));
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf->setBNInference(lazy));
    //   for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
    //     Potential<prm_float> m;
    //     try {
    //       inf->marginal(bn.variableNodeMap().name(*node), m);
    //       //std::string name = "c1.(boolean)working_printer";
    //       //(inf->marginal(name, m));
    //     } catch (Exception& e) {
    //       TS_GUM_ASSERT_THROWS_NOTHING(throw e);
    //       break;
    //     }
    //   }
    //   delete inf;
    // }

};

} // namespace tests
} // namespace gum
// ============================================================================
