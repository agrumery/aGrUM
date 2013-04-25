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
	
	int x = 1;
	//std::cout << (x <<= )
	
	gum::credal::CredalNet< double > cn ( GET_PATH_STR( gl2u2_min.bif ), GET_PATH_STR( gl2u2_max.bif ) );
	cn.intervalToCredal();
	std::cout << cn.toString() << std::endl;
	
	gum::credal::CNMonteCarloSampling< double, gum::LazyPropagation< double > > mc ( cn );
	
	mc.makeInference();
	
	for ( gum::DAG::NodeIterator id = cn.current_bn().beginNodes(); id != cn.current_bn().endNodes(); ++id ) {
		unsigned int dSize = cn.current_bn().variable(*id).domainSize();
		for( unsigned int mod = 0; mod < dSize; mod++ ) {
			std::cout << "mc p(" << cn.current_bn().variable(*id).name() << " = " << mod  << ") = [ " << mc.marginalMin(*id)[mod] << ", " << mc.marginalMax(*id)[mod] << " ] " << std::endl;
		}
	}
	
	
	cn.approximatedBinarization();
	
	std::cout << cn.toString() << std::endl;
	
	cn.computeCPTMinMax ();
	gum::setNumberOfThreads( 1 );
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
  } catch (gum::Exception& e) {
    GUM_SHOWERROR(e);
  }

  std::cout << "Press ENTER to continue...";
  std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

  return EXIT_SUCCESS;
}
