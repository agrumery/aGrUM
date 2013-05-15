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
//#include <agrum/prm/PRMFactory.h>
//#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER

//#include <agrum/prm/skool/cocoR/Parser.h>
//#endif //DOXYGEN_SHOULD_SKIP_THIS
//#include <agrum/prm/skool/SkoolReader.h>
//>>>>>>> Bug found for test -t all : now use of string::size() instead of string::length()


#include <agrum/core/Rational.h> // custom decimal to rational
#include <agrum/core/pow.h> // custom pow functions with integers, faster implementation

//=======

#include <agrum/CN/LrsWrapper.h>

//#include <agrum/prm/PRMFactory.h>
//#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER

//#include <agrum/prm/skool/cocoR/Parser.h>
//#endif //DOXYGEN_SHOULD_SKIP_THIS
//#include <agrum/prm/skool/SkoolReader.h>


//#include <agrum/core/Rational.h> // custom decimal to rational
//#include <agrum/core/pow.h> // custom pow functions with integers, faster implementation

//>>>>>>> Bug found for test -t all : now use of string::size() instead of string::length()

#include <agrum/CN/CredalNet.h>
#include <agrum/CN/CNMonteCarloSampling.h>
#include <agrum/CN/CNLoopyPropagation.h>

#include <agrum/CN/LpInterface.h>


#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

void test_credal() {

	/////////////// OMP test stuff ///////////////////

  std::cout << "isOMP () ? : " << gum::isOMP() << std::endl;
  std::cout << "threads : " << gum::getMaxNumberOfThreads() << std::endl;
  gum::setNumberOfThreads( gum::getNumberOfLogicalProcessors()*2 );
  std::cout << "new number : " << gum::getMaxNumberOfThreads() << std::endl;

  std::cout << "number of procs : " << gum::getNumberOfLogicalProcessors() << std::endl;

  //gum::setDynamicThreadsNumber(true);
  //gum::setNestedParallelism(true);

  std::cout << "dynamic threads : " << gum::getDynamicThreadsNumber() << std::endl;
  std::cout << "nested parallelism : " << gum::getNestedParallelism() << std::endl;
  std::cout << "thread limit : " << omp_get_thread_limit() << std::endl;
  std::cout << "nested max level : " << omp_get_max_active_levels() << std::endl;
	/////////////////////////////
	
	gum::cn::LpInterface< double > lpr;
	const gum::cn::LpCol & c1 = lpr.addCol();
	const gum::cn::LpCol & c2 = lpr.addCol();
	
	gum::cn::LpRow< double > & r1 = lpr.addRow();
	//r1.setColCoeff(c1, 1);
	//r1.setColCoeff(c2, -1);
	
	std::map< gum::cn::LpCol, double > cf;
	cf[c1] = 1; 
	cf[c2] = -1;
	
	r1.setColsCoeffs( cf );
	
	gum::cn::LpRow< double > & r2 = lpr.addRow();
	//r2.setColCoeff(c1, 1);
	//r2.setBounds(0., 0.7);
	
	cf.clear();
	cf[c1] = 1;
	
	r2.setColsCoeffs( cf, 0., 0.7 );
	
	std::vector< std::vector< double > > output = lpr.solve();
	std::cout << output << std::endl;
	
	cf.clear();
	cf[c1] = 0.; 
	cf[c2] = 0.;
	gum::cn::LpExpr< double > expr;//( cf );
	
	expr = 2. * c1 - c2 + 2. + 1. + 2. * c2 - 0.5 * c1;
	expr.print();
	
	expr = c1;
	expr.print();
	
	expr = 2.;
	expr.print();
	
	expr = c1 + 2.;
	expr.print();
	
	expr = 2. - c1;
	expr.print();
	
	expr = c1 + c2;
	expr.print();
	
	expr = c1 - c2;
	expr.print();
	
	expr = 0.2 <= c1 + c2;
	expr.print();
	
	expr = c1 + c2 <= 0.2;
	expr.print();
	
	expr = c1 <= c2;
	expr.print();
	
	expr = c1 + c2 <= c1 + c2;
	expr.print();
	
	expr = c1 - 2. + c2 + 3. <= 2. + c2;
	expr.print();
	
	expr = 2. <= c1 + c2 <= 3.;
	expr.print();
	
	expr = 2. <= 2. * c1 <= c2;
	expr.print();
	
	expr = c2 <= c1 + c2 <= 3.;
	expr.print();
	
	expr = 2. <= c1 + c2 <= c1 + c2;
	expr.print();
	
	expr = c1 + c2 <= c1 + c2 <= 3.;
	expr.print();
	
	expr = c1 + c2 <= c1 + c2 <= c1 + c2;
	expr.print();
	
	return;
	
	////////////////////////////////
	
	gum::credal::CredalNet< double > nCN;
	
	gum::NodeId a = nCN.addNode( "toto", 2 );
	gum::NodeId b = nCN.addNode( "titi", 3 );
	
	nCN.addArc( a, b );
	
	std::vector< double > lower( 2, 0 );
	std::vector< double > upper( 2, 1 );
	nCN.fillConstraints( a, lower, upper );
	
	nCN.fillConstraints( b, std::vector< double >( 6, 0 ), std::vector< double >( 6, 1 ) );
	
	nCN.intervalToCredal();
	
	std::cout << nCN.toString() << std::endl;	
	
	return;
	
	/////////////////////////////
	
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
	
	/*myCN3.idmLearning( 10, true );
	std::cout << myCN3.toString() << std::endl;*/
	
	myCN3.bnToCredal( 0.8, true, true );
	std::cout << myCN3.toString() << std::endl;
	
	return;
	
	//////////////////////////////////
	
	gum::credal::CredalNet< double > cn ( GET_PATH_STR( /*gl2u2_*/min.bif ), GET_PATH_STR( /*gl2u2_*/max.bif ) );
	cn.intervalToCredal();
	//std::cout << cn.toString() << std::endl;
	
	//cn.approximatedBinarization();
	
	gum::credal::CNMonteCarloSampling< double, gum::LazyPropagation< double > > mc ( cn );
	//mc.setMaxTime ( 60 * 10 );
	//mc.setPeriodSize ( 4000 );
	
	mc.makeInference();
	
	for ( gum::DAG::NodeIterator id = cn.current_bn().beginNodes(); id != cn.current_bn().endNodes(); ++id ) {
		unsigned int dSize = cn.current_bn().variable(*id).domainSize();
		for( unsigned int mod = 0; mod < dSize; mod++ ) {
			std::cout << "mc p(" << cn.current_bn().variable(*id).name() << " = " << mod  << ") = [ " << mc.marginalMin(*id)[mod] << ", " << mc.marginalMax(*id)[mod] << " ] " << std::endl;
		}
	}
	
	cn.approximatedBinarization();
	
	//std::cout << cn.toString() << std::endl;

	cn.computeCPTMinMax ();
	
	gum::credal::CNLoopyPropagation< double > lp ( cn );
	
	lp.makeInference();
	
	for ( gum::DAG::NodeIterator id = cn.current_bn().beginNodes(); id != cn.current_bn().endNodes(); ++id ) {
		unsigned int dSize = cn.current_bn().variable(*id).domainSize();
		for( unsigned int mod = 0; mod < dSize; mod++ ) {
			std::cout << "l2u p(" << cn.current_bn().variable(*id).name() << " = " << mod  << ") = [ " << lp.marginalMin(*id)[mod] << ", " << lp.marginalMax(*id)[mod] << " ] " << std::endl;
		}
	}
}

int main ( int argc, char *argv[] ) {
  try {
    test_credal();
		std::cout.flush();
  } catch (gum::Exception& e) {
    GUM_SHOWERROR(e);
  }
  std::cout.clear();
  std::cout << "Press ENTER to continue...";
  std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

  return EXIT_SUCCESS;
}
