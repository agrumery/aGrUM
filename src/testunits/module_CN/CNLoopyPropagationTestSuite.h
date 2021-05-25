/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/CN/inference/CNLoopyPropagation.h>
#include <agrum/CN/credalNet.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

#include <agrum/tools/core/OMPThreads.h>

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(@LIP6)
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {

  ////////////////////////////////////////////////////////////////////
  class L2UListener: public gum::ApproximationSchemeListener {
    private:
    int          __nbr;
    std::string  __msg;

    protected:
    public:
    explicit L2UListener(gum::ApproximationScheme& aS) :
        gum::ApproximationSchemeListener(aS),  __nbr(0),  __msg(""){};

    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
       __nbr++;
    }

    void whenStop(const void* buffer, const std::string s) {  __msg = s; }

    int nbr() { return  __nbr; }

    std::string& msg() { return  __msg; }
  };   // end of : class l2uListener

  ////////////////////////////////////////////////////////////////
  class CNLooopyPropagationTestSuite: public CxxTest::TestSuite {
    private:
    protected:
    public:
    gum::credal::CredalNet< double >* cn;

    // not dynamic (2U network - fast)
    void initCNet() {
#ifdef _OPENMP
      gum::setNumberOfThreads(1);
#endif
      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa,
                                       GET_RESSOURCES_PATH("cn/2Umin.bif"));
      readera.proceed();

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb,
                                       GET_RESSOURCES_PATH("cn/2Umax.bif"));
      readerb.proceed();

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeBinaryCPTMinMax();
    }

    // dynamic (dynaCheese network - slow)
    void initDCNet() {
#ifdef _OPENMP
      gum::setNumberOfThreads(1);
#endif
      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa,
                                       GET_RESSOURCES_PATH("cn/dbn_bin_min.bif"));
      readera.proceed();

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb,
                                       GET_RESSOURCES_PATH("cn/dbn_bin_max.bif"));
      readerb.proceed();

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeBinaryCPTMinMax();
    }

    void clearCNet() { delete cn; }

    // not dynamic (2U network) - with evidence
    void testL2UInference() {
      initCNet();

      gum::credal::CNLoopyPropagation< double > lp
         = gum::credal::CNLoopyPropagation< double >(*cn);

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/L2U.evi"));
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      // evidence from map
      std::map< std::string, std::vector< double > > eviMap;
      std::vector< double >                          evi0(2, 0);
      evi0[0] = 1;
      std::vector< double > evi1(2, 0);
      evi1[1]     = 1;
      eviMap["L"] = evi1;
      eviMap["G"] = evi0;

      try {
        lp.insertEvidence(eviMap);
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      std::map< std::string, std::vector< double > > modals;
      std::vector< double >                          binaryModal(2, 0);
      binaryModal[1] = 1;

      // modalities from map
      // from file with dynamic network, not 2U
      try {
        for (const auto node: cn->current_bn().nodes())
          modals[cn->current_bn().variable(node).name()] = binaryModal;
      } catch (gum::Exception&) { TS_ASSERT(false); }

      /*try {
        lp.insertModals( modals );
      } catch ( gum::Exception & ) {
        TS_ASSERT ( false );
      }*/

      try {
        lp.makeInference();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto pmin = lp.marginalMin(node);
          auto pmax = lp.marginalMax(node);

          // double e_inf = lp.expectationMin ( node_idIt );
          // double e_sup = lp.expectationMax ( node_idIt );
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      clearCNet();
    }   // end of : testL2UInference (2U network)

    // dynamic (dynaCheese) - strong indep
    void testL2UInferenceD() {
      initDCNet();

      gum::credal::CNLoopyPropagation< double > lp
         = gum::credal::CNLoopyPropagation< double >(*cn);

      //////////////////////////////////////////////////////
      // strong independence
      //////////////////////////////////////////////////////

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/dbn_bin_evi.evi"));
      } catch (gum::Exception&) { TS_ASSERT(false); }

      // modalities from file
      /*try {
        lp.insertModalsFile( GET_RESSOURCES_PATH ( modalities.modal ) );
      } catch ( gum::Exception & ) {
        TS_ASSERT ( false );
      }*/

      try {
        lp.makeInference();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto inf(lp.marginalMin(node));
          auto sup(lp.marginalMax(node));
          // double e_inf = lp.expectationMin ( node_idIt );
          // double e_sup = lp.expectationMax ( node_idIt );
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }

      /*
      try {
        lp.dynamicExpectations();
      } catch ( gum::Exception & ) {
        TS_ASSERT ( false );
      }*/
      /*
              try {
                //std::vector< double >  ekm_inf ( mcs.dynamicExpMin ( "km" ) );
                //std::vector< double >  ekm_sup ( mcs.dynamicExpMax ( "km" ) );
                //std::vector< double >  elo_inf ( mcs.dynamicExpMin ( "lo" ) );
                //std::vector< double >  elo_sup ( mcs.dynamicExpMax ( "lo" ) );
                //std::vector< double >  etemp_inf ( lp.dynamicExpMin ( "temp" )
         );
                //std::vector< double >  etemp_sup ( lp.dynamicExpMax ( "temp" )
         );
              } catch ( gum::Exception & ) {
                TS_ASSERT ( false );
              }*/

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      clearCNet();
    }   // end of : testL2UInferenceD

    // with dynamic network
    void testL2UListener() {
      initDCNet();
      gum::credal::CNLoopyPropagation< double > lp
         = gum::credal::CNLoopyPropagation< double >(*cn);

      // evidence from file
      try {
        lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/dbn_bin_evi.evi"));
      } catch (gum::Exception&) { TS_ASSERT(false); }

      // lp.inferenceType(gum::CNLoopyPropagation<double>::InferenceType::randomOrder);
      L2UListener mcl(lp);

      try {
        lp.makeInference();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      TS_ASSERT_EQUALS(mcl.nbr() * lp.periodSize(), lp.nbrIterations());
      TS_ASSERT_DIFFERS(mcl.msg(), std::string(""));

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      clearCNet();
    }   // end of : testL2UListener

    // not dynamic (2U network) - with evidence
    void testL2UInferenceFromBug() {
      initCNet();

      gum::credal::CNLoopyPropagation< double > lp
         = gum::credal::CNLoopyPropagation< double >(*cn);

      // evidence from file
      lp.eraseAllEvidence();
      try {
        lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/L2U.evi"));
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        lp.makeInference();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        for (const auto node: cn->current_bn().nodes()) {
          auto inf(lp.marginalMin(node));
          auto sup(lp.marginalMax(node));
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }

      try {
        lp.eraseAllEvidence();
      } catch (gum::Exception&) { TS_ASSERT(false); }

      clearCNet();
    }


  };   // end of : class L2UInferenceTestSuite

}   // namespace gum_tests
