/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <sstream>

#include <agrum/PRM/elements/PRMSystem.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/variables/labelizedVariable.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMSystemTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMSystem< double >          PRMSystem;
    typedef gum::prm::PRMInstance< double >        PRMInstance;
    typedef gum::prm::PRMClass< double >           PRMClass;
    typedef gum::prm::PRMType                      PRMType;
    typedef gum::prm::PRMScalarAttribute< double > PRMAttribute;
    typedef gum::prm::PRMReferenceSlot< double >   Reference;
    typedef gum::prm::PRMSlotChain< double >       PRMSlotChain;

    PRMType*                                    boolean__;
    PRMClass*                                   asia__;
    gum::HashTable< std::string, gum::NodeId >* nodeMap__;

    public:
    void setUp() {
      boolean__ = PRMType::boolean();
      nodeMap__ = new gum::HashTable< std::string, gum::NodeId >();
      asia__ = new PRMClass("asia");
      buildAsiaBN__();
    }

    void tearDown() {
      delete boolean__;
      delete nodeMap__;
      delete asia__;
    }

    void buildAsiaBN__() {
      visitToAsia__();
      tuberculosis__();
      smoking__();
      lungCancer__();
      bronchitis__();
      tubOrCancer__();
      positiveXRay__();
      dyspnea__();
    }

    void visitToAsia__() {
      std::string           name = "visitToAsia";
      auto                  attr = new PRMAttribute(name, *boolean__);
      auto                  id = asia__->add(attr);
      std::vector< double > values{0.99, 0.01};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void tuberculosis__() {
      std::string name = "tuberculosis";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("visitToAsia", "tuberculosis");
      std::vector< double > values{0.99, 0.95, 0.01, 0.05};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void smoking__() {
      std::string           name = "smoking";
      auto                  attr = new PRMAttribute(name, *boolean__);
      auto                  id = asia__->add(attr);
      std::vector< double > values{0.50, 0.50};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void lungCancer__() {
      std::string name = "lungCancer";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("smoking", "lungCancer");
      std::vector< double > values{0.99, 0.90, 0.01, 0.10};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void bronchitis__() {
      std::string name = "bronchitis";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("smoking", "bronchitis");
      std::vector< double > values{0.70, 0.40, 0.30, 0.60};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void tubOrCancer__() {
      std::string name = "tubOrCancer";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("tuberculosis", "tubOrCancer");
      asia__->addArc("lungCancer", "tubOrCancer");
      std::vector< double > values{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void positiveXRay__() {
      std::string name = "positiveXRay";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("tubOrCancer", "positiveXRay");
      std::vector< double > values{0.95, 0.02, 0.05, 0.98};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void dyspnea__() {
      std::string name = "dyspnea";
      auto        attr = new PRMAttribute(name, *boolean__);
      auto        id = asia__->add(attr);
      asia__->addArc("tubOrCancer", "dyspnea");
      asia__->addArc("bronchitis", "dyspnea");
      std::vector< double > values{0.9, 0.2, 0.3, 0.1, 0.1, 0.8, 0.7, 0.9};
      attr->cpf().fillWith(values);
      nodeMap__->insert(name, id);
    }

    void testClassConstruction() {
      TS_ASSERT_EQUALS(asia__->attributes().size(), (gum::Size)8);
    }

    void testAddInstance() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      // Act
      TS_ASSERT_THROWS_NOTHING(sys.add(inst));
      // Assert
      TS_ASSERT(sys.exists("asia"));
      TS_ASSERT(sys.isInstantiated(*asia__));
      TS_ASSERT_EQUALS(sys.size(), (gum::Size)1);
    }

    void testInstantiate() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      // Act
      TS_ASSERT_THROWS_NOTHING(sys.instantiate());
      // Assert
      TS_ASSERT(sys.exists("asia"));
      TS_ASSERT(sys.isInstantiated(*asia__));
      TS_ASSERT_EQUALS(sys.size(), (gum::Size)1);
    }

    void testGroundBN() {
      // Arrange
      std::string x0, y0, x1, y1;
      auto        bn = new gum::BayesNet< double >("asia");
      {
        PRMSystem sys("asia");
        auto      inst = new PRMInstance("asia", *asia__);
        sys.add(inst);
        sys.instantiate();
        gum::BayesNetFactory< double > factory(bn);
        // Act
        TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory));
        x0 = bn->cpt(0).toString();
        y0 = bn->cpt(1).toString();
      }
      // Assert
      TS_ASSERT_EQUALS(bn->size(), (gum::Size)8);
      TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8);
      x1 = bn->cpt(0).toString();
      y1 = bn->cpt(1).toString();
      TS_ASSERT_EQUALS(x0, x1);
      TS_ASSERT_EQUALS(y0, y1);
      delete bn;
    }

    void testGroundBNAfterDelete() {
      // Arrange
      PRMSystem* sys = new PRMSystem("asia");
      auto       inst = new PRMInstance("asia", *asia__);
      sys->add(inst);
      sys->instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys->groundedBN(factory);
      // Act
      TS_ASSERT_THROWS_NOTHING(delete sys);
      // Assert
      TS_ASSERT_EQUALS(bn->size(), (gum::Size)8);
      TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8);
      for (auto node: bn->dag()) {
        const gum::Potential< double >* cpt = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(node)));
        gum::Instantiation inst(*cpt);
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          TS_GUM_ASSERT_THROWS_NOTHING(cpt->get(inst));
        }
      }
      delete bn;
    }

    void testVisitToAsiaId() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      // Act
      TS_ASSERT_THROWS_NOTHING(bn->idFromName("asia.(boolean)visitToAsia"));
      // Assert
      delete bn;
    }

    void testVisitToAsia() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)visitToAsia");
      std::vector< double > values;
      const auto&           cpf = asia__->get("visitToAsia").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testTuberculosis() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)tuberculosis");
      std::vector< double > values;
      const auto&           cpf = asia__->get("tuberculosis").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testSmoking() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                            id = bn->idFromName("asia.(boolean)smoking");
      std::vector< double >           values;
      const auto&                     cpf = asia__->get("smoking").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testLungCancer() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)lungCancer");
      std::vector< double > values;
      const auto&           cpf = asia__->get("lungCancer").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testBronchitis() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)bronchitis");
      std::vector< double > values;
      const auto&           cpf = asia__->get("bronchitis").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testTubOrCancer() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)tubOrCancer");
      std::vector< double > values;
      const auto&           cpf = asia__->get("tubOrCancer").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testPositiveXRay() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                  id = bn->idFromName("asia.(boolean)positiveXRay");
      std::vector< double > values;
      const auto&           cpf = asia__->get("positiveXRay").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }

    void testDyspnea() {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *asia__);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                            id = bn->idFromName("asia.(boolean)dyspnea");
      std::vector< double >           values;
      const auto&                     cpf = asia__->get("dyspnea").cpf();
      gum::Potential< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)));
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j));
      }
      delete bn;
    }
  };

}   // namespace gum_tests
