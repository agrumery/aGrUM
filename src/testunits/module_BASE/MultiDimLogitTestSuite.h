/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/exceptions.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/ICIModels/multiDimLogit.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class MultiDimLogitTestSuite : public CxxTest::TestSuite {
    public:
    void testCreationLogit() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2), c("c", "", 2),
          d("d", "", 2);
      gum::MultiDimLogit<float> p(0.2);

      // trying to change weight for a non cause
      TS_ASSERT_THROWS(p.causalWeight(b, 0.4), gum::InvalidArgument);
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0), gum::InvalidArgument);

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);

      // doing the right stuff :)
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2)");
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(b, 0.4));
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1>)");
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(d, 0.7));
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)");

      TS_ASSERT_EQUALS(p.nbrDim(), (gum::Size)4);
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      gum::MultiDimLogit<float> q(p);
      TS_ASSERT_EQUALS(q.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      TS_GUM_ASSERT_THROWS_NOTHING(q.causalWeight(c, -1.3));
      TS_ASSERT_EQUALS(q.toString(),
                       "a<0,1>=logit(0.2 +0.4*b<0,1> -1.3*c<0,1> +0.7*d<0,1>)");
    }

    void testComputationInLogit() {
      // from "Pratique de la Régression Logique" / Ricco Rakotomalala / p33
      gum::RangeVariable age("age", "", 35, 67);
      gum::RangeVariable taux("taux", "", 115, 171);
      gum::LabelizedVariable angine("angine", "", 2);
      gum::LabelizedVariable coeur("coeur", "", 0);
      coeur.addLabel("NON").addLabel("OUI");

      gum::MultiDimLogit<float> p(14.4937);
      p << coeur << age << taux
        << angine; // taux, angine and coeur are causes of coeur
      p.causalWeight(age, -0.1256);
      p.causalWeight(taux, -0.0636);
      p.causalWeight(angine, 1.779);

      gum::Instantiation i(p);

      /*for( i.setFirst(); ! i.end(); ++i ) {
        GUM_TRACE( i << " : " << p[i] );
      }*/

      std::string witness_age[] = {"50", "49", "46", "49", "62",
                                   "35", "67", "65", "47"};
      std::string witness_taux[] = {"126", "126", "144", "139", "154",
                                    "156", "160", "140", "143"};
      std::string witness_angine[] = {"1", "0", "0", "0", "1", "1", "0", "0", "0"};
      std::string witness_coeur[] = {"OUI", "OUI", "OUI", "OUI", "OUI",
                                     "OUI", "NON", "NON", "NON"};
      float witness_proba[] = {0.8786, 0.5807,     0.3912,     0.3773,    0.2127,
                               0.8760, 1 - 0.0163, 1 - 0.0710, 1 - 0.3765};
      int nbr = 9;

      for (int l = 0; l < nbr; l++) {
        try {
          i.chgVal(age, age[witness_age[l]]);
          i.chgVal(taux, taux[witness_taux[l]]);
          i.chgVal(angine, angine[witness_angine[l]]);
          i.chgVal(coeur, coeur[witness_coeur[l]]);
          TS_ASSERT_DELTA(witness_proba[l], p[i], 15e-3);
        } catch (gum::Exception &e) {
          GUM_SHOWERROR(e);
        }
      }

      /*
              float witness[] =
         {0.2315,0.7685,0.1978,0.8022,0.4256,0.5744,0.3775,0.6225,0.0148,0.9852,0.0121,0.9879,0.0356,0.9644,0.0293,0.9707};

              int j = 0;
              for ( i.setFirst(); ! i.end(); ++i, j++ ) {
                TS_ASSERT_DELTA ( p[i], witness[j], 1e-3 );
              }

              gum::MultiDimLogit<float> q ( p );

              j = 0;

              for ( i.setFirst(); ! i.end(); ++i, j++ ) {
                TS_ASSERT_DELTA ( q[i], witness[j], 1e-6 );
              }*/
    }

    void /*test*/ ComputationInLogit2() {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimLogit<float> p(1 - 0.0001);
      p << unemployment << competition << requirement << motivation << degree
        << lazy;
      p.causalWeight(lazy, 0.8);
      p.causalWeight(degree, 0.7);
      p.causalWeight(motivation, 0.9);
      p.causalWeight(requirement, 0.8);
      p.causalWeight(competition, 0.9);

      gum::Instantiation i(p);
      float witness[] = {
          0.999880012, 0.000119988, 0.998920108, 0.001079892, 0.999520048,
          0.000479952, 0.995680432, 0.004319568, 0.998920108, 0.001079892,
          0.990280972, 0.009719028, 0.995680432, 0.004319568, 0.961123888,
          0.038876112, 0.999720028, 0.000279972, 0.997480252, 0.002519748,
          0.998880112, 0.001119888, 0.989921008, 0.010078992, 0.997480252,
          0.002519748, 0.977322268, 0.022677732, 0.989921008, 0.010078992,
          0.909289072, 0.090710928, 0.999520048, 0.000479952, 0.995680432,
          0.004319568, 0.998080192, 0.001919808, 0.982721728, 0.017278272,
          0.995680432, 0.004319568, 0.961123888, 0.038876112, 0.982721728,
          0.017278272, 0.844495552, 0.155504448, 0.998880112, 0.001119888,
          0.989921008, 0.010078992, 0.995520448, 0.004479552, 0.959684032,
          0.040315968, 0.989921008, 0.010078992, 0.909289072, 0.090710928,
          0.959684032, 0.040315968, 0.637156288, 0.362843712};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::MultiDimLogit<float> q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }

    void /*test*/ LogitInBN() {
      gum::BayesNet<float> bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold = bn.add(cold);
      gum::NodeId idFlu = bn.add(flu);
      gum::NodeId idMalaria = bn.add(malaria);
      gum::NodeId idFever = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(idFever = bn.addLogit(fever, 0.3));
      gum::NodeId idOneMore = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9);
      bn.addWeightedArc(idFlu, idFever, 0.8);
      bn.addWeightedArc(idCold, idFever, 0.4);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8), gum::InvalidArc);

      const gum::Potential<float> &pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      // FILLING PARAMS
      pOneMoreParent1.fillWith(std::vector<float>{0.2, 0.8});

      const gum::Potential<float> &pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      // FILLING PARAMS
      pOneMoreParent2.fillWith(std::vector<float>{0.3, 0.7});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential<float> &pOneMore = bn.cpt(idOneMore);
      // FILLING PARAMS
      pOneMore.fillWith(std::vector<float>{0.1, 0.9, 0.8, 0.2, 0.1, 0.9, 0.8, 0.2,
                                           0.1, 0.9, 0.8, 0.2, 0.1, 0.9, 0.8, 0.2});

      const gum::Potential<float> &p = bn.cpt(idFever);

      gum::Instantiation i(p);
      float witness[] = {0.988012, 0.011988, 0.892108, 0.107892, 0.952048, 0.047952,
                         0.568432, 0.431568, 0.992008, 0.007992, 0.928072, 0.071928,
                         0.968032, 0.031968, 0.712288, 0.287712};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::LazyPropagation<float> inf_LazyProp(bn);

      inf_LazyProp.makeInference();
    }
  };
}
