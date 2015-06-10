/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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

#include <agrum/CN/credalNet.h>
#include <agrum/CN/CNLoopyPropagation.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

#include <agrum/core/OMPThreads.h>

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {

  ////////////////////////////////////////////////////////////////////
  class L2UListener : public gum::ApproximationSchemeListener {
    private:
    int __nbr;
    std::string __msg;

    protected:
    public:
    L2UListener(gum::ApproximationScheme &aS)
        : gum::ApproximationSchemeListener(aS), __nbr(0), __msg(""){};

    void whenProgress(const void *buffer, const gum::Size a, const double b,
                      const double c) {
      __nbr++;
    }

    void whenStop(const void *buffer, const std::string s) { __msg = s; }

    int nbr() { return __nbr; }

    std::string &msg() { return __msg; }
  }; // end of : class l2uListener

  ////////////////////////////////////////////////////////////////
  class L2UInferenceTestSuite : public CxxTest::TestSuite {
    private:
    protected:
    public:
    gum::credal::CredalNet<double> *cn;

    // not dynamic (2U network - fast)
    void initCNet() {
#ifdef _OPENMP
      gum::setNumberOfThreads(1);
#endif
      gum::BayesNet<double> monBNa;
      gum::BIFReader<double> readera(&monBNa, GET_PATH_STR("cn/2Umin.bif"));
      readera.proceed();

      gum::BayesNet<double> monBNb;
      gum::BIFReader<double> readerb(&monBNb, GET_PATH_STR("cn/2Umax.bif"));
      readerb.proceed();

      cn = new gum::credal::CredalNet<double>(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeCPTMinMax();
    }

    // dynamic (dynaCheese network - slow)
    void initDCNet() {
#ifdef _OPENMP
      gum::setNumberOfThreads(1);
#endif
      gum::BayesNet<double> monBNa;
      gum::BIFReader<double> readera(&monBNa, GET_PATH_STR("cn/dbn_bin_min.bif"));
      readera.proceed();

      gum::BayesNet<double> monBNb;
      gum::BIFReader<double> readerb(&monBNb, GET_PATH_STR("cn/dbn_bin_max.bif"));
      readerb.proceed();

      cn = new gum::credal::CredalNet<double>(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeCPTMinMax();
    }

    void clearCNet() { delete cn; }

    // not dynamic (2U network) - with evidence
    void testL2UInference() {
      initCNet();

      gum::credal::CNLoopyPropagation<double> lp =
          gum::credal::CNLoopyPropagation<double>(*cn);

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_PATH_STR("cn/L2U.evi"));
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      // evidence from map
      std::map<std::string, std::vector<double>> eviMap;
      std::vector<double> evi0(2, 0);
      evi0[0] = 1;
      std::vector<double> evi1(2, 0);
      evi1[1] = 1;
      eviMap["L"] = evi1;
      eviMap["G"] = evi0;

      try {
        lp.insertEvidence(eviMap);
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      std::map<std::string, std::vector<double>> modals;
      std::vector<double> binaryModal(2, 0);
      binaryModal[1] = 1;

      // modalities from map
      // from file with dynamic network, not 2U
      try {
        for (const auto node : cn->current_bn().nodes())
          modals[cn->current_bn().variable(node).name()] = binaryModal;
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      /*try {
        lp.insertModals( modals );
      } catch ( gum::Exception & e ) {
        TS_ASSERT ( false );
      }*/

      try {
        lp.makeInference();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      try {
        for (const auto node : cn->current_bn().nodes()) {
          std::vector<double> inf(lp.marginalMin(node));
          std::vector<double> sup(lp.marginalMax(node));
          // double e_inf = lp.expectationMin ( node_idIt );
          // double e_sup = lp.expectationMax ( node_idIt );
        }
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      clearCNet();
    } // end of : testL2UInference (2U network)

    // dynamic (dynaCheese) - strong indep
    void testL2UInferenceD() {
      initDCNet();

      gum::credal::CNLoopyPropagation<double> lp =
          gum::credal::CNLoopyPropagation<double>(*cn);

      //////////////////////////////////////////////////////
      // strong independence
      //////////////////////////////////////////////////////

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_PATH_STR("cn/dbn_bin_evi.evi"));
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      // modalities from file
      /*try {
        lp.insertModalsFile( GET_PATH_STR ( modalities.modal ) );
      } catch ( gum::Exception & e ) {
        TS_ASSERT ( false );
      }*/

      try {
        lp.makeInference();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      try {
        for (const auto node : cn->current_bn().nodes()) {
          std::vector<double> inf(lp.marginalMin(node));
          std::vector<double> sup(lp.marginalMax(node));
          // double e_inf = lp.expectationMin ( node_idIt );
          // double e_sup = lp.expectationMax ( node_idIt );
        }
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      /*
      try {
        lp.dynamicExpectations();
      } catch ( gum::Exception & e ) {
        TS_ASSERT ( false );
      }*/
      /*
              try {
                //std::vector< double >  ekm_inf ( mcs.dynamicExpMin ( "km" ) );
                //std::vector< double >  ekm_sup ( mcs.dynamicExpMax ( "km" ) );
                //std::vector< double >  elo_inf ( mcs.dynamicExpMin ( "lo" ) );
                //std::vector< double >  elo_sup ( mcs.dynamicExpMax ( "lo" ) );
                //std::vector< double >  etemp_inf ( lp.dynamicExpMin ( "temp" ) );
                //std::vector< double >  etemp_sup ( lp.dynamicExpMax ( "temp" ) );
              } catch ( gum::Exception & e ) {
                TS_ASSERT ( false );
              }*/

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      clearCNet();
    } // end of : testL2UInferenceD

    // with dynamic network
    void testL2UListener() {
      initDCNet();
      gum::credal::CNLoopyPropagation<double> lp =
          gum::credal::CNLoopyPropagation<double>(*cn);

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_PATH_STR("cn/dbn_bin_evi.evi"));
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      // lp.inferenceType(gum::CNLoopyPropagation<double>::InferenceType::randomOrder);
      L2UListener mcl(lp);

      try {
        lp.makeInference();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      TS_ASSERT_EQUALS(mcl.nbr() * lp.periodSize() + lp.burnIn(),
                       lp.nbrIterations());
      TS_ASSERT_DIFFERS(mcl.msg(), std::string(""));

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception &e) {
        TS_ASSERT(false);
      }

      clearCNet();
    } // end of : testL2UListener

  }; // end of : class L2UInferenceTestSuite

} // end of : namespace gum_tests
