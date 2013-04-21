#include <iostream>
#include <string>

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/MCSampling.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/algorithms/approximationSchemeListener.h>
#include <agrum/CN/OMPThreads.h>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>


/**
 * Mono-threaded version
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {
  ////////////////////////////////////////////////////////////////////
  class MCSamplingListener : public gum::ApproximationSchemeListener {
    private :
      int __nbr;
      std::string __msg;
    protected :

    public :
      MCSamplingListener( gum::ApproximationScheme & aS ) : gum::ApproximationSchemeListener ( aS ), __nbr ( 0 ), __msg ( "" ) {};

      void whenProgress ( const void * buffer, const gum::Size a, const double b, const double c ) {
        __nbr++;
      }

      void whenStop ( const void * buffer, const std::string s ) {
        __msg = s;
      }

      int nbr() {
        return __nbr;
      }

      std::string & msg() {
        return __msg;
      }
  }; // end of : class mcSamplingListener

  ////////////////////////////////////////////////////////////////
  class MCSamplingInferenceTestSuite : public CxxTest::TestSuite {
    private :

    protected :

    public :
      gum::CredalNet< double > *cn;

      // not dynamic (2U network - fast)
      void initCNet () {
        //#ifdef NDEBUG
          gum_threads::setNumberOfThreads(1);
        //#endif
        gum::BayesNet<double> monBNa;
        gum::BIFReader< double > readera ( &monBNa, GET_PATH_STR ( cn/2Umin.bif ) );
        readera.proceed();

        gum::BayesNet<double> monBNb;
        gum::BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( cn/2Umax.bif ) );
        readerb.proceed();
        
        cn = new gum::CredalNet < double > ( monBNa, monBNb );

        cn->intervalToCredal();
      }

      // dynamic (dynaCheese network - slow)
      void initDCNet () {
        //#ifdef NDEBUG
          gum_threads::setNumberOfThreads(1);
        //#endif
        gum::BayesNet<double> monBNa;
        gum::BIFReader< double > readera ( &monBNa, GET_PATH_STR ( bn_c_3.bif ) );
        readera.proceed();

        gum::BayesNet<double> monBNb;
        gum::BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( den_c_3.bif ) );
        readerb.proceed();
        
        cn = new gum::CredalNet < double > ( monBNa, monBNb );

        double beta = 0.8;
        cn->bnToCredal(beta);
      }

      void clearCNet () {
        delete cn;
      }

      // not dynamic (2U network) - with evidence
      void /*test*/MCSamplingInference () {
        initCNet();
        gum::MCSampling < double, gum::LazyPropagation < double > > mcs ( *cn );
        
        // evidence from file
        try {
          mcs.insertEvidenceFile ( GET_PATH_STR ( L2U.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        // evidence from map
        std::map< std::string, std::vector<double> > eviMap;
        std::vector<double> evi0(2,0); evi0[0] = 1;
        std::vector<double> evi1(2,0); evi1[1] = 1;
        eviMap["L"] = evi1;
        eviMap["G"] = evi0;

        try {
          mcs.insertEvidence ( eviMap );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );
        //mcs.storeBNOpt ( true );

        std::map < std::string, std::vector < double > > modals;
        std::vector < double > binaryModal(2,0);
        binaryModal[1] = 1;

        // modalities from map
        // from file with dynamic network, not 2U
        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            modals[ cn->current_bn().variable( *node_idIt ).name() ] = binaryModal;
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.insertModals( modals );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            std::vector< double > inf ( mcs.marginalMin ( *node_idIt ) );
            std::vector< double > sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testMCSamplingInference (2U network)

      // dynamic (dynaCheese) - strong indep
      void testMCSamplingInferenceDStrong () {
        initDCNet();
        typedef std::vector< double > exp;

        gum::MCSampling < double, gum::LazyPropagation < double > > mcs ( *cn );
        
        //////////////////////////////////////////////////////
        // strong independence
        //////////////////////////////////////////////////////

        // evidence from file
        try {
          mcs.insertEvidenceFile ( GET_PATH_STR ( f_3.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        // modalities from file
        try {
          mcs.insertModalsFile( GET_PATH_STR ( modalities.modal ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }
        
        try {
          mcs.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            exp inf ( mcs.marginalMin ( *node_idIt ) );
            exp sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.dynamicExpectations();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          //exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
          //exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
          //exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
          //exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
          exp etemp_inf ( mcs.dynamicExpMin ( "temp" ) );
          exp etemp_sup ( mcs.dynamicExpMax ( "temp" ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testMCSamplingInferenceDStrong

      // dynamic (dynaCheese) - repetitive indep
      void testMCSamplingInferenceDRep () {
        initDCNet();
        typedef std::vector< double > exp;

        gum::MCSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        //////////////////////////////////////////////////////
        // repetitive independence
        //////////////////////////////////////////////////////

        // evidence from file
        try {
          mcs.insertEvidenceFile ( GET_PATH_STR ( f_3.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        mcs.setRepetitiveInd ( true );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        // modalities from file
        try {
          mcs.insertModalsFile( GET_PATH_STR ( modalities.modal ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }
        
        try {
          mcs.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
          GUM_SHOWERROR(e);
        }

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            exp inf ( mcs.marginalMin ( *node_idIt ) );
            exp sup ( mcs.marginalMax ( *node_idIt ) );
            double e_inf = mcs.expectationMin ( *node_idIt );
            double e_sup = mcs.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.dynamicExpectations();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          //exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
          //exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
          //exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
          //exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
          exp etemp_inf ( mcs.dynamicExpMin ( "temp" ) );
          exp etemp_sup ( mcs.dynamicExpMax ( "temp" ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testMCSamplingInferenceDRep (dynamic - dynacheese)


      // with dynamic network
      void testMCSamplingListener () {
        initDCNet();
        gum::MCSampling < double, gum::LazyPropagation < double > > mcs ( *cn );

        
        // evidence from file
        try {
          mcs.insertEvidenceFile ( GET_PATH_STR ( f_3.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }
        
        
        mcs.setRepetitiveInd ( false );
        mcs.setTimeLimit ( 1 );
        mcs.setEpsilon ( 0.1 );
        mcs.setIterStop ( 8 );

        MCSamplingListener mcl ( mcs );

        try {
          mcs.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        TS_ASSERT_EQUALS ( mcl.nbr() * mcs.periodSize() + mcs.burnIn(), mcs.nbrIterations() );
        TS_ASSERT_DIFFERS ( mcl.msg(), std::string( "" ) );

        try {
          mcs.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testMCSamplingListener

  }; // end of : class MCSamplingInferenceTestSuite

} // end of : namespace gum_tests
