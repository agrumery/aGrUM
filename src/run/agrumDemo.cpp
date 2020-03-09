#define GUM_TRACE_ON

#include <iostream>
#include <fstream>
#include <sstream>

#include <agrum/config.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>

#define xstrfy( s ) strfy( s )
#define strfy( x ) #x

#define GET_RESSOURCES_PATH( x ) xstrfy( GUM_SRC_PATH ) "/testunits/ressources/" x
#define GET_PATH_XSTR( x ) \
  xstrfy( GUM_SRC_PATH ) "/testunits/ressources/" xstrfy( x )


int main( int argc, char* argv[] ) {
  std::string file = GET_RESSOURCES_PATH( "FMDP/taxi/taxi.dat" );

  gum::FMDP<double> fmdp( true );

  gum::FMDPDatReader<double> reader( &fmdp, file );
  reader.trace( false );
  reader.proceed();

  std::cout << fmdp.toString() << std::endl;


}
