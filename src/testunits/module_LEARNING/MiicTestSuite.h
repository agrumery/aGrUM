/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori2.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation2.h>
#include <agrum/learning/Miic.h>

namespace gum_tests {

  class MiicTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::DBInitializerFromCSV<> initializer(
        GET_RESSOURCES_PATH("alarm.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< gum::Size > modalities;
      for (auto dom : database.domainSizes())
        modalities.push_back(dom);
      gum::MixedGraph graph;
      for (std::size_t i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (std::size_t j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      gum::learning::AprioriNoApriori2<>    apriori(database);
      gum::learning::CorrectedMutualInformation2<> I(parser, apriori);
      I.useNML();
      I.useKCache(true);

      gum::learning::Miic miic;

      miic.learnMixedStructure(I, graph);
      std::cout << graph << std::endl;

      /*
      const auto& kNML = I.kNML();
      std::cout << "knml cache : " << kNML.cache_size
                << "  hits : " << kNML.cache_hit << std::endl;
      std::cout << "corrMutInfo I2 cache : " << I.I2_cache_size
                << "  hits : " << I.I2_cache_hits << std::endl;
      std::cout << "corrMutInfo I3 cache : " << I.I3_cache_size
                << "  hits : " << I.I3_cache_hits << std::endl;
      std::cout << "corrMutInfo K2 cache : " << I.K2_cache_size
                << "  hits : " << I.K2_cache_hits << std::endl;
      std::cout << "corrMutInfo K3 cache : " << I.K3_cache_size
                << "  hits : " << I.K3_cache_hits << std::endl;
      std::cout << "partial ent cache    : " << I.partialEnt().cache_size
                << "  hits : " << I.partialEnt().cache_hits << std::endl;
      */
    }
  };

}   // namespace gum_tests
