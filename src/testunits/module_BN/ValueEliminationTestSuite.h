/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class VariableElimination : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float> *bn;
    gum::Id i1, i2, i3, i4, i5;
    gum::Potential<float> *e_i1, *e_i4;
    std::string base_dir;

    void setUp() {
      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);

      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);

      e_i1 = new gum::Potential<float>();
      (*e_i1) << bn->variable(i1);
      e_i1->fill((float)0);
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(bn->variable(i1), 0);
      e_i1->set(inst_1, (float)1);

      e_i4 = new gum::Potential<float>();
      (*e_i4) << bn->variable(i4);
      e_i4->fill((float)0);
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(bn->variable(i4), 1);
      e_i4->set(inst_4, (float)1);

      base_dir = "../../../";
    }

    void tearDown() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    void testFill() {
      const gum::Potential<float> &p1 = bn->cpt(i1);
      TS_ASSERT(p1.nbrDim() == 1);

      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<float> &p2 = bn->cpt(i2);
      TS_ASSERT(p2.nbrDim() == 1);

      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<float> &p3 = bn->cpt(i3);
      TS_ASSERT(p3.nbrDim() == 2);
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<float> v(t, t + n);
        p3.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p;
        p << bn->variable(i1);
        p.marginalize(p3);

        for (gum::Instantiation j(p); !j.end(); ++j)
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
      }

      const gum::Potential<float> &p4 = bn->cpt(i4);
      TS_ASSERT(p4.nbrDim() == 3);
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<float> v(t, t + n);
        p4.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p;
        p << bn->variable(i1) << bn->variable(i2);
        p.marginalize(p4);

        for (gum::Instantiation j(p); !j.end(); ++j)
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
      }

      const gum::Potential<float> &p5 = bn->cpt(i5);
      TS_ASSERT(p5.nbrDim() == 4);
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                             0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                             0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0};
        int n = 24;
        const std::vector<float> v(t, t + n);
        p5.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p;
        p << bn->variable(i4) << bn->variable(i2) << bn->variable(i3);
        p.marginalize(p5);

        for (gum::Instantiation j(p); !j.end(); ++j) {
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
        }
      }
    }

    // Testing when there is no evidence
    void testMakeInference() {
      fill(*bn);
      // Testing the inference
      gum::VariableElimination<float> *inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination<float>(*bn));

      if (inf != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->makeInference());
        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testValueElim() {
      fill(*bn);
      gum::VariableElimination<float> *inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination<float>(*bn));

      if (inf != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->makeInference());

        const gum::Potential<float> *posterior_1 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(posterior_1 = &(inf->posterior(i1)));

        if (posterior_1 != 0)
          printProba(*posterior_1);

        const gum::Potential<float> *posterior_2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_2 = &(inf->posterior(i2)));

        if (posterior_2 != 0)
          printProba(*posterior_2);

        const gum::Potential<float> *posterior_3 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_3 = &(inf->posterior(i3)));

        if (posterior_3 != 0)
          printProba(*posterior_3);

        const gum::Potential<float> *posterior_4 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_4 = &(inf->posterior(i4)));

        if (posterior_4 != 0)
          printProba(*posterior_4);

        const gum::Potential<float> *posterior_5 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_5 = &(inf->posterior(i5)));

        if (posterior_5 != 0)
          printProba(*posterior_5);

        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testShaferShenoyInf_3() {
      fill(*bn);
      gum::List<const gum::Potential<float> *> e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::VariableElimination<float> *inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination<float>(*bn));

      if (inf != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->insertEvidence(e_list));

        const gum::Potential<float> *posterior_1 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(posterior_1 = &(inf->posterior(i1)));

        if (posterior_1 != 0)
          printProba(*posterior_1);

        const gum::Potential<float> *posterior_2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_2 = &(inf->posterior(i2)));

        if (posterior_2 != 0)
          printProba(*posterior_2);

        const gum::Potential<float> *posterior_3 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_3 = &(inf->posterior(i3)));

        if (posterior_3 != 0)
          printProba(*posterior_3);

        const gum::Potential<float> *posterior_4 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_4 = &(inf->posterior(i4)));

        if (posterior_4 != 0)
          printProba(*posterior_4);

        const gum::Potential<float> *posterior_5 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_5 = &(inf->posterior(i5)));

        if (posterior_5 != 0)
          printProba(*posterior_5);

        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    // void testAlarm() {
    //   std::string file_name = "src/testunits/ressources/alarm.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> alarm;
    //   gum::BIFReader<float> reader(&alarm, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(alarm);
    //   gum::ShaferShenoyInference<float> shafer(alarm);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = alarm.dag().beginNodes(); iter !=
    //   alarm.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testDiabetes() {
    //   std::string file_name = "src/testunits/ressources/Diabetes.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> diabetes;
    //   gum::BIFReader<float> reader(&diabetes, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(diabetes);
    //   gum::ShaferShenoyInference<float> shafer(diabetes);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = diabetes.dag().beginNodes(); iter !=
    //   diabetes.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testLink() {
    //   std::string file_name = "src/testunits/ressources/Link.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> link;
    //   gum::BIFReader<float> reader(&link, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(link);
    //   gum::ShaferShenoyInference<float> shafer(link);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = link.dag().beginNodes(); iter !=
    //   link.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testWater() {
    //   std::string file_name = "src/testunits/ressources/Water.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> water;
    //   gum::BIFReader<float> reader(&water, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(water);
    //   gum::ShaferShenoyInference<float> shafer(water);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = water.dag().beginNodes(); iter !=
    //   water.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testCarpo() {
    //   std::string file_name = "src/testunits/ressources/carpo.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> carpo;
    //   gum::BIFReader<float> reader(&carpo, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(carpo);
    //   gum::ShaferShenoyInference<float> shafer(carpo);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = carpo.dag().beginNodes(); iter !=
    //   carpo.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testMildew() {
    //   std::string file_name = "src/testunits/ressources/Mildew.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> mildew;
    //   gum::BIFReader<float> reader(&mildew, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(mildew);
    //   gum::ShaferShenoyInference<float> shafer(mildew);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = mildew.dag().beginNodes(); iter !=
    //   mildew.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testHailfinder() {
    //   std::string file_name = "src/testunits/ressources/hailfinder.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> hailfinder;
    //   gum::BIFReader<float> reader(&hailfinder, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(hailfinder);
    //   gum::ShaferShenoyInference<float> shafer(hailfinder);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = hailfinder.dag().beginNodes(); iter !=
    //   hailfinder.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testMunin1() {
    //   std::string file_name = "src/testunits/ressources/Munin1.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> munin1;
    //   gum::BIFReader<float> reader(&munin1, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(munin1);
    //   gum::ShaferShenoyInference<float> shafer(munin1);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = munin1.dag().beginNodes(); iter !=
    //   munin1.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testInsurance() {
    //   std::string file_name = "src/testunits/ressources/insurance.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> insurance;
    //   gum::BIFReader<float> reader(&insurance, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(insurance);
    //   gum::ShaferShenoyInference<float> shafer(insurance);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = insurance.dag().beginNodes(); iter !=
    //   insurance.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testPigs() {
    //   std::string file_name = "src/testunits/ressources/Pigs.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> pigs;
    //   gum::BIFReader<float> reader(&pigs, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(pigs);
    //   gum::ShaferShenoyInference<float> shafer(pigs);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = pigs.dag().beginNodes(); iter !=
    //   pigs.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    // void testBarley() {
    //   std::string file_name = "src/testunits/ressources/Barley.bif";
    //   std::string file_path = base_dir + file_name;
    //   gum::BayesNet<float> barley;
    //   gum::BIFReader<float> reader(&barley, file_path);
    //   reader.proceed();
    //   gum::VariableElimination<float> ve(barley);
    //   gum::ShaferShenoyInference<float> shafer(barley);
    //   TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
    //   for (gum::DAG::NodeIterator iter = barley.dag().beginNodes(); iter !=
    //   barley.dag().endNodes(); ++iter) {
    //     const gum::Potential<float>* pot_1 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(*iter)));
    //     const gum::Potential<float>* pot_2 = 0;
    //     TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(*iter)));
    //     if ((*pot_1) != (*pot_2)) {
    //       TS_ASSERT(false);
    //     }
    //   }
    // }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet<float> &bn) {
      const gum::Potential<float> &p1 = bn.cpt(i1);
      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<float> &p2 = bn.cpt(i2);
      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<float> &p3 = bn.cpt(i3);
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<float> v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential<float> &p4 = bn.cpt(i4);
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<float> v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential<float> &p5 = bn.cpt(i5);
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                             0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                             0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0};
        int n = 24;
        const std::vector<float> v(t, t + n);
        p5.fillWith(v);
      }
    }

    // Uncomment this to have some outputs.
    void printProba(const gum::Potential<float> &p) {
      // gum::Instantiation inst(p);
      // for (inst.setFirst(); !inst.end(); ++inst) {
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
