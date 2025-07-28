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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/hashTable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMSystem.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class [[maybe_unused]] PRMSystemTestSuite: public CxxTest::TestSuite {
    private:
    using PRMSystem    = gum::prm::PRMSystem< double >;
    using PRMInstance  = gum::prm::PRMInstance< double >;
    using PRMClass     = gum::prm::PRMClass< double >;
    using PRMType      = gum::prm::PRMType;
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;
    using Reference    = gum::prm::PRMReferenceSlot< double >;
    using PRMSlotChain = gum::prm::PRMSlotChain< double >;

    PRMType*                                    _boolean_;
    PRMClass*                                   _asia_;
    gum::HashTable< std::string, gum::NodeId >* _nodeMap_;

    public:
    void setUp() {
      _boolean_ = PRMType::boolean();
      _nodeMap_ = new gum::HashTable< std::string, gum::NodeId >();
      _asia_    = new PRMClass("asia");
      _buildAsiaBN_();
    }

    void tearDown() {
      delete _boolean_;
      delete _nodeMap_;
      delete _asia_;
    }

    void _buildAsiaBN_() {
      _visitToAsia_();
      _tuberculosis_();
      _smoking_();
      _lungCancer_();
      _bronchitis_();
      _tubOrCancer_();
      _positiveXRay_();
      _dyspnea_();
    }

    void _visitToAsia_() {
      std::string           name = "visitToAsia";
      auto                  attr = new PRMAttribute(name, *_boolean_);
      auto                  id   = _asia_->add(attr);
      std::vector< double > values{0.99, 0.01};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _tuberculosis_() {
      std::string name = "tuberculosis";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("visitToAsia", "tuberculosis");
      std::vector< double > values{0.99, 0.95, 0.01, 0.05};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _smoking_() {
      std::string           name = "smoking";
      auto                  attr = new PRMAttribute(name, *_boolean_);
      auto                  id   = _asia_->add(attr);
      std::vector< double > values{0.50, 0.50};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _lungCancer_() {
      std::string name = "lungCancer";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("smoking", "lungCancer");
      std::vector< double > values{0.99, 0.90, 0.01, 0.10};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _bronchitis_() {
      std::string name = "bronchitis";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("smoking", "bronchitis");
      std::vector< double > values{0.70, 0.40, 0.30, 0.60};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _tubOrCancer_() {
      std::string name = "tubOrCancer";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("tuberculosis", "tubOrCancer");
      _asia_->addArc("lungCancer", "tubOrCancer");
      std::vector< double > values{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _positiveXRay_() {
      std::string name = "positiveXRay";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("tubOrCancer", "positiveXRay");
      std::vector< double > values{0.95, 0.02, 0.05, 0.98};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    void _dyspnea_() {
      std::string name = "dyspnea";
      auto        attr = new PRMAttribute(name, *_boolean_);
      auto        id   = _asia_->add(attr);
      _asia_->addArc("tubOrCancer", "dyspnea");
      _asia_->addArc("bronchitis", "dyspnea");
      std::vector< double > values{0.9, 0.2, 0.3, 0.1, 0.1, 0.8, 0.7, 0.9};
      attr->cpf().fillWith(values);
      _nodeMap_->insert(name, id);
    }

    GUM_ACTIVE_TEST(ClassConstruction) {
      TS_ASSERT_EQUALS(_asia_->attributes().size(), (gum::Size)8)
    }

    GUM_ACTIVE_TEST(AddInstance) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      // Act
      TS_ASSERT_THROWS_NOTHING(sys.add(inst))
      // Assert
      TS_ASSERT(sys.exists("asia"))
      TS_ASSERT(sys.isInstantiated(*_asia_))
      TS_ASSERT_EQUALS(sys.size(), (gum::Size)1)
    }

    GUM_ACTIVE_TEST(Instantiate) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      // Act
      TS_ASSERT_THROWS_NOTHING(sys.instantiate())
      // Assert
      TS_ASSERT(sys.exists("asia"))
      TS_ASSERT(sys.isInstantiated(*_asia_))
      TS_ASSERT_EQUALS(sys.size(), (gum::Size)1)
    }

    GUM_ACTIVE_TEST(GroundBN) {
      // Arrange
      std::string x0, y0, x1, y1;
      auto        bn = new gum::BayesNet< double >("asia");
      {
        PRMSystem sys("asia");
        auto      inst = new PRMInstance("asia", *_asia_);
        sys.add(inst);
        sys.instantiate();
        gum::BayesNetFactory< double > factory(bn);
        // Act
        TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory))
        x0 = bn->cpt(0).toString();
        y0 = bn->cpt(1).toString();
      }
      // Assert
      TS_ASSERT_EQUALS(bn->size(), (gum::Size)8)
      TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8)
      x1 = bn->cpt(0).toString();
      y1 = bn->cpt(1).toString();
      TS_ASSERT_EQUALS(x0, x1)
      TS_ASSERT_EQUALS(y0, y1)
      delete bn;
    }

    GUM_ACTIVE_TEST(GroundBNAfterDelete) {
      // Arrange
      PRMSystem* sys  = new PRMSystem("asia");
      auto       inst = new PRMInstance("asia", *_asia_);
      sys->add(inst);
      sys->instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys->groundedBN(factory);
      // Act
      TS_ASSERT_THROWS_NOTHING(delete sys)
      // Assert
      TS_ASSERT_EQUALS(bn->size(), (gum::Size)8)
      TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8)
      for (auto node: bn->dag()) {
        const gum::Tensor< double >* cpt = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(node)))
        gum::Instantiation inst(*cpt);
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          TS_GUM_ASSERT_THROWS_NOTHING(cpt->get(inst))
        }
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(VisitToAsiaId) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      // Act
      TS_ASSERT_THROWS_NOTHING(bn->idFromName("asia.(boolean)visitToAsia"))
      // Assert
      delete bn;
    }

    GUM_ACTIVE_TEST(VisitToAsia) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)visitToAsia");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("visitToAsia").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(Tuberculosis) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)tuberculosis");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("tuberculosis").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(Smoking) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)smoking");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("smoking").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(LungCancer) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)lungCancer");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("lungCancer").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(Bronchitis) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)bronchitis");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("bronchitis").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(TubOrCancer) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)tubOrCancer");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("tubOrCancer").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(PositiveXRay) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)positiveXRay");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("positiveXRay").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }

    GUM_ACTIVE_TEST(Dyspnea) {
      // Arrange
      PRMSystem sys("asia");
      auto      inst = new PRMInstance("asia", *_asia_);
      sys.add(inst);
      sys.instantiate();
      auto                           bn = new gum::BayesNet< double >("asia");
      gum::BayesNetFactory< double > factory(bn);
      sys.groundedBN(factory);
      auto                         id = bn->idFromName("asia.(boolean)dyspnea");
      std::vector< double >        values;
      const auto&                  cpf = _asia_->get("dyspnea").cpf();
      gum::Tensor< double > const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cpt = &(bn->cpt(id)))
      // Assert
      gum::Instantiation i(cpf);
      gum::Instantiation j(cpt);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(cpf.get(i), cpt->get(j))
      }
      delete bn;
    }
  };

}   // namespace gum_tests
