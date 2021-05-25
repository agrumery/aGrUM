/**
 *
 *   Copyright 2005-2019 Pierre-Henri WUILLEMIN & Christophe GONZALES(@AMU)
 *   {prenom.nom}_at_lip6.fr
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
#include <ctime>
// #include <experimental/filesystem>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/BN/inference/tools/aggregatorDecomposition.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/tools/variables/discretizedVariable.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum_tests {

  class AggregatorDecompositionTestSuite: public CxxTest::TestSuite {
    private:
    gum::prm::PRM< double >*                _prm_;
    float                                   _epsilon_{1e-6f};
    gum::BayesNet< double >*                _bn_;
    gum::BayesNet< double >*                _bn2_;
    gum::LazyPropagation< double >*         _inf_;
    gum::LazyPropagation< double >*         _inf2_;
    gum::AggregatorDecomposition< double >* _aggregatorDecomposition_;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH("o3prm/watertanks.o3prm"));
        reader.showElegantErrorsAndWarnings();
        _prm_ = reader.prm();
        if (reader.errors() > 0) { GUM_ERROR(gum::FatalError, "could not load ressource file") }
      }
      _bn_                      = new gum::BayesNet< double >();
      _bn2_                     = new gum::BayesNet< double >();
      _inf_                     = 0;
      _inf2_                    = 0;
      _aggregatorDecomposition_ = new gum::AggregatorDecomposition< double >();
    }

    void tearDown() {
      delete _prm_;
      delete _bn_;
      delete _bn2_;
      delete _inf_;
      delete _inf2_;
      delete _aggregatorDecomposition_;
    }

    void testDecomposition() {
      gum::BayesNetFactory< double >* factory  = 0;
      gum::BayesNetFactory< double >* factory2 = 0;
      gum::NodeId                     node     = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(_bn_));
      TS_GUM_ASSERT_THROWS_NOTHING(factory2 = new gum::BayesNetFactory< double >(_bn2_));

      TS_GUM_ASSERT_THROWS_NOTHING(_prm_->getSystem("aSys").groundedBN(*factory));
      TS_GUM_ASSERT_THROWS_NOTHING(_prm_->getSystem("aSys").groundedBN(*factory2));

      TS_GUM_ASSERT_THROWS_NOTHING(_aggregatorDecomposition_->setMaximumArity(2));
      TS_GUM_ASSERT_THROWS_NOTHING(*_bn_
                                   = _aggregatorDecomposition_->getDecomposedAggregator(*_bn_));

      TS_GUM_ASSERT_THROWS_NOTHING(_inf_ = new gum::LazyPropagation< double >(_bn_))
      TS_GUM_ASSERT_THROWS_NOTHING(_inf_->makeInference();)

      TS_GUM_ASSERT_THROWS_NOTHING(_inf2_ = new gum::LazyPropagation< double >(_bn2_))
      TS_GUM_ASSERT_THROWS_NOTHING(_inf2_->makeInference();)

      TS_GUM_ASSERT_THROWS_NOTHING(node = _bn_->idFromName("city.(total_quantity)waterlevel"));

      gum::Instantiation inst(_inf_->posterior(node));
      gum::Instantiation inst2(_inf2_->posterior(node));

      inst.setFirst();
      inst2.setFirst();

      while (!inst.end()) {
        TS_ASSERT_DELTA(abs(_inf_->posterior(node).get(inst) - _inf2_->posterior(node).get(inst2)),
                        0,
                        _epsilon_)
        inst.inc();
        inst2.inc();
      }

      delete factory;
      delete factory2;
    }
  };
}   // namespace gum_tests
