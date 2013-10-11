#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include "../agrum/CN/CredalNet.h"



#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/algorithms/approximationSchemeListener.h>
#include <agrum/core/OMPThreads.h>
//<<<<<<< HEAD

#include <agrum/CN/LrsWrapper.h>

//<<<<<<< HEAD
//#include <agrum/CN/CNMonteCarloSamplingInferenceTestSuite.h>
//=======
//#include <agrum/PRM/PRMFactory.h>
//#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER

//#include <agrum/PRM/o3prm/cocoR/Parser.h>
//#endif //DOXYGEN_SHOULD_SKIP_THIS
//#include <agrum/PRM/o3prm/O3prmReader.h>
//>>>>>>> Bug found for test -t all : now use of string::size() instead of string::length()


#include <agrum/core/Rational.h> // custom decimal to rational
#include <agrum/core/pow.h> // custom pow functions with integers, faster implementation

//=======

#include <agrum/CN/LrsWrapper.h>

//#include <agrum/PRM/PRMFactory.h>
//#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER

//#include <agrum/PRM/o3prm/cocoR/Parser.h>
//#endif //DOXYGEN_SHOULD_SKIP_THIS
//#include <agrum/PRM/o3prm/O3prmReader.h>


//#include <agrum/core/Rational.h> // custom decimal to rational
//#include <agrum/core/pow.h> // custom pow functions with integers, faster implementation

//>>>>>>> Bug found for test -t all : now use of string::size() instead of string::length()

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/CNMonteCarloSampling.h>
#include <agrum/CN/CNLoopyPropagation.h>

#include <agrum/CN/LpInterface.h>

#include <agrum/core/Rational.h>

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

void test_credal() {

  /////////////// OMP test stuff ///////////////////

  std::cout << "isOMP () ? : " << gum::isOMP() << std::endl;
  std::cout << "threads : " << gum::getMaxNumberOfThreads() << std::endl;
  gum::setNumberOfThreads ( gum::getNumberOfLogicalProcessors() * 2 );
  std::cout << "new number : " << gum::getMaxNumberOfThreads() << std::endl;

  std::cout << "number of procs : " << gum::getNumberOfLogicalProcessors() << std::endl;

  //gum::setDynamicThreadsNumber(true);
  //gum::setNestedParallelism(true);

  std::cout << "dynamic threads : " << gum::getDynamicThreadsNumber() << std::endl;
  std::cout << "nested parallelism : " << gum::getNestedParallelism() << std::endl;
  std::cout << "thread limit : " << omp_get_thread_limit() << std::endl;
  std::cout << "nested max level : " << omp_get_max_active_levels() << std::endl;

  ////////////////////////////////

  /*
  gum::Rational< double >::testRationalAlgorithms();
  return;

  gum::credal::CredalNet< double > nCN;

  gum::NodeId a = nCN.addNode( "toto", 2 );
  gum::NodeId b = nCN.addNode( "titi", 3 );
  gum::NodeId c = nCN.addNode( "tata", 3 );

  nCN.addArc( a, c );
  nCN.addArc( b, c );

  std::vector< double > lower( 2, 0 );
  std::vector< double > upper( 2, 1 );

  std::vector< std::vector< double > > v {{0.2, 0.8}};
  std::vector< std::vector< double > > v1 {{0.2, 0.4, 0.4}};

  gum::Instantiation ins( nCN.current_bn().cpt( a ) );
  nCN.setCPT(a, ins, v);

  ins = gum::Instantiation( nCN.current_bn().cpt( b ) );
  nCN.setCPT(b, ins, v1);

  ins = gum::Instantiation( nCN.current_bn().cpt( c ) );

  gum::credal::lp::LpInterface< double > llp;
  auto cols = llp.addCols( 3 );

  for( ; ! ins.end(); ) {
    for ( const auto col : cols )
      llp.addRow( rand() / RAND_MAX  <= col );

    llp.addRow( cols[ rand() % cols.size() ] <= cols[ rand() % cols.size() ] );

    llp.addSumIsOne ();

    nCN.setCPT(c, ins, llp.solve());
    ins += 3; // card ( c ) = 3

    llp.clearRows();
  }


  //nCN.fillConstraints( a, lower, upper );

  //nCN.fillConstraints( b, std::vector< double >( 6, 0 ), std::vector< double >( 6, 1 ) );

  //nCN.intervalToCredal();

  std::cout << nCN.toString() << std::endl;

  return;
  */

  /////////////////////////////

  /*
  gum::credal::CredalNet< double > myCN ( GET_PATH_STR( numerators.bif ) );
  myCN.idmLearning( 0, true );
  std::cout << myCN.toString() << std::endl;

  gum::credal::CredalNet< double > myCN1 ( GET_PATH_STR( numerators.bif ) );
  myCN1.lagrangeNormalization();
  myCN1.idmLearning( 0, true );
  std::cout << myCN1.toString() << std::endl;

  gum::credal::CredalNet< double > myCN2 ( GET_PATH_STR( numerators.bif ) );
  myCN2.bnToCredal( 0.8, true, true );
  std::cout << myCN2.toString() << std::endl;

  gum::credal::CredalNet< double > myCN3 ( GET_PATH_STR( numerators.bif ) );
  myCN3.lagrangeNormalization();
  */

  /*myCN3.idmLearning( 10, true );
  std::cout << myCN3.toString() << std::endl;*/

  /*
  myCN3.bnToCredal( 0.8, true, true );
  std::cout << myCN3.toString() << std::endl;

  return;
  */

  //////////////////////////////////



  std::cout << GET_PATH_STR ( bn_c.bif ) << std::endl;

  gum::BayesNet<double> monBNa;
  gum::BIFReader< double > readera ( &monBNa, GET_PATH_STR ( gl2uERR_dts3_min.bif ) );
  //GET_PATH_STR ( bn_c.bif ) );
  readera.proceed();

  gum::BayesNet<double> monBNb;
  gum::BIFReader< double > readerb ( &monBNb, GET_PATH_STR ( gl2uERR_dts3_max.bif ) );
  //GET_PATH_STR ( den_c.bif ) );
  readerb.proceed();


  // (G)(L)2U test
  gum::credal::CredalNet<double> myCNb ( monBNa, monBNb );

  //myCNb.bnToCredal(0.5);

  myCNb.intervalToCredal();//intervalToCredal();
  //std::cout << "computing min/max vertex" << std::endl;


  std::cout << myCNb.toString() << std::endl;

  myCNb.computeCPTMinMax();
  gum::credal::CNLoopyPropagation<double> lp2 = gum::credal::CNLoopyPropagation<double> ( myCNb );
  lp2.makeInference();

  //for ( gum::DAG::NodeIterator id = myCNb.src_bn().beginNodes(); id != myCNb.src_bn().endNodes(); ++id ) {
  for ( const auto id : myCNb.src_bn().nodes() ) {
    unsigned int dSize = myCNb.src_bn().variable ( id ).domainSize();

    for ( unsigned int mod = 0; mod < dSize; mod++ ) {
      std::cout << "l2u p(" << myCNb.src_bn().variable ( id ).name() << " = " << mod  << ") = [ " << lp2.marginalMin ( id ) [mod] << ", " << lp2.marginalMax ( id ) [mod] << " ] " << std::endl;
    }
  }

  gum::credal::CNMonteCarloSampling<double, gum::LazyPropagation<double> > MCE ( myCNb );
  MCE.storeVertices ( true );
  MCE.makeInference();

  //for ( gum::DAG::NodeIterator id = myCNb.src_bn().beginNodes(); id != myCNb.src_bn().endNodes(); ++id ) {
  for ( const auto id : myCNb.src_bn().nodes() ) {
    unsigned int dSize = myCNb.src_bn().variable ( id ).domainSize();

    for ( unsigned int mod = 0; mod < dSize; mod++ ) {
      std::cout << "MC p(" << myCNb.src_bn().variable ( id ).name() << " = " << mod  << ") = [ " << MCE.marginalMin ( id ) [mod] << ", " << MCE.marginalMax ( id ) [mod] << " ] " << std::endl;
    }

    std::cout << "MC vertices of : " << myCNb.src_bn().variable ( id ).name() << std::endl;
    std::cout << MCE.vertices ( id ) << std::endl;
  }

  return;



  gum::credal::CredalNet< double > cn ( GET_PATH_STR ( /*gl2u2_*/2Umin.bif ), GET_PATH_STR ( /*gl2u2_*/2Umax.bif ) );
  cn.intervalToCredal();
  std::cout << cn.toString() << std::endl;

  ///cn.approximatedBinarization();

  gum::credal::CNMonteCarloSampling< double, gum::LazyPropagation< double > > mc ( cn );
  //mc.setMaxTime ( 60 * 10 );
  //mc.setPeriodSize ( 4000 );
  mc.setMaxTime ( 1 * 60 );
  mc.setPeriodSize ( 1000 );
  mc.makeInference();

  //for ( gum::DAG::NodeIterator id = cn.current_bn().beginNodes(); id != cn.current_bn().endNodes(); ++id ) {
  for ( const auto id : cn.current_bn().nodes() ) {
    unsigned int dSize = cn.current_bn().variable ( id ).domainSize();

    for ( unsigned int mod = 0; mod < dSize; mod++ ) {
      std::cout << "mc p(" << cn.current_bn().variable ( id ).name() << " = " << mod  << ") = [ " << mc.marginalMin ( id ) [mod] << ", " << mc.marginalMax ( id ) [mod] << " ] " << std::endl;
    }
  }

  cn.approximatedBinarization();

  //std::cout << cn.toString() << std::endl;

  cn.computeCPTMinMax();

  gum::credal::CNLoopyPropagation< double > lp ( cn );

  lp.makeInference();

  //for ( gum::DAG::NodeIterator id = cn.current_bn().beginNodes(); id != cn.current_bn().endNodes(); ++id ) {
  for(const auto id : cn.current_bn().nodes()) {
    unsigned int dSize = cn.current_bn().variable ( id ).domainSize();

    for ( unsigned int mod = 0; mod < dSize; mod++ ) {
      std::cout << "l2u p(" << cn.current_bn().variable ( id ).name() << " = " << mod  << ") = [ " << lp.marginalMin ( id ) [mod] << ", " << lp.marginalMax ( id ) [mod] << " ] " << std::endl;
    }
  }
}

int main ( int argc, char* argv[] ) {
  try {
    test_credal();
    std::cout.flush();
  } catch ( gum::Exception& e ) {
    GUM_SHOWERROR ( e );
  }

  std::cout.clear();
  std::cout << "Press ENTER to continue...";
  std::cin.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );

  return EXIT_SUCCESS;
}
