
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/core/math/math.h>
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

    class AggregatorDecompositionTestSuite : public CxxTest::TestSuite {
    private:
        gum::prm::PRM<double>* __prm;
        float __epsilon{1e-6f};
		gum::BayesNet< double >* __bn;
		gum::BayesNet< double >* __bn2;
		gum::LazyPropagation< double >* __inf;
		gum::LazyPropagation< double >* __inf2;
		gum::AggregatorDecomposition< double >* __aggregatorDecomposition;

	public:
        void setUp() {
            {
                gum::prm::o3prm::O3prmReader<double> reader;
                reader.readFile(GET_RESSOURCES_PATH("o3prm/watertanks.o3prm"));
                reader.showElegantErrorsAndWarnings();
                __prm = reader.prm();
                if (reader.errors() > 0) {
                    GUM_ERROR(gum::FatalError, "could not load ressource file");
                }
            }
			__bn = new gum::BayesNet< double >();
			__bn2 = new gum::BayesNet< double >();
			__inf = 0;
			__inf2 = 0;
			__aggregatorDecomposition = new gum::AggregatorDecomposition< double >();
		}

        void tearDown() {
            delete __prm;
            delete __bn;
            delete __bn2;
            delete __inf;
            delete __inf2;
            delete __aggregatorDecomposition;
        }

        void testDecomposition() {
			gum::BayesNetFactory< double >* factory = 0;
			gum::BayesNetFactory< double >* factory2 = 0;
			gum::NodeId node = 0;

			TS_GUM_ASSERT_THROWS_NOTHING(factory =
												 new gum::BayesNetFactory< double >(__bn));
			TS_GUM_ASSERT_THROWS_NOTHING(factory2 =
												 new gum::BayesNetFactory< double >(__bn2));

			TS_GUM_ASSERT_THROWS_NOTHING(__prm->getSystem("aSys").groundedBN(*factory));
			TS_GUM_ASSERT_THROWS_NOTHING(__prm->getSystem("aSys").groundedBN(*factory2));

			TS_GUM_ASSERT_THROWS_NOTHING(__aggregatorDecomposition->setMaximumArity(2));
			TS_GUM_ASSERT_THROWS_NOTHING(*__bn = __aggregatorDecomposition->getDecomposedAggregator(*__bn));

			TS_GUM_ASSERT_THROWS_NOTHING(__inf = new gum::LazyPropagation< double >(__bn))
			TS_GUM_ASSERT_THROWS_NOTHING(__inf->makeInference();)

			TS_GUM_ASSERT_THROWS_NOTHING(__inf2 = new gum::LazyPropagation< double >(__bn2))
			TS_GUM_ASSERT_THROWS_NOTHING(__inf2->makeInference();)

			TS_GUM_ASSERT_THROWS_NOTHING(node = __bn->idFromName("city.(total_quantity)waterlevel"));

			gum::Instantiation inst(__inf->posterior(node));
			gum::Instantiation inst2(__inf2->posterior(node));

			inst.setFirst();
			inst2.setFirst();

			while (!inst.end()) {

				TS_ASSERT_DELTA(abs(__inf->posterior(node).get(inst) - __inf2->posterior(node).get(inst2)), 0, __epsilon)
				inst.inc();
				inst2.inc();
			}

			delete factory;
			delete factory2;
        }
    };
}   // namespace gum_tests
