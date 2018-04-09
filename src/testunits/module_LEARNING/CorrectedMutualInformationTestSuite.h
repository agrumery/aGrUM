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
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>

namespace gum_tests {

  class CorrectedMutualInformationTestSuite : public CxxTest::TestSuite {
    public:
    void test_Ixy_Kmdl() {
      gum::learning::DBInitializerFromCSV<>
        initializer ( GET_RESSOURCES_PATH( "asia.csv" ) );
      const auto& var_names = initializer.variableNames ();
      const std::size_t nb_vars = var_names.size ();
      
      gum::learning::DBTranslatorSet<> translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        translator_set.insertTranslator ( translator, i );
      }
      
      gum::learning::DatabaseTable<> database ( translator_set );
      database.setVariableNames( initializer.variableNames () );
      initializer.fillDatabase ( database );

      gum::learning::DBRowGeneratorSet<> genset;
      gum::learning::DBRowGeneratorParser<>
        parser ( database.handler (), genset );

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::CorrectedMutualInformation<> score(parser, modalities);
      score.useMDL();
      /*
       * Edge 2-6 Ixy 0.2515024365984908 K 4.605170185988092
       * Edge 3-4 Ixy 0.000686553442274418 K 4.605170185988092
       * Edge 4-7 Ixy 0.02569138409986288 K 4.605170185988092
       * Edge 4-7 [5] Ixy 7.691726685465916e-06 K 9.210340371976184
       * Edge 1-6 [0] Ixy 0.011185182353024459 K 9.210340371976184
       */
      TS_ASSERT_DELTA(score.score(2, 6), 0.2515024 - 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(std::pair< gum::Idx, gum::Idx >{4, 7}),
                      0.0256913 - 4.605 / 10000,
                      1e-4);
      TS_ASSERT_DELTA(score.score(std::pair< gum::Idx, gum::Idx >{4, 7},
                                  std::vector< gum::Idx >{5}),
                      7.69172e-6 - 9.2103 / 10000,
                      1e-5);
      TS_ASSERT_DELTA(score.score(1, 6, std::vector< gum::Idx >{0}),
                      0.011185 - 9.210 / 10000,
                      1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, std::vector< gum::Idx >{}),
                      0.2515024 - 4.605 / 10000,
                      1e-4);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), 0.2515024 - 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6), 0.2515024 - 4.605 / 10000, 1e-4);

      /* Ixyz de 263 vaut -4.990557243433891e-05
       * Triple 0 - 1 - 5 | [] with I(x,y,z)=0.016970292639588713
       * k=4.605170185988092
       * Triple 4 - 5 - 7 | [] with I(x,y,z)=0.025683692373177247
       * k=4.605170185988092
       * Triple 2 - 6 - 5 | [1] with I(x,y,z)=-0.0038160334747804336
       * k=9.210340371976184
       * Triple 2 - 7 - 5 | [1, 4] with I(x,y,z)=-1.1102230246251565e-16
       * Triple 0 - 6 - 5 | [1, 4] with I(x,y,z)=0.0
       */
      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), 0.01697029 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, 5, std::vector< gum::Idx >{1}),
                      -0.003816 + 9.210 / 10000,
                      1e-4);
      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), 0.0256836 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7, 5), 0.0256836 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 4, 7), 0.0256836 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 7, 4), 0.0256836 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 5, 4), 0.0256836 + 4.605 / 10000, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 4, 5), 0.0256836 + 4.605 / 10000, 1e-4);
    }

    void test_Ixy_NoCorr() {
      gum::learning::DBInitializerFromCSV<>
        initializer ( GET_RESSOURCES_PATH( "asia.csv" ) );
      const auto& var_names = initializer.variableNames ();
      const std::size_t nb_vars = var_names.size ();
      
      gum::learning::DBTranslatorSet<> translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        translator_set.insertTranslator ( translator, i );
      }
      
      gum::learning::DatabaseTable<> database ( translator_set );
      database.setVariableNames( initializer.variableNames () );
      initializer.fillDatabase ( database );

      gum::learning::DBRowGeneratorSet<> genset;
      gum::learning::DBRowGeneratorParser<>
        parser ( database.handler (), genset );

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::CorrectedMutualInformation<> score(parser, modalities);
      score.useNoCorr();
      /*
       * Edge 2-6 Ixy 0.2515024365984908
       * Edge 3-4 Ixy 0.000686553442274418
       * Edge 4-7 Ixy 0.02569138409986288
       * Edge 1-6 [0] Ixy 0.011185182353024459
       */
      TS_ASSERT_DELTA(score.score(2, 6), 0.2515, 0.001);
      TS_ASSERT_DELTA(
        score.score(std::pair< gum::Idx, gum::Idx >{4, 7}), 0.0256, 0.001);
      TS_ASSERT_DELTA(
        score.score(1, 6, std::vector< gum::Idx >{0}), 0.01118, 0.001);
      TS_ASSERT_DELTA(score.score(2, 6, std::vector< gum::Idx >{}), 0.2515, 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), 0.2515, 0.001);
      TS_ASSERT_DELTA(score.score(2, 6), 0.2515, 0.001);

      /* Ixyz de 263 vaut -4.990557243433891e-05
       * Triple 0 - 1 - 5 | [] with I(x,y,z)=0.016970292639588713
       * Triple 4 - 5 - 7 | [] with I(x,y,z)=0.025683692373177247
       * Triple 2 - 6 - 5 | [1] with I(x,y,z)=-0.0038160334747804336
       * Triple 2 - 7 - 5 | [1, 4] with I(x,y,z)=-1.1102230246251565e-16
       * Triple 0 - 6 - 5 | [1, 4] with I(x,y,z)=0.0
       */
      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), 0.0169, 0.01);
      TS_ASSERT_DELTA(
        score.score(2, 6, 5, std::vector< gum::Idx >{1}), -0.003816, 0.001);
      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), 0.025683, 0.001);
      TS_ASSERT_DELTA(score.score(4, 7, 5), 0.025683, 0.001);
      TS_ASSERT_DELTA(score.score(5, 4, 7), 0.025683, 0.001);
      TS_ASSERT_DELTA(score.score(5, 7, 4), 0.025683, 0.001);
      TS_ASSERT_DELTA(score.score(7, 5, 4), 0.025683, 0.001);
      TS_ASSERT_DELTA(score.score(7, 4, 5), 0.025683, 0.001);

      score.clear();
      TS_ASSERT_DELTA(
        score.score(0, 6, 5, std::vector< gum::Idx >{1, 4}), 0.0, 1e-4);
      TS_ASSERT_DELTA(
        score.score(6, 0, 5, std::vector< gum::Idx >{1, 4}), 0.0, 1e-4);
    }
  };

} /* namespace gum_tests */
