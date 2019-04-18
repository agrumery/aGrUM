
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


#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/core/exceptions.h>

#include <agrum/multidim/ICIModels/multiDimLogit.h>
#include <agrum/multidim/potential.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>

namespace gum_tests {

  class MultiDimLogitTestSuite : public CxxTest::TestSuite {
    public:
    void testCreationLogit() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2), c("c", "", 2),
         d("d", "", 2);
      gum::MultiDimLogit< double > p(0.2f);

      // trying to change weight for a non cause
      TS_ASSERT_THROWS(p.causalWeight(b, 0.4f), gum::InvalidArgument);
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0f), gum::InvalidArgument);

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);

      // doing the right stuff :)
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2)");
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(b, 0.4f));
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1>)");
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(d, 0.7f));
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)");

      TS_ASSERT_EQUALS(p.nbrDim(), (gum::Size)4);
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      gum::MultiDimLogit< double > q(p);
      TS_ASSERT_EQUALS(q.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      TS_GUM_ASSERT_THROWS_NOTHING(q.causalWeight(c, -1.3f));
      TS_ASSERT_EQUALS(q.toString(),
                       "a<0,1>=logit(0.2 +0.4*b<0,1> -1.3*c<0,1> +0.7*d<0,1>)");
    }

    void testComputationInLogit() {
      // from "Pratique de la Régression Logique" / Ricco Rakotomalala / p33
      gum::RangeVariable     age("age", "", 35, 67);
      gum::RangeVariable     taux("taux", "", 115, 171);
      gum::LabelizedVariable angine("angine", "", 2);
      gum::LabelizedVariable coeur("coeur", "", 0);
      coeur.addLabel("NON").addLabel("OUI");

      gum::MultiDimLogit< double > p(14.4937f);
      // taux, angine and coeur are causes of coeur
      p << coeur << age << taux << angine;
      p.causalWeight(age, -0.1256f);
      p.causalWeight(taux, -0.0636f);
      p.causalWeight(angine, 1.779f);

      gum::Instantiation i(p);

      std::string witness_age[] = {
         "50", "49", "46", "49", "62", "35", "67", "65", "47"};
      std::string witness_taux[] = {
         "126", "126", "144", "139", "154", "156", "160", "140", "143"};
      std::string witness_angine[] = {"1", "0", "0", "0", "1", "1", "0", "0", "0"};
      std::string witness_coeur[] = {
         "OUI", "OUI", "OUI", "OUI", "OUI", "OUI", "NON", "NON", "NON"};
      float witness_proba[] = {0.8786f,
                               0.5807f,
                               0.3912f,
                               0.3773f,
                               0.2127f,
                               0.8760f,
                               1 - 0.0163f,
                               1 - 0.0710f,
                               1 - 0.3765f};
      int   nbr = 9;

      for (int l = 0; l < nbr; l++) {
        try {
          i.chgVal(age, age[witness_age[l]]);
          i.chgVal(taux, taux[witness_taux[l]]);
          i.chgVal(angine, angine[witness_angine[l]]);
          i.chgVal(coeur, coeur[witness_coeur[l]]);
          TS_ASSERT_DELTA(witness_proba[l], p[i], 15e-3);
        } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
      }

      gum::MultiDimLogit< double > q(p);
      gum::Instantiation           j(p);
      for (i.setFirst(), j.setFirst(); !i.end(); ++i, ++j) {
        TS_ASSERT_DELTA(q[i], p[j], 1e-6);
      }
    }

    void testComputationInLogit2() {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimLogit< double > p(1 - 0.0001f);
      p << unemployment << competition << requirement << motivation << degree
        << lazy;
      p.causalWeight(lazy, 0.8f);
      p.causalWeight(degree, 0.7f);
      p.causalWeight(motivation, 0.9f);
      p.causalWeight(requirement, 0.8f);
      p.causalWeight(competition, 0.9f);

      gum::Instantiation i(p);
      float              witness[] = {
         // clang-format off
              0.26896108301760213f,0.7310389169823979f,
              0.13011979280757435f,0.8698802071924256f,
              0.14186323827049419f,0.8581367617295058f,
              0.06297925708613061f,0.9370207429138694f,
              0.13011979280757435f,0.8698802071924256f,
              0.057329579949564224f,0.9426704200504358f,
              0.06297925708613061f,0.9370207429138694f,
              0.026599582658782595f,0.9734004173412174f,
              0.15447832610952417f,0.8455216738904758f,
              0.06914485645056612f,0.9308551435494339f,
              0.075865190690244f,0.924134809309756f,
              0.03229859009138969f,0.9677014099086103f,
              0.06914485645056612f,0.9308551435494339f,
              0.02931507618767315f,0.9706849238123268f,
              0.03229859009138991f,0.9677014099086101f,
              0.013388238662763174f,0.9866117613372368f,
              0.14186323827049419f,0.8581367617295058f,
              0.06297925708613061f,0.9370207429138694f,
              0.06914485645056612f,0.9308551435494339f,
              0.02931507618767315f,0.9706849238123268f,
              0.06297925708613061f,0.9370207429138694f,
              0.026599582658782595f,0.9734004173412174f,
              0.02931507618767315f,0.9706849238123268f,
              0.012129633176334642f,0.9878703668236654f,
              0.075865190690244f,0.924134809309756f,
              0.03229859009138991f,0.9677014099086101f,
              0.035574620019943715f,0.9644253799800563f,
              0.014775487339871773f,0.9852245126601282f,
              0.03229859009138991f,0.9677014099086101f,
              0.013388238662763174f,0.9866117613372368f,
              0.014775487339871773f,0.9852245126601282f,
              0.006060403829365346f,0.9939395961706347f   // clang-format on
      };

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::MultiDimLogit< double > q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }
  };
}   // namespace gum_tests
