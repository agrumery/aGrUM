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

#define GUM_TRACE_ON

#include <agrum/config.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/generator/defaultBayesNetGenerator.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

int main( void ) {
  try {
    gum::BayesNet<float> bn;
    gum::BIFReader<float> reader( &bn, GET_PATH_STR( dBN.bif ) );
    reader.trace(true);
    GUM_CHECKPOINT;

    if ( ! reader.proceed() ) {
      reader.showElegantErrorsAndWarnings();
      reader.showErrorCounts();
      return false;
    }
    GUM_TRACE_VAR(bn);
/*
    GUM_TRACE("============================");
    GUM_TRACE("affectation after definition");
    gum::BayesNet<float> bn2;
    bn2=bn;
    GUM_TRACE_VAR(bn2);
    
    GUM_TRACE("==========================");
    GUM_TRACE("affectation and definition");
    gum::BayesNet<float> bn3=bn;
    GUM_TRACE_VAR(bn3);
    
    GUM_TRACE("============================================");
    GUM_TRACE("affectation after definition and affectation");    
    gum::BayesNet<float> bn4;
    gum::BIFReader<float> reader4( &bn4, GET_PATH_STR( asia.bif ) );
    if ( ! reader4.proceed() ) {
      reader4.showElegantErrorsAndWarnings();
      reader4.showErrorCounts();
      return false;
    }
    bn4=bn2;
    GUM_TRACE_VAR(bn4);
    
    GUM_TRACE("=======================================");
    GUM_TRACE("affectation of dynamically allocated BN");
    gum::BayesNet<float>* pBn5=new gum::BayesNet<float>();
    *pBn5=bn;
    GUM_TRACE_VAR(*pBn5);
    delete(pBn5);    
  */
  }
  catch ( gum::IOError& e ) {
    GUM_SHOWERROR( e );
  }
  /*
{
    gum::Size density[] = {9, 18, 27, 36, 45}; 
          int trial_nb = 5;
           for ( int i = 0; i < trial_nb; ++i ) {
          gum::DefaultBayesNetGenerator<float> bnGen( 10, density[i] );

            gum::BayesNet<float>* bayesNet = new gum::BayesNet<float>();
            bnGen.generateBN( *bayesNet);

            GUM_TRACE(bayesNet->toDot());
            gum::ShaferShenoyInference<float>* inf = 0;

            inf = new gum::ShaferShenoyInference<float>( *bayesNet );
           inf->makeInference();
//inf->makeInference();
            if ( inf != 0 ) {
             delete inf;
            }

            delete bayesNet;
	    std::cout << "end forloop " << std::endl;
          }
}
*/  
  gum::__atexit();
}


#endif // DOXYGEN_SHOULD_SKIP_THIS
