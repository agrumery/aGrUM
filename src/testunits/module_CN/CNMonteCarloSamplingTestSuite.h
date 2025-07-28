/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/core/threads/threads.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CN/credalNet.h>
#include <agrum/CN/inference/CNMonteCarloSampling.h>

#define GET_CN_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {
  ////////////////////////////////////////////////////////////////////
  class CNMonteCarloSamplingListener: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __msg;

    protected:

    public:
    CNMonteCarloSamplingListener(gum::ApproximationScheme& aS) :
        gum::ApproximationSchemeListener(aS), __nbr(0), __msg("") {};

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, const std::string& s) { __msg = s; }

    int nbr() { return __nbr; }

    std::string& msg() { return __msg; }
  };   // end of : class mcSamplingListener

  ////////////////////////////////////////////////////////////////
  class [[maybe_unused]] CNMonteCarloSamplingInferenceTestSuite: public CxxTest::TestSuite {
    private:

    protected:

    public:
    gum::credal::CredalNet< double >* cn;

    // not dynamic (2U network - fast)
    void initCNet() {
#ifdef _OPENMP
      GUM_DEBUG_ONLY(gum::setNumberOfThreads(1););
#endif

      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_CN_PATH_STR(2Umin.bif));
      readera.proceed();

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_CN_PATH_STR(2Umax.bif));
      readerb.proceed();

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
    }

    // dynamic (dynaCheese network - slow)
    void initDCNet() {
#ifdef _OPENMP
      GUM_DEBUG_ONLY(gum::setNumberOfThreads(1);)
#endif

      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_CN_PATH_STR(bn_c_3.bif));
      readera.proceed();

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_CN_PATH_STR(den_c_3.bif));
      readerb.proceed();

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      double beta = 0.8;
      cn->bnToCredal(beta, false);
    }

    void clearCNet() { delete cn; }

    // not dynamic (2U network) - with evidence
    GUM_ACTIVE_TEST(CNMonteCarloSamplingInference) {
      initCNet();

      // using gum::LazyPropagation<double> by default
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);

      // evidence from file
      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(L2U.evi));)

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)


      // evidence from map
      std::map< std::string, std::vector< double > > eviMap;
      std::vector< double >                          evi0(2, 0);
      evi0[0] = 1;
      std::vector< double > evi1(2, 0);
      evi1[1]     = 1;
      eviMap["L"] = evi1;
      eviMap["G"] = evi0;


      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertEvidence(eviMap);)

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)


      mcs.setRepetitiveInd(false);
      mcs.setMaxTime(1);

      // mcs.storeBNOpt ( true );

      std::map< std::string, std::vector< double > > modals;
      std::vector< double >                          binaryModal(2, 0);
      binaryModal[1] = 1;


      // modalities from map
      // from file with dynamic network, not 2U
      try {
        for (const auto node: cn->current_bn().nodes())
          modals[cn->current_bn().variable(node).name()] = binaryModal;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }


      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertModals(modals);)
      try {
        mcs.makeInference();
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }


      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto   pmin  = mcs.marginalMin(node);
          auto   pmax  = mcs.marginalMax(node);
          double e_inf = mcs.expectationMin(node);
          double e_sup = mcs.expectationMax(node);
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }


      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)

      clearCNet();


    }   // end of : testCNMonteCarloSamplingInference (2U network)

    // dynamic (dynaCheese) - strong indep
    GUM_ACTIVE_TEST(CNMonteCarloSamplingInferenceDStrong) {
      initDCNet();

      using exp = std::vector< double >;

      gum::credal::CNMonteCarloSampling< double > mcs(*cn);

      //////////////////////////////////////////////////////
      // strong independence
      //////////////////////////////////////////////////////

      // evidence from file

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi));)

      mcs.setRepetitiveInd(false);
      mcs.setMaxTime(1);

      // modalities from file
      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertModalsFile(GET_CN_PATH_STR(modalities.modal));)

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.makeInference();)

      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto   inf(mcs.marginalMin(node));
          auto   sup(mcs.marginalMax(node));
          double e_inf = mcs.expectationMin(node);
          double e_sup = mcs.expectationMax(node);
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.dynamicExpectations();)

      try {
        // exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
        // exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
        // exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
        // exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
        exp etemp_inf(mcs.dynamicExpMin("temp"));
        exp etemp_sup(mcs.dynamicExpMax("temp"));
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)

      clearCNet();

    }   // end of : testCNMonteCarloSamplingInferenceDStrong

    // dynamic (dynaCheese) - repetitive indep
    GUM_ACTIVE_TEST(CNMonteCarloSamplingInferenceDRep) {
      initDCNet();
      using exp = std::vector< double >;

      gum::credal::CNMonteCarloSampling< double > mcs(*cn);

      //////////////////////////////////////////////////////
      // repetitive independence
      //////////////////////////////////////////////////////

      // evidence from file
      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi));)

      mcs.setRepetitiveInd(true);
      mcs.setMaxTime(1);

      // modalities from file
      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertModalsFile(GET_CN_PATH_STR(modalities.modal));)

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.makeInference();)

      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto   inf(mcs.marginalMin(node));
          auto   sup(mcs.marginalMax(node));
          double e_inf = mcs.expectationMin(node);
          double e_sup = mcs.expectationMax(node);
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.dynamicExpectations();)

      try {
        // exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
        // exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
        // exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
        // exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
        exp etemp_inf(mcs.dynamicExpMin("temp"));
        exp etemp_sup(mcs.dynamicExpMax("temp"));
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)

      clearCNet();
    }   // end of : testCNMonteCarloSamplingInferenceDRep (dynamic - dynacheese)

    // with dynamic network
    GUM_ACTIVE_TEST(CNMonteCarloSamplingListener) {
      initDCNet();
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);

      // evidence from file
      TS_GUM_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi));)

      mcs.setRepetitiveInd(false);
      mcs.setMaxTime(1);

      /// mcs.setBurnIn( 100000 ); // so we can test time out criterion during
      /// burn
      /// in
      mcs.setPeriodSize(10);

      CNMonteCarloSamplingListener mcl(mcs);

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.makeInference();)

      /// time out criterion during burn in test
      TS_ASSERT_EQUALS(mcl.nbr() * mcs.periodSize(), mcs.nbrIterations())
      TS_ASSERT_DIFFERS(mcl.msg(), std::string(""))

      TS_GUM_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence();)

      clearCNet();
    }   // end of : testCNMonteCarloSamplingListener

  };   // end of : class CNMonteCarloSamplingInferenceTestSuite

}   // namespace gum_tests
