#include <iostream>
#include <string>

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/LoopyPropagation.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/approximationSchemeListener.h>

#include <cxxtest/AgrumTestSuite.h>

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

#include <agrum/CN/OMPThreads.h>

/**
 * Mono-threaded version
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {
  using namespace gum;
  ////////////////////////////////////////////////////////////////////
  class L2UListener : public gum::ApproximationSchemeListener {
    private :
      int __nbr;
      std::string __msg;
    protected :

    public :
      L2UListener( gum::ApproximationScheme & aS ) : gum::ApproximationSchemeListener ( aS ), __nbr ( 0 ), __msg ( "" ) {};

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
  }; // end of : class l2uListener

  ////////////////////////////////////////////////////////////////
  class L2UInferenceTestSuite : public CxxTest::TestSuite {
    private :

    protected :

    public :
      gum::CredalNet< double > *cn;

      // not dynamic (2U network - fast)
      void initCNet () {
        //#ifdef NDEBUG
          gum_threads::setNumberOfThreads(1);
        //#endif
        BayesNet<double> monBNa;
        BIFReader< double > readera ( &monBNa, GET_PATH_STR ( 2Umin.bif ) );
        readera.proceed();

        BayesNet<double> monBNb;
        BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( 2Umax.bif ) );
        readerb.proceed();
        
        cn = new gum::CredalNet < double > ( monBNa, monBNb );

        cn->intervalToCredal();
      }

      // dynamic (dynaCheese network - slow)
      void initDCNet () {
        //#ifdef NDEBUG
          gum_threads::setNumberOfThreads(1);
        //#endif
        BayesNet<double> monBNa;
        BIFReader< double > readera ( &monBNa, GET_PATH_STR ( bn_c_3.bif ) );
        readera.proceed();

        BayesNet<double> monBNb;
        BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( den_c_3.bif ) );
        readerb.proceed();
        
        cn = new gum::CredalNet < double > ( monBNa, monBNb );

        double beta = 0.8;
        cn->bnToCredal(beta);
      }

      void clearCNet () {
        delete cn;
      }

      // not dynamic (2U network) - with evidence
      void testL2UInference () {
        initCNet();
        LoopyPropagation<double> lp = LoopyPropagation<double>(*cn, cn->current_bn());
        // evidence from file
        try {
          lp.insertEvidenceFile ( GET_PATH_STR ( L2U.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.eraseAllEvidence ();
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
          lp.insertEvidence ( eviMap );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

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
          lp.insertModals( modals );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            std::vector< double > inf ( lp.marginalMin ( *node_idIt ) );
            std::vector< double > sup ( lp.marginalMax ( *node_idIt ) );
            //double e_inf = lp.expectationMin ( *node_idIt );
            //double e_sup = lp.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testL2UInference (2U network)

      // dynamic (dynaCheese) - strong indep
      void testL2UInferenceD () {
        initDCNet();
        typedef std::vector< double > exp;

        LoopyPropagation<double> lp = LoopyPropagation<double>(*cn, cn->current_bn());
        
        //////////////////////////////////////////////////////
        // strong independence
        //////////////////////////////////////////////////////

        // evidence from file
        try {
          lp.insertEvidenceFile ( GET_PATH_STR ( f_3.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        // modalities from file
        /*try {
          lp.insertModalsFile( GET_PATH_STR ( modalities.modal ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }*/
        
        try {
          lp.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          for ( gum::DAG::NodeIterator node_idIt = cn->current_bn().beginNodes(); node_idIt != cn->current_bn().endNodes(); ++node_idIt ) {
            exp inf ( lp.marginalMin ( *node_idIt ) );
            exp sup ( lp.marginalMax ( *node_idIt ) );
            double e_inf = lp.expectationMin ( *node_idIt );
            double e_sup = lp.expectationMax ( *node_idIt );
          }
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.dynamicExpectations();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          //exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
          //exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
          //exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
          //exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
          exp etemp_inf ( lp.dynamicExpMin ( "temp" ) );
          exp etemp_sup ( lp.dynamicExpMax ( "temp" ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        try {
          lp.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testL2UInferenceD

      
      // with dynamic network
      void testL2UListener () {
        initDCNet();
        LoopyPropagation<double> lp = LoopyPropagation<double>(*cn, cn->current_bn());
        
        // evidence from file
        try {
          lp.insertEvidenceFile ( GET_PATH_STR ( f_3.evi ) );
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }
       
        L2UListener mcl ( lp );

        try {
          lp.makeInference();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        TS_ASSERT_EQUALS ( mcl.nbr() * lp.periodSize() + lp.burnIn(), lp.nbrIterations() );
        TS_ASSERT_DIFFERS ( mcl.msg(), std::string( "" ) );

        try {
          lp.eraseAllEvidence ();
        } catch ( gum::Exception & e ) {
          TS_ASSERT ( false );
        }

        clearCNet();
      } // end of : testL2UListener

  }; // end of : class L2UInferenceTestSuite

} // end of : namespace gum_tests
