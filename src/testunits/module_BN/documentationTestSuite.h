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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {
  class DocumentationTestSuite: public CxxTest::TestSuite {
    public:
    void testBayesNet() {
      try {
        auto bn = gum::BayesNet< double >("Asia");
        // Variables are added by copy to the BayesNet, so you can use a single
        // gum::LabelizedVariable to add all varaibles with the same domain
        auto var = gum::LabelizedVariable(
           "template", "A variable of the Asia Bayesian Network", 0);
        var.addLabel("True");
        var.addLabel("False");

        var.setName("Visit to Asia");
        auto visitToAsia = bn.add(var);
        var.setName("Smoker");
        auto smoker = bn.add(var);
        var.setName("Has Tuberculosis");
        auto hasTuberculosis = bn.add(var);
        var.setName("Has Lung Cancer");
        auto hasLungCancer = bn.add(var);
        var.setName("Has Bronchitis");
        auto hasBronchitis = bn.add(var);
        var.setName("Tuberculosis or Cancer");
        auto tubOrCancer = bn.add(var);
        var.setName("XRay Result");
        auto xray = bn.add(var);
        var.setName("Dyspnea");
        auto dyspnea = bn.add(var);
        bn.addArc(visitToAsia, hasTuberculosis);
        bn.addArc(hasTuberculosis, tubOrCancer);
        bn.addArc(smoker, hasLungCancer);
        bn.addArc(smoker, hasBronchitis);
        bn.addArc(hasLungCancer, tubOrCancer);
        bn.addArc(tubOrCancer, xray);
        bn.addArc(tubOrCancer, dyspnea);
        bn.addArc(hasBronchitis, dyspnea);
        bn.cpt(visitToAsia).fillWith({0.1f, 0.9f});
        bn.cpt(smoker).fillWith({0.7f, 0.3f});
        bn.cpt(hasTuberculosis)
           .fillWith({
              // True | False  == hasTuberculosis
              0.05f,
              0.01f,   // visitToAsia == True
              0.95f,
              0.99f   // visitToAsia == False
           });
        bn.cpt(hasLungCancer)
           .fillWith({
              // True | False  == hasLungCancer
              0.10f,
              0.90f,   // smoker == True
              0.01f,
              0.99f   // smoker == False
           });
        bn.cpt(tubOrCancer)
           .fillWith({
              // True | False  == tubOrCancer
              1.00f,
              0.00f,   // hasTuberculosis == True,  hasLungCancer == True
              1.00f,
              0.00f,   // hasTuberculosis == False, hasLungCancer == True
              1.00f,
              0.00f,   // hasTuberculosis == True,  hasLungCancer == False
              0.00f,
              1.00f   // hasTuberculosis == False, hasLungCancer == False
           });
        bn.cpt(xray).fillWith({
           // True | False  == xray
           0.98f,
           0.02f,   // tubOrCancer == 0
           0.05f,
           0.95f   // tubOrCancer == 1
        });
        bn.cpt(dyspnea).fillWith({
           // True | False  == dyspnea
           0.90f,
           0.10f,   // tubOrCancer == True,  hasBronchitis == True
           0.70f,
           0.30f,   // tubOrCancer == False, hasBronchitis == True
           0.80f,
           0.20f,   // tubOrCancer == True,  hasBronchitis == False
           0.10f,
           0.90f   // tubOrCancer == False, hasBronchitis == False
        });
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }

    void testBayesNetFactory() {
      try {
        auto asia = gum::BayesNet< double >();
        auto factory = gum::BayesNetFactory< double >(&asia);
        // Visit to Asia
        factory.startVariableDeclaration();
        factory.variableName("Visit To Asia");
        factory.variableType(gum::VarType::Labelized);
        factory.variableDescription(
           "True if patient visited Asia in the past months");
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Smoker
        factory.startVariableDeclaration();
        factory.variableName("Smoker");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Has Tuberculosis
        factory.startVariableDeclaration();
        factory.variableName("Has Tuberculosis");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Has Lung Cancer
        factory.startVariableDeclaration();
        factory.variableName("Has Lung Cancer");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Tuberculosis or Cancer
        factory.startVariableDeclaration();
        factory.variableName("Tuberculosis or Cancer");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Has Bronchitis
        factory.startVariableDeclaration();
        factory.variableName("Has Bronchitis");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // XRay Result
        factory.startVariableDeclaration();
        factory.variableName("XRay Result");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Dyspnea
        factory.startVariableDeclaration();
        factory.variableName("Dyspnea");
        factory.variableType(gum::VarType::Labelized);
        factory.addModality("True");
        factory.addModality("False");
        factory.endVariableDeclaration();

        // Parents of Has Tuberculosis
        factory.startParentsDeclaration("Has Tuberculosis");
        factory.addParent("Visit To Asia");
        factory.endParentsDeclaration();

        // Parents of Has Lung Cancer
        factory.startParentsDeclaration("Has Lung Cancer");
        factory.addParent("Smoker");
        factory.endParentsDeclaration();

        // Parents of Tuberculosis or Cancer
        factory.startParentsDeclaration("Tuberculosis or Cancer");
        factory.addParent("Has Tuberculosis");
        factory.addParent("Has Lung Cancer");
        factory.endParentsDeclaration();

        // Parents of Has Bronchitis
        factory.startParentsDeclaration("Has Bronchitis");
        factory.addParent("Smoker");
        factory.endParentsDeclaration();

        // Parents of XRay Result
        factory.startParentsDeclaration("XRay Result");
        factory.addParent("Tuberculosis or Cancer");
        factory.endParentsDeclaration();

        // Parents of Dyspnea
        factory.startParentsDeclaration("Dyspnea");
        factory.addParent("Tuberculosis or Cancer");
        factory.addParent("Has Bronchitis");
        factory.endParentsDeclaration();

        // CPT of Visit To Asia
        factory.startRawProbabilityDeclaration("Visit To Asia");
        auto variables = std::vector< std::string >{"Visit To Asia"};
        auto values = std::vector< float >{0.01f, 0.99f};
        factory.rawConditionalTable(variables, values);
        factory.endRawProbabilityDeclaration();

        // CPT of Tuberculosis or Cancer
        factory.startRawProbabilityDeclaration("Tuberculosis or Cancer");
        variables = std::vector< std::string >{
           "Tuberculosis or Cancer", "Has Tuberculosis", "Has Lung Cancer"};
        values = std::vector< float >   // clang-format off
          //      True     ||    False        => Has Lung Cancer
          //  True | False || True | False    => Has Tuberculosis
          {0.00f,0.00f,0.00f,1.00f,   // False
           1.00f,1.00f,1.00f,0.00f};   // True
        // clang-format on
        factory.rawConditionalTable(variables, values);
        factory.endRawProbabilityDeclaration();

        // Factorized CPT of Tuberculosis or Cancer
        factory.startFactorizedProbabilityDeclaration("Tuberculosis or Cancer");
        // Setting [ 1.00, 0.00 ] as the default distribution
        factory.startFactorizedEntry();
        // Tuberculosis or Cancer -> True | False
        values = std::vector< float >{1.00f, 0.00f};
        factory.setVariableValues(values);
        factory.endFactorizedEntry();
        factory.startFactorizedEntry();
        factory.setParentModality("Has Lung Cancer", "False");
        factory.setParentModality("Has Tuberculosis", "False");
        // Tuberculosis or Cancer -> True | False
        values = std::vector< float >{0.00f, 1.00f};
        factory.setVariableValues(values);
        factory.endFactorizedEntry();
        factory.endFactorizedProbabilityDeclaration();
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }

    void testInference() {
      auto asia = gum::BayesNet< double >("Asia");
      // Constructing the BayesNet...
      {
        auto var = gum::LabelizedVariable(
           "template", "A variable of the Asia Bayesian Network", 0);
        var.addLabel("True");
        var.addLabel("False");

        var.setName("Visit to Asia");
        auto visitToAsia = asia.add(var);
        var.setName("Smoker");
        auto smoker = asia.add(var);
        var.setName("Has Tuberculosis");
        auto hasTuberculosis = asia.add(var);
        var.setName("Has Lung Cancer");
        auto hasLungCancer = asia.add(var);
        var.setName("Has Bronchitis");
        auto hasBronchitis = asia.add(var);
        var.setName("Tuberculosis or Cancer");
        auto tubOrCancer = asia.add(var);
        var.setName("XRay Result");
        auto xray = asia.add(var);
        var.setName("Dyspnea");
        auto dyspnea = asia.add(var);
        asia.addArc(visitToAsia, hasTuberculosis);
        asia.addArc(hasTuberculosis, tubOrCancer);
        asia.addArc(smoker, hasLungCancer);
        asia.addArc(smoker, hasBronchitis);
        asia.addArc(hasLungCancer, tubOrCancer);
        asia.addArc(tubOrCancer, xray);
        asia.addArc(tubOrCancer, dyspnea);
        asia.addArc(hasBronchitis, dyspnea);
        asia.cpt(visitToAsia).fillWith({0.1f, 0.9f});
        asia.cpt(smoker).fillWith({0.7f, 0.3f});
        asia.cpt(hasTuberculosis).fillWith({0.05f, 0.01f, 0.95f, 0.99f});
        asia.cpt(hasLungCancer).fillWith({0.10f, 0.90f, 0.01f, 0.99f});
        asia.cpt(tubOrCancer)
           .fillWith({1.00f, 0.00f, 1.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f})
           .translate(TS_GUM_SMALL_ERROR)
           .normalizeAsCPT();
        asia.cpt(xray).fillWith({0.98f, 0.02f, 0.05f, 0.95f});
        asia.cpt(dyspnea).fillWith(
           {0.90f, 0.10f, 0.70f, 0.30f, 0.80f, 0.20f, 0.10f, 0.90f});
      }

      try {
        gum::LazyPropagation< double > inference(&asia);
        auto                           id = asia.idFromName("Has Lung Cancer");
        const auto&                    marginal = inference.posterior(id);
        TS_FAIL("Inference should not be correct with a undefined CPT");
      } catch (gum::IncompatibleEvidence&) {
        // OK to be here : CPT of Has Bronchitis has not been defined.
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }

      asia.cpt("Has Bronchitis").fillWith({0.7f, 0.3f, 0.85f, 0.15f});

      try {
        // To prevent warning for unused variable
        gum::LazyPropagation< double > inference(&asia);
        auto                           id = asia.idFromName("Has Lung Cancer");
        const auto& marginal = inference.posterior("Has Lung Cancer");
        TS_ASSERT_EQUALS(marginal.domainSize(), gum::Size(2));


        // We can add some evidence
        // Index 0 is False, 1 True
        inference.addEvidence(asia.idFromName("Visit to Asia"), 0);
        inference.addEvidence(asia.idFromName("Dyspnea"), 0);
        const auto& updated_marginal = inference.posterior(id);

        // To prevent warning for unused variable
        TS_ASSERT_EQUALS(updated_marginal.domainSize(), gum::Size(2));
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }

    void testSerialization() {
      try {
        auto asia = gum::BayesNet< double >("Asia");
        // One implementation of the gum::BNReader class
        std::string file = GET_RESSOURCES_PATH("bif/asia.bif");
        auto        reader = gum::BIFReader< double >(&asia, file);
        try {
          reader.proceed();
        } catch (gum::IOError& e) { TS_FAIL(e.errorContent()); }

      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }

    void testDeserialization() {
      try {
        auto asia = gum::BayesNet< double >("Asia");
        // Constructing BayesNet...
        {
          auto var = gum::LabelizedVariable(
             "template", "A variable of the Asia Bayesian Network", 0);
          var.addLabel("True");
          var.addLabel("False");

          var.setName("Visit to Asia");
          auto visitToAsia = asia.add(var);
          var.setName("Smoker");
          auto smoker = asia.add(var);
          var.setName("Has Tuberculosis");
          auto hasTuberculosis = asia.add(var);
          var.setName("Has Lung Cancer");
          auto hasLungCancer = asia.add(var);
          var.setName("Has Bronchitis");
          auto hasBronchitis = asia.add(var);
          var.setName("Tuberculosis or Cancer");
          auto tubOrCancer = asia.add(var);
          var.setName("XRay Result");
          auto xray = asia.add(var);
          var.setName("Dyspnea");
          auto dyspnea = asia.add(var);
          asia.addArc(visitToAsia, hasTuberculosis);
          asia.addArc(hasTuberculosis, tubOrCancer);
          asia.addArc(smoker, hasLungCancer);
          asia.addArc(smoker, hasBronchitis);
          asia.addArc(hasLungCancer, tubOrCancer);
          asia.addArc(tubOrCancer, xray);
          asia.addArc(tubOrCancer, dyspnea);
          asia.addArc(hasBronchitis, dyspnea);
          asia.cpt(visitToAsia).fillWith({0.1f, 0.9f});
          asia.cpt(smoker).fillWith({0.7f, 0.3f});
          asia.cpt(hasTuberculosis).fillWith({0.05f, 0.01f, 0.95f, 0.99f});
          asia.cpt(hasLungCancer).fillWith({0.10f, 0.90f, 0.01f, 0.99f});
          asia.cpt(tubOrCancer)
             .fillWith({1.00f, 0.00f, 1.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f});
          asia.cpt(xray).fillWith({0.98f, 0.02f, 0.05f, 0.95f});
          asia.cpt(dyspnea).fillWith(
             {0.90f, 0.10f, 0.70f, 0.30f, 0.80f, 0.20f, 0.10f, 0.90f});
        }
        // One implementation of the gum::BNWriter class
        auto writer = gum::BIFWriter< double >();
        try {
          std::stringstream buff;   // Don't want to write in files for this test
          // This will print the asia BayesNet on the standard output stream
          writer.write(buff, asia);

          // This will write the asia BayesNet in the given file
          writer.write(buff, asia);
        } catch (gum::IOError& e) {
          TS_FAIL(e.errorContent());
          // A gum::IOError will be raised if an error occured
        }
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }
  };

}   // namespace gum_tests
