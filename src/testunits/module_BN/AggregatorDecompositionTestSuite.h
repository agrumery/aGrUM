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
    gum::prm::PRM< double >*                prm__;
    float                                   epsilon__{1e-6f};
    gum::BayesNet< double >*                bn__;
    gum::BayesNet< double >*                bn2__;
    gum::LazyPropagation< double >*         inf__;
    gum::LazyPropagation< double >*         inf2__;
    gum::AggregatorDecomposition< double >* aggregatorDecomposition__;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH("o3prm/watertanks.o3prm"));
        reader.showElegantErrorsAndWarnings();
        prm__ = reader.prm();
        if (reader.errors() > 0) {
          GUM_ERROR(gum::FatalError, "could not load ressource file");
        }
      }
      bn__                      = new gum::BayesNet< double >();
      bn2__                     = new gum::BayesNet< double >();
      inf__                     = 0;
      inf2__                    = 0;
      aggregatorDecomposition__ = new gum::AggregatorDecomposition< double >();
    }

    void tearDown() {
      delete prm__;
      delete bn__;
      delete bn2__;
      delete inf__;
      delete inf2__;
      delete aggregatorDecomposition__;
    }

    void testDecomposition() {
      gum::BayesNetFactory< double >* factory  = 0;
      gum::BayesNetFactory< double >* factory2 = 0;
      gum::NodeId                     node     = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(factory
                                   = new gum::BayesNetFactory< double >(bn__));
      TS_GUM_ASSERT_THROWS_NOTHING(factory2
                                   = new gum::BayesNetFactory< double >(bn2__));

      TS_GUM_ASSERT_THROWS_NOTHING(prm__->getSystem("aSys").groundedBN(*factory));
      TS_GUM_ASSERT_THROWS_NOTHING(prm__->getSystem("aSys").groundedBN(*factory2));

      TS_GUM_ASSERT_THROWS_NOTHING(aggregatorDecomposition__->setMaximumArity(2));
      TS_GUM_ASSERT_THROWS_NOTHING(*bn__ = aggregatorDecomposition__
                                              ->getDecomposedAggregator(*bn__));

      TS_GUM_ASSERT_THROWS_NOTHING(inf__
                                   = new gum::LazyPropagation< double >(bn__))
      TS_GUM_ASSERT_THROWS_NOTHING(inf__->makeInference();)

      TS_GUM_ASSERT_THROWS_NOTHING(inf2__
                                   = new gum::LazyPropagation< double >(bn2__))
      TS_GUM_ASSERT_THROWS_NOTHING(inf2__->makeInference();)

      TS_GUM_ASSERT_THROWS_NOTHING(
         node = bn__->idFromName("city.(total_quantity)waterlevel"));

      gum::Instantiation inst(inf__->posterior(node));
      gum::Instantiation inst2(inf2__->posterior(node));

      inst.setFirst();
      inst2.setFirst();

      while (!inst.end()) {
        TS_ASSERT_DELTA(abs(inf__->posterior(node).get(inst)
                            - inf2__->posterior(node).get(inst2)),
                        0,
                        epsilon__)
        inst.inc();
        inst2.inc();
      }

      delete factory;
      delete factory2;
    }
  };
}   // namespace gum_tests
