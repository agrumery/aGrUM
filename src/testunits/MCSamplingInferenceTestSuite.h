#include <iostream>
#include <string>

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/CNMonteCarloSampling.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/algorithms/approximationSchemeListener.h>
#include <agrum/core/OMPThreads.h>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#define GET_CN_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

/*
 * Mono-threaded version
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {
  ////////////////////////////////////////////////////////////////////
  class CNMonteCarloSamplingListener : public gum::ApproximationSchemeListener {
    private :
      int __nbr;
      std::string __msg;
    protected :

    public :
      CNMonteCarloSamplingListener ( gum::ApproximationScheme &aS ) : gum::ApproximationSchemeListener ( aS ), __nbr ( 0 ), __msg ( "" ) {};

      void whenProgress ( const void *buffer, const gum::Size a, const double b, const double c ) {
        __nbr++;
      }

      void whenStop ( const void *buffer, const std::string s ) {
        __msg = s;
      }

      int nbr() {
        return __nbr;
      }

      std::string &msg() {
        return __msg;
      }
  }; // end of : class mcSamplingListener

  ////////////////////////////////////////////////////////////////
  class CNMonteCarloSamplingInferenceTestSuite : public CxxTest::TestSuite {
    private :

    protected :

    public :
      gum::credal::CredalNet< double > *cn;

      // not dynamic (2U network - fast)
      void initCNet () {
        //#ifdef NDEBUG
        gum::setNumberOfThreads ( 1 );
        //#endif
        GUM_TRACE ( GET_CN_PATH_STR ( 2Umin.bif ) );
        gum::BayesNet<double> monBNa;
        gum::BIFReader< double > readera ( &monBNa, GET_CN_PATH_STR ( 2Umin.bif ) );
        readera.proceed();

        gum::BayesNet<double> monBNb;
        gum::BIFReader< double > readerb ( &monBNb, GET_CN_PATH_STR ( 2Umax.bif ) );
        readerb.proceed();

        cn = new gum::credal::CredalNet < double > ( monBNa, monBNb );

        cn->intervalToCredal();
      }

      // dynamic (dynaCheese network - slow)
      void initDCNet () {
        //#ifdef NDEBUG
        gum::setNumberOfThreads ( 1 );

        //#endif
        gum::BayesNet<double> monBNa;
        gum::BIFReader< double > readera ( &monBNa, GET_CN_PATH_STR ( bn_c_3.bif ) );
        readera.proceed();

        gum::BayesNet<double> monBNb;
        gum::BIFReader< double > readerb ( &monBNb, GET_CN_PATH_STR ( den_c_3.bif ) );
        readerb.proceed();

        cn = new gum::credal::CredalNet < double > ( monBNa, monBNb );

        double beta = 0.8;
        cn->bnToCredal ( beta );

      }

      void clearCNet () {
        delete cn;
      }

      // not dynamic (2U network) - with evidence
      void /*test*/CNMonteCarloSamplingInference () {
        initCNet();
        gum::credal::CNMonteCarloSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        // evidence from file
        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertEvidenceFile ( GET_CN_PATH_STR ( L2U.evi ) ); );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        // evidence from map
        std::map< std::string, std::vector<double> > eviMap;
        std::vector<double> evi0 ( 2, 0 ); evi0[0] = 1;
        std::vector<double> evi1 ( 2, 0 ); evi1[1] = 1;
        eviMap["L"] = evi1;
        eviMap["G"] = evi0;

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertEvidence ( eviMap ); );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );
        //mcs.storeBNOpt ( true );

        std::map < std::string, std::vector < double > > modals;
        std::vector < double > binaryModal ( 2, 0 );
        binaryModal[1] = 1;

        // modalities from map
        // from file with dynamic network, not 2U
        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            modals[ cn->current_bn().variable ( *node_idIt ).name() ] = binaryModal;
          }
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertModals ( modals ); );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.makeInference(); );

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            std::vector< double > inf ( mcs.marginalMin ( *node_idIt ) );
            std::vector< double > sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        clearCNet();
      } // end of : testCNMonteCarloSamplingInference (2U network)

      // dynamic (dynaCheese) - strong indep
      void testCNMonteCarloSamplingInferenceDStrong () {

        initDCNet();

        typedef std::vector< double > exp;

        gum::credal::CNMonteCarloSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        //////////////////////////////////////////////////////
        // strong independence
        //////////////////////////////////////////////////////

        // evidence from file

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertEvidenceFile ( GET_CN_PATH_STR ( f_3.evi ) ); );

        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        // modalities from file
        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertModalsFile ( GET_CN_PATH_STR ( modalities.modal ) ); );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.makeInference(); );

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            exp inf ( mcs.marginalMin ( *node_idIt ) );
            exp sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.dynamicExpectations(); );

        try {
          //exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
          //exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
          //exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
          //exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
          exp etemp_inf ( mcs.dynamicExpMin ( "temp" ) );
          exp etemp_sup ( mcs.dynamicExpMax ( "temp" ) );
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        clearCNet();

      } // end of : testCNMonteCarloSamplingInferenceDStrong

      // dynamic (dynaCheese) - repetitive indep
      void testCNMonteCarloSamplingInferenceDRep () {
        initDCNet();
        typedef std::vector< double > exp;

        gum::credal::CNMonteCarloSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        //////////////////////////////////////////////////////
        // repetitive independence
        //////////////////////////////////////////////////////

        // evidence from file
        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertEvidenceFile ( GET_CN_PATH_STR ( f_3.evi ) ); );

        mcs.setRepetitiveInd ( true );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        // modalities from file
        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertModalsFile ( GET_CN_PATH_STR ( modalities.modal ) ); );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.makeInference(); );

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            exp inf ( mcs.marginalMin ( *node_idIt ) );
            exp sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.dynamicExpectations(); );

        try {
          //exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
          //exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
          //exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
          //exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
          exp etemp_inf ( mcs.dynamicExpMin ( "temp" ) );
          exp etemp_sup ( mcs.dynamicExpMax ( "temp" ) );
        } catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          TS_ASSERT ( false );
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        clearCNet();
      } // end of : testCNMonteCarloSamplingInferenceDRep (dynamic - dynacheese)

      // with dynamic network
      void testCNMonteCarloSamplingListener () {
        initDCNet();
        gum::credal::CNMonteCarloSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        // evidence from file
        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.insertEvidenceFile ( GET_CN_PATH_STR ( f_3.evi ) ); );

        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        CNMonteCarloSamplingListener mcl ( mcs );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.makeInference(); );

        TS_ASSERT_EQUALS ( mcl.nbr() * mcs.periodSize() + mcs.burnIn(), mcs.nbrIterations() );
        TS_ASSERT_DIFFERS ( mcl.msg(), std::string ( "" ) );

        TS_GUM_ASSERT_THROWS_NOTHING ( mcs.eraseAllEvidence (); );

        clearCNet();
      } // end of : testCNMonteCarloSamplingListener

  }; // end of : class CNMonteCarloSamplingInferenceTestSuite

} // end of : namespace gum_tests
