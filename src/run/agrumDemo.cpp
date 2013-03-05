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
#include <agrum/CN/LoopyPropagation.h>
#include <agrum/BN/inference/lazyPropagation.h>

using namespace gum;


#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

void test_credal() {
  std::cout << GET_PATH_STR ( bn_c.bif ) << std::endl;

  BayesNet<double> monBNa;
  BIFReader< double > readera ( &monBNa, GET_PATH_STR ( bn_c.bif ) );
  readera.proceed();

  BayesNet<double> monBNb;
  BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( den_c.bif ) );
  readerb.proceed();

  //std::cout << myCNa.toString() << std::endl;
  //myCNa.intervalToCredal(0);

  //std::cout << myCNa.toString();
  //
  //std::cout << "LP" << std::endl;

  //LoopyPropagation< double > lp(myCNa);
  //lp.makeInference();

  //std::cout << "MC rep" << std::endl;

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

  for ( int i = 0; i < 1; i++ ) {
    for ( int j = 1; j < 2; j++ ) {
      CredalNet<double> * myCNa = new CredalNet<double> ( monBNa, monBNb );

      //myCNa.intervalToCredal(10); // IDM s = 10
      if ( i == 0 )
        myCNa->bnToCredal ( 0.95, false );

      if ( i == 1 )
        myCNa->bnToCredal ( 0.8, false );

      if ( i == 2 )
        myCNa->bnToCredal ( 0.85, false );

      //std::cout << myCNa->toString() << std::endl;

      MCSampling<double, LazyPropagation<double> > * MCE = new MCSampling<double, LazyPropagation<double> > ( *myCNa );

      //MCE->insertModals(modals);
      MCE->insertModals( GET_PATH_STR ( modalities.modal ) );

      MCE->setRepetitiveInd ( false );
      MCE->setTimeLimit ( 1 );

      if ( j == 0 )
        MCE->insertEvidence ( GET_PATH_STR ( forward.evi ) );

      if ( j == 1 )
        MCE->insertEvidence ( GET_PATH_STR ( fb.evi ) );

      //if(j==2)
      //MCE.insertEvidence("./temp.evi");

      MCE->makeInference();

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
      //MCE->eraseAllEvidence();
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
