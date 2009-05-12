/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include <iostream>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <agrum/BN/bayesNet.h>
#include <agrum/BN/io/BIFIO.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>


#define GET_PATH_STR(x) ("src/testunits/resources/" #x)

void f( std::string n ) {
  GUM_TRACE( n+" en cours " );

  gum::BIFReader b;
  gum::BayesNet<float>* bn=b.read( n );

  gum::LazyPropagation<float> inf( *bn );
  // gum::ShaferShenoyInference<float> inf( *bn );
  inf.makeInference();

  const gum::Potential<float>& xxx = inf.marginal (2);

  delete( bn );

  GUM_TRACE( n+" OK" );

}
int main( void ) {
  try {
    /*
    f( GET_PATH_STR(Barley.bif) );
    f( GET_PATH_STR(alarm.bif) );
    f( GET_PATH_STR(carpo.bif) );
    f( GET_PATH_STR(Pigs.bif) );
    f( GET_PATH_STR(Water.bif) );
    f( GET_PATH_STR(Link.bif) );
    f( GET_PATH_STR(Mildew.bif) );
    f( GET_PATH_STR(Munin1.bif) );
    f( GET_PATH_STR(hailfinder.bif) );
    */
    f( GET_PATH_STR(Diabetes.bif) );
    /*
    f( GET_PATH_STR(insurance.bif) );
    */
  }
  catch ( gum::Exception& e ) {
    GUM_SHOWERROR( e );
  }
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
