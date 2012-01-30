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

#include <clocale>

#define GET_PATH_STR(x) "/home/phw/Documents/svn/agrum/trunk/src/testunits/ressources/" #x

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

int main( void ) {
  try {
		
		std::cout<<"Locale is: "<<std::locale("")<<std::endl;
// 		std::locale::globale(std::locale("Fr_FR"));
		std::cout<<"Locale is: "<<std::locale("")<<std::endl;
		
    gum::BayesNet<float> bn;
    gum::BIFReader<float> reader( &bn, GET_PATH_STR( Diabetes.bif ) );

    if ( ! reader.proceed() ) {
      reader.showElegantErrorsAndWarnings();
      reader.showErrorCounts();
      return false;
    } else {
      std::cout<<bn<<std::endl;
      const Sequence<NodeId>&to=bn.getTopologicalOrder();
      
      for(Sequence<NodeId>::const_iterator it=to.begin();it!=to.end();++it) {
				if (bn.cpt(*it).sum()>0) {
					std::cout<<bn.variable(*it).name()<<" : " <<bn.dag().parents(*it)<<std::endl;
					std::cout<<"    "<<bn.cpt(*it)<<std::endl;
				}
      }
      return true;
    }


  } catch ( gum::IOError& e ) {
    GUM_SHOWERROR( e );
  }

  gum::__atexit();
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
