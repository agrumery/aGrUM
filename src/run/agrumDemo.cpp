/*#include "CredalNet.h"
#include "InferenceEngine.h"
#include "MCSampling.h"
#include "LocalSearch.h"
#include "LoopyPropagation.h"
#include "Errors.h"
*/

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/InferenceEngine.h>
#include <agrum/CN/MCSampling.h>
#include <agrum/CN/LocalSearch.h>
#include <agrum/CN/LoopyPropagation_v0.h>
#include <agrum/BN/inference/lazyPropagation.h>

//#include <agrum/CN/MCSamplingInferenceTestSuite.h>

using namespace gum;


#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

void test_credal() {
  std::cout << GET_PATH_STR ( bn_c.bif ) << std::endl;

  BayesNet<double> monBNa;
  BIFReader< double > readera ( &monBNa, //GET_PATH_STR ( 2Umin.bif ) );
 GET_PATH_STR ( bn_c.bif ) );
  readera.proceed();

  BayesNet<double> monBNb;
  BIFReader< double > readerb ( &monBNb, //GET_PATH_STR ( 2Umax.bif ) );
 GET_PATH_STR ( den_c.bif ) );
  readerb.proceed();

/*
  // (G)(L)2U test
  CredalNet<double> myCNb(monBNa, monBNb);

  myCNb.intervalToCredal(0);
  std::cout << "computing min/max vertex" << std::endl;
  myCNb.computeCPTMinMax();
  std::cout << "computing done" << std::endl;
  LoopyPropagation<double> lp = LoopyPropagation<double>(myCNb, myCNb.current_bn());

  //lp.insertEvidence ( GET_PATH_STR ( L2U.evi ) );
  std::map< std::string, std::vector<double> > eviMap;
  std::vector<double> evi0(2,0); evi0[0] = 1;
  std::vector<double> evi1(2,0); evi1[1] = 1;
  eviMap["L"] = evi1;
  eviMap["G"] = evi0;
  lp.insertEvidence(eviMap);

  lp.makeInference();
  lp.saveInference("test.marginals");
  lp.saveMarginals("l2u.marginals");

  
  // LocalSearch test
   
  LocalSearch<double, LazyPropagation<double> > ls(myCNb);
  //ls.insertEvidence( GET_PATH_STR ( L2U.evi ) );
  ls.insertEvidence(eviMap);
  ls.setPassN(5);
  ls.setMaxVertices(4);
  ls.makeInference();
*/

  // dynacheese modalities
  std::map< std::string, std::vector< double > > modals;
  typename std::vector< double > km ( 5 );
  typename std::vector< double > lo ( 8 );
  typename std::vector< double > Temp ( 3 );

  for ( double k = 5.5, p = 0; k <= 7.5; k = k + 0.5, p = p + 1 )
    km[p] = k;

  for ( double k = 0, p = 0; k <= 14; k = k + 2, p = p + 1 )
    lo[p] = k;

  Temp[0] = 8;
  Temp[1] = 12;
  Temp[2] = 16;

  modals["km"] = km;
  modals["lo"] = lo;
  modals["temp"] = Temp;

  // not dynamic network test (expectations) with 2U network
  std::map< std::string, std::vector< double > > modals2;
  std::vector< double > L(2);
  L[0] = 0;
  L[1] = 1;
  modals2["A"] = L;
  modals2["B"] = L;
  modals2["C"] = L; //...

  for ( int i = 0; i < 1; i++ ) {
    for ( int j = 1; j < 2; j++ ) {
      CredalNet<double> * myCNa = new CredalNet<double> ( monBNa, monBNb );
      //myCNa->intervalToCredal(0); // 2U network

       
      if ( i == 0 )
        myCNa->bnToCredal ( 0.95, false );

      if ( i == 1 )
        myCNa->bnToCredal ( 0.8, false );

      if ( i == 2 )
        myCNa->bnToCredal ( 0.85, false );

      myCNa->saveBNsMinMax("min.bif", "max.bif"); // interval BNs saved

      //std::cout << myCNa->toString() << std::endl;
      // dynacheese network
      MCSampling<double, LazyPropagation<double> > * MCE = new MCSampling<double, LazyPropagation<double> > ( *myCNa );

      // 2U network
      //MCSampling<double, LazyPropagation<double> > * MCE = new MCSampling<double, LazyPropagation<double> > ( myCNb );



      //MCE->insertModals(modals2); //L2U modals
      //MCE->insertModals( GET_PATH_STR ( modalities.modal ) ); //dyna cheese
      //MCE->insertModals(modals); //dyna cheese

      MCE->setRepetitiveInd ( false );
      MCE->setTimeLimit ( 1 );

      if ( j == 0 )
        MCE->insertEvidence ( GET_PATH_STR ( forward.evi ) );

      if ( j == 1 )
        //MCE->insertEvidence ( GET_PATH_STR ( L2U.evi ) );
        MCE->insertEvidence ( GET_PATH_STR ( fb.evi ) );

      //if(j==2)
      //MCE.insertEvidence("./temp.evi");

      MCE->makeInference_v3();

      //std::vector<double> toto(MCE->dynamicExpMin("A"));
      //std::cout << toto << std::endl;

      //std::vector<double> toto2(MCE->marginalMin("km"));
      //std::cout << toto2 << std::endl;

      if ( i == 0 && j == 0 ) {
        //MCE->saveMarginals("./MCr_0.6c_f.mar");
        MCE->saveExpectations ( "./MCs_0.6c_f.exp" );
      } else if ( i == 1 && j == 0 ) {
        //MCE->saveMarginals("./MCr_0.7c_f.mar");
        MCE->saveExpectations ( "./MCs_0.7c_f.exp" );
      } else if ( i == 2 && j == 0 ) {
        //MCE->saveMarginals("./MCr_0.8c_f.mar");
        MCE->saveExpectations ( "./MCs_0.95c_f.exp" );
      } else if ( i == 0 && j == 1 ) {
        MCE->saveMarginals("./MCr_0.95c_fb.mar");
        MCE->saveExpectations ( "./MCs_0.95c_fb.exp" );
      } else if ( i == 1 && j == 1 ) {
        //MCE->saveMarginals("./MCs_0.95c_fb.mar");
        MCE->saveExpectations ( "./MCs_0.95c_fb.exp" );
      } else if ( i == 2 && j == 1 ) {
        //MCE->saveMarginals("./MCs_0.8_fb.mar");
        MCE->saveExpectations ( "./MCs_0.8c_fb.exp" );
      }

      //MCE.saveVertices("./SMCVr.res");
      //MCE.saveExpectations("./rep.expect");
      MCE->eraseAllEvidence();
      delete MCE;
      delete myCNa;
    }
  }

  /*
    std::cout << "MC strong" << std::endl;

    MCSampling<double, gum::LazyPropagation<double> > MCE2(myCNa);
    MCE2.setRepetitiveInd(false);
    MCE2.setTimeLimit(20);
    //MCE2.insertEvidence("./forward.evi");
    MCE2.makeInference();
    MCE2.saveMarginals("./SMCs.res");
    MCE2.saveVertices("./SMCVs.res");
    MCE2.eraseAllEvidence();
  */
  //std::cout << "LS" << std::endl;

  //LocalSearch<double, gum::LazyPropagation<double> > LSa(myCNa);
  //LSa.setRepetitiveInd(true);
  //LSa.insertEvidence("./evi.evi");
  //LSa.makeInference();
  //LSa.saveMarginals("./LS.res");

  /*
    gum::BayesNet<double> monBN;
    gum::BIFReader< double > reader(&monBN, "./2Umin.bif");
    reader.proceed();

    gum::BayesNet<double> monBN5;
    gum::BIFReader< double > reader5(&monBN5, "./2Umax.bif");
    reader5.proceed();
  */

  /*
    std::string dot_bn = monBN.toDot();
    std::ofstream h_file("bn.dot", std::ios::out | std::ios::trunc);
    h_file << dot_bn;
    h_file.close();
    return 0;
  */

  /*
    CredalNet<double> myCN(monBN, monBN5);

    myCN.intervalToCredal();

    std::cout << myCN.toString();

    LocalSearch<double, gum::LazyPropagation<double> > LS(myCN);

    LS.insertEvidence("./evi.evi");
    LS.insertQuery("./evi.evi");

    LS.setPassN(4);
    LS.setMaxVertices(10);
    LS.setRepetitiveInd(true);

    LS.makeInference();

    LS.saveMarginals("./LS.res");

    MCSampling<double, gum::LazyPropagation<double> > MCE(myCN);
    MCE.insertEvidence("./evi.evi");
    MCE.insertQuery("./evi.evi");
    //MCE.setIterStop(1000000000);
    MCE.makeInference();
    MCE.saveMarginals("./mc.res");

    std::cout << "dts" << std::endl;
    myCN.dts();

    std::cout << myCN.toString();
  */
}

int main ( int argc, char *argv[] ) {
  try {
    test_credal();
  } catch (gum::Exception& e) {
    GUM_SHOWERROR(e);
  }
  return EXIT_SUCCESS;
}
