#include <agrum/CN/CredalNet.h>
#include <agrum/CN/CNInferenceEngine.h>
#include <agrum/CN/MCSampling.h>
//#include <agrum/CN/LocalSearch.h>
#include <agrum/CN/LoopyPropagation.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <agrum/CN/ExtensiveCredalNet.h>

//#include <agrum/CN/MCSamplingInferenceTestSuite.h>

#include <agrum/CN/OMPThreads.h>

#include <agrum/CN/OptBN.h>

using namespace gum;

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

void test_credal() {

/////////////// OMP test stuff ///////////////////

  std::cout << "isOMP () ? : " << gum_threads::isOMP() << std::endl;
  std::cout << "threads : " << gum_threads::getMaxNumberOfThreads() << std::endl;
  gum_threads::setNumberOfThreads( gum_threads::getNumberOfLogicalProcessors()*2 );
  std::cout << "new number : " << gum_threads::getMaxNumberOfThreads() << std::endl;

  std::cout << "number of procs : " << gum_threads::getNumberOfLogicalProcessors() << std::endl;

  //gum_threads::setDynamicThreadsNumber(true);
  //gum_threads::setNestedParallelism(true);

  std::cout << "dynamic threads : " << gum_threads::getDynamicThreadsNumber() << std::endl;
  std::cout << "nested parallelism : " << gum_threads::getNestedParallelism() << std::endl;
  std::cout << "thread limit : " << omp_get_thread_limit() << std::endl;
  std::cout << "nested max level : " << omp_get_max_active_levels() << std::endl;

  //gum_threads::setNumberOfThreads(1);

  
//////////////////////////////////////////////////
  std::cout << GET_PATH_STR ( bn_c.bif ) << std::endl;

  BayesNet<double> monBNa;
  BIFReader< double > readera ( &monBNa, GET_PATH_STR ( /*bn_c_3.bif*/2Umin.bif ) );
 //GET_PATH_STR ( bn_c.bif ) );
  readera.proceed();

  BayesNet<double> monBNb;
  BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( /*den_c_3.bif*/2Umax.bif ) );
 //GET_PATH_STR ( den_c.bif ) );
  readerb.proceed();


  // (G)(L)2U test
  CredalNet<double> myCNb(monBNa, monBNb);

/*
  myCNb.testFrac();
  return;
*/

  //myCNb.bnToCredal(0.5);

  myCNb.intervalToCredal();
  //std::cout << "computing min/max vertex" << std::endl;
  myCNb.computeCPTMinMax();
  //std::cout << "computing done" << std::endl;
  LoopyPropagation<double> lp = LoopyPropagation<double>(myCNb);

  //lp.insertEvidence ( GET_PATH_STR ( L2U.evi ) );
  std::map< std::string, std::vector<double> > eviMap;
  std::vector<double> evi0(2,0); evi0[0] = 1;
  std::vector<double> evi1(2,0); evi1[1] = 1;
  eviMap["L"] = evi1;
  eviMap["G"] = evi0;

  lp.insertEvidence(eviMap);

  std::map< std::string, std::vector< double > > Umodals;
  std::vector< double > Lm(2);
  Lm[0] = 0;
  Lm[1] = 1;
  Umodals["A"] = Lm;
  Umodals["B"] = Lm;
  Umodals["C"] = Lm;

  lp.insertModals(Umodals);

  //lp.inferenceType(LoopyPropagation<double>::InferenceType::randomOrder);

  lp.makeInference();
  
  lp.saveInference ( GET_PATH_STR ( test.marginals ) );
  lp.saveMarginals ( GET_PATH_STR ( l2u.marginals ) );

  for ( gum::DAG::NodeIterator id = myCNb.src_bn().beginNodes(); id != myCNb.src_bn().endNodes(); ++id ) {
    unsigned int dSize = myCNb.src_bn().variable(*id).domainSize();
    for( unsigned int mod = 0; mod < dSize; mod++ ) {
      std::cout << "l2u p(" << myCNb.src_bn().variable(*id).name() << " = " << mod  << ") = [ " << lp.marginalMin(*id)[mod] << ", " << lp.marginalMax(*id)[mod] << " ] " << std::endl;
    }
  }

  std::cout << lp.expectationMin(0) << std::endl;

  std::cout << lp.messageApproximationScheme() << std::endl;

  std::cout << lp.getApproximationSchemeMsg() << std::endl;

  //lp.eraseAllEvidence();

  // agrum hashmap with bool vectors test
  /*
  std::hash< std::vector< bool > > _vectHash;

  gum::HashTable< unsigned long int, std::vector< unsigned int > > myMap;
  std::vector< unsigned int > myKey(3);
  myKey[2] = 1;
  unsigned long int key = _vectHash(std::vector<bool>(4));
  myMap.insert(key, myKey);
  std::cout << myMap[key] << std::endl;

  std::cout << "sizes" << std::endl;
  std::cout << "size_t : " << sizeof(size_t) << std::endl;
  std::cout << "key : " << sizeof(key) << std::endl;
  std::cout << "unsigned int : " << sizeof(unsigned int) << std::endl;
  std::cout << "unsigned long int : " << sizeof(unsigned long int) << std::endl;
  */

  /*
  gum::HashTable< std::vector< bool >, std::vector< int> > myMap;
  std::vector< bool > myKey(4);
  myKey[2] = 1;
  std::vector< int > myValue(2);
  myValue[1] = 1;
  myMap.insert(myKey, myValue);
  std::cout << myMap[myKey] << std::endl;
  */
  
  //return;
  

  // LocalSearch test
/*  
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

  ////////////////////////////////////////////
 

  
  ////////////////////////////////////////////

  for ( int i = 0; i < 1; i++ ) {
    for ( int j = 1; j < 2; j++ ) {
      CredalNet<double> * myCNa = new CredalNet<double> ( monBNa, monBNb );
      myCNa->intervalToCredal(); // 2U network

       
      /*if ( i == 0 )
        myCNa->bnToCredal ( 0.95 );

      if ( i == 1 )
        myCNa->bnToCredal ( 0.8 );

      if ( i == 2 )
        myCNa->bnToCredal ( 0.85);*/

      //myCNa->saveBNsMinMax("min.bif", "max.bif"); // interval BNs saved

      //std::cout << myCNa->toString() << std::endl;
      // dynacheese network
      MCSampling<double, LazyPropagation<double> > * MCE = new MCSampling<double, LazyPropagation<double> > ( *myCNa );

      // 2U network
      //MCSampling<double, LazyPropagation<double> > * MCE = new MCSampling<double, LazyPropagation<double> > ( myCNb );



      MCE->insertModals(modals2); //L2U modals
      //MCE->insertModalsFile( GET_PATH_STR ( modalities.modal ) ); //dyna cheese
      //MCE->insertModals(modals); //dyna cheese

      MCE->setRepetitiveInd ( false );
      MCE->setTimeLimit ( 1 );
      MCE->setIterStop ( 1000 );

      MCE->storeBNOpt ( true );

      if ( j == 0 )
        MCE->insertEvidenceFile ( GET_PATH_STR ( forward.evi ) );

      if ( j == 1 )
        MCE->insertEvidenceFile ( GET_PATH_STR ( L2U.evi ) );
        //MCE->insertEvidenceFile ( GET_PATH_STR ( fb.evi ) );

      //if(j==2)
      //MCE.insertFileOfEvidence("./temp.evi");

      //MCE->setIterStop(1001);
      //MCE->makeInference_v2();

      //MCE->makeInference_v3();

/*      
      MCSampling<double, LazyPropagation<double> > * MCE2 = new MCSampling<double, LazyPropagation<double> > ( *myCNa );
      MCE2->insertEvidenceFile ( GET_PATH_STR ( fb.evi ) );
      MCE2->setRepetitiveInd ( false );
      MCE2->setTimeLimit ( 5 );
      MCE2->insertModals( modals ); //dyna cheese

      unsigned int sigmaMod = 0;
      unsigned int sigmaExp = 0;
      for( Size var = 0; var < myCNa->current_bn().size(); var++ ) {
        sigmaMod += myCNa->current_bn().variable(var).domainSize()*2;
        sigmaExp += 2;
      }

      std::cout << " marginals (min+max) : " << sigmaMod << std::endl;
      std::cout << " expecttions (min+max) : " << sigmaExp << std::endl;


      std::cout << "\n begin compare v2, v3 \n" << std::endl;
      gum::Timer chrono2;
      chrono2.reset();
      MCE2->makeInference_v2();
      double timeElapsed = chrono2.step();
      std::cout << " v2 timeElapsed : " << timeElapsed << std::endl;
      std::cout << " iters : " << MCE2->nbrIterations() << std::endl;
      std::cout << " iters/sec : " << MCE2->nbrIterations()*1.0 / timeElapsed << std::endl;

    
      gum::Timer chrono;
      chrono.reset();
      MCE->makeInference_v3();
      timeElapsed = chrono.step();
      std::cout << " v3 timeElapsed : " << timeElapsed << std::endl;
      std::cout << " iters : " << MCE->nbrIterations() << std::endl;
      std::cout << " iters/sec : " << MCE->nbrIterations()*1.0 / timeElapsed << std::endl;

      MCSampling<double, LazyPropagation<double> > * MCE3 = new MCSampling<double, LazyPropagation<double> > ( *myCNa );
      MCE3->insertEvidenceFile ( GET_PATH_STR ( fb.evi ) );
      MCE3->setRepetitiveInd ( false );
      MCE3->setTimeLimit ( 5 );
      MCE3->insertModals( modals ); //dyna cheese

      gum::Timer chrono3;
      chrono3.reset();
      MCE3->makeInference();
      timeElapsed = chrono3.step();
      std::cout << " v1 timeElapsed : " << timeElapsed << std::endl;


      std::cout << " marginals delta : " << std::endl;
      double delta = 0;
      unsigned int v3b = 0, v2b = 0, v23e = 0;
      for( Size var = 0; var < myCNa->current_bn().size(); var++ ) {
        Size dSize = myCNa->current_bn().variable(var).domainSize();
        for( Size mod = 0; mod < dSize; mod++ ) {
          double diff = MCE->marginalMin(var)[mod] - MCE2->marginalMin(var)[mod];
          if (diff < 0) {
            diff = -diff;
            v3b++;
          } else if (diff > 0) {
            v2b++;
          }
          else {
            v23e++;
          }
          delta = (delta < diff) ? diff : delta;

          diff = MCE->marginalMax(var)[mod] - MCE2->marginalMax(var)[mod];
          if (diff < 0) {
            diff = -diff;
            v2b++;
          } else if(diff > 0){
            v3b++;
          }
          else {
            v23e++;
          }
          delta = (delta < diff) ? diff : delta;
        }
      }

      std::cout << " v3 better than v2 : " << v3b << std::endl;
      std::cout << " v2 better than v3 : " << v2b << std::endl;
      std::cout << " V3 == v2 : " << v23e << std::endl;

      std::cout << " expectations delta : " << std::endl;
      delta = 0;
      v3b = 0, v2b = 0, v23e = 0;
      for( Size var = 0; var < myCNa->current_bn().size(); var++ ) {
        double diff = MCE->expectationMin(var) - MCE2->expectationMin(var);
        if (diff < 0) {
          diff = -diff;
          v3b++;
        } else if (diff > 0) {
          v2b++;
        }
        else {
          v23e++;
        }
        delta = (delta < diff) ? diff : delta;

        diff = MCE->expectationMax(var) - MCE2->expectationMax(var);
        if (diff < 0) {
          diff = -diff;
          v2b++;
        } else if(diff > 0){
          v3b++;
        }
        else {
          v23e++;
        }
        delta = (delta < diff) ? diff : delta;
      }

      std::cout << " v3 better than v2 : " << v3b << std::endl;
      std::cout << " v2 better than v3 : " << v2b << std::endl;
      std::cout << " V3 == v2 : " << v23e << std::endl;
*/

      //MCE2->eraseAllEvidence();
      //MCE3->eraseAllEvidence();

      //std::vector<double> toto(MCE->dynamicExpMin("A"));
      //std::cout << toto << std::endl;

      //std::vector<double> toto2(MCE->marginalMin("km"));
      //std::cout << toto2 << std::endl;
      //
      MCE->makeInference();
std::cout << MCE->messageApproximationScheme() << std::endl;
std::cout << MCE->currentTime() << std::endl;
std::cout << MCE->nbrIterations() << std::endl;


std::cout << "MCS p(A) = [ " << MCE->marginalMin(0)[1] << ", " << MCE->marginalMax(0)[1] << " ] " << std::endl;




std::vector< unsigned int > key(3);
//key[0] = 0; key[1] = 1; key[2] = 0;
//const std::vector< unsigned int > key(key1);

typedef std::vector< bool > dBN;
//////////////////////////////////////////////////////////////////////////
// this stuff can lead to deletes in wrong places
// DO NOY COPY anything !
OptBN<double> * opt = MCE->getOptBN(); // & replaced by * to be sure 

for ( gum::DAG::NodeIterator id = myCNb.src_bn().beginNodes(); id != myCNb.src_bn().endNodes(); ++id ) {
  std::cout << myCNb.src_bn().variable(*id).name() << "\t";
}
std::cout << std::endl;

for ( gum::DAG::NodeIterator id = myCNb.src_bn().beginNodes(); id != myCNb.src_bn().endNodes(); ++id ) {
  unsigned int dSize = myCNb.src_bn().variable(*id).domainSize();
  std::cout << "bn opts de : " << myCNb.src_bn().variable(*id).name() << std::endl;
  for( unsigned int mod = 0; mod < dSize; mod++ ) {
    key[0] = *id;
    key[1] = mod;
    key[2] = 0; // min
    std::vector< std::vector< std::vector< std::vector< bool > > > > tOpts = opt->getFullBNOptsFromKey( key ); 
    
    std::cout << "min sur : " << mod << std::endl;
    unsigned int bnSet = tOpts.size();
    
    if ( bnSet > 10 ) {
      std::cout << "more than 10" << std::endl;
    }
    else {

      for ( unsigned int bn = 0; bn < bnSet; bn++ ) {
        std::cout << tOpts[bn] << std::endl;
      }
    }

    key[2] = 1; // max
    const std::vector< std::vector< std::vector< std::vector< bool > > > > & tOpts2 = opt->getFullBNOptsFromKey( key );
    bnSet = tOpts2.size();

    std::cout << "max sur : " << mod << std::endl;

    if ( bnSet > 10 ) {
      std::cout << "more than 10" << std::endl;
    }
    else {
      for ( unsigned int bn = 0; bn < bnSet; bn++ ) {
        std::cout << tOpts2[bn] << std::endl;
      }
    }

  }

}
    
   


//////////////////////////////////////////////////////////////////////////
/*unsigned int bnSet = tOpts.size();
std::cout << "bn opts de nodeid  : " << bnSet << std::endl;
for ( unsigned int bn = 0; bn < bnSet; bn++ ) {
  std::cout << tOpts[bn] << std::endl;
}

const std::vector< dBN* > & tOpts2 = opt->getBNOptsFromKey( key );
for ( unsigned int bn = 0; bn < bnSet; bn++ ) {
  std::cout << *(tOpts2[bn]) << std::endl;
}

for(unsigned int i = 0; i < tOpts2[0]->size(); i++)
  std::cout << (*(tOpts2[0]))[i] << " ";
std::cout << std::endl;*/

std::cout << "deleted nets (useless sample) : " << MCE->notOptDelete << std::endl;

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
        MCE->saveMarginals(GET_PATH_STR ( MCr_0.95c_fb.mar ) );
        MCE->saveExpectations ( GET_PATH_STR ( MCs_0.95c_fb.exp ) );
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
      //delete MCE2;
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

  std::cout << "Press ENTER to continue...";
  std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

  return EXIT_SUCCESS;
}
